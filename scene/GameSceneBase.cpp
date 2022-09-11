#include "GameSceneBase.h"

GameSceneBase::~GameSceneBase()
{
	Sprite::StaticFinalize();
}

void GameSceneBase::Initialize()
{
	//テクスチャ
	TextureManager::Load(0, "Texture.jpg");

	//スプライト
	//Sprite::StaticInitialize(dxCommon, window->GetWindowWidth(), window->GetWindowHeight());
}

void GameSceneBase::Draw()
{
	Sprite::SetPipelineState();
}
