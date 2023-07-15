#pragma once
#include "Vector4.h"

class BaseObjObject;
class BaseCollider;

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo(BaseObjObject* objObject, BaseCollider* collider, const Vector4& inter){
		this->objObject = objObject;
		this->collider = collider;
		this->inter = inter; 
	}

public:
	//衝突相手のオブジェクト
	BaseObjObject* objObject = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	Vector4 inter;
};