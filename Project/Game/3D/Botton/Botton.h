#pragma once
#include "BaseObjObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"
#include "AudioUtility.h"

#include "Audio.h"
#include "RhythmManager.h"

class Botton : public BaseObjObject
{
private:
	//描画範囲
	const int DrawingRange = 11;
	//描画範囲(暗めの表示)
	const int DrawingRange_Half = 7;

	const float PushDownPosY = -0.4f;

public:
	~Botton();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize(std::string filePath, bool IsSmoothing);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera, Vector3 playerPos);

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

	//Setter
	void SetPlayerPos(Vector3 pos)	{playerPos_ = pos;}
	void SetRhythmManager(RhythmManager* rhythm)	{rhythm_ = rhythm;}

private:
	void ReturnButton();

	void RhythmNormalChange();

	void ColliderInitialize();
	void ColliderSet();
	void ColliderRemove();

protected:
	virtual void IntervalNumSet();
	virtual void RhythmChange();

private:
	Vector3 playerPos_;
	float distance_ = 0;

	//距離に応じた非表示
	bool isHide_ = false;

	//メッシュコライダー用モデル
	ObjModelManager* colliderModel_ = nullptr;
	bool isCollision_ = false;

	MeshCollider* collider_ = nullptr;

	bool isPush_ = false;

protected:
	int intervalBeatCountMax = 12;
	int intervalBeatCount = 0;

	Audio* audio_ = nullptr;
	RhythmManager* rhythm_ = nullptr;
};

