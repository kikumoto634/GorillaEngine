#include "CollisionPrimitive.h"

void Triangle::ComputeNormal()
{
	Vector4 p0_p1 = p1 - p0;
	Vector4 p0_p2 = p2 - p0;

	//外積により、2辺にスイ着なベクトルを算出する
	normal = p0_p1.CorssVector3(p0_p2);
	normal.NormalizeVector3();
}
