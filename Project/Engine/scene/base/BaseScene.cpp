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

	//画像
	sp = new BaseSprites();
	sp->Initialize(white1x1_tex.number);
	sp->SetPosition({100,100});
	sp->SetAnchorPoint({0.5f,0.5f});
	sp->SetSize({100,100});


	//モデル
	model = new ObjModelManager();
	model->CreateModel("GroundBlock");

	//オブジェクト
	for(int i = 0; i < num; i++){
		obj[i] = new BaseObjObject();
		obj[i]->Initialize(model);
	}

	//パーティクル
	particleObj = new ParticleObject();
	particleObj->Initialize();


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
	for(int i = 0; i < num; i++){
		obj[i]->Update(camera);
	}

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
		ImGui::Begin(obj[0]->GetName());
		obj[0]->SetPosition(imgui->ImGuiDragVector3("Pos", obj[0]->GetPosition(),0.1f));
		obj[0]->SetRotation(imgui->ImGuiDragVector3("Rot", obj[0]->GetRotation(), 0.1f));
		obj[0]->SetScale(imgui->ImGuiDragVector3("Sca", obj[0]->GetScale(),0.1f));
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
		ImGui::SetNextWindowSize(ImVec2{300,125});
		ImGui::Begin(camera->GetName());
		camera->SetPosition(imgui->ImGuiDragVector3("Pos", camera->GetPosition(), 0.1f));
		camera->SetRotation(imgui->ImGuiDragVector3("Rot", camera->GetRotation(), 0.1f));
		if(ImGui::Button("Shake")) camera->ShakeStart();
		Vector2 temp = imgui->ImGuiDragVector2("frame/power ", {camera->GetShakeMaxFrame(), (float)camera->GetShakeMaxPower()}, 0.1f, 2.f);
		camera->SetShake(temp.x,(int)temp.y);
		ImGui::End();
	}

	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{00,385});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,300});
		ImGui::Begin("Particle");

		num_ = imgui->ImGuiDrawInt("Num", &num_, 1);
		particle.life = imgui->ImGuiDrawInt("Life", &particle.life, 1);

		particle.position = (imgui->ImGuiDragVector3("Pos", particle.position, 0.1f));

		MinSize_ = (imgui->ImGuiDragFloat("MinSize", &MinSize_, 0.001f));
		MaxSize_ = (imgui->ImGuiDragFloat("MaxSize", &MaxSize_, 0.001f));

		MinVel_ = (imgui->ImGuiDragVector3("MinVel", MinVel_, 0.001f));
		MaxVel_ = (imgui->ImGuiDragVector3("MaxVel", MaxVel_, 0.001f));
		
		MinAcc_ = (imgui->ImGuiDragVector3("MinAcc", MinAcc_, 0.001f));
		MaxAcc_ = (imgui->ImGuiDragVector3("MaxAcc", MaxAcc_, 0.001f));

		if(ImGui::Button("App")){
			for(int i = 0; i < num_; i++){
				//速度
				particle.velocity.x = MinVel_.x + (float)rand() * (MaxVel_.x-MinVel_.x) / RAND_MAX;
				particle.velocity.y = MinVel_.y + (float)rand() * (MaxVel_.y-MinVel_.y) / RAND_MAX;
				particle.velocity.z = MinVel_.z + (float)rand() * (MaxVel_.z-MinVel_.z) / RAND_MAX;
				//加速度
				particle.accel.x = MinAcc_.x + (float)rand() * (MaxAcc_.x-MinAcc_.x) / RAND_MAX;
				particle.accel.y = MinAcc_.y + (float)rand() * (MaxAcc_.y-MinAcc_.y) / RAND_MAX;
				particle.accel.z = MinAcc_.z + (float)rand() * (MaxAcc_.z-MinAcc_.z) / RAND_MAX;

				//サイズ
				particle.start_scale = MinSize_ + (float)rand() * (MaxSize_ - MinSize_) /RAND_MAX;
				particle.end_scale = MaxSize_;

				particleObj->ParticleSet(particle);
				particleObj->ParticleAppearance();
			}
		}

		ImGui::End();
	}

#endif // _DEBUG
}

void BaseScene::EndUpdate()
{
	particleObj->Update(camera);

	camera->Update();
}

void BaseScene::Draw()
{
	for(int i = 0; i < num; i++){
		obj[i]->Draw();
	}

	particleObj->Draw();
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

	for(int i = 0; i < num; i++){
		obj[i]->Finalize();
		delete obj[i];
	}

	particleObj->Finalize();
}
