#include "GameScene01.h"

GameScene01::GameScene01(Window *window)
{
	this->pWindow = window;
	nowScene = "Area01";
	nextScene = "Area02";
}

GameScene01::~GameScene01()
{
	delete sprite;
}

void GameScene01::Initialize()
{
	GameSceneBase::Initialize();

	isEnd = false;

	sprite = Sprite::Create(0, Vector2(100.f,100.f));
}

void GameScene01::Update()
{
}

void GameScene01::Draw()
{
	GameSceneBase::Draw();

	sprite->Draw();
}
