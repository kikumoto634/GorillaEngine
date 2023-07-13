#pragma once
#include "Audio.h"
#include "LightGroup.h"

#include "BaseSprites.h"
#include "combo.h"

#include <string>
#include <vector>

#include "AudioUtility.h"
#include "TextureUtility.h"

class GameManager
{
private:
	template <class T> using unique_ptr = std::unique_ptr<T>;
	template <class T> using vector = std::vector<T>;

private:
	//地面の色変化
	const int PlaneColorChangeConboNum = 10;

	//XビートでPOPターン
	const int EnemyPopBeatTurn = 6;

	//1ビートでの敵POP数
	const int EnemyPopCreateNum = 1;

	//2Dテキスト
	const int NumberSpSize = 3;
	const int TexNumberBegin = number0_tex.number;

	//影
	const Vector3 CircleShadowDir = { 0,-1,0 };
	const Vector3 CircleShadowAtten = { 0.5f,0.6f,0.0f };
	const Vector2 CircleShadowFactorAngle = { 0.0f, 0.5f };

	//体力
	const int HpSpSize = 5;

public:
	//初期化
	void Initialize();
	//後処理
	void Finalize();

	//コンボ加算
	void ComboIncrement();
	//コンボリセット
	void ComboReset();
	//コンボ数取得
	inline int GetComboNum()	{return combo_->GetComboNum();}
	//地面色変化コンボ数
	inline int GetPlaneColorChangeCombo()	{return PlaneColorChangeConboNum;}

	//コイン加算
	void CoinIncrement();
	//コイン数取得
	inline int GetCoinNum()	{return coinNum_;}

	//体力初期設定
	void InitializeSetHp(int _hp);
	//体力スプライトリズムフラグ
	void IsBeatEndOn()	{isHpScaleChange_ = true;}
	//体力減少
	void HpDecrement();
	//体力増加
	void HpIncrement();

	//オーディオ再生
	void AudioPlay(int number, float volume = 1.f, bool loop = false);
	//オーディオ停止
	void AudioStop(int number);
	//比率変更
	void AudioRatio(int number, float ratio);

	//ライト更新
	void LightUpdate(bool IsPlayerShadowDead = false);
	//プレイヤー丸影の設置
	void PlayerCircleShadowSet(Vector3 pos);

	//敵生成
	int EnemyPopTurnCount();
	Vector2 EnemyRandomPos(const int groundWidth, const float Plane_Size);
	Vector2 EnemyRandomDir(Vector2 pos);

	//2Dスプライト更新
	void SpriteUpdate();

	//2Dスプライト描画
	void SpriteDraw();

private:
	//オーディオ初期化
	void AudioInitialize();

	//ライト初期化
	void LightInitialize();

	//スプライト初期化
	void SpriteInitialize();

private:
	
	//オーディオ
	Audio* audio_ = nullptr;

	//ライト
	LightGroup* lightGroup_ = nullptr;
	//丸影
	//プレイヤー
	bool isPlayerShadowAlive_ = false;

	//コイン
	unique_ptr<BaseSprites> coinSp_;
	unique_ptr<BaseSprites> numberSp_coin_[3];
	int coinNum_ = 0;

	//体力
	vector<unique_ptr<BaseSprites>> hpSp_;
	int hp_ = 0;
	int damageHpSpriteIndex_ = 0;
	bool isHpScaleChange_ = false;

	//コンボ
	unique_ptr<Combo> combo_;

	//敵のランダム生成
	//XビートでPOPターン
	int currentEnemyPopBeatTurn_ = 0;

};

