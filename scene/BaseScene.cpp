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
	camera->Initialize(window);
}

void BaseScene::Update()
{
	//入力情報更新
	input->Update();

#ifdef _DEBUG
	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{0,40});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,55});
		ImGui::Begin("Demo");
		//デモウィンドウ 
		ImGui::Checkbox("demoWindow", &show_demo_window);
		//フラグによる出現物
		if(show_demo_window)	ImGui::ShowDemoWindow(&show_demo_window);
		ImGui::End();
	}
#endif // _DEBUG
}

void BaseScene::Draw()
{
}

void BaseScene::Finalize()
{
}

void BaseScene::NextSceneChange()
{
}

void BaseScene::EndUpdate()
{
#pragma region 汎用機能更新
	//カメラ
	camera->Update();
#pragma endregion
}
