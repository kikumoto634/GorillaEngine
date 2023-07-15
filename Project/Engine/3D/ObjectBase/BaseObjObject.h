#pragma once

#include "ObjModelObject.h"
#include "ObjModelManager.h"

#include "CollisionInfo.h"


class BaseCollider;

class BaseObjObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseObjObject() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseObjObject();


	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(std::string filePath, bool IsSmmothing = false);
	virtual void Initialize(ObjModelManager* model);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="matBillboard">ビルボード開始</param>
	virtual void Update(Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 後処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	virtual void OnCollision(const CollisionInfo& info){};

	//当たり判定時処理
	virtual void ContactUpdate();

	//光計算
	inline void CaveLightOn()	{isLightCal_ = true;}
	inline void CaveLightOff()	{isLightCal_ = false;}

	//Getter
	bool GetIsContactTrigger();
	inline bool GetIsAlive()	{return isAlive_;}
	inline const Vector3& GetPosition()	{return world_.translation;}
	inline const Vector3& GetRotation()	{return world_.rotation;}
	inline const Vector3& GetScale()		{return world_.scale;}
	inline WorldTransform& GetmatWorld()	{return world_;}
	inline const ObjModelObject* GetObjObject()	{return object_;}
	inline const char* GetName()	{return name;}

	//Setter
	void SetPosition(const Vector3& position);
	void SetCollider(BaseCollider* collider);
	inline void SetRotation(const Vector3& rotation)	{world_.rotation = rotation, world_.UpdateMatrix();}
	inline void SetScale(const Vector3& scale)			{world_.scale = scale, world_.UpdateMatrix();}
	inline void SetObject(ObjModelObject* object)	{this->object_ = object;}
	inline void SetModel(ObjModelManager* model)	{this->model_ = model;}

protected:
	//クラス名(デバック用)
	const char* name = nullptr;

	bool isLendModel_ = false;
	ObjModelManager* model_ = nullptr;
	ObjModelObject* object_ = nullptr;
	WorldTransform world_ = {};

	//コライダー
	BaseCollider* baseCollider_ = nullptr;

	//カメラ(借り物)
	Camera* camera_ = nullptr;

	//生存フラグ
	bool isAlive_ = true;

	//接触
	bool isContactTrigger_ = false;

	//光計算
	bool isLightCal_ = false;
};

