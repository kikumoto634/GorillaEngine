#include "SampleScene.h"

#include "../Engine/math//Easing/Easing.h"

//ライト動かすよう
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

#pragma region 汎用初期化
	lightGroup = LightGroup::Create();
	//色設定
	lightGroup->SetAmbientColor({1,1,1});
	//3Dオブジェクト(.obj)にセット
	ObjModelObject::SetLight(lightGroup);

	lightGroup->SetDirLightActive(0, false);
	lightGroup->SetDirLightActive(1, false);
	lightGroup->SetDirLightActive(2, false);
	lightGroup->SetPointLightActive(0, true);
	pointLightPos[0] = 0.5f;
	pointLightPos[1] = 1.0f;
	pointLightPos[2] = 0.0f;
#pragma endregion 汎用初期化

#pragma region _3D初期化
	//obj = make_unique<SampleFbxObject>();
	//obj->Initialize("cube");
	//obj->SetPosition({0, -50, 200});

	//obj2 = make_unique<SampleParticleObject>();
	//obj2->Initialize(1);

	obj3_1 = make_unique<SampleObjObject>();
	obj3_1->Initialize("chr_sword");
	obj3_1->SetPosition({1,0,0});

	obj3_2 = make_unique<SampleObjObject>();
	obj3_2->Initialize("ground");
	obj3_2->SetPosition({0,-1,0});

	obj3_3 = make_unique<SampleObjObject>();
	obj3_3->Initialize("sphere", true);
	obj3_3->SetPosition({-1,0,0});


	obj3_4 = make_unique<SampleObjObject>();
	obj3_4->Initialize("skydome");

#pragma endregion _3D初期化

#pragma region _2D初期化
	//sp = make_unique<SampleSprite>();
	//sp->Initialize(0);
#pragma endregion _2D初期化
}

void SampleScene::Update()
{
	BaseScene::Update();

#pragma region 入力処理

	if(input->Push(DIK_A)){
		camera->RotVector({0.f, XMConvertToRadians(3.f), 0.f});
	}
	else if(input->Push(DIK_D)){
		camera->RotVector({0.f,XMConvertToRadians(-3.f), 0.f});
	}

#pragma endregion 入力処理

#pragma region _3D更新
	/*Vector3 pos = Easing_Linear_Point2({0, 0, 300}, {0, -100, 300}, Time_OneWay(time, 2.f));
	Vector3 pos = Easing_Linear_Point3({0, 0, 300}, {0, -50, 500}, {0, -100, 300}, Time_Loop(time, 2.f));
	obj->SetPosition(pos);*/

	//obj->Update(camera);

	//obj2->Update(camera);

	Vector3 rot1 = obj3_1->GetRotation();
	rot1.y += XMConvertToRadians(1.f);
	obj3_1->SetRotation(rot1);
	obj3_1->Update(camera);

	obj3_2->Update(camera);

	Vector3 rot2 = obj3_3->GetRotation();
	rot2.y += XMConvertToRadians(1.f);
	obj3_3->SetRotation(rot2);
	obj3_3->Update(camera);

	obj3_4->Update(camera);
#pragma endregion _3D更新

#pragma region _2D更新
	//sp->Update();
#pragma endregion _2D更新

#pragma region 汎用更新
	{
		/*lightGroup->SetAmbientColor(Vector3{1,1,1});
		lightGroup->SetDirLightDir(0, XMVECTOR{0,-1,0});
		lightGroup->SetDirLightColor(0, Vector3{1,1,1});*/

		lightGroup->SetPointLightPos(0, Vector3{pointLightPos[0], pointLightPos[1], pointLightPos[2]});
		lightGroup->SetPointLightColor(0, Vector3(pointLightColor[0], pointLightColor[1], pointLightColor[2]));
		lightGroup->SetPointLightAtten(0, Vector3(pointLightAtten[0], pointLightAtten[1], pointLightAtten[2]));
	}
	lightGroup->Update();

#pragma endregion 汎用更新

	BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
	BaseScene::Draw();

#pragma region _2D_背景描画
	//sp->Draw();
#pragma endregion _2D_背景描画

#pragma region _3D描画
	//obj->Draw();

	//obj2->Draw();

	obj3_1->Draw();
	obj3_2->Draw();
	obj3_3->Draw();
	obj3_4->Draw();

#pragma endregion _3D描画

#pragma region _2D_UI描画
	Sprite::SetPipelineState();

#pragma endregion _2D_UI描画

	BaseScene::EndDraw();
}

void SampleScene::Finalize()
{
#pragma region _3D解放
	//obj->Finalize();

	//obj2->Finalize();

	obj3_1->Finalize();
	obj3_2->Finalize();
	obj3_3->Finalize();
	obj3_4->Finalize();
#pragma endregion _3D解放

#pragma region _2D解放
	//sp->Finalize();
#pragma endregion _2D解放

#pragma region 汎用解放

	delete lightGroup;
	lightGroup = nullptr;

#pragma endregion 汎用解放

	BaseScene::Finalize();
}