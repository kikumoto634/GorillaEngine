#pragma once
#include "GameSceneBase.h"
#include "Sprite.h"

class Window;
class GameScene01 : public GameSceneBase
{
public:
	GameScene01(Window* window);
	~GameScene01();
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Sprite* sprite = nullptr;
};

