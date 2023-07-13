#pragma once
#include "Vector3.h"

//前方宣言
class PlayerStateManager;
class Player;

//状態
class PlayerState{
public:
	virtual void SetStateManager(PlayerStateManager* stateManager)	{stateManager_ = stateManager;}

	void Initialize(Player* player);

	virtual void UpdateTrigger() = 0;
	virtual void Update() = 0;

protected:
	//借り物
	PlayerStateManager* stateManager_ = nullptr;
	Player* player_ = nullptr;
};

//通常
class IdelPlayerState   : public PlayerState{
private:
	void UpdateTrigger() override;
	void Update() override;
};

//移動
class MovePlayerState   : public PlayerState{
private:
	//移動時の時間
	const float EasingMoveTimeMax = 0.075f;

private:
	void UpdateTrigger() override;
	void Update() override;

private:
	//移動
	//イージングの移動開始座標
	Vector3 easigStartPos_ = {};
	//イージングの移動終了座標
	Vector3 easingEndPos_ = {};
	//イージングの移動タイム
	float easingMoveTime_ = 0;
};

//攻撃
class AttackPlayerState : public PlayerState{
private:
	void UpdateTrigger() override;
	void Update() override;
};

//掘る
class DigPlayerState    : public PlayerState{
private:
	void UpdateTrigger() override;
	void Update() override;
};

//死亡
class DeadPlayerState   : public PlayerState{
private:
	void UpdateTrigger() override;
	void Update() override;
};