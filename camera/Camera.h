#pragma once
#include "../Engine/base/Window.h"
#include "../Engine/math/Vector/Vector3.h"

#include "../Engine/3D/ViewProjection.h"

#include <DirectXMath.h>

/// <summary>
/// カメラ
/// </summary>
class Camera
{
protected:
	using XMMATRIX = DirectX::XMMATRIX;

public:
	static Camera* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 注視点移動
	/// </summary>
	/// <param name="move">移動量(radianを設定)</param>
	void MoveEyeVeector(Vector3 move);

	/// <summary>
	/// カメラ移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveVector(Vector3 move);

	//getter
	const XMMATRIX& GetMatProjection()	{return view.matProjection;}
	const XMMATRIX& GetMatView()	{return view.matView;}
	const XMMATRIX& GetViewProjectionMatrix()	{return view.matViewProjection;}


	const Vector3& GetEye() {return view.eye; }
	const Vector3& GetTarget() {return view.target; }
	const Vector3& GetUp() {return view.up; }
	const DirectX::XMMATRIX& GetBillboard() {return view.matBillboard;}

	//setter
	void SetEye(const Vector3& eye)	{this->view.eye = eye; }
	void SetTarget(const Vector3& target)	{this->view.target = target; }
	void SetUp(const Vector3& up)	{this->view.up = up; }

private:
	//アスペクト用
	Window* window;

protected:
	ViewProjection view;
};

