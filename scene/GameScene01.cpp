#include "GameScene01.h"

GameScene01::GameScene01(Window *window)
{
	this->pWindow = window;
	nowScene = "Area01";
	nextScene = "Area02";
}

GameScene01::~GameScene01()
{
}

void GameScene01::Initialize()
{
	isEnd = false;
}

void GameScene01::Update()
{
}

void GameScene01::Draw()
{
}
