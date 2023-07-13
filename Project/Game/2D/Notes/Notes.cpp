#include "Notes.h"
#include "Easing.h"

void Notes::Initialize(UINT textureNumber)
{
	BaseSprites::Initialize(textureNumber);
	SetAnchorPoint(Notes_Anc);
	SetSize(Notes_Size);
}

void Notes::InputUpdate()
{
	IsMoveStop = true;
}

void Notes::Update(float goalTime)
{
	//停止
	if(IsMoveStop){

		//消滅色変化
		this->color.w = Easing_Point2_Linear<float>(1.0f,0.0f,Time_OneWay(curAlphaFrame, AlphaMaxTime));

		if(this->color.w <= 0){
			IsNoteAlive = false;
			curBeatTime = 0;
			curAlphaFrame = 0;
			this->color.w = 1;
			SetPosition(L_Start_Pos);
		}
	}

	BaseSprites::Update();
}

void Notes::BeatUpdate()
{
	BaseSprites::Update();
	IsNoteAlive = true;
	IsMoveStop = false;
}

void Notes::Draw()
{
	if(!IsNoteAlive)return;
	BaseSprites::Draw();
}
