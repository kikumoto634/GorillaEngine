#pragma once

//前方宣言
class PlayerState;
class Player;

//状態遷移クラス
class PlayerStateManager{
public:
	~PlayerStateManager();

	static PlayerStateManager* GetInstance();
	static void Delete();

	void SetNextState(PlayerState* nextState)	{nextState_ = nextState;}

	void Update(Player* player);

private:
	static PlayerStateManager* instance;

	PlayerState* state_ = nullptr;
	PlayerState* nextState_ = nullptr;
};
