#include "Easing.h"
#include <cmath>

float& Time_OneWay(float& frame, const float second)
{
	return (frame < 1.f) ? frame += 1.f/(second*60.f) : frame = 1.f;
}

float &Time_Loop(float &frame, const float second)
{
	return (frame < 1.f) ? frame += 1.f/(second*60.f) : frame = 0.f;
}



float EaseOutBounec(float value)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	float lframe = value;

	float answer;

	if (lframe < 1.0f / d1) {
		answer = (n1 * lframe * lframe);
	} else if (lframe < 2.0f / d1) {
	    answer = n1 * (lframe -= 1.5f / d1) * lframe + 0.75f;
	} else if (lframe < 2.5f / d1) {
	    answer = n1 * (lframe -= 2.25f / d1) * lframe + 0.9375f;
	} else {
	    answer = n1 * (lframe -= 2.625f / d1) * lframe + 0.984375f;
	}

	return answer;
}

float EaseInOutBounce(float value)
{
	return (value < 0.5f) ? (1-EaseOutBounec(1-2*value))/2 : (1 + EaseOutBounec(2*value-1))/2;
}

float EaseInCubic(float value)
{
	return value*value*value;
}
