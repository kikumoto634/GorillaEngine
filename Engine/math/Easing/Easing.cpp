#include "Easing.h"
#include <cmath>

float& Time_OneWay(float& frame, const float& second)
{
	return (frame < 1.f) ? frame += 1.f/(second*60.f) : frame = 1.f;
}

float &Time_Loop(float &frame, const float &second)
{
	return (frame < 1.f) ? frame += 1.f/(second*60.f) : frame = 0.f;
}

const Vector3 Easing_Linear_Point2(const Vector3 &start, const Vector3 &end, const float &frame)
{
	Vector3 pos{};

	pos.x = (1.f-frame) * start.x + frame * end.x;
	pos.y = (1.f-frame) * start.y + frame * end.y;
	pos.z = (1.f-frame) * start.z + frame * end.z;

	return pos;
}

const Vector3 Easing_Linear_Point3(const Vector3 &start, const Vector3 &center, const Vector3 &end, const float &frame)
{
	Vector3 pos{};

	pos.x = ((1.f-frame)*(1.f-frame) * start.x) + (2 * (1.f-frame) * frame * center.x) + (frame*frame * end.x);
	pos.y = ((1.f-frame)*(1.f-frame) * start.y) + (2 * (1.f-frame) * frame * center.y) + (frame*frame * end.y);
	pos.z = ((1.f-frame)*(1.f-frame) * start.z) + (2 * (1.f-frame) * frame * center.z) + (frame*frame * end.z);

	return pos;
}
