#include "SampleScene.h"

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
}

void SampleScene::Update()
{
	BaseScene::Update();

	BaseScene::EndUpdate();
}

void SampleScene::Draw()
{
	BaseScene::Draw();

	BaseScene::EndDraw();
}

void SampleScene::Finalize()
{
	BaseScene::Finalize();
}