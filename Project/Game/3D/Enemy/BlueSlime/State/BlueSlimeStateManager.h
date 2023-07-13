#pragma once

//前方宣言
class BlueSlimeState;
class BlueSlime;

class BlueSlimeStateManager
{
public:
	~BlueSlimeStateManager();

	void SetNextState(BlueSlimeState* nextState)	{nextState_ = nextState;}

	void Update(BlueSlime* blueSlime_);

	void ParticleDraw();

private:
	BlueSlimeState* state_ = nullptr;
	BlueSlimeState* nextState_ = nullptr;
};

