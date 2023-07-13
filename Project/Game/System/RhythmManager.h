#pragma once
#include <chrono>

class RhythmManager
{
//エイリアス
public:
	template <class T> using Duration = std::chrono::duration<T>;
	using Time = std::chrono::system_clock::time_point;

	struct RhythmDetail{
		double BPM;
		double SUB;
		float NoteAdjustmentSp;
	};

private:
	const RhythmDetail Low	 {  90, 5, 0.02f};
	const RhythmDetail Normal{ 120, 5, 0.02f};
	const RhythmDetail High  { 150, 5, 0.025f};

public:
	//初期化
	void Initialize();

	//更新前
	void PreUpdate();
	
	//更新終了後
	void PostUpdate();


	//時間計測開始
	void TimeStart();

	//入力時間取得
	void InputTime();
	//入力判定
	bool IsInputRhythmJudge();

	//BPM変更
	inline void BPMLowSet()	{
		BPM=Low.BPM,	 SUB=Low.SUB,	 NoteSpeedAdjustment=Low.NoteAdjustmentSp;
		Initialize();
	}
	void BPMNormalSet()		{
		BPM=Normal.BPM, SUB=Normal.SUB, NoteSpeedAdjustment=Normal.NoteAdjustmentSp;
		Initialize();
	}
	void BPMHighSet()		{
		BPM=High.BPM,	 SUB=High.SUB,	 NoteSpeedAdjustment=High.NoteAdjustmentSp;
		Initialize();
	}


	//Getter
	inline bool GetIsJustRhythm()	{return isJustRhythm_;}
	inline double GetCalTime()	{return calTime_.count();}
	inline double GetInputTimeTarget()	{return inputTimeTarget_;}
	inline double GetInputTimet()	{return inputTime_.count();}
	inline double GetBPMTime()	{return beatTime_;}
	inline bool GetIsMeasureUp()	{return isMeasureUp_;}

	inline float GetNoteSpeedAdjustment()	{return NoteSpeedAdjustment;}

	//Setter
	void SetBPM(double bpm, double sub = 5.0);

private:
	//BPM
	double BPM = 120;
	//入力判定時の増減差分(1小節の何分割分の時間)
	double SUB = 5;
	//ノーツの速度調整用//BPM120 90 = 0.02 && BPM 150 = 0.015
	float NoteSpeedAdjustment = 0.02f;

	//計測時間
	Duration<double> calTime_;
	//入力した時間
	Duration<double> inputTime_;

	//計測時間
	//スタート
	Time startTime_;
	//計測用iループ終了時間
	Time endTime_;
	//入力時の時間
	Time inputEndTime_;


	//1小節時間
	double beatTime_ = 60.0 / BPM;

	//入力目標時間
	double inputTimeTarget_ = 0;
	//入力時間差分
	double beatSub_ = 0;


	//計測開始
	bool isStart_ = false;
	//リズムピッタリ
	bool isJustRhythm_ = false;
	//リズム繰り上げ
	bool isRhythmMoveUp_ = false;
	//次の小節移動
	bool isMeasureUp_ = false;
};

