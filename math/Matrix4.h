#pragma once
#include "Vector3.h"

class Matrix4
{
public:
	float m[4][4];

public:
	Matrix4();
	Matrix4(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

	Matrix4 Identity();
	Matrix4 Scale(const Vector3& scale);

	Matrix4 RotationX(const float x);
	Matrix4 RotationY(const float y);
	Matrix4 RotationZ(const float z);
	Matrix4 RotationXYZ(const Vector3& rot);

	Matrix4 Translation(const Vector3& pos);
};

Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);
Matrix4 operator* (Matrix4& m1, Matrix4& m2);

