#pragma once
class Vector3
{
public:
	float x;
	float y;
	float z;

public:
	Vector3();
	Vector3(float x, float y, float z);

	float length();
	Vector3 normalize();
	float dot(const Vector3& v) const;
	Vector3 cross(const Vector3& v) const;

	Vector3 operator+() const;
	Vector3 operator-() const;

	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);

	bool operator==(const Vector3 v)
	{
		bool is_hit = false;
		is_hit = (x==v.x&&y==v.y&&z==v.z);
		return is_hit;
	}
};

const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);