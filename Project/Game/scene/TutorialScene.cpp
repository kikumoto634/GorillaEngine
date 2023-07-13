#include "TutorialScene.h"

#include "TitleScene.h"
#include "HomeScene.h"
#include "SceneManager.h"

using namespace std;

TutorialScene::~TutorialScene()
{
	Finalize();
}

TutorialScene::TutorialScene(DirectXCommon *dxCommon, Window *window, int saveHP)
	:BaseBattleScene(
		dxCommon,
		window)
{
}

void TutorialScene::NextSceneChange()
{
	sceneManager->SetNextScene(new HomeScene(dxCommon,window));
}

void TutorialScene::AddCommonInitialize()
{
	//ダンジョン
	areaManager_->CSVAreaInitialize("Tutorial");
}

void TutorialScene::AddObject3DInitialize()
{
	ActorCreateInitialize();
}

void TutorialScene::AddObject2DInitialize()
{
	{
		moveSp_ = make_unique<TutorialSp>();
		moveSp_->Initialize(tutorialText1_tex.number);
		Vector3 ltarget = MoveSpPos_;
		Vector2 lpos = moveSp_->ChangeTransformation(ltarget, this->camera);
		moveSp_->SetPosition(lpos);
		moveSp_->SetSize(MoveSpSize);
		moveSp_->SetAnchorPoint(SpAnc);
	}
	
	{
		buttonSpNumber = buttonCross_tex.number;
		buttonSp_ = make_unique<TutorialSp>();
		buttonSp_->Initialize(buttonSpNumber);
		Vector3 ltarget = ButtonSpPos;
		Vector2 lpos = buttonSp_->ChangeTransformation(ltarget, this->camera);
		buttonSp_->SetPosition(lpos);
		buttonSp_->SetAnchorPoint(SpAnc);
	}
}

void TutorialScene::AddCommonUpdate()
{
	areaManager_->CSVAreaUpdate(camera, player_->GetPosition());
}

void TutorialScene::AddObject3DUpdate()
{
	for(auto it = enemy_.begin(); it != enemy_.end(); it++){
		if((*it)->GetIsContactTrigger()){
			gameManager_->AudioPlay(damage_audio.number, damage_audio.volume);
			coinDropPos_.push((*it)->GetPopPosition());
		}
		(*it)->Update(camera,player_->GetPosition());
	}

	for(auto it = obj_.begin(); it != obj_.end(); it++){
		if((*it)->GetIsContactTrigger()){
			gameManager_->CoinIncrement();
			gameManager_->AudioPlay(coinGet_audio.number,coinGet_audio.volume);
		}
		if(!coinDropPos_.empty() && (*it)->GetIsPopsPmposibble_()){
			(*it)->Pop(coinDropPos_.front());
			coinDropPos_.pop();
		}

		(*it)->Update(this->camera);
	}
}

void TutorialScene::AddObject2DUpdate()
{
	{
		Vector3 ltarget = MoveSpPos_;
		Vector2 lpos = moveSp_->ChangeTransformation(ltarget, this->camera);
		moveSp_->SetPosition(lpos);
		moveSp_->SetPlayerPos(player_->GetPosition());
		moveSp_->Update();
	}
	{
		//コントローラ接続確認
		buttonSpNumber = input->GetIsPadConnect()==true ? PadLStick_tex.number : buttonCross_tex.number;
		buttonSp_->SetTexNumber(buttonSpNumber);

		Vector3 ltarget = ButtonSpPos;
		Vector2 lpos = buttonSp_->ChangeTransformation(ltarget, this->camera);
		buttonSp_->SetPosition(lpos);
		buttonSp_->SetPlayerPos(player_->GetPosition());
		buttonSp_->Update();
	}
}

void TutorialScene::AddBeatEndUpdate()
{
	int index = 0;
	for(auto it = enemy_.begin(); it != enemy_.end(); it++){
		(*it)->IsBeatEndOn();
		if((*it)->GetIsPopsPmposibble_()){
			Vector3 lpos;

			if(areaManager_->GetCSVObjectPopActive(index,false)) {
				lpos = areaManager_->GetCSVObjectPopPosition(index);
				(*it)->Pop(lpos);
			}
			index++;
		}
	}

	for(auto it = obj_.begin(); it != obj_.end(); it++){
		if((*it)->GetIsAlive()){
			(*it)->IsBeatEndOn();
		}
	}
}

void TutorialScene::AddObject3DDraw()
{
	areaManager_->CSVAreaDraw();

	for(auto it = enemy_.begin(); it != enemy_.end(); it++){
		(*it)->Draw();
	}
	for(auto it = obj_.begin(); it != obj_.end(); it++){
		(*it)->Draw();
	}
}

void TutorialScene::AddParticleDraw()
{
	for(auto it = enemy_.begin(); it != enemy_.end(); it++){
		(*it)->ParticleDraw();
	}
}

void TutorialScene::AddFrontUIDraw()
{
	moveSp_->Draw();
	buttonSp_->Draw();
	/*attackSp->Draw();*/
}

void TutorialScene::AddBackUIDraw()
{
}

void TutorialScene::AddObjectFinalize()
{
	//attackSp->Finalize();
	moveSp_->Finalize();
	buttonSp_->Finalize();

	for(auto it = enemy_.begin(); it != enemy_.end(); it++){
		(*it)->Finalize();
	}

	for(auto it = obj_.begin(); it != obj_.end(); it++){
		(*it)->Finalize();
	}
}

void TutorialScene::AddCommonFinalize()
{
	areaManager_->CSVAreaFinalize();
}


void TutorialScene::ActorCreateInitialize()
{
	for(int i = 0; i < SlimePopNumMax_; i++){
		unique_ptr<BlueSlime> newObj = make_unique<BlueSlime>();
		newObj->Initialize("slime");
		enemy_.push_back(move(newObj));
	}

	for(int i = 0; i < CoinPopNumMax_; i++){
		unique_ptr<Coins> newObj = make_unique<Coins>();
		newObj->Initialize("Coins");
		obj_.push_back(move(newObj));
	}
}
