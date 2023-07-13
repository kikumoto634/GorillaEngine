#include "BlueSlimeState.h"
#include "BlueSlimeStateManager.h"
#include "BlueSlime.h"

#include <cassert>

void BlueSlimeState::Initialize(BlueSlime *blueSlime)
{
	assert(blueSlime);
	blueSlime_ = blueSlime;
}



void IdelBlueSlimeState::UpdateTrigger()
{
}

void IdelBlueSlimeState::Update()
{
	//死亡時
	if(!blueSlime_->isAlive_){
		stateManager_->SetNextState(new DeadBlueSlimeState);
	}
}

void IdelBlueSlimeState::ParticleDraw()
{
}




void DeadBlueSlimeState::UpdateTrigger()
{
	blueSlime_->particlePos_ = blueSlime_->GetPosition();
	blueSlime_->SetPosition(blueSlime_->NotAlivePos);
	blueSlime_->world_.UpdateMatrix();
	blueSlime_->baseCollider_->Update();

	blueSlime_->isContactTrigger_ = true;

	App();
}

void DeadBlueSlimeState::Update()
{
	//更新処理
	blueSlime_->deadParticle_->Update(blueSlime_->camera_);
	blueSlime_->particleAliveFrame_++;


	if(blueSlime_->particleAliveFrame_ < ParticleAliveFrameMax) return;

	blueSlime_->isPopsPosibble_ = true;	
	blueSlime_->particleAliveFrame_ = 0;

	blueSlime_->world_.UpdateMatrix();
	stateManager_->SetNextState(new PopBlueSlimeState);
}

void DeadBlueSlimeState::ParticleDraw()
{
	blueSlime_->deadParticle_->Draw();
}

void DeadBlueSlimeState::App()
{
	for (int i = 0; i < CreateNum; i++) {
		//速度
		vel.x = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel_Half;
		vel.y = VelY;
		vel.z = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel_Half;

		//加速度
		acc.y = AccY;

		//生成
		blueSlime_->deadParticle_->ParticleSet(
			ParticleAliveFrameMax,
			blueSlime_->particlePos_,
			vel,
			acc,
			SizeStart,
			SizeEnd,
			TextureNumber,
			Color
		);
		blueSlime_->deadParticle_->ParticleAppearance();
	}
}




void PopBlueSlimeState::UpdateTrigger()
{
}

void PopBlueSlimeState::Update()
{
	if(blueSlime_->isPopsPosibble_) return;
	//更新処理
	App();
	blueSlime_->popParticle_->Update(blueSlime_->camera_);
	blueSlime_->particleAliveFrame_++;


	if(blueSlime_->particleAliveFrame_ < ParticleCreateFrameMax) return;

	blueSlime_->isAlive_ = true;
	blueSlime_->world_.translation = blueSlime_->particlePos_;
	blueSlime_->particleAliveFrame_ = 0;

	blueSlime_->world_.UpdateMatrix();
	stateManager_->SetNextState(new IdelBlueSlimeState);
}

void PopBlueSlimeState::ParticleDraw()
{
	if(blueSlime_->particleAliveFrame_ < ParticleCreateWaitFrameMax) return;
	blueSlime_->popParticle_->Draw();
}

void PopBlueSlimeState::App()
{
	for (int i = 0; i < CreateNum; i++) {

		//座標
		pos.x = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos_Half;
		pos.y = PosY;
		pos.z = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos_Half;
		pos += blueSlime_->particlePos_;

		//速度
		vel.x = (float)rand()/RAND_MAX*Rand_Vel - Rand_Vel_Half;
		vel.y = VelY;
		vel.z = (float)rand()/RAND_MAX*Rand_Vel - Rand_Vel_Half;

		//加速度
		acc.y = -(float)rand()/RAND_MAX*Rand_Acc;

		//生成
		blueSlime_->popParticle_->ParticleSet(
			ParticleAliveFrameMax,
			pos,
			vel,
			acc,
			SizeStart,
			SizeEnd,
			TextureNumber,
			Color
		);
		blueSlime_->popParticle_->ParticleAppearance();
	}
}
