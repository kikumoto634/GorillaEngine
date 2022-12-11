#pragma once
#include "../BaseObjObject.h"
#include "../SampleParticleObject/SampleParticleObject.h"

class Player : public BaseObjObject
{
//メンバ関数
public:
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string filePath, bool IsSmoothing = false) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 3D描画
	/// </summary>
	void Draw3D();

	/// <summary>
	/// 2D描画
	/// </summary>
	/// <returns></returns>
	void Draw2D();

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollision(const CollisionInfo& info) override;

private:
	std::unique_ptr<SampleParticleObject> particle;
};

