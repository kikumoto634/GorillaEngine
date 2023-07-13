#pragma once
#include "BaseEnemy.h"
#include "MapNode.h"
#include <vector>
#include <queue>

class BossStateManager;

class Boss1 : public BaseEnemy 
{
public:
	friend class IdelBossState;
	friend class SummonBossState;
	friend class TrackBossState;
	friend class RunAwayBossState;
	friend class RhythmChangeBossState;
	friend class DeadBossState;

	template <class T> using queue = std::queue<T>;

	struct BPMValue{
		double BPM;
		double SUB;
	};

private:
	const int SummonMax = 9;

	//ダメージ状態の総フレーム
	const int DamageFrameMax = 240;
	//色変化の間隔
	const int damageFrameInterval = 6;
	//通常色
	const Vector4 NormalColor = {1.0f,1.0f,1.0f,1.0f};
	//ダメージ色1
	const Vector4 Damage1Color = {0.0f,0.0f,0.0f,0.0f};
	//ダメージ色2
	const Vector4 Damage2Color = {1.0f,0.0f,0.0f,1.0f};

	//通常リズム
	const BPMValue Normal = {120, 5};
	const BPMValue Fast = {150, 5};

public:
	void AddInitialize() override;
	void AddUpdate() override;
	void AddDraw() override;
	void AddParticleDraw() override;
	void AddFinalize() override;

	void AddContactUpdate() override;

	void EventBegin()	{isEvent_ = true;}
	void EventEnd()	{isEvent_ = false;}

	//Getter
	Vector3 GetSummonObjPos();
	inline BPMValue GetBPMValue()	{return currentBPM;}
	inline bool GetIsSummon() {return summonObjPos.empty();}
	inline int GetSummonMax()	{return SummonMax;}
	inline Vector3 GetPopPosition() override {return particlePos_;}
	inline bool GetIsEvent()	{return isEvent_;}
	bool GetIsBpmChange();

	void BossPopInit(Vector3 pos);

private:
	//経路探索(A*アルゴリズム)
	vector<MapNode*> PathSearch(
		vector<vector<int>>& grid, 
		int start_x, int start_y, 
		int end_x, int end_y);
	//ヒューリスティック推定値計算
	int Heuristic(int x1,int y1, int x2,int y2){
		return abs(x1-x2) + abs(y1-y2);
	}

	void DamageUpdate();

private:
	//状態
	BossStateManager* state_= nullptr;
	//出現位置
	Vector3 popPosition_ = {};

	bool isEvent_ = false;

	queue<Vector3> summonObjPos;
	int summonNum_ = 0;
	bool isSummon_ = false;
	bool isSummonComp_ = false;
	unique_ptr<ParticleObject> summonParticle_;

	int moveCount = 0;

	//ダメージ
	bool isDamage_ = false;
	int damageFrame_ = 0;
	unique_ptr<ParticleObject> runAwayParticle_;

	//体力
	int hp_ = 10;

	//リズム変更
	bool isBpmChange = false;
	BPMValue currentBPM = Normal;
	unique_ptr<ParticleObject> rhythmChangeParticle_;
};

