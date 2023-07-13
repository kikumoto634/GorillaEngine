#include "BlueSlimeStateManager.h"
#include "BlueSlimeState.h"
#include "BlueSlime.h"


BlueSlimeStateManager::~BlueSlimeStateManager()
{
	delete state_;
}

void BlueSlimeStateManager::Update(BlueSlime* blueSlime_)
{
	if(nextState_){
		if(state_){
			delete state_;
		}
		state_ = nextState_;

		nextState_ = nullptr;

		state_->SetStateManager(this);
		state_->Initialize(blueSlime_);
		state_->UpdateTrigger();
	}

	state_->Update();
}

void BlueSlimeStateManager::ParticleDraw()
{
	state_->ParticleDraw();
}
