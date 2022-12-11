#pragma once
#include "../BaseParticleObjects.h"

class SampleParticleObject : public BaseParticleObjects
{
	//メンバ関数
public:
	~SampleParticleObject();

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

	void Add(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale);
};

