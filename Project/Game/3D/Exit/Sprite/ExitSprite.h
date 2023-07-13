#pragma once
#include "BaseSprites.h"

class ExitSprite
{
private:
	template <class T> using unique_ptr = std::unique_ptr<T>;

private:
	const int TexNumberBegin = 3;	//テクスチャ番号
	const int CoinSpNumSize = 3;	//サイズ

public:
	void Initialize();
	void Update(bool isOpen, bool isPlayerContact);
	void Draw();
	void Finalize();

	//必要コイン更新
	void NeedCoinSpriteUpdate();

	//Getter
	inline const int GetExitNeedCoinNum()	{return exitNeedCoinsNum_;}
	inline BaseSprites* GetCoinSp()	{return coinSp_.get();}

	//Setter
	inline void SetCoinSpPosition(Vector2 pos)	{coinSp_->SetPosition(pos);}
	inline void SetExitOpenNeedCoin(int num)	{exitNeedCoinsNum_ = num;}

private:
	//画像
	unique_ptr<BaseSprites> coinSp_;
	unique_ptr<BaseSprites> coinSpNum_[3];

	unique_ptr<BaseSprites> exitOpenSp_;
	int buttonSpNumber = 0;

	//必要コイン数
	int exitNeedCoinsNum_ = 1;

	//プレイヤー接触
	bool isPlayerContact_ = false;

	//開閉
	bool isOpen_ = false;
};

