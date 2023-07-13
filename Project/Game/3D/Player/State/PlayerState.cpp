#include "PlayerState.h"
#include "PlayerStateManager.h"
#include "Easing.h"
#include "Player.h"

#include <cassert>

void PlayerState::Initialize(Player *player)
{
	assert(player);
	player_ = player;
}


void IdelPlayerState::UpdateTrigger()
{
}

void IdelPlayerState::Update()
{
	if(player_->isDamage_) return;
}


void MovePlayerState::UpdateTrigger()
{
	easigStartPos_ = player_->world_.translation;
	easingEndPos_ = player_->world_.translation + player_->addVector3_;
}

void MovePlayerState::Update()
{
	//移動処理
	player_->world_.translation = Easing_Point2_Linear(easigStartPos_, easingEndPos_, Time_OneWay(easingMoveTime_, EasingMoveTimeMax));

	//モデル変更
	player_->object_->SetModel(player_->model_);

	//移動完了時
	if(easingMoveTime_ >= 1.0f){
		player_->world_.translation = easingEndPos_;
		easigStartPos_ = {};
		easingEndPos_ = {};
		easingMoveTime_ = 0.f;
		
		stateManager_->SetNextState(new IdelPlayerState);
	}
}


void AttackPlayerState::UpdateTrigger()
{
}

void AttackPlayerState::Update()
{
	player_->weapon_->Attack();
	//モデル変更
	player_->object_->SetModel(player_->attackModel_);

	stateManager_->SetNextState(new IdelPlayerState);
}


void DigPlayerState::UpdateTrigger()
{
}

void DigPlayerState::Update()
{
	player_->weapon_->Attack();
	//モデル変更
	player_->object_->SetModel(player_->attackModel_);

	stateManager_->SetNextState(new IdelPlayerState);
}


void DeadPlayerState::UpdateTrigger()
{
	player_->ColliderRemove();
}

void DeadPlayerState::Update()
{
	//初回のみ
	if(!player_->isAlive_) player_->damageFrame_ ++;
	player_->isAlive_ = false;

	player_->object_->SetModel(player_->deadModel_);
}
