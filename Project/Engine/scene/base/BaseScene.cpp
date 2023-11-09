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

	GPUParticleManager::StaticInitialize();
	particle = new GPUParticleManager();
	particle = GPUParticleManager::Create();

	world.Initialize();

	//ライト
	lightGroup_ = LightGroup::Create();
	//3Dオブジェクト(.obj)にセット
	ObjModelObject::SetLight(lightGroup_);
	//lightGroup_->CircleShadowSet(0,player->GetPosition());


#pragma region シャドウマップ初期化
	//レンダリングターゲット
	float clearColor[4] = {1.0f,1.0f,1.0f,1.0f};


#pragma endregion


#ifdef _DEBUG
	debugCamera = DebugCamera::GetInstance();
	debugCamera->Initialize();
#endif // _DEBUG

	levelData = LevelLoader::Load("levelSample");

	//レベルデータからオブジェクトを生成
	for(auto& objectData : levelData->objects){
		//ファイル名からモデルを生成
		ObjModelManager* model_ = new ObjModelManager();
		model_->CreateModel(objectData.fileName);

		//モデルを指定して3Dオブジェクトを生成
		BaseObjObject* newObj_ = new BaseObjObject();
		newObj_->Initialize(model_);

		//座標
		newObj_->SetPosition(objectData.translation);
		//回転
		newObj_->SetRotation(objectData.rotation);
		//スケール
		newObj_->SetScale(objectData.scaling);

		//配列に登録
		objects.push_back(newObj_);
	}

}

void BaseScene::Update()
{
	//入力情報更新
	input->Update();

	sp->Update();

	camera->Update();
#ifdef _DEBUG
	//デバックカメラ
	if(isDebugCamera){
		debugCamera->Update();
		camera->SetWorld(debugCamera->GetWorld());
	}
#endif // _DEBUG


	player->Update(camera);
	particle->Update(world,camera);

	for(auto& object : objects){
		object->Update(camera);
	}

	//lightGroup_->SetCircleShadowCasterPos(0, player->GetPosition());
	lightGroup_->Update();

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

	lightGroup_->DebugUpdate();

	{
		debugText->Printf3D(player->GetPosition(), 1.0f, camera, "T X:%f Y:%f Z:%f",player->GetPosition().x,player->GetPosition().y,player->GetPosition().z);
	}
#endif // _DEBUG
}

void BaseScene::EndUpdate()
{
}

void BaseScene::Draw()
{
	for(auto& object : objects){
		object->Draw();
	}

	player->Draw();

	GPUParticleManager::SetPipelineState();
	//particle->Draw();
}

void BaseScene::DrawBack()
{
}

void BaseScene::DrawNear()
{
	Sprite::SetPipelineState();
	//sp->Draw();
}

void BaseScene::Finalize()
{
	for(auto& object : objects){
		object->Finalize();
		delete object;
	}

	sp->Finalize();
	delete sp;

	player->Finalize();
	delete player;

	GPUParticleManager::StaticFinalize();
	delete particle;
}
