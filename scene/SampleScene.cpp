#include "SampleScene.h"

#include "../Engine/math//Easing/Easing.h"

#include "../Game/Collision/Collision.h"
#include "../Game/Collision/SphereCollider.h"

#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG


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

#pragma region 汎用初期化
	//ライト
	lightGroup = LightGroup::Create();
	//色設定
	lightGroup->SetAmbientColor({1,1,1});
	//3Dオブジェクト(.obj)にセット
	ObjModelObject::SetLight(lightGroup);

	lightGroup->SetDirLightActive(0, false);
	lightGroup->SetDirLightActive(1, false);
	lightGroup->SetDirLightActive(2, false);

	//丸影
	lightGroup->SetCircleShadowActive(0, true);

	//衝突マネージャー
	collisionManager = CollisionManager::GetInstance();

#pragma endregion 汎用初期化

#pragma region _3D初期化

	player = make_unique<Player>();
	player->Initialize("chr_sword");
	player->SetPosition({1, 0, 0});

	obj3_2 = make_unique<SampleObjObject>();
	obj3_2->Initialize("ground");
	obj3_2->SetPosition({0,-1,0});

	obj3_3 = make_unique<SampleObjObject>();
	obj3_3->Initialize("sphere", true);
	obj3_3->SetPosition({-1,0,0});
	obj3_3->SetCollider(new SphereCollider);

	obj3_4 = make_unique<SampleObjObject>();
	obj3_4->Initialize("skydome");

#pragma endregion _3D初期化

#pragma region _2D初期化

#pragma endregion _2D初期化

#ifdef _DEBUG
	imgui = new imguiManager();
	imgui->Initialize(window, dxCommon);
#endif // _DEBUG

}

void SampleScene::Update()
{
	BaseScene::Update();

#ifdef _DEBUG
	imgui->Begin();
#endif // _DEBUG

#pragma region 入力処理
	if(input->Push(DIK_A)){
		camera->RotVector({0.f, XMConvertToRadians(3.f), 0.f});
	}
	else if(input->Push(DIK_D)){
		camera->RotVector({0.f,XMConvertToRadians(-3.f), 0.f});
	}

	if(input->Push(DIK_W)){
		camera->RotVector({XMConvertToRadians(-3.f), 0.f, 0.f});
	}
	else if(input->Push(DIK_S)){
		camera->RotVector({XMConvertToRadians(3.f), 0.f, 0.f});
	}
#pragma endregion 入力処理

#pragma region _3D更新
	player->Update(camera);

	obj3_2->Update(camera);
	Vector3 rot2 = obj3_3->GetRotation();
	rot2.y += XMConvertToRadians(1.f);
	obj3_3->SetRotation(rot2);
	obj3_3->Update(camera);

	obj3_4->Update(camera);
#pragma endregion _3D更新

#pragma region _2D更新

#pragma endregion _2D更新

#pragma region 汎用更新
	{
		//丸影
		lightGroup->SetCircleShadowDir(0, DirectX::XMVECTOR({circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0}));
		lightGroup->SetCircleShadowAtten(0, Vector3(circleShadowAtten[0], circleShadowAtten[1], circleShadowAtten[2]));
		lightGroup->SetCircleShadowFactorAngle(0, Vector2(circleShadowFactorAngle[0], circleShadowFactorAngle[1]));

		//プレイヤー、丸影座標
		lightGroup->SetCircleShadowCasterPos(0, player->GetPosition());
	}
	lightGroup->Update();

	//すべての衝突をチェック
	collisionManager->CheckAllCollisions();

#pragma endregion 汎用更新

#ifdef _DEBUG
	if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

	//ウィンドウサイズ
	ImGui::SetNextWindowSize(ImVec2{500,55});
	//ウィンドウ座標
	ImGui::SetNextWindowPos(ImVec2{0,130});
	//開始、タイトル名設定
	ImGui::Begin("PlayerPos && SpotLightPos && CircleShadowPos");
	ImGui::SetNextWindowPos(ImVec2{0,0});
	ImGui::DragFloat3("circlePos", (float*)&player->GetPosition(), 0.1f);
	//終了
	ImGui::End();

	//ウィンドウサイズ
	ImGui::SetNextWindowSize(ImVec2{500,100});
	//ウィンドウ座標
	ImGui::SetNextWindowPos(ImVec2{0,0});
	//開始、タイトル名設定
	ImGui::Begin("CircleShadow");
	ImGui::DragFloat3("circleShadowDir", circleShadowDir, 0.1f);
	ImGui::DragFloat3("circleShadowAtten", circleShadowAtten, 0.1f);
	ImGui::DragFloat2("circleShadowFactorAngle", circleShadowFactorAngle, 0.1f);
	//終了
	ImGui::End();

	imgui->End();
#endif // _DEBUG

	BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
	BaseScene::Draw();

#pragma region _2D_背景描画
	
#pragma endregion _2D_背景描画

#pragma region _3D描画
	player->Draw();
	obj3_2->Draw();
	obj3_3->Draw();
	obj3_4->Draw();

#pragma region パーティクル
	player->Draw2D();

#pragma endregion パーティクル

#pragma endregion _3D描画

#pragma region _2D_UI描画
	Sprite::SetPipelineState();

#ifdef _DEBUG
	debugText->Printf(0,0,1.f,"Camera Target  X:%f, Y:%f, Z:%f", camera->GetTarget().x, camera->GetTarget().y, camera->GetTarget().z);
	debugText->Printf(0,16,1.f,"Camera Eye  X:%f, Y:%f, Z:%f", camera->GetEye().x, camera->GetEye().y, camera->GetEye().z);

#endif // _DEBUG
	BaseScene::EndDraw();
#pragma endregion _2D_UI描画

#ifdef _DEBUG
	imgui->Draw();
#endif // _DEBUG

}

void SampleScene::Finalize()
{
#ifdef _DEBUG
	imgui->Finalize();
	delete imgui;
#endif // _DEBUG

#pragma region _3D解放
	player->Finalize();
	obj3_2->Finalize();
	obj3_3->Finalize();
	obj3_4->Finalize();
#pragma endregion _3D解放

#pragma region _2D解放

#pragma endregion _2D解放

#pragma region 汎用解放

	delete lightGroup;
	lightGroup = nullptr;

#pragma endregion 汎用解放

	BaseScene::Finalize();
}