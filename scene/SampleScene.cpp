#include "SampleScene.h"

#include "../Engine/math//Easing/Easing.h"

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

#pragma region _3D‰Šú‰»
	obj = make_unique<SampleFbxObject>();
	obj->Initialize("cube");
	obj->SetPosition({0, -50, 200});

	obj2 = make_unique<SampleParticleObject>();
	obj2->Initialize(1);

	obj3_1 = make_unique<SampleObjObject>();
	obj3_1->Initialize("chr_sword");
	obj3_1->SetPosition({1,0,0});

	obj3_2 = make_unique<SampleObjObject>();
	obj3_2->Initialize("ground");
	obj3_2->SetPosition({0,-1,0});

	obj3_3 = make_unique<SampleObjObject>();
	obj3_3->Initialize("sphere");
	obj3_3->SetPosition({-1,0,0});

#pragma endregion _3D‰Šú‰»

#pragma region _2D‰Šú‰»
	//sp = make_unique<SampleSprite>();
	//sp->Initialize(0);
#pragma endregion _2D‰Šú‰»
}

void SampleScene::Update()
{
	BaseScene::Update();

#pragma region “ü—Íˆ—

	if(input->Push(DIK_A)){
		camera->RotVector({0.f, XMConvertToRadians(3.f), 0.f});
	}
	else if(input->Push(DIK_D)){
		camera->RotVector({0.f,XMConvertToRadians(-3.f), 0.f});
	}

#pragma endregion “ü—Íˆ—

#pragma region _3DXV
	/*Vector3 pos = Easing_Linear_Point2({0, 0, 300}, {0, -100, 300}, Time_OneWay(time, 2.f));
	Vector3 pos = Easing_Linear_Point3({0, 0, 300}, {0, -50, 500}, {0, -100, 300}, Time_Loop(time, 2.f));
	obj->SetPosition(pos);*/
	/*Vector3 rot = obj->GetRotation();
	rot.y += XMConvertToRadians(3.f);
	obj->SetRotation(rot);*/
	obj->Update(camera);

	obj2->Update(camera);

	obj3_1->Update(camera);
	obj3_2->Update(camera);
	obj3_3->Update(camera);
#pragma endregion _3DXV

#pragma region _2DXV
	//sp->Update();
#pragma endregion _2DXV

	BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
	BaseScene::Draw();

#pragma region _2D_”wŒi•`‰æ
	//sp->Draw();
#pragma endregion _2D_”wŒi•`‰æ

#pragma region _3D•`‰æ
	//obj->Draw();

	obj2->Draw();

	obj3_1->Draw();
	obj3_2->Draw();
	obj3_3->Draw();

#pragma endregion _3D•`‰æ

#pragma region _2D_UI•`‰æ
	Sprite::SetPipelineState();

#pragma endregion _2D_UI•`‰æ

	BaseScene::EndDraw();
}

void SampleScene::Finalize()
{
#pragma region _3D‰ğ•ú
	obj->Finalize();

	obj2->Finalize();

	obj3_1->Finalize();
	obj3_2->Finalize();
	obj3_3->Finalize();
#pragma endregion _3D‰ğ•ú

#pragma region _2D‰ğ•ú
	//sp->Finalize();
#pragma endregion _2D‰ğ•ú

	BaseScene::Finalize();
}