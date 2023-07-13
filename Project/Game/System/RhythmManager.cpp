#include "RhythmManager.h"

using namespace std::chrono;

void RhythmManager::Initialize()
{
	//1小節時間の計算
	beatTime_ = 60.0 / BPM;
	//差分時間
	beatSub_ = beatTime_/SUB;

	//初期化
	Duration<double> calTime(0);
	calTime_ = calTime;
}

void RhythmManager::PreUpdate()
{
	isJustRhythm_ = false;
	isMeasureUp_ = false;

	//リズム値更新
	if(inputTimeTarget_ + beatSub_ <= calTime_.count()){
		inputTimeTarget_ += beatTime_;
		isRhythmMoveUp_ = false;
		isMeasureUp_ = true;
	}

	if(!isStart_) return ;

	//リズムピッタリ時
	if(isRhythmMoveUp_) return;
	if(inputTimeTarget_ <= calTime_.count()){
		isJustRhythm_ = true;
		isRhythmMoveUp_ = true;
	}
}

void RhythmManager::PostUpdate()
{
	if(!isStart_) return;

	//1ループ処理時間
	endTime_ = system_clock::now();

	//最終時間計測
	calTime_ = endTime_ - startTime_;
	inputTime_ = inputEndTime_ - startTime_;
}

void RhythmManager::TimeStart()
{
	startTime_ = system_clock::now();
	isStart_ = true;
}

void RhythmManager::InputTime()
{
	inputEndTime_ = std::chrono::system_clock::now();
}

bool RhythmManager::IsInputRhythmJudge()
{
	if(inputTime_.count() >= inputTimeTarget_ - beatSub_ && 
		inputTime_.count() <= inputTimeTarget_ + beatSub_){
		return true;
	}

	return false;
}

void RhythmManager::SetBPM(double bpm, double sub)
{
	BPM = bpm;
	SUB = sub;

	//1小節時間の計算
	beatTime_ = 60.0 / BPM;
	//差分時間
	beatSub_ = beatTime_/SUB;
}
