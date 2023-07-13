#include "BottonHigh.h"

void BottonHigh::IntervalNumSet()
{
	intervalBeatCountMax = 24;
}

void BottonHigh::RhythmChange()
{
	intervalBeatCount = 0;
	rhythm_->BPMHighSet();
	audio_->SetRation(bpm120Game_audio.number, 1.25f);
}
