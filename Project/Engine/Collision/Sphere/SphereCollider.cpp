#include "SphereCollider.h"

using namespace DirectX;

void SphereCollider::Update()
{
	//ワールド行列から座標を抽出
	const Matrix4x4& matWorld = objObject->GetmatWorld().matWorld;

	//球のメンバ変数を更新
	Sphere::center = {matWorld.m[3][0]+offset.x, matWorld.m[3][1]+offset.y, matWorld.m[3][2]+offset.z, matWorld.m[3][3]+offset.w};
	Sphere::radius = radius;
}
