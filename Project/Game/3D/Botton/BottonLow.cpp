#include "BottonLow.h"


void BottonLow::IntervalNumSet()
{
	intervalBeatCountMax = 12;
}

void BottonLow::RhythmChange()
{
	intervalBeatCount = 0;
	rhythm_->BPMLowSet();
	audio_->SetRation(bpm120Game_audio.number, 0.75f);
}
