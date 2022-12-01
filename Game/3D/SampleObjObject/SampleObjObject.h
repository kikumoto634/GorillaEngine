#pragma once
#include "../BaseObjObject.h"

class SampleObjObject : public BaseObjObject
{
//ƒƒ“ƒoŠÖ”
public:
	~SampleObjObject();

	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	void Initialize(std::string filePath, bool IsSmoothing = false) override;

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

