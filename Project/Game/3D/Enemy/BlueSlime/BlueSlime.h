#pragma once
#include "BaseEnemy.h"

//前方宣言
class BlueSlimeStateManager;

class BlueSlime : public BaseEnemy
{
public:
	//フレンド関数
	friend class IdelBlueSlimeState;
	friend class DeadBlueSlimeState;
	friend class PopBlueSlimeState;

public:

	void AddInitialize() override;
	void AddUpdate() override;
	void AddDraw() override;
	void AddParticleDraw() override;
	void AddFinalize() override;

	void AddContactUpdate() override;

	//Getter
	inline Vector3 GetPopPosition() override {return particlePos_;}

private:
	//状態
	BlueSlimeStateManager* state_ = nullptr;

};