#include "BlueSlime.h"
#include "BlueSlimeStateManager.h"
#include "BlueSlimeState.h"

void BlueSlime::AddInitialize()
{
	state_ = new BlueSlimeStateManager();
	//state_->SetNextState(new IdelSkeltonState);
	state_->SetNextState(new PopBlueSlimeState);
}

void BlueSlime::AddUpdate()
{
	state_->Update(this);
}

void BlueSlime::AddDraw()
{
}

void BlueSlime::AddParticleDraw()
{
	state_->ParticleDraw();
}

void BlueSlime::AddFinalize()
{
	delete state_;
	state_ = nullptr;
}

void BlueSlime::AddContactUpdate()
{
	isAlive_ = false;
}
