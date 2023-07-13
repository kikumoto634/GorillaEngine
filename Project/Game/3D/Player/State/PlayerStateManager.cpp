#include "PlayerStateManager.h"
#include "PlayerState.h"
#include "Player.h"


//静的
PlayerStateManager* PlayerStateManager::instance = nullptr;

PlayerStateManager::~PlayerStateManager()
{
	delete state_;
}

PlayerStateManager* PlayerStateManager::GetInstance()
{
	if(!instance)
	{
		instance = new PlayerStateManager();
	}
	return instance;
}

void PlayerStateManager::Delete()
{
	if(instance){
		delete instance;
		instance = nullptr;
	}
}

void PlayerStateManager::Update(Player* player)
{
	if(nextState_){
		if(state_){
			delete state_;
		}
		state_ = nextState_;

		nextState_ = nullptr;

		state_->SetStateManager(this);
		state_->Initialize(player);
		state_->UpdateTrigger();
	}

	state_->Update();
}