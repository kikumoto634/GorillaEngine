#pragma once
#include "../BaseFbxObjects.h"
#include "../../../Engine/input/Input.h"

class SampleFbxObject : public BaseFbxObjects
{
//メンバ関数
public:
	~SampleFbxObject();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string filePath) override;

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

//メンバ変数
private:
	Input* input = nullptr;
};

