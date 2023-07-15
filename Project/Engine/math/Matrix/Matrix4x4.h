#pragma once
#include "Vector3.h"

class Matrix4x4
{
public:
	float m[4][4];

public:
	Matrix4x4();
	Matrix4x4(float m00,float m01,float m02,float m03,
	          float m10,float m11,float m12,float m13,
	          float m20,float m21,float m22,float m23,
	          float m30,float m31,float m32,float m33);


	Matrix4x4 MakeIdentityMatrix();

	//スケーリング行列
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	//回転行列
	Matrix4x4 MakeRotationXMatrix(float theta);
	Matrix4x4 MakeRotationYMatrix(float theta);
	Matrix4x4 MakeRotationZMatrix(float theta);

	//平行移動行列
	Matrix4x4 MakeTranslateMatrix(const Vector3& trans);

	//アフィン変換行列
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& trans);

	//逆行列
	float Det(Matrix4x4 m);
	Matrix4x4 Inverse(Matrix4x4 m);

	//代入演算子オーバーロード
	Matrix4x4& operator*=(Matrix4x4& m1);
};

//2公演算子オーバーロード
Matrix4x4 operator*(Matrix4x4& m1, Matrix4x4& m2);
