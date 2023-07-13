#include "SkeltonStateManager.h"
#include "SkeltonState.h"
#include "Skelton.h"

SkeltonStateManager::~SkeltonStateManager()
{
	delete state_;
}

void SkeltonStateManager::Update(Skelton* skelton)
{
	if(nextState_){
		if(state_){
			delete state_;
		}
		state_ = nextState_;

		nextState_ = nullptr;

		state_->SetStateManager(this);
		state_->Initialize(skelton);
		state_->UpdateTrigger();
	}

	state_->Update();
}

void SkeltonStateManager::ParticleDraw()
{
	state_->ParticleDraw();
}
