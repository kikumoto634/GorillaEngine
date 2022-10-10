#include "FbxLoader.h"
#include <cassert>

const std::string FbxLoader::baseDirectory = "Resources/";
const std::string FbxLoader::defaultTextureFileName = "white1x1.png";

using namespace DirectX;

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}

void FbxLoader::Initialize(ID3D12Device *device)
{
    assert(fbxManager == nullptr);
    this->device = device;
    fbxManager = FbxManager::Create();
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager,IOSROOT);
    fbxManager->SetIOSettings(ios);
    fbxImporter = FbxImporter::Create(fbxManager,"");
}

void FbxLoader::Finalize()
{
    fbxImporter->Destroy();
    fbxManager->Destroy();
}

FbxModelManager* FbxLoader::LoadModelFromFile(const string &modelName)
{
    const string directoryPath = baseDirectory + modelName + "/";
    const string fileName = modelName + ".fbx";
    const string fullPath = directoryPath + fileName;

    auto result = fbxImporter->Initialize(fullPath.c_str(), -1, fbxManager->GetIOSettings());
    assert(result);

    FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");
    fbxImporter->Import(fbxScene);

    FbxModelManager* model = new FbxModelManager();
    model->name = modelName;

    int nodeCount = fbxScene->GetNodeCount();

    model->nodes.reserve(nodeCount);

    ParseNodeRecursive(model, fbxScene->GetRootNode());

    fbxScene->Destroy();
    model->CreateBuffer(device);

    return model;
}

void FbxLoader::ParseNodeRecursive(FbxModelManager *model, FbxNode *fbxNode, Node* parent)
{
    model->nodes.emplace_back();
    Node& node = model->nodes.back();

    string name = fbxNode->GetName();

    FbxDouble3 rotation = fbxNode->LclRotation.Get();
    FbxDouble3 scaling = fbxNode->LclScaling.Get();
    FbxDouble3 translation = fbxNode->LclTranslation.Get();

    node.rotation = {(float)rotation[0], (float)rotation[1], (float)rotation[2], 0.f};
    node.scaling = {(float)scaling[0], (float)scaling[1], (float)scaling[2], 0.f};
    node.translation = {(float)translation[0], (float)translation[1], (float)translation[2], 0.f};

    node.rotation.m128_f32[0] = XMConvertToRadians(node.rotation.m128_f32[0]);
    node.rotation.m128_f32[1] = XMConvertToRadians(node.rotation.m128_f32[1]);
    node.rotation.m128_f32[2] = XMConvertToRadians(node.rotation.m128_f32[2]);

    XMMATRIX matScaling, matRotation, matTranslation;
    matScaling = XMMatrixScalingFromVector(node.scaling);
    matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
    matTranslation = XMMatrixTranslationFromVector(node.translation);

    node.transform = XMMatrixIdentity();
    node.transform *= matScaling;
    node.transform *= matRotation;
    node.transform *= matTranslation;

    node.globalTransform = node.transform;
    if(parent){
        node.parent = parent;
        node.globalTransform *= parent->globalTransform;
    }

    FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

    if(fbxNodeAttribute){
        if(fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh){
            model->meshNode = &node;
            ParseMesh(model, fbxNode);
        }
    }

    for(int i = 0; i < fbxNode->GetChildCount(); i++){
        ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
    }
}

void FbxLoader::ParseMesh(FbxModelManager *model, FbxNode *fbxNode)
{
    FbxMesh* fbxMesh = fbxNode->GetMesh();

    ParseMeshVertices(model, fbxMesh);

    ParseMeshFaces(model, fbxMesh);

    ParseMaterial(model, fbxNode);
}

void FbxLoader::ParseMeshVertices(FbxModelManager *model, FbxMesh *fbxMesh)
{
    auto& vertices = model->vertices;

    const int controlPointsCount = fbxMesh->GetControlPointsCount();

    FbxModelManager::VertexPosNormalUv vert{};
    model->vertices.resize(controlPointsCount, vert);

    FbxVector4* pCoord = fbxMesh->GetControlPoints();

    for(int i = 0; i < controlPointsCount; i++){
        FbxModelManager::VertexPosNormalUv& vertex = vertices[i];
        vertex.pos.x = (float)pCoord[i][0];
        vertex.pos.y = (float)pCoord[i][1];
        vertex.pos.z = (float)pCoord[i][2];
    }
}

void FbxLoader::ParseMeshFaces(FbxModelManager *model, FbxMesh *fbxMesh)
{
    auto& vertices = model->vertices;
    auto& indices = model->indices;

    assert(indices.size() == 0);

    const int polygonCount = fbxMesh->GetPolygonCount();
    const int textureUVCount = fbxMesh->GetTextureUVCount();

    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);

    for(int i = 0; i < polygonCount; i++){
        const int polygonSize = fbxMesh->GetPolygonSize(i);
        assert(polygonSize <= 4);

        for(int j = 0; j < polygonSize; j++){
            int index = fbxMesh->GetPolygonVertex(i, j);
            assert(index >= 0);

            FbxModelManager::VertexPosNormalUv& vertex = vertices[index];
            FbxVector4 normal;
            if(fbxMesh->GetPolygonVertexNormal(i, j, normal)){
                vertex.normal.x = (float)normal[0];
                vertex.normal.y = (float)normal[1];
                vertex.normal.z = (float)normal[2];
            }

            if(textureUVCount > 0){
                FbxVector2 uvs;
                bool lUnmappedUV;
                if(fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV)){
                    vertex.uv.x = (float)uvs[0];
                    vertex.uv.y = (float)uvs[1];
                }
            }

            if(j < 3){
                indices.push_back(index);
            }
            else{
                int index2 = indices[indices.size() - 1];
                int index3 = index;
                int index0 = indices[indices.size() - 3];
                indices.push_back(index2);
                indices.push_back(index3);
                indices.push_back(index0);
            }
        }
    }
}

void FbxLoader::ParseMaterial(FbxModelManager *model, FbxNode *fbxNode)
{
    const int materialCount = fbxNode->GetMaterialCount();
    if(materialCount > 0){
        FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
        bool textureLoaded = false;

        if(material){
            if(material->GetClassId().Is(FbxSurfaceLambert::ClassId)){
                FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);

                FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
                model->ambient.x = (float)ambient.Get()[0];
                model->ambient.y = (float)ambient.Get()[1];
                model->ambient.z = (float)ambient.Get()[2];

                FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
                model->diffuse.x = (float)diffuse.Get()[0];
                model->diffuse.y = (float)diffuse.Get()[1];
                model->diffuse.z = (float)diffuse.Get()[2];
            }

            const FbxProperty diffusePoperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if(diffusePoperty.IsValid()){
                const FbxFileTexture* texture = diffusePoperty.GetSrcObject<FbxFileTexture>();
                if(texture){
                    const char* filepath = texture->GetFileName();
                    string path_str(filepath);
                    string name = ExtractFileName(path_str);

                    LoadTexture(model, baseDirectory + model->name + "/" + name);
                    textureLoaded = true;
                }
            }
        }

        if(!textureLoaded){
            LoadTexture(model, baseDirectory + defaultTextureFileName);
        }
    }
}

void FbxLoader::LoadTexture(FbxModelManager *model, const std::string &fullpath)
{
    HRESULT result = S_FALSE;

    TexMetadata& metaData = model->metaData;
    ScratchImage& scrachImage = model->scratchImage;

    wchar_t wfilepath[128];
    MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath, _countof(wfilepath));
    result = LoadFromWICFile(wfilepath, WIC_FLAGS_NONE, &metaData, scrachImage);
    assert(SUCCEEDED(result));
}

std::string FbxLoader::ExtractFileName(const std::string &path)
{
    size_t pos1;
    pos1 = path.rfind("\\");
    if(pos1 != string::npos){
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }

    pos1 = path.rfind("/");
    if(pos1 != string::npos){
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }

    return path;
}
