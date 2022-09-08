#pragma once
#include "GameSceneBase.h"

class Window;
class GameScene01 : public GameSceneBase
{
public:
	GameScene01(Window* window);
	~GameScene01();
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

