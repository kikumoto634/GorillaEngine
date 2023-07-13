#include "ObjModelManager.h"
#include <cassert>
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

DirectXCommon* ObjModelManager::dxCommon = nullptr;

void ObjModelManager::StaticInitialize(DirectXCommon* dxCommon_)
{
	dxCommon = dxCommon_;

	ObjModelMesh::StaticInitialize(dxCommon);
}

ObjModelManager::~ObjModelManager()
{
	for(auto& m : meshs){
		delete m;
	}
	meshs.clear();
}

void ObjModelManager::CreateModel(std::string filePath, bool smmothing)
{
	HRESULT result = S_FALSE;

	// デスクリプタヒープを生成	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	descHeapDesc.NumDescriptors = 1; // シェーダーリソースビュー1つ
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//生成
	assert(SUCCEEDED(result));


	LoadModel(filePath, smmothing);

	for(auto& m : meshs){
		m->CreateBuffers();
	}
}

void ObjModelManager::Draw(ID3D12GraphicsCommandList* commandList)
{
	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// シェーダリソースビューをセット
	 commandList->SetGraphicsRootDescriptorTable(2, descHeap->GetGPUDescriptorHandleForHeapStart());
	
	 for(auto& m : meshs){
		m->Draw(commandList);
	 }
}

void ObjModelManager::LoadModel(const std::string &filePath, bool smmothing)
{
	//ファイルストリーム
	ifstream file;
	int indexCountTex = 0;

	//.objファイルを開く
	const string modelname = filePath;
	const string filename = modelname + ".obj";
	const string directoryPath = "Resources/" + modelname + "/";
	file.open(directoryPath + filename);
	//ファイルオープン失敗
	assert(file.fill());

	// メッシュ生成
	meshs.emplace_back(new ObjModelMesh);
	ObjModelMesh* mesh = meshs.back();

	vector<Vector3> positions;
	vector<Vector3> normals;
	vector<Vector2> texcoords;
	string line;
	while(getline(file,line)){
		istringstream line_stream(line);
		string key;
		getline(line_stream, key, ' ');

		if(key == "g")
		{
			if(modelname.size() > 0)
			{
				mesh->CalculateSmoothedVertexNormals();
				indexCountTex = 0;
			}
			// グループ名読み込み
			string groupName;
			line_stream >> groupName;
		}
		if(key == "v"){
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);
		}
		if(key == "f"){
			int faceIndexCount = 0;
			string index_string;
			while(getline(line_stream, index_string, ' ')){
				istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1,ios_base::cur);


				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);
				index_stream >> indexNormal;

				ObjModelMesh::VertexPosNormalUv vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				mesh->AddVertex(vertex);

				//エッジ平滑化用のデータ追加
				if(smmothing)
				{
					//vキー(座標データ)の番号と、すべて合成した頂点のインデックスをセットで登録する
					mesh->AddSmmpthData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
				}

				// インデックスデータの追加
				if (faceIndexCount >= 3) {
					// 四角形ポリゴンの4点目なので、
					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
					mesh->AddIndex(indexCountTex - 1);
					mesh->AddIndex(indexCountTex);
					mesh->AddIndex(indexCountTex - 3);
				} else {
					mesh->AddIndex(indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
		if(key == "vt"){
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.emplace_back(texcoord);
		}
		if(key == "vn"){
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			normals.emplace_back(normal);
		}
		if(key == "mtllib"){
			string filename;
			line_stream >> filename;
			LoadMaterial(directoryPath, filename);
		}
	}

	file.close();

	//頂点法線の平均によるエッジの平滑化
	if(smmothing){
		mesh->CalculateSmoothedVertexNormals();
	}
}

void ObjModelManager::LoadTexture(const std::string &directoryPath, const std::string &filename)
{
	HRESULT result = S_FALSE;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ファイルパスを結合
	 string filepath = directoryPath + filename;

	 // ユニコード文字列に変換する
	 wchar_t wfilepath[128];
	 int iBufferSize = MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	// WICテクスチャのロード
	result = LoadFromWICFile( wfilepath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format, metadata.width, (UINT)metadata.height, (UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// テクスチャ用バッファの生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
		nullptr, IID_PPV_ARGS(&texbuff));
	assert(SUCCEEDED(result));

	// テクスチャバッファにデータ転送
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0); // 生データ抽出
		result = texbuff->WriteToSubresource(
			(UINT)i,
			nullptr,              // 全領域へコピー
			img->pixels,          // 元データアドレス
			(UINT)img->rowPitch,  // 1ラインサイズ
			(UINT)img->slicePitch // 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	dxCommon->GetDevice()->CreateShaderResourceView(texbuff.Get(), //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		descHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void ObjModelManager::LoadMaterial(const std::string &directoryPath, const std::string &filename)
{
	ifstream file;
	file.open(directoryPath + filename);
	if(file.fail()){
		assert(0);
	}

	string line;
	while(getline(file, line)){
		istringstream line_stream(line);

		string key;
		getline(line_stream,key,' ');

		if(key[0] == '\t'){
			key.erase(key.begin());
		}

		// 先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			// マテリアル名読み込み
			line_stream >> material.name;
		}
		// 先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		// 先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		// 先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			// テクスチャのファイル名読み込み
			line_stream >> material.textureFilename;
			// テクスチャ読み込み
			LoadTexture(directoryPath, material.textureFilename);
		}
	}
	file.close();
}
