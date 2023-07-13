#pragma once
#include "BaseObjObject.h"
#include "SphereCollider.h"

#include "ParticleObject.h"

class BaseEnemy : public BaseObjObject
{
//エイリアス
protected:
	template <class T> using unique_ptr = std::unique_ptr<T>;
	template <class T> using vector = std::vector<T>;

//定数
private:
	//描画範囲(非表示)
	const int DrawingRange_Not = 11;
	//描画範囲(暗めの表示)
	const int DrawingRange_Half = 7;

	//拡縮最小サイズ
	const Vector3 SizeMin = {0.7f, 0.7f, 0.7f};

	//Y座標固定
	const float PositionY = -3.5f;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string filePath, bool IsSmoothing = false) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera, const Vector3& playerPos);

	/// <summary>
	/// 3D描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// パーティクル描画
	/// </summary>
	void ParticleDraw();

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollision(const CollisionInfo& info) override;

	//出現
	void Pop(Vector3 pos) override;

	//当たり判定時処理
	void ContactUpdate() override;

	void ColliderSet();
	void ColliderRemove();

	//Getter

	//Setter
	inline void SetMapInfo(vector<vector<int>> info) {mapInfo_ = info;}
	inline void SetIsAlive(bool isFlag)	{isAlive_ = isFlag;}

public:
	virtual void AddInitialize() = 0;
	virtual void AddUpdate() = 0;
	virtual void AddDraw() = 0;
	virtual void AddParticleDraw() = 0;
	virtual void AddFinalize() = 0;

	virtual void AddContactUpdate() = 0;

	virtual inline Vector3 GetPopPosition() = 0;

private:
	//アクション更新
	void ActionUpdate();

	//ビート更新
	void BeatUpdate();

	//距離に応じた処理
	void DistanceUpdate() override;

	//コライダー
	void ColliderInitialize();

	//パーティクル
	void ParticleInitialize();

protected:
	//借り物
	//プレイヤー座標
	Vector3 playerPos_;

	//非表示
	bool isInvisible_ = false;

	//ビート時の拡縮
	bool isScaleChange_ = false;

	//プレイヤーとの距離
	float distance_ = 0.f;

	//経路探索
	vector<vector<int>> mapInfo_;

	//コライダー
	SphereCollider* sphereCollider_ = nullptr;
	float colliderRadius_ = 1.0f;

	//パーティクル
	unique_ptr<ParticleObject> deadParticle_;
	unique_ptr<ParticleObject> popParticle_;
	Vector3 particlePos_ = {0,-10,0};
	int particleAliveFrame_ = 0;

};

