#include "BossStateManager.h"
#include "BossState.h"
#include "Boss1.h"

BossStateManager::~BossStateManager()
{
	delete state_;
}

void BossStateManager::Update(Boss1 *boss_)
{
	if(nextState_){
		if(state_){
			delete state_;
		}
		state_ = nextState_;

		nextState_ = nullptr;

		state_->SetStateManager(this);
		state_->Initialize(boss_);
		state_->UpdateTrigger();
	}
	state_->Update();
}

void BossStateManager::ParticleDraw()
{
	state_->ParticleDraw();
}
