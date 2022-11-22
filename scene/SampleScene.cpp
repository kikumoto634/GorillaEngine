#include "SampleScene.h"

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

#pragma region _3D初期化
	obj = make_unique<SampleFbxObject>();
	obj->Initialize("cube");
	obj->SetPosition({0, 0, 100});
#pragma endregion _3D初期化
}

void SampleScene::Update()
{
	BaseScene::Update();

#pragma region _3D更新
	obj->Update(camera);
#pragma endregion _3D更新

	BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
	BaseScene::Draw();

#pragma region _3D描画
	obj->Draw();
#pragma endregion _3D描画

	BaseScene::EndDraw();
}

void SampleScene::Finalize()
{
#pragma region _3D解放
	obj->Finalize();
#pragma endregion _3D解放

	BaseScene::Finalize();
}