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


	model = new ObjModelManager();
	model->CreateModel("GroundBlock");

	obj = new BaseObjObject();
	obj->Initialize(model);

	//ライト
	lightGroup_ = LightGroup::Create();
	//色設定
	lightGroup_->SetAmbientColor(LightColor);
	//3Dオブジェクト(.obj)にセット
	ObjModelObject::SetLight(lightGroup_);

	lightGroup_->SetDirLightActive(0, true);
	lightGroup_->SetDirLightActive(1, false);
	lightGroup_->SetDirLightActive(2, false);
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
		ImGui::SetNextWindowSize(ImVec2{300,100});
		ImGui::Begin(obj->GetName());
		obj->SetPosition(imgui->ImGuiDragVector3("Pos", obj->GetPosition()));
		obj->SetRotation(imgui->ImGuiDragVector3("Rot", obj->GetRotation()));
		obj->SetScale(imgui->ImGuiDragVector3("Sca", obj->GetScale()));
		ImGui::End();
	}

	if(input->Push(DIK_LEFT)){
		camera->RotVector({0,-1,0});
	}

#endif // _DEBUG
}

void BaseScene::EndUpdate()
{
	camera->Update();
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
