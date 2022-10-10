#pragma once
#include "Window.h"
#include "Vector3.h"

#include <DirectXMath.h>

/// <summary>
/// カメラ
/// </summary>
class Camera
{
public:
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT3 = DirectX::XMFLOAT3;

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

	void UpdateViewMatrix();

	void UpdateProjectionMatrix();


	//getter
	const XMMATRIX& GetMatProjection()	{return matProjection;}
	const XMMATRIX& GetMatView()	{return matView;}
	const XMMATRIX& GetViewProjectionMatrix()	{return matViewProjection;}


	const Vector3& GetEye() {return eye; }
	const Vector3& GetTarget() {return target; }
	const Vector3& GetUp() {return up; }

	//setter
	void SetEye(const Vector3& eye)	{this->eye = eye; }
	void SetTarget(const Vector3& target)	{this->target = target; }
	void SetUp(const Vector3& up)	{this->up = up; }

	void CameraMovement(XMFLOAT3 pos);

private:
	//アスペクト用
	Window* window;

protected:

	//透視投影
	XMMATRIX matProjection;	//プロジェクション行列
	//ビュー変換行列
	XMMATRIX matView;		//ビュー行列
	Vector3 eye;			//視点座標
	Vector3 target;		//注視点座標
	Vector3 up;			//上方向ベクトル

	XMMATRIX matViewProjection;
};

