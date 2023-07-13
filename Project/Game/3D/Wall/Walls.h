#pragma once
#include "BaseObjObject.h"
#include "ParticleObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

class Walls : public BaseObjObject
{
private:
	//描画範囲
	const int DrawingRange = 11;
	//描画範囲(暗めの表示)
	const int DrawingRange_Half = 7;

public:
	~Walls();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize(ObjModelManager* model, ObjModelManager* collider);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera) override;

	/// <summary>
	/// 3D描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollision(const CollisionInfo& info) override;

	//当たり判定時処理
	void ContactUpdate() override;

	//Getter
	inline bool GetIsDIg()	{return isDig_;}
	inline Vector3 GetDigPosition()	{return digPosition_;}

	//Setter
	void SetPlayerPos(Vector3 pos)	{playerPos_ = pos;}

private:
	void ColliderInitialize();
	void ColliderSet();
	void ColliderRemove();

private:
	Vector3 playerPos_;
	float distance_ = 0;

	bool isDig_ = false;
	Vector3 digPosition_ = {};

	//距離に応じた非表示
	bool isHide_ = false;

	//メッシュコライダー用モデル
	ObjModelManager* colliderModel_ = nullptr;
	bool isCollision_ = false;

	MeshCollider* collider_ = nullptr;
};

