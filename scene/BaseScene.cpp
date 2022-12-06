#include "BaseScene.h"

using namespace std;

BaseScene::BaseScene(DirectXCommon* dxCommon, Window* window)
{
	this->dxCommon = dxCommon;
	this->window = window;
}

void BaseScene::Application()
{
	input = Input::GetInstance();
	camera = Camera::GetInstance();
}

void BaseScene::Initialize()
{
	//Input初期化
	input->Initialize(window->GetHwnd());

	//カメラ
	camera->Initialize();

#ifdef _DEBUG
	debugText = std::make_unique<DebugText>();
	debugText->Initialize(0);
#endif // _DEBUG
}

void BaseScene::Update()
{
	//入力情報更新
	input->Update();
}

void BaseScene::EndUpdate()
{
#ifdef _DEBUG

	//シーン遷移
	if(input->Trigger(DIK_SPACE)){
		IsSceneChange = true;
	}
#endif // _DEBUG

#pragma region 汎用機能更新
	//カメラ
	camera->Update();
#pragma endregion
}

void BaseScene::Draw()
{
}

void BaseScene::EndDraw()
{
#ifdef _DEBUG
	debugText->DrawAll();
#endif // _DEBUG
}

void BaseScene::Finalize()
{
	window->Finalize();
}
