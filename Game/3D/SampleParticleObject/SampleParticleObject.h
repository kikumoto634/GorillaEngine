#pragma once
#include "../BaseParticleObjects.h"

class SampleParticleObject : public BaseParticleObjects
{
	//ƒƒ“ƒoŠÖ”
public:
	~SampleParticleObject();

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	void Initialize(UINT texNumber) override;

	/// <summary>
	/// XV
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// •`‰æ
	/// </summary>
	void Draw() override;

	/// <summary>
	/// Œãˆ—
	/// </summary>
	void Finalize() override;
};

