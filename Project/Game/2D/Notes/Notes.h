#pragma once
#include "BaseSprites.h"

class Notes : public BaseSprites
{
protected:
	const Vector2 L_Start_Pos = {1288,600};
	const Vector2 R_Start_Pos = {-8,600};

	//イージング目標地
	const Vector2 L_End_Pos = {650,600};
	const Vector2 R_End_Pos = {630,600};
	//ノーツ削除位置
	const Vector2 Delete_Pos = {640,600};

	const Vector2 Notes_Size = {16,80};
	const Vector2 Notes_Anc = {0.5,0.5};

	//透過率変化時間
	const float AlphaMaxTime = 0.5f;

public:
	virtual void Initialize(UINT textureNumber);
	
	void InputUpdate();

	virtual void Update(float goalTime);
	virtual void BeatUpdate();

	void Draw() override;

	//Getter
	inline bool GetIsNoteAlive()	{return IsNoteAlive;}

protected:
	//生存
	bool IsNoteAlive = false;
	//停止
	bool IsMoveStop = false;

	//透過率変化時間
	float curAlphaFrame = 0.f;


	//移動用タイム
	float curBeatTime = 0;
	//移動時間
	float easingTime = 0;

	//イージング座標
	Vector2 easingPos = {};

	//移動速度
	float easingSp = 0;
};

