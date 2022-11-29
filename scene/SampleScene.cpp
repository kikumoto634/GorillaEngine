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

#pragma region _3Dú»
	obj = make_unique<SampleFbxObject>();
	obj->Initialize("cube");
	obj->SetPosition({0, -50, 200});

	obj2 = make_unique<SampleParticleObject>();
	obj2->Initialize(1);

	//obj3 = make_unique<SampleObjObject>();
	//obj3->Initialize("triangle_mat");

#pragma endregion _3Dú»

#pragma region _2Dú»
	//sp = make_unique<SampleSprite>();
	//sp->Initialize(0);
#pragma endregion _2Dú»
}

void SampleScene::Update()
{
	BaseScene::Update();

#pragma region üÍ

	if(input->Push(DIK_A)){
		camera->RotVector({0.f, XMConvertToRadians(3.f), 0.f});
	}
	else if(input->Push(DIK_D)){
		camera->RotVector({0.f,XMConvertToRadians(-3.f), 0.f});
	}

#pragma endregion üÍ

#pragma region _3DXV
	//Vector3 pos = Easing_Linear_Point2({0, 0, 300}, {0, -100, 300}, Time_OneWay(time, 2.f));
	//Vector3 pos = Easing_Linear_Point3({0, 0, 300}, {0, -50, 500}, {0, -100, 300}, Time_Loop(time, 2.f));
	//obj->SetPosition(pos);
	obj->Update(camera);

	obj2->Update(camera);

	//obj3->Update(camera);
#pragma endregion _3DXV

#pragma region _2DXV
	//sp->Update();
#pragma endregion _2DXV

	BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
	BaseScene::Draw();

#pragma region _2D_wi`æ
	//sp->Draw();
#pragma endregion _2D_wi`æ

#pragma region _3D`æ
	obj->Draw();

	obj2->Draw();

	//obj3->Draw();

#pragma endregion _3D`æ

#pragma region _2D_UI`æ
	Sprite::SetPipelineState();

#pragma endregion _2D_UI`æ

	BaseScene::EndDraw();
}

void SampleScene::Finalize()
{
#pragma region _3Dðú
	obj->Finalize();

	obj2->Finalize();

	//obj3->Finalize();
#pragma endregion _3Dðú

#pragma region _2Dðú
	//sp->Finalize();
#pragma endregion _2Dðú

	BaseScene::Finalize();
}