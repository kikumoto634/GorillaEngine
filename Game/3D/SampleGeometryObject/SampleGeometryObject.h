#pragma once
#include "../BaseGeometryObjects.h"

class SampleGeometryObject : public BaseGeometryObjects
{
	//メンバ関数
public:
	~SampleGeometryObject();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(UINT texNumber) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize() override;
};

