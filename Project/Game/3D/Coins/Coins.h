#pragma once
#include "BaseObjObject.h"

class Coins : public BaseObjObject
{
private:
	//消失時の座標
	Vector3 DeadPos = {50,50,50};

	//一定ビート数に応じて消滅
	const int LostCountMax = 8;

	//コライダー
	const float SphereColliderRadius = 0.6f;

	//Y座標固定
	const float PositionY = -5.f;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string filePath, bool IsSmoothing = false) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera) override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 当たり判定
	/// </summary>
	void OnCollision(const CollisionInfo& info) override;


	/// <summary>
	/// 出現
	/// </summary>
	void Pop(Vector3 pos) override;

	//当たり判定時処理
	void ContactUpdate() override;
	
	//Getter
	bool GetIsAlive() {return isAlive_;}

private:
	//コライダーセット
	void ColliderSet();

private:
	//ロスト時間(ビート数で処理)
	int loatCount_ = 0;
};

