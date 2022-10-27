#include "Matrix4.h"
#include <cmath>

using namespace std;

Matrix4::Matrix4()
{
	for(int col = 0; col < 4; col++){
		for(int row = 0; row < 4; row++){
			m[col][row] = 0;
		}
	}
}

Matrix4::Matrix4(float m00, float m01, float m02, float m03, 
				 float m10, float m11, float m12, float m13, 
				 float m20, float m21, float m22, float m23, 
				 float m30, float m31, float m32, float m33)
{
	m[0][0] = m00, m[0][1] = m01, m[0][2] = m02, m[0][3] = m03;
	m[1][0] = m10, m[1][1] = m11, m[1][2] = m12, m[1][3] = m13;
	m[2][0] = m20, m[2][1] = m21, m[2][2] = m22, m[2][3] = m23;
	m[3][0] = m30, m[3][1] = m31, m[3][2] = m32, m[3][3] = m33;
}

Matrix4 Matrix4::Identity()
{
	Matrix4 result{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	return result;
}

Matrix4 Matrix4::Scale(const Vector3 &scale)
{
	Matrix4 result{
		scale.x, 0.f,	  0.f,	   0.f,
		0.f,	 scale.y, 0.f,	   0.f,
		0.f,	 0.f,	  scale.z, 0.f,
		0.f,	 0.f,	  0.f,	   1.f
	};

	return result;
}

Matrix4 Matrix4::RotationX(const float x)
{
	Matrix4 result{
		1.f,	 0.f,	   0.f,	    0.f,
		0.f,	 cosf(x),  sinf(x),	0.f,
		0.f,	 -sinf(x), cosf(x), 0.f,
		0.f,	 0.f,	   0.f,	    1.f
	};

	return result;
}

Matrix4 Matrix4::RotationY(const float y)
{
	Matrix4 result
	{
		cosf(y), 0.0f, -sinf(y), 0.0f,
		0.0f,	 1.0f, 0.0f,	 0.0f,
		sinf(y), 0.0f, cosf(y),  0.0f,
		0.0f,	 0.0f, 0.0f,	 1.0f
	};

	return result;
}

Matrix4 Matrix4::RotationZ(const float z)
{
	Matrix4 result
	{
		cosf(z),  sinf(z),  0.0f, 0.0f,
		-sinf(z), cosf(z),  0.0f, 0.0f,
		0.0f,	  0.0f,		1.0f, 0.0f,
		0.0f,	  0.0f,		0.0f, 1.0f
	};

	return result;
}

Matrix4 Matrix4::RotationXYZ(const Vector3 &rot)
{
	Matrix4 result = Identity();

	result *= RotationX(rot.x);
	result *= RotationY(rot.y);
	result *= RotationZ(rot.z);

	return result;
}

Matrix4 Matrix4::Translation(const Vector3 &pos)
{
	Matrix4 result
	{
		1.0f,  0.0f,  0.0f,  0.0f,
		0.0f,  1.0f,  0.0f,  0.0f,
		0.0f,  0.0f,  1.0f,  0.0f,
		pos.x, pos.y, pos.z, 1.0f
	};

	return result;
}

Matrix4 &operator*=(Matrix4 &m1, const Matrix4 &m2)
{
	Matrix4 result{};

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}

	m1 = result;
	return m1;
}

Matrix4 operator*(Matrix4 &m1, Matrix4 &m2)
{
	Matrix4 result = m1;

	return result *= m2;
}
