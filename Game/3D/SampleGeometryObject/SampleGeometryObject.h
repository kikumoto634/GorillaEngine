#pragma once
#include "../BaseGeometryObjects.h"

class SampleGeometryObject : public BaseGeometryObjects
{
	//ƒƒ“ƒoŠÖ”
public:
	~SampleGeometryObject();

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

