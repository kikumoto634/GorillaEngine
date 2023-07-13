#include "TitleScene.h"

#include "SceneManager.h"
#include "TutorialScene.h"
#include "GameScene.h"
#include "Boss1Area.h"

#include "Easing.h"
#include "AudioUtility.h"
#include "TextureUtility.h"

using namespace std;
using namespace DirectX;

TitleScene::TitleScene(DirectXCommon *dxCommon, Window *window)
	: BaseScene(
		dxCommon,
		window)
{
}

void TitleScene::Application()
{
	BaseScene::Application();
}

void TitleScene::Initialize()
{
	BaseScene::Initialize();

	ObjectInitialize();

	SpriteInitialize();

	audio_ = Audio::GetInstance();

	//ライト
	lightGroup_ = LightGroup::Create();
	//色設定
	lightGroup_->SetAmbientColor(LightColor);
	//3Dオブジェクト(.obj)にセット
	ObjModelObject::SetLight(lightGroup_);

	lightGroup_->SetDirLightActive(0, true);
	lightGroup_->SetDirLightActive(1, false);
	lightGroup_->SetDirLightActive(2, false);

	//ポストエフェクト
	postEffect->FadeStart();
}

void TitleScene::Update()
{
	BaseScene::Update();

	bool isEnter = (input->Trigger(DIK_Z) || input->Trigger(DIK_SPACE) || 
		input->PadButtonTrigger(XINPUT_GAMEPAD_A));

	if(isEnter && !isNextSceneChange_){
		isNextSceneChange_ = true;
		postEffect->FadeStart();
		audio_->PlayWave(coinGet_audio.number,coinGet_audio.volume);
	}
#pragma region _3DObj Update
	for(auto it = objs_.begin(); it != objs_.end(); ++it){
		(*it)->Update(camera);
	}
#pragma endregion

#pragma region _2DObj Update
	SceneChange();

	//コントローラ接続確認
	buttonSpNumber = input->GetIsPadConnect()==true ? PadA_tex.number : buttonZ_tex.number;
	sp_[buttonSpIndex]->SetTexNumber(buttonSpNumber);
	for(auto it = sp_.begin(); it != sp_.end(); it++){
		(*it)->Update();
	}

#pragma endregion

#pragma region Common Update
	lightGroup_->Update();
#pragma endregion

#ifdef _DEBUG
	//Scene
	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{1000,40});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{280,100});
		ImGui::Begin("SCENE");

		ImGui::Text("Now:Title   Next:Home");
		if(!isPrevSceneChange_ && ImGui::Button("NextScene")){
			isNextSceneChange_ = true;
			postEffect->FadeStart();
		}
		ImGui::Text("Now:Title   Next:Debug");
		if(!isPrevSceneChange_ && ImGui::Button("DebugScene")){
			isDebugScene_ = true;
			postEffect->FadeStart();
		}

		ImGui::End();
	}
#endif // _DEBUG


	BaseScene::EndUpdate();
}

void TitleScene::Draw()
{
	if(isDrawStop) return;

	for(auto it = objs_.begin(); it != objs_.end(); ++it){
		(*it)->Draw();
	}
}

void TitleScene::DrawBack()
{
	if(isDrawStop) return;
}

void TitleScene::DrawNear()
{
	if(isDrawStop) return;

	Sprite::SetPipelineState();
	for(auto it = sp_.begin(); it != sp_.end(); it++){
		(*it)->Draw();
	}
}


void TitleScene::Finalize()
{
	audio_->Finalize();

	for(auto it = sp_.begin(); it != sp_.end(); it++){
		(*it)->Finalize();
	}

	delete lightGroup_;
	lightGroup_ = nullptr;

	for(auto it = objs_.begin(); it != objs_.end(); it++){
		(*it)->Finalize();
	}

	BaseScene::Finalize();
}


void TitleScene::NextSceneChange()
{
	sceneManager->SetNextScene(new TutorialScene(dxCommon,window));
}

void TitleScene::DebugSceneChange()
{
	sceneManager->SetNextScene(new GameScene(dxCommon,window));
}

void TitleScene::SceneChange()
{
	//PrevSceneからの移動後処理
	if(isPrevSceneChange_){
		if(!postEffect->FadeIn()) return;
		
		isPrevSceneChange_ = false;
		return;
	}
	//NextSceneへの移動
	else if(isNextSceneChange_ || isDebugScene_){
		if(!postEffect->FadeOut()) return;

		isDrawStop = true;
		
		if(isNextSceneChange_)NextSceneChange();
		else if(isDebugScene_)DebugSceneChange();
	}
}


void TitleScene::ObjectInitialize()
{
	//プレイヤー
	unique_ptr<BaseObjObject> playerObj_ = make_unique<BaseObjObject>();
	playerObj_->Initialize("human2");
	playerObj_->SetPosition(PlayerPos);
	playerObj_->SetRotation(PlayerRot);
	playerObj_->SetScale(PlayerScale);
	objs_.push_back(move(playerObj_));

	//エネミー
	unique_ptr<BaseObjObject> enemyObj_ = make_unique<BaseObjObject>();
	enemyObj_->Initialize("slime");
	enemyObj_->SetPosition(SlimePos);
	enemyObj_->SetRotation(SlimeRot);
	enemyObj_->SetScale(SlimeScale);
	objs_.push_back(move(enemyObj_));

	unique_ptr<BaseObjObject> enemyObj2_ = make_unique<BaseObjObject>();
	enemyObj2_->Initialize("Skeleton");
	enemyObj2_->SetPosition(SkeltonPos);
	enemyObj2_->SetRotation(SkeltonRot);
	enemyObj2_->SetScale(SkeltonScale);
	objs_.push_back(move(enemyObj2_));

	unique_ptr<BaseObjObject> backObj_ = make_unique<BaseObjObject>();
	backObj_->Initialize("wall");
	backObj_->SetPosition(WallBackPos);
	objs_.push_back(move(backObj_));
}

void TitleScene::SpriteInitialize()
{
	unique_ptr<BaseSprites> title_ = make_unique<BaseSprites>();
	title_->Initialize(titleLogo_tex.number);
	title_->SetPosition(TitlePos);
	title_->SetSize(TitleSize);
	title_->SetAnchorPoint(TitleAnc);
	sp_.push_back(move(title_));

	unique_ptr<BaseSprites> pushText_ = make_unique<BaseSprites>();
	pushText_->Initialize(pushText_tex.number);
	pushText_->SetPosition(PushTextPos);
	pushText_->SetSize(PushTextSize);
	pushText_->SetAnchorPoint(PushtextAnc);
	sp_.push_back(move(pushText_));

	buttonSpNumber = buttonZ_tex.number;
	unique_ptr<BaseSprites> button_ = make_unique<BaseSprites>();
	button_->Initialize(buttonSpNumber);
	button_->SetPosition(ButtonPos);
	button_->SetSize(ButtonSize);
	button_->SetAnchorPoint(ButtonAnc);
	sp_.push_back(move(button_));
}
