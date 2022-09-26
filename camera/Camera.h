#pragma once

#include "Window.h"
#include "ViewProjection.h"

/// <summary>
/// ÉJÉÅÉâ
/// </summary>
class Camera
{
public:
	using XMMATRIX = DirectX::XMMATRIX;

public:

	static Camera* GetInstance();

	/// <summary>
	/// èâä˙âª
	/// </summary>
	void Initialize();

	/// <summary>
	/// çXêV
	/// </summary>
	virtual void Update();


	//getter
	const XMMATRIX& GetMatProjection()	{return viewProjection.matProjection;}
	const XMMATRIX& GetMatView()	{return viewProjection.matView;}
	const XMMATRIX& GetViewProjectionMatrix()	{return matViewProjection;}

	const Vector3& GetEye() {return viewProjection.eye; }
	const Vector3& GetTarget() {return viewProjection.target; }
	const Vector3& GetUp() {return viewProjection.up; }

	//setter
	void SetEye(const Vector3& eye)	{viewProjection.eye = eye; }
	void SetTarget(const Vector3& target)	{viewProjection.target = target; }
	void SetUp(const Vector3& up)	{viewProjection.up = up; }

protected:

	ViewProjection viewProjection;

	XMMATRIX matViewProjection;
};

