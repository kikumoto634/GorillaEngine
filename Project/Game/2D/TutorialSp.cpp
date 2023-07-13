#include "TutorialSp.h"

void TutorialSp::Update()
{
	Vector3 lpos = PlayerPos - targetPos;
	distance = lpos.length();

	//距離に応じたハイド
	if(-DrawingRange_Not <= distance && distance <= DrawingRange_Not) IsHide = false;
	else if(-DrawingRange_Not > distance || distance > DrawingRange_Not) IsHide = true;

	if(IsHide) return;
	BaseSprites::Update();
}

void TutorialSp::Draw()
{
	if(IsHide) return;
	BaseSprites::Draw();
}
