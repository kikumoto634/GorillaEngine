#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "../../3D/BaseObjObject.h"

/// <summary>
/// コライダー基底クラス
/// </summary>
class BaseCollider
{
public:
	BaseCollider() = default;
	//仮想デストラクタ
	virtual ~BaseCollider() = default;

public:
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	inline void OnCollision(const CollisionInfo& info){
		objObject->OnCollision(info);
	}

	//Getter
	inline BaseObjObject* GetObjObject()	{return objObject;}
	inline CollisionShapeType GetShapeType()	{return shapeType;}

	//Setter
	inline void SetObjObject(BaseObjObject* objObject)	{this->objObject = objObject;}

protected:
	//オブジェクト
	BaseObjObject* objObject = nullptr;
	//形状タイプ
	CollisionShapeType shapeType = SHAPE_UNKNOWN;
};