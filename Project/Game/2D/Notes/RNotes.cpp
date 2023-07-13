#include "RNotes.h"
#include "Easing.h"

void RNotes::Initialize(UINT textureNumber)
{
	Notes::Initialize(textureNumber);
	SetPosition(R_Start_Pos);
}

void RNotes::Update(float goalTime)
{
	if(!IsNoteAlive) return;

	//移動
	if(!IsMoveStop){

		easingSp = goalTime*3;
		easingTime = (easingSp*55.f);

		//戻し
		if(GetPosition().x >= Delete_Pos.x){
			IsNoteAlive = false;
			curBeatTime = 0;
			return;
		}

		//移動
		curBeatTime += 1.f/easingTime;
		easingPos = Easing_Point2_Linear(R_Start_Pos,R_End_Pos,curBeatTime);

		SetPosition(easingPos);
	}

	Notes::Update(goalTime);
}

void RNotes::BeatUpdate()
{
	if(IsNoteAlive) return;

	SetPosition(R_Start_Pos);
	Notes::BeatUpdate();
}
