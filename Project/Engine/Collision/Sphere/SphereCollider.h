#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include "Vector4.h"

/// <summary>
/// 球衝突判定オブジェクト
/// </summary>
class SphereCollider : public BaseCollider, public Sphere
{

//メンバ関数
public:
	SphereCollider(Vector4 offset = {0,0,0,0}, float radius = 1.0f):
		offset(offset),
		radius(radius)
	{
		//球形状をセット
		shapeType = COLLISIONSHAPE_SPHERE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;


	//Getter
	inline const Vector4& GetOffset()	{return offset;}
	inline float GetRadius()	{return radius;}

	//Setter
	inline void SetOffset(const Vector4& offset)	{this->offset = offset;}
	inline void SetRadius(float radius)	{this->radius = radius;}

private:
	//オブジェクト中心からのオフセット
	Vector4 offset;
	//半径
	float radius;
};

