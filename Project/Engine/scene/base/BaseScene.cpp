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
		obj->SetPosition(imgui->ImGuiDragVector3("Pos", obj->GetPosition(),0.1f));
		obj->SetRotation(imgui->ImGuiDragVector3("Rot", obj->GetRotation(), 0.1f));
		obj->SetScale(imgui->ImGuiDragVector3("Sca", obj->GetScale(),0.1f));
		ImGui::End();
	}

	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{000,200});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,100});
		ImGui::Begin(sp->GetName());
		sp->SetPosition(imgui->ImGuiDragVector2("Pos", sp->GetPosition(),1.0f, -2000.f, 2000.f));
		sp->SetSize(imgui->ImGuiDragVector2("Sca", sp->GetSize(),1.0f, 0.0f, 1280.f));
		sp->SetColor(imgui->ImGuiDragVector4("Col",sp->GetColor(), 0.01f, 0.f,1.f));
		ImGui::End();
	}

	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{0,300});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,125});
		ImGui::Begin(camera->GetName());
		camera->SetPosition(imgui->ImGuiDragVector3("Pos", camera->GetPosition(), 0.1f));
		camera->SetRotation(imgui->ImGuiDragVector3("Rot", camera->GetRotation(), 0.1f));
		if(ImGui::Button("Shake")) camera->ShakeStart();
		Vector2 temp = imgui->ImGuiDragVector2("frame/power ", {camera->GetShakeMaxFrame(), (float)camera->GetShakeMaxPower()}, 0.1f, 2.f);
		camera->SetShake(temp.x,(int)temp.y);
		ImGui::End();
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
