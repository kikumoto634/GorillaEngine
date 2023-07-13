#pragma once
#include "BaseSprites.h"
#include "TextureUtility.h"

class Combo
{
private:
	//2Dテキスト
	const int NumberSpSize = 3;
	const int TexNumberBegin = number0_tex.number;

	//色変化時間
	const float ComboResetColorSecond = 0.5f;

	//テキスト
	//座標
	const Vector2 PositionText = {50,300};
	//サイズ
	const Vector2 SizeText = {150, 75};

	//ナンバー
	//座標
	const Vector2 PositionNumber = {50, 375};
	//サイズ
	const Vector2 SizeNumber = {50, 75};

	//コンボ
	//加算数
	const int IncrementValue = 1;
	//リセット色青
	const Vector3 ResetColor = {0,0,1};

	//コンボ計算用
	const int ValueHundred = 100;
	const int ValueTen = 10;
	const int ValueOne = 1;

//エイリアス
private:
	template <class T> using unique_ptr = std::unique_ptr<T>;

public:
	void Initialize();
	
	void Update();

	void Draw();
	
	void Finalize();

	void Increment();
	void Reset();

	//Getter
	inline int GetComboNum()	{return comboNum_;}

private:
	//コンボ
	int comboNum_ = 0;

	//コンボリセット関連
	bool isReset_ = false;
	Vector3 comboSpColor_ = {1,1,1};
	float curColorChangeFrame_ = 0;

	//コンボテキスト
	unique_ptr<BaseSprites> comboSp_;
	//数字(TexNumber 3~12)
	unique_ptr<BaseSprites> numberSp_combo_[3];
};

