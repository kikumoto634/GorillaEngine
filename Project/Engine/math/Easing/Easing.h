#pragma once
#include "Vector2.h"
#include "Vector3.h"

#include <cmath>

/// <summary>
/// イージング_時間(一回)
/// </summary>
/// <param name="frame">フレーム</param>
/// <param name="second">何秒で到達</param>
/// <returns></returns>
float& Time_OneWay(float& frame, const float second);

/// <summary>
/// イージング_時間(ループ)
/// </summary>
/// <param name="frame">フレーム</param>
/// <param name="second">何秒で到達</param>
/// <returns></returns>
float& Time_Loop(float& frame, const float second);



/// <summary>
/// EaseOutBounce用フレーム変化
/// </summary>
float EaseOutBounec(float value);

/// <summary>
/// EaseOutInBounce
/// </summary>
float EaseInOutBounce(float value);

/// <summary>
/// EaseInCubic
/// </summary>
float EaseInCubic(float value);



/// <summary>
/// 等速直線 2点
/// </summary>
template<typename T>
const T Easing_Point2_Linear(const T& start, const T& end, const float& frame);

/// <summary>
/// 等速直線 3点
/// </summary>
template<class T>
const T Easing_Point3_Linear(const T& start, const T& center, const T& end, const float& frame);

/// <summary>
/// EaseOutBounce 2点
/// </summary>
template<class T>
const T Easing_Point2_EaseOutBounce(const T& start, const T& end, const float& frame);

/// <summary>
/// EaseOutInBounce 2点
/// </summary>
template<class T>
const T Easing_Point2_EaseOutInBounce(const T& start, const T& end, const float& frame);

/// <summary>
/// EaseInCubic 2点
/// </summary>
template<class T>
const T Easing_Point2_EaseInCubic(const T& start, const T& end, const float& frame);



template<class T>
inline const T Easing_Point2_Linear(const T &start, const T &end, const float &frame)
{
	T pos{};
	pos = (1.f-frame) * start + frame * end;
	return pos;
}

template<class T>
inline const T Easing_Point3_Linear(const T &start, const T &center, const T &end, const float &frame)
{
	T pos{};
	pos = ((1.f-frame)*(1.f-frame) * start) + (2.f * (1.f-frame) * frame * center) + (frame*frame * end);
	return pos;
}


template<class T>
inline const T Easing_Point2_EaseOutBounce(const T &start, const T &end, const float &frame)
{
	T pos{};
	float value = EaseOutBounec(frame);
	pos = (1.f-value) * start + value * end;
	return pos;
}

template<class T>
inline const T Easing_Point2_EaseOutInBounce(const T &start, const T &end, const float &frame)
{
	T pos{};
	float value = EaseInOutBounce(frame);
	pos = (1.f-value) * start + value * end;
	return pos;
}

template<class T>
inline const T Easing_Point2_EaseInCubic(const T &start, const T &end, const float &frame)
{
	T pos{};
	float value = EaseInCubic(frame);
	pos = (1.f-value) * start + value * end;
	return pos;
}
