#pragma once
#include "BaseSprites.h"

class TutorialSp : public BaseSprites
{
private:
	//描画範囲(非表示)
	const int DrawingRange_Not = 13;

public:
	void Update() override;
	void Draw() override;

	void SpriteHide()	{IsHide = true, Draw();}

	//Setter
	inline void SetPlayerPos(Vector3 pos)	{PlayerPos = pos;}

private:
	float distance = 0.f;
	Vector3 PlayerPos ={};
	bool IsHide = false;
};

