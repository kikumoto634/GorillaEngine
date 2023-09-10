#pragma once
#include "Window.h"
#include "Vector3.h"

#include "WorldTransform.h"
#include "ViewProjection.h"

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
	void Initialize(Window* window);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	//移動(移動量)
	void Movement(Vector3 move){
		world.translation += move;
		world.UpdateMatrix();
	}
	//回転
	void Rotation(Vector3 rot){
		world.rotation += rot;
		world.UpdateMatrix();
	}
	// カメラ追従
	void Tracking(Vector3 target, Vector3 offset);

	//シェイク
	void ShakeStart();

#pragma region Getter
	//名前
	const char* GetName()	{return name;}

	//座標
	const Vector3 GetPosition()	{return world.translation;}
	//回転
	const Vector3 GetRotation()	{return world.rotation;}

	const XMMATRIX& GetMatProjection()	{return view.matProjection;}
	const XMMATRIX& GetMatView()	{return view.matView;}
	const XMMATRIX& GetViewProjectionMatrix()	{return view.matViewProjection;}

	const Vector3& GetEye() {return view.eye; }
	const Vector3& GetTarget() {return view.target; }
	const Vector3& GetUp() {return view.up; }
	const DirectX::XMMATRIX& GetBillboard() {return view.matBillboard;}

	const float GetShakeMaxFrame()	{return shakeMaxFrame;}
	const int GetShakeMaxPower()	{return shakeMaxPower;}
#pragma endregion

#pragma region Setter
	//座標
	void SetPosition(const Vector3& pos)	
	{
		world.translation = pos;
		world.UpdateMatrix();
	}
	//回転
	void SetRotation(const Vector3& rot){
		world.rotation = rot;
		world.UpdateMatrix();
	}

	void SetEye(const Vector3& eye)	{this->view.eye = eye; }
	void SetTarget(const Vector3& target)	{this->view.target = target; }
	void SetUp(const Vector3& up)	{this->view.up = up; }

	void SetShake(float frame, int power)	{shakeMaxFrame = frame, shakeMaxPower = power;}
#pragma endregion

private:
	void Shake();

private:
	//アスペクト用
	Window* window;

	//クラス名(デバック用)
	const char* name = nullptr;
	
	float distance = 2.f;	//カメラの距離
	//回転行列
	XMMATRIX matRot = DirectX::XMMatrixIdentity();


	bool isShake = false;
	Vector3 savePos = {};
	
	float shakeMaxFrame = 60;
	float shakeFrame = 0;

	int shakeMaxPower = 10;
	int shakePower = 0;

protected:

	WorldTransform world;
	ViewProjection view;

};

