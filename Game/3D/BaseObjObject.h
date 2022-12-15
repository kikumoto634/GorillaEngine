#pragma once

#include "../../Engine/3D/ObjModelObject.h"
#include "../../Engine/base/ObjModelManager.h"

#include "../Collision/CollisionSystem/CollisionInfo.h"


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

	//Getter
	const Vector3& GetPosition()	{return world.translation;}
	const Vector3& GetRotation()	{return world.rotation;}
	const WorldTransform& GetmatWorld()	{return world;}
	const ObjModelObject* GetObjObject()	{return object;}

	//Setter
	void SetPosition(const Vector3& position)	{world.translation = position;}
	void SetRotation(const Vector3& rotation)	{world.rotation = rotation;}
	void SetCollider(BaseCollider* collider);
	void SetObject(ObjModelObject* object)	{this->object = object;}
	void SetModel(ObjModelManager* model)	{this->model = model;}

protected:
	//クラス名(デバック用)
	const char* name = nullptr;

	ObjModelManager* model;
	ObjModelObject* object;
	WorldTransform world;

	//コライダー
	BaseCollider* collider = nullptr;

	Camera* camera = nullptr;
};

