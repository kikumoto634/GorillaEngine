#pragma once
#include "Input.h"
#include "WorldTransform.h"

class DebugCamera
{
public:
	static DebugCamera* GetInstance();

	void Initialize();

	void Update();

	//Getter
	WorldTransform GetWorld()	{return world_;}

private:
	Input* input = nullptr;
	WorldTransform world_;

	//マウスクリック場所
	Vector2 mousePos = {};
	Vector2 mouseRot = {};
};

