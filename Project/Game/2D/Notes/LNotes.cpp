#include "LNotes.h"
#include "Easing.h"

void LNotes::Initialize(UINT textureNumber)
{
	Notes::Initialize(textureNumber);
	SetPosition(L_Start_Pos);
}

void LNotes::Update(float goalTime)
{
	if(!IsNoteAlive) return;

	//移動
	if(!IsMoveStop){

		easingSp = goalTime*3;
		easingTime = (easingSp*55.f);

		//戻し
		if(GetPosition().x <= Delete_Pos.x){
			IsNoteAlive = false;
			curBeatTime = 0;
			return;
		}

		//移動
		curBeatTime += 1.f/easingTime;
		easingPos = Easing_Point2_Linear(L_Start_Pos,L_End_Pos,curBeatTime);

		SetPosition(easingPos);
	}

	Notes::Update(goalTime);
}

void LNotes::BeatUpdate()
{
	if(IsNoteAlive) return;

	SetPosition(L_Start_Pos);
	Notes::BeatUpdate();
}
