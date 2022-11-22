#include "SampleScene.h"

#include "../Engine/math//Easing/Easing.h"

using namespace std;

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
#pragma endregion _3D‰Šú‰»

#pragma region _2D‰Šú‰»
	sp = make_unique<SampleSprite>();
	sp->Initialize(0);
#pragma endregion _2D‰Šú‰»
}

void SampleScene::Update()
{
	BaseScene::Update();

#pragma region _3DXV
	//Vector3 pos = Easing_Linear_Point2({0, 0, 300}, {0, -100, 300}, Time_OneWay(time, 2.f));
	//Vector3 pos = Easing_Linear_Point3({0, 0, 300}, {0, -50, 500}, {0, -100, 300}, Time_Loop(time, 2.f));
	//obj->SetPosition(pos);
	obj->Update(camera);
#pragma endregion _3DXV

#pragma region _2DXV
	sp->Update();
#pragma endregion _2DXV

	BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
	BaseScene::Draw();

#pragma region _2D_”wŒi•`‰æ
	sp->Draw();
#pragma endregion _2D_”wŒi•`‰æ

#pragma region _3D•`‰æ
	obj->Draw();
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
#pragma endregion _3D‰ğ•ú

#pragma region _2D‰ğ•ú
	sp->Finalize();
#pragma endregion _2D‰ğ•ú

	BaseScene::Finalize();
}