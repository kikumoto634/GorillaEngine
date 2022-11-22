#pragma once
#include "../Vector/Vector3.h"

/// <summary>
/// イージング用時間(一回)
/// </summary>
/// <param name="frame">フレーム</param>
/// <param name="second">何秒で到達</param>
/// <returns></returns>
float& Time_OneWay(float& frame, const float& second);

/// <summary>
/// イージング用時間(ループ)
/// </summary>
/// <param name="frame">フレーム</param>
/// <param name="second">何秒で到達</param>
/// <returns></returns>
float& Time_Loop(float& frame, const float& second);

/// <summary>
/// 等速直線 2点
/// </summary>
/// <param name="start">開始座標</param>
/// <param name="end">終了座標</param>
/// <param name="frame">フレーム</param>
/// <returns></returns>
const Vector3 Easing_Linear_Point2(const Vector3& start, const Vector3& end, const float& frame);

/// <summary>
/// 等速直線 3点
/// </summary>
/// <param name="start">開始座標</param>
/// <param name="center">中間座標</param>
/// <param name="end">終了座標</param>
/// <param name="frame">フレーム</param>
/// <returns></returns>
const Vector3 Easing_Linear_Point3(const Vector3& start, const Vector3& center, const Vector3& end, const float& frame);
