#pragma once
#include "BaseObjObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

class Planes : public BaseObjObject
{
private:
	//描画範囲(非表示)
	const int DrawingRange_Not = 11;
	//描画範囲(暗めの表示)
	const int DrawingRange_Half = 7;

	//色
	const Vector4 IniColor = {1.0f, 1.0f, 1.0f, 1.0f};
	//緑
	const Vector4 GreenColor = {0.0f, 0.8f, 0.0f, 1.0f};
	//赤
	const Vector4 RedColor = {0.8f, 0.0f, 0.0f, 1.0f};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize(ObjModelManager* model);

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


	/// <summary>
	/// 色変化
	/// </summary>
	void PlaneColorChange(bool IsSwitch,  bool IsColorChange);
	inline void PlaneColorReset()	{object_->SetColor({1.f,1.f,1.f,1.f});}


	inline void CaveLightOn()	{IsCaveLight = true;}
	inline void CaveLightOff()	{IsCaveLight = false;}

	//Getter
	inline bool GetIsPlayerContact()	{return IsPlayerContact;}

	//Setter
	inline void SetIsPlayerContact(bool IsFlag)	{IsPlayerContact = IsFlag;}
	inline void SetPlayerPos(Vector3 pos)	{PlayerPos = pos;}

private:
	//距離計算
	void DistanceUpdate();

	//コライダー
	void ColliderInitialize();
	void ColliderSet();
	void ColliderRemove();

private:
	bool IsPlayerContact = false;

	Vector3 PlayerPos;
	float distance = 0;

	//距離に応じた非表示
	bool IsHide = false;

	bool IsCollision = false;

	//ライティング
	bool IsCaveLight = false;

	//コライダー
	MeshCollider* collider = nullptr;
};

