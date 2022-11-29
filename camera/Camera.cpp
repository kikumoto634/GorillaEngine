#include "Camera.h"

using namespace DirectX;

Camera *Camera::GetInstance()
{
	static Camera instance;
	return &instance;
}

void Camera::Initialize()
{
	window = Window::GetInstance();

	//カメラ
	view.eye = {0, 0, -distance};
	view.target = {0, 0, 0};
	view.up = {0, 1, 0};


	view.UpdateViewMatrix();
	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
}

void Camera::Update()
{
	view.matViewProjection = view.matView * view.matProjection;

	view.UpdateViewMatrix();
	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
}

void Camera::MoveEyeVeector(Vector3 move)
{
	Vector3 eye_= GetEye();
	eye_ += move;
	SetEye(eye_);
}

void Camera::MoveVector(Vector3 move)
{
	Vector3 eye_ = GetEye();
	Vector3 target_ = GetTarget();
	eye_ += move;
	target_ += move;
	SetEye(eye_);
	SetTarget(target_);
}

void Camera::RotVector(Vector3 rot)
{
	// 追加回転分の回転行列を生成
	XMMATRIX matRotNew = XMMatrixIdentity();
	matRotNew *= XMMatrixRotationZ(-rot.z);
	matRotNew *= XMMatrixRotationX(-rot.x);
	matRotNew *= XMMatrixRotationY(-rot.y);
	// 累積の回転行列を合成
	// ※回転行列を累積していくと、誤差でスケーリングがかかる危険がある為
	// クォータニオンを使用する方が望ましい
	matRot = matRotNew * matRot;

	// 注視点から視点へのベクトルと、上方向ベクトル
	XMVECTOR vTargetEye = {0.0f, 0.0f, -distance, 1.0f};
	XMVECTOR vUp = {0.0f, 1.0f, 0.0f, 0.0f};

	// ベクトルを回転
	vTargetEye = XMVector3Transform(vTargetEye, matRot);
	vUp = XMVector3Transform(vUp, matRot);

	// 注視点からずらした位置に視点座標を決定
	const Vector3& target = GetTarget();
	SetEye(
		{target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1],
		target.z + vTargetEye.m128_f32[2]});
	SetUp({vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2]});
}


