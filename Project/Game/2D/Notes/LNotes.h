#pragma once
#include "Notes.h"
class LNotes : public Notes
{
public:
	void Initialize(UINT textureNumber) override;
	void Update(float goalTime) override;
	void BeatUpdate() override;

};

