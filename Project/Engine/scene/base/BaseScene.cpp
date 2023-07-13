#include "BaseScene.h"
#include "TextureUtility.h"

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
	postEffect = PostEffect::GetInstance();
	postEffect = PostEffect::Create(white1x1_tex.number, {0,0}, {500,500});
}

void BaseScene::Initialize()
{
	//Input初期化
	input->Initialize(window->GetHwnd());

	//カメラ
	camera->Initialize(window);

	isDrawStop = false;
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

void BaseScene::EndUpdate()
{
#pragma region 汎用機能更新
	//カメラ
	camera->Update();
#pragma endregion
}

void BaseScene::Draw()
{
}

void BaseScene::DrawBack()
{
}

void BaseScene::DrawNear()
{
}

void BaseScene::Finalize()
{
}
