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

	sp = new BaseSprites();
	sp->Initialize(white1x1_tex.number);
	sp->SetPosition({100,100});
	sp->SetAnchorPoint({0.5f,0.5f});
	sp->SetSize({100,100});


	obj = new BaseObjObject();
	obj->Initialize("GroundBlock");
	obj->SetPosition({0,10,0});
	obj->CaveLightOn();
}

void BaseScene::Update()
{
	//入力情報更新
	input->Update();

	sp->Update();
	obj->Update(camera);

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

	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{0,100});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,55});
		ImGui::Begin("3DObj");
		float tempPos[3] = {obj->GetPosition().x,obj->GetPosition().y,obj->GetPosition().z};
		ImGui::SliderFloat3("Pos", tempPos, -100,100);
		obj->SetPosition({tempPos[0],tempPos[1],tempPos[2]});
		ImGui::End();
	}

	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{0,160});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,55});
		ImGui::Begin("Camera");
		float tempPos[3] = {camera->GetEye().x,camera->GetEye().y,camera->GetEye().z};
		ImGui::SliderFloat3("Pos", tempPos, -100,100);
		camera->SetEye({tempPos[0],tempPos[1],tempPos[2]});
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
	obj->Draw();
}

void BaseScene::DrawBack()
{
}

void BaseScene::DrawNear()
{
	Sprite::SetPipelineState();
	sp->Draw();
}

void BaseScene::Finalize()
{
	sp->Finalize();
	delete sp;

	obj->Finalize();
	delete obj;
}
