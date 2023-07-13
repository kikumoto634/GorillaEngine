#pragma once

class BossState;
class Boss1;

class BossStateManager
{
public:
	~BossStateManager();

	void SetNextState(BossState* nextState)	{nextState_ = nextState;}

	void Update(Boss1* boss_);

	void ParticleDraw();

private:
	BossState* state_ = nullptr;
	BossState* nextState_ = nullptr;
};

