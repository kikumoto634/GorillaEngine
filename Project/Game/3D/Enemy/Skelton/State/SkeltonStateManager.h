#pragma once

//前方宣言
class SkeltonState;
class Skelton;

class SkeltonStateManager
{
public:
	~SkeltonStateManager();

	void SetNextState(SkeltonState* nextState)	{nextState_ = nextState;}

	void Update(Skelton* skelton);

	void ParticleDraw();

private:
	SkeltonState* state_ = nullptr;
	SkeltonState* nextState_ = nullptr;
};

