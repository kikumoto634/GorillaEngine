#include "JudgeLocation.h"

void JudgeLocation::Initialize()
{
	BaseSprites::Initialize(SizeMinTextureNumber);
	SetAnchorPoint(AnchorPoint);
	SetPosition(Position);
	SetSize(Size_Min);
}

void JudgeLocation::Update(bool &IsInput)
{
	//入力時
	if(IsInput){
		SetTexNumber(SizeMaxTextureNumber);
		//サイズ変化
		if(ScaleChange(Size_Min, Size_Max)){
			IsInput = false;
			SetTexNumber(SizeMinTextureNumber);
		}
	}
	BaseSprites::Update();
}

