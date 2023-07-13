#pragma once
#include "Notes.h"
class RNotes : public Notes
{
public:
	void Initialize(UINT textureNumber) override;
	void Update(float goalTime) override;
	void BeatUpdate() override;

};

