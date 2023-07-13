#pragma once

#include "CollisionTypes.h"
#include "CollisionInfo.h"
#include "BaseObjObject.h"

/// <summary>
/// コライダー基底クラス
/// </summary>
class BaseCollider
{
public:
	friend class CollisionManager;

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


	//当たり判定属性
	inline void SetAttribute(unsigned short attribute)	{this->attribute = attribute;}
	inline unsigned short GetAttribute()	{return attribute;}
	inline void AddAttribute(unsigned short attribute)	{this->attribute |= attribute;}
	inline void RemoveAttribute(unsigned short attribute)	{this->attribute &= !attribute;}


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
	//当たり判定属性
	unsigned short attribute = 0b1111111111111111;
};