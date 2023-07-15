#include "Vector4.h"
#include <cmath>

Vector4::Vector4():
	x(0),y(0),z(0),w(0)
{
}

Vector4::Vector4(float x, float y, float z, float w):
	x(x),y(y),z(z),w(w)
{
}

Vector4::Vector4(Vector3 v, float w):
	x(v.x),y(v.y),z(v.z),w(w) 
{
}

float Vector4::LengthVector3()
{
	return (float)sqrt(x*x + y*y + z*z);
}

Vector4 Vector4::NormalizeVector3()
{
	float len = LengthVector3();
	if(len != 0)
	{
		return *this /= len;
	}
	return *this;
}


Vector4 Vector4::TransformVector3(Vector4 v, Matrix4x4 m)
{
	Vector4 temp;
	temp.x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3];
	temp.y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3];
	temp.z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3];
	temp.w = 1.0f;

	return Vector4();
}

Vector4 Vector4::TransformNormalizeVector3(Vector4 v, Matrix4x4 m)
{
	Vector4 temp;
	temp.x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z;
	temp.y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z;
	temp.z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z;
	temp.w = 0.0f;

	return Vector4();
}

float Vector4::DotVector3(Vector4 v) const
{
	return x*v.x + y*v.y + z*v.z;
}

Vector4 Vector4::CorssVector3(Vector4 v) const
{
	Vector4 temp;
	temp.x = this->y * v.z - this->z * v.y;
	temp.y = this->z * v.x - this->x * v.z;
	temp.z = this->x * v.y - this->y * v.x;
	temp.w = 0;
	return temp;
}

Vector4 Vector4::Lerp(const Vector4 &v1, const Vector4 &v2, float t)
{
	Vector4 temp = {};
	temp.x = (1.f-t)*v1.x + t*v2.x;
	temp.y = (1.f-t)*v1.y + t*v2.y;
	temp.z = (1.f-t)*v1.z + t*v2.z;
	temp.w = (1.f-t)*v1.w + t*v2.w;
	return temp; 
}

Vector4 Vector4::operator+() const
{
	return *this;
}

Vector4 Vector4::operator-() const
{
	return Vector4(-x,-y,-z,-w);
}

Vector4 &Vector4::operator+=(const Vector4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

Vector4 &Vector4::operator-=(const Vector4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

Vector4 &Vector4::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Vector4 &Vector4::operator*=(const Vector4 &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
	return *this;
}

Vector4 &Vector4::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}

const Vector4 operator+(const Vector4 &v1, const Vector4 &v2)
{
	Vector4 temp(v1);
	return temp += v2;
}

const Vector4 operator-(const Vector4 &v1, const Vector4 &v2)
{
	Vector4 temp(v1);
	return temp -= v2;
}

const Vector4 operator*(const Vector4 &v, float s)
{
	Vector4 temp(v);
	return temp *= s;
}

const Vector4 operator*(float s, const Vector4 &v)
{
	return v * s;
}

const Vector4 operator*(const Vector4 &v1, const Vector4 &v2)
{
	Vector4 temp(v1);
	return temp *= v2;
}

const Vector4 operator/(const Vector4 &v, float s)
{
	Vector4 temp(v);
	return temp /= s;
}
