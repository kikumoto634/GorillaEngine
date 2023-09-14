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
	camera = FollowCamera::GetInstance();
	postEffect = PostEffect::GetInstance();
	postEffect = PostEffect::Create(white1x1_tex.number, {0,0}, {500,500});
}

void BaseScene::Initialize()
{
	//Input初期化
	input->Initialize();

	//カメラ
	camera->Initialize();

	isDrawStop = false;

	//画像
	sp = new BaseSprites();
	sp->Initialize(white1x1_tex.number);
	sp->SetPosition({100,100});
	sp->SetAnchorPoint({0.5f,0.5f});
	sp->SetSize({100,100});

	//オブジェクト
	player = new Action();
	player->Initialize();

	//Skydome
	skydome = new BaseObjObject();
	skydome->Initialize("skydome");


	//ライト
	lightGroup_ = LightGroup::Create();
	//色設定
	lightGroup_->SetAmbientColor(LightColor);
	//3Dオブジェクト(.obj)にセット
	ObjModelObject::SetLight(lightGroup_);

	lightGroup_->SetDirLightActive(0, true);
	lightGroup_->SetDirLightActive(1, true);
	lightGroup_->SetDirLightActive(2, true);

#ifdef _DEBUG
	debugCamera = DebugCamera::GetInstance();
	debugCamera->Initialize();
	isDebugCamera = true;
#endif // _DEBUG

}

void BaseScene::Update()
{
	//入力情報更新
	input->Update();

	sp->Update();

	camera->Update(player->GetmatWorld());
	player->Update(camera);

	skydome->Update(camera);

#ifdef _DEBUG
	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{0,0});
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
		ImGui::SetNextWindowPos(ImVec2{0,60});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,100});
		ImGui::Begin(player->GetName());
		player->SetPosition(imgui->ImGuiDragVector3("Pos", player->GetPosition(),0.1f));
		player->SetRotation(imgui->ImGuiDragVector3("Rot", player->GetRotation(), 0.1f));
		player->SetScale(imgui->ImGuiDragVector3("Sca", player->GetScale(),0.1f));
		ImGui::End();
	}

	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{000,160});
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
		ImGui::SetNextWindowPos(ImVec2{0,260});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,140});
		ImGui::Begin(camera->GetName());
		camera->SetPosition(imgui->ImGuiDragVector3("Pos", camera->GetPosition(), 0.1f));
		camera->SetRotation(imgui->ImGuiDragVector3("Rot", camera->GetRotation(), 0.1f));
		if(ImGui::Button("Shake")) camera->ShakeStart();
		Vector2 temp = imgui->ImGuiDragVector2("frame/power ", {camera->GetShakeMaxFrame(), (float)camera->GetShakeMaxPower()}, 0.1f, 2.f);
		camera->SetShake(temp.x,(int)temp.y);
		ImGui::Checkbox("Debug", &isDebugCamera);
		ImGui::End();
	}


	//デバックカメラ
	if(isDebugCamera){
		debugCamera->Update();
		camera->SetWorld(debugCamera->GetWorld());
	}
#endif // _DEBUG
}

void BaseScene::EndUpdate()
{
}

void BaseScene::Draw()
{
	player->Draw();
	skydome->Draw();
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

	player->Finalize();
	delete player;

	skydome->Finalize();
	delete skydome;
}
