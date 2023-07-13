#pragma once
#include "BaseObjObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

class IndestructibleWall : public BaseObjObject
{
//死亡後のリセット時間
	const int DigAppearanceFrame = 50;

	//描画範囲
	const int DrawingRange = 11;
	//描画範囲(暗めの表示)
	const int DrawingRange_Half = 7;
public:
	~IndestructibleWall();

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
	inline bool GetIsAlive()	{return isAlive_;}
	inline bool GetIsReflect()	{return isReflect_;}

	//Setter
	inline void SetPlayerPos(Vector3 pos)	{PlayerPos_ = pos;}

	void ColliderRemove();

private:
	void ColliderInitialize();
	void ColliderSet();

private:
	Vector3 PlayerPos_;
	float distance_= 0;

	//距離に応じた非表示
	bool isHide_ = false;

	//メッシュコライダー用モデル
	ObjModelManager* colliderModel_ = nullptr;
	bool isCollision_ = false;

	//反射音
	bool isReflect_ = false;

	MeshCollider* collider_ = nullptr;
};

