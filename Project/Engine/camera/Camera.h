#pragma once
#include "Window.h"
#include "Vector3.h"

#include "WorldTransform.h"
#include "ViewProjection.h"

#include "Matrix4x4.h"

/// <summary>
/// カメラ
/// </summary>
class Camera
{
public:
	static Camera* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Window* window);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	//移動(移動量)
	void Movement(Vector3 move);

#pragma region Getter

#pragma endregion

#pragma region Setter

#pragma endregion






	/// <summary>
	/// 注視点移動
	/// </summary>
	/// <param name="move">移動量(radianを設定)</param>
	void MoveEyeVector(Vector3 move);

	/// <summary>
	/// 視点移動
	/// </summary>
	/// <param name="move">移動量(radianを設定)</param>
	void MoveTargetVector(Vector3 move);

	/// <summary>
	/// カメラ移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveVector(Vector3 move);

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="rot">回転量</param>
	void RotVector(Vector3 rot);

	/// <summary>
	/// カメラ追従
	/// </summary>
	/// <param name="target">ターゲット</param>
	void Tracking(Vector3 target, bool isActive);

	//シェイク
	void ShakeStart(int MaxFrame = 5);
	//シェイク停止
	void Reset();

	/// <summary>
	/// View更新
	/// </summary>
	inline void ViewUpdate()	{view.UpdateViewMatrix();}

	const Matrix4x4& GetMatProjection()	{return view.matProjection;}
	const Matrix4x4& GetMatView()	{return view.matView;}
	const Matrix4x4& GetViewProjectionMatrix()	{return view.matViewProjection;}

	const Vector3& GetEye() {return view.eye; }
	const Vector3& GetTarget() {return view.target; }
	const Vector3& GetUp() {return view.up; }
	const float& Distance() {return distance; }
	const Matrix4x4& GetBillboard() {return view.matBillboard;}

	void SetEye(const Vector3& eye)	{this->view.eye = eye; }
	void SetTarget(const Vector3& target)	{this->view.target = target; }
	void SetUp(const Vector3& up)	{this->view.up = up; }
	void SetDistance(const float& distance)	{this->distance = distance; }

private:
	void Shake();

private:
	//アスペクト用
	Window* window;

public:
	WorldTransform world;
	ViewProjection view;

	Vector3 pos_;
	Vector3 rot_;

protected:
	float distance = 20.f;	//カメラの距離

	//回転行列
	Matrix4x4 matRot = MakeIdentityMatrix();

	//シェイク
	bool IsShake = false;
	int shakePower = 1;
	int ShakeFrame = 5;
	int frame = 0;
	Vector3 saveTarget{};
	Vector3 saveEye{};
};

