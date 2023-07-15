#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"


class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

public:
	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(Vector3 v, float w);

	//長さ
	float LengthVector3();
	//正規化
	Vector4 NormalizeVector3();

	//行列とベクトル4積
	Vector4 TransformVector3(Vector4 v, Matrix4x4 m);
	Vector4 TransformNormalizeVector3(Vector4 v, Matrix4x4 m);

	float DotVector3(Vector4 v) const;
	Vector4 CorssVector3(Vector4 v) const;

	Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);


	Vector4 operator+() const;
	Vector4 operator-() const;

	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(float s);
	Vector4& operator*=(const Vector4& v);
	Vector4& operator/=(float s);
};

const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator*(const Vector4& v1, const Vector4& v2);
const Vector4 operator/(const Vector4& v, float s);
