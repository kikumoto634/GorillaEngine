#pragma once

#include "Vector4.h"

///<summary>
/// 当たり判定プリミティブ
///</summary>


/// <summary>
/// 球
/// </summary>
struct Sphere
{
	//中心座標
	Vector4 center = {0,0,0,1};
	//半径
	float radius = 1.0f;
};

/// <summary>
/// 平面
/// </summary>
struct Plane
{
	//法線ベクトル
	Vector4 normal = {0,1,0,0};
	//原点(0,0,0)から距離
	float distance = 0.0f;
};


/// <summary>
/// 法線付き三角形、(時計周りが表面)
/// </summary>
class Triangle
{
public:
	//頂点座標3つ
	Vector4 p0;
	Vector4 p1;
	Vector4 p2;
	//法線ベクトル
	Vector4 normal;

public:
	/// <summary>
	/// 法線の計算
	/// </summary>
	void ComputeNormal();
};


/// <summary>
/// レイ(半直線)
/// </summary>
struct Ray
{
	//始点座標
	Vector4 start = {0,0,0,1};
	//方向
	Vector4 dir = {1,0,0,0};
};