#include "HomeScene.h"

#include "TitleScene.h"
#include "GameScene.h"
#include "SceneManager.h"

#include "BottonHigh.h"
#include "BottonLow.h"

using namespace std;

HomeScene::~HomeScene()
{
	Finalize();
}

HomeScene::HomeScene(DirectXCommon *dxCommon, Window *window, int saveHP)
	:BaseBattleScene(
		dxCommon,
		window)
{
}

void HomeScene::NextSceneChange()
{
	sceneManager->SetNextScene(new GameScene(dxCommon,window));
}

void HomeScene::AddCommonInitialize()
{
	//ダンジョン
	areaManager_->CSVAreaInitialize("Home");

	needCoin_ = NeedCoinNum;
}

void HomeScene::AddObject3DInitialize()
{
	exit_->SetExitOpenNeedCoin(needCoin_);
	exit_->NeedCoinSpriteUpdate();

	ActorCreateInitialize();
}

void HomeScene::AddObject2DInitialize()
{
	{
		exitText_ = make_unique<TutorialSp>();
		exitText_->Initialize(goText.number);
		Vector3 ltarget = ExitTextPos;
		Vector2 lpos = exitText_->ChangeTransformation(ltarget, this->camera);
		exitText_->SetPosition(lpos);
		exitText_->SetSize(TrainingTextSize);
		exitText_->SetAnchorPoint(TraningTextAnc);
	}
	
	{
		trainingText_ = make_unique<TutorialSp>();
		trainingText_->Initialize(training_tex.number);
		Vector3 ltarget = TrainingTextPos;
		Vector2 lpos = exitText_->ChangeTransformation(ltarget, this->camera);
		trainingText_->SetPosition(lpos);
		trainingText_->SetSize(ExitTextSize);
		trainingText_->SetAnchorPoint(TrainingTextAnc);
	}
}


void HomeScene::AddCommonUpdate()
{
	areaManager_->CSVAreaUpdate(camera, player_->GetPosition());
}

void HomeScene::AddObject3DUpdate()
{
	for(auto it = slime_.begin(); it != slime_.end(); it++){
		if((*it)->GetIsContactTrigger()){
			gameManager_->AudioPlay(damage_audio.number, damage_audio.volume);
		}
		(*it)->Update(camera,player_->GetPosition());
	}

	bottonHigh_->Update(camera, player_->GetPosition());
	bottonLow_->Update(camera, player_->GetPosition());
}

void HomeScene::AddObject2DUpdate()
{
	{
		Vector3 ltarget = ExitTextPos;
		Vector2 lpos = exitText_->ChangeTransformation(ltarget, this->camera);
		exitText_->SetPosition(lpos);
		exitText_->SetPlayerPos(player_->GetPosition());
		exitText_->Update();
	}
	{
		Vector3 ltarget = TrainingTextPos;
		Vector2 lpos = trainingText_->ChangeTransformation(ltarget, this->camera);
		trainingText_->SetPosition(lpos);
		trainingText_->SetPlayerPos(player_->GetPosition());
		trainingText_->Update();
	}
}

void HomeScene::AddBeatEndUpdate()
{
	int index = 0;
	Vector3 lpos;
	for(auto it = slime_.begin(); it != slime_.end(); it++){
		(*it)->IsBeatEndOn();
		if((*it)->GetIsPopsPmposibble_()){
			if(areaManager_->GetCSVObjectPopActive(index)) {
				lpos = areaManager_->GetCSVObjectPopPosition(index);
				(*it)->Pop(lpos);
			}
			index++;
		}
	}

	bottonHigh_->IsBeatEndOn();
	bottonLow_->IsBeatEndOn();
}


void HomeScene::AddObject3DDraw()
{
	areaManager_->CSVAreaDraw();

	for(auto it = slime_.begin(); it != slime_.end(); it++){
		(*it)->Draw();
	}

	bottonHigh_->Draw();
	bottonLow_->Draw();
}

void HomeScene::AddParticleDraw()
{
	for(auto it = slime_.begin(); it != slime_.end(); it++){
		(*it)->ParticleDraw();
	}
}

void HomeScene::AddFrontUIDraw()
{
	exitText_->Draw();
	trainingText_->Draw();
}

void HomeScene::AddBackUIDraw()
{
}


void HomeScene::AddObjectFinalize()
{

	exitText_->Finalize();
	trainingText_->Finalize();

	for(auto it = slime_.begin(); it != slime_.end(); it++){
		(*it)->Finalize();
	}

	bottonHigh_->Finalize();
	bottonLow_->Finalize();
}

void HomeScene::AddCommonFinalize()
{
	areaManager_->CSVAreaFinalize();
}

void HomeScene::ActorCreateInitialize()
{
	for(int i = 0; i < SlimePopNumMax; i++){
		unique_ptr<BlueSlime> newObj = make_unique<BlueSlime>();
		newObj->Initialize("Slime");
		slime_.push_back(move(newObj));
	}

	bottonHigh_ =make_unique<BottonHigh>();
	bottonHigh_->Initialize("BottonHigh", true);
	bottonHigh_->SetPosition({-2.f,0,0});
	bottonHigh_->SetRhythmManager(rhythmManager_.get());

	bottonLow_ =make_unique<BottonLow>();
	bottonLow_->Initialize("BottonLow", true);
	bottonLow_->SetPosition({+2.f,0,0});
	bottonLow_->SetRhythmManager(rhythmManager_.get());
}
