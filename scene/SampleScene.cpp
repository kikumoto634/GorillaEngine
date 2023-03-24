#include "SampleScene.h"

#include "../Engine/math//Easing/Easing.h"

#include "../Game/Collision/CollisionSystem/Collision.h"
#include "../Game/Collision/SphereCollider.h"
#include "../Game/Collision/MeshCollider.h"

#include <sstream>
#include <iomanip>

using namespace std;
using namespace DirectX;

SampleScene::SampleScene(DirectXCommon *dxCommon, Window *window)
        : BaseScene(
        dxCommon,
        window)
{
}

void SampleScene::Application()
{
    BaseScene::Application();
}

void SampleScene::Initialize()
{
    BaseScene::Initialize();

#pragma region Common
    //Light
    lightGroup = LightGroup::Create();
    //LightColor
    lightGroup->SetAmbientColor({1,1,1});
    //LightSetting
    ObjModelObject::SetLight(lightGroup);

    lightGroup->SetDirLightActive(0, true);
    lightGroup->SetDirLightActive(1, true);
    lightGroup->SetDirLightActive(2, true);

    //CircleShadow
    lightGroup->SetCircleShadowActive(0, true);

    //Collision
    collisionManager = CollisionManager::GetInstance();

#pragma endregion Common

#pragma region _3DObject

    player = make_unique<Player>();
    player->Initialize("chr_sword");
    player->SetPosition({1, 0, 0});


    for(int i = 0; i < DIV_NUM; i++){
        for(int j = 0; j < DIV_NUM; j++){
            plane[i][j] = make_unique<TouchableObject>();
            plane[i][j]->Initialize("plane1x1");
            plane[i][j]->SetPosition({ float(-((DIV_NUM/2)*Plane_Size) + (i*Plane_Size)) ,-1 ,float(-((DIV_NUM/2)*Plane_Size) + (j*Plane_Size))});
        }
    }

    box = make_unique<TouchableObject>();
    box->Initialize("box1x1x1");
    box->SetPosition({-4, -1, -2});

    pyramid = make_unique<TouchableObject>();
    pyramid->Initialize("pyramid1x1");
    pyramid->SetPosition({4, -1, -2});

    sphere = make_unique<SampleObjObject>();
    sphere->Initialize("sphere", true);
    sphere->SetPosition({-1,0,0});
    sphere->SetCollider(new SphereCollider);

#pragma endregion _3DObject

#pragma region _2DObject

#pragma endregion _2DObject
}

void SampleScene::Update()
{
    BaseScene::Update();

#pragma region Input
#ifdef _DEBUG
	if(input->Push(DIK_A)){
		if(!IsCameraMovementChange)		camera->RotVector({0.f, XMConvertToRadians(3.f), 0.f});
		else if(IsCameraMovementChange)	camera->MoveVector({-1.f, 0.f, 0.f});
	}
	else if(input->Push(DIK_D)){
		if(!IsCameraMovementChange)		camera->RotVector({0.f,XMConvertToRadians(-3.f), 0.f});
		else if(IsCameraMovementChange)	camera->MoveVector({1.f, 0.f, 0.f});
	}

	if(input->Push(DIK_W)){
		if(!IsCameraMovementChange)		camera->RotVector({XMConvertToRadians(-3.f), 0.f, 0.f});
		else if(IsCameraMovementChange)	camera->MoveVector({0.f, 0.f, 1.f});
	}
	else if(input->Push(DIK_S)){
		if(!IsCameraMovementChange)		camera->RotVector({XMConvertToRadians(3.f), 0.f, 0.f});
		else if(IsCameraMovementChange)	camera->MoveVector({0.f, 0.f, -1.f});
	}

	if(input->Push(DIK_Q)){
		camera->MoveVector({0.f, 1.f, 0.f});
	}
	else if(input->Push(DIK_E)){
		camera->MoveVector({0.f, -1.f, 0.f});
	}

#endif // _DEBUG
#pragma endregion Input

#pragma region _3DObject
    player->Update(camera);

    for(int i = 0; i < DIV_NUM; i++){
        for(int j = 0; j < DIV_NUM; j++){
            plane[i][j]->Update(camera);
        }
    }

    box->Update(camera);

    pyramid->Update(camera);

    sphere->Update(camera);
#pragma endregion _3DObject

#pragma region _2DObject

#pragma endregion _2DObject

#pragma region Common
    {
        //DirLightPositionSetting
        lightGroup->SetCircleShadowDir(0, DirectX::XMVECTOR({circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0}));
        lightGroup->SetCircleShadowAtten(0, Vector3(circleShadowAtten[0], circleShadowAtten[1], circleShadowAtten[2]));
        lightGroup->SetCircleShadowFactorAngle(0, Vector2(circleShadowFactorAngle[0], circleShadowFactorAngle[1]));

        //CirclePositionSetting
        lightGroup->SetCircleShadowCasterPos(0, player->GetPosition());
    }
    lightGroup->Update();


    //Collsiion
    collisionManager->CheckAllCollisions();


#pragma endregion Common

#ifdef _DEBUG
    {
		//座標
		ImGui::SetNextWindowPos(ImVec2{0,100});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,150});
		ImGui::Begin("Debug");
		//カメラ 回転:false , 移動:true
		ImGui::Text("Camera");
		ImGui::Text("true = transform / false = rotation");
		ImGui::Checkbox("Change", &IsCameraMovementChange);
		if(ImGui::Button("Shake")){
			camera->ShakeStart();
		}
		ImGui::End();
	}

    {
        ImGui::SetNextWindowPos(ImVec2{0,250});
        ImGui::SetNextWindowSize(ImVec2{500,55});
        ImGui::Begin("PlayerPos && SpotLightPos && CircleShadowPos");
        ImGui::SetNextWindowPos(ImVec2{0,0});
        ImGui::DragFloat3("circlePos", (float*)&player->GetPosition(), 0.1f);
        ImGui::End();
    }

    {
        ImGui::SetNextWindowPos(ImVec2{0,310});
        ImGui::SetNextWindowSize(ImVec2{500,100});
        ImGui::Begin("CircleShadow");
        ImGui::DragFloat3("circleShadowDir", circleShadowDir, 0.1f);
        ImGui::DragFloat3("circleShadowAtten", circleShadowAtten, 0.1f);
        ImGui::DragFloat2("circleShadowFactorAngle", circleShadowFactorAngle, 0.1f);
        ImGui::End();
    }
#endif // _DEBUG

    BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
    BaseScene::Draw();

    player->Draw();
    for(int i = 0; i < DIV_NUM; i++){
        for(int j = 0; j < DIV_NUM; j++){
            plane[i][j]->Draw();
        }
    }
    box->Draw();
    pyramid->Draw();
    sphere->Draw();

    Sprite::SetPipelineState();
    UIDraw();
}

void SampleScene::UIDraw()
{
#ifdef _DEBUG
    debugText->Printf(0,0,1.f,"Camera Target  X:%f, Y:%f, Z:%f", camera->GetTarget().x, camera->GetTarget().y, camera->GetTarget().z);
    debugText->Printf(0,16,1.f,"Camera Eye  X:%f, Y:%f, Z:%f", camera->GetEye().x, camera->GetEye().y, camera->GetEye().z);
#endif // _DEBUG
}

void SampleScene::Finalize()
{
#pragma region _3DObject
    player->Finalize();
    for(int i = 0; i < DIV_NUM; i++){
        for(int j = 0; j < DIV_NUM; j++){
            plane[i][j]->Finalize();
        }
    }
    box->Finalize();
    pyramid->Finalize();
    sphere->Finalize();
#pragma endregion _3DObject

#pragma region _2DObject

#pragma endregion _2DObject

#pragma region Common
    delete lightGroup;
    lightGroup = nullptr;
#pragma endregion Common

    BaseScene::Finalize();
}

void SampleScene::NextSceneChange()
{
}
