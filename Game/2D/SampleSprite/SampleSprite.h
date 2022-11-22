#pragma once
#include "../BaseSprites.h"

class SampleSprite : public BaseSprites
{
public:
	~SampleSprite();

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	void Initialize(UINT textureNumber) override;

	/// <summary>
	/// XV
	/// </summary>
	void Update() override;

	/// <summary>
	/// •`‰æ
	/// </summary>
	void Draw() override;

	/// <summary>
	/// Œãˆ—
	/// </summary>
	void Finalize() override;
};

