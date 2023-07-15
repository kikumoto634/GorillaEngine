#include "Camera.h"

#include <DirectXMath.h>
using namespace DirectX;

Camera *Camera::GetInstance()
{
	static Camera instance;
	return &instance;
}

void Camera::Initialize(Window* window)
{
	this->window = window;

	//カメラ
	view.eye = {0, 0, -distance};
	view.target = {0, 0, 0};
	view.up = {0, 1, 0};
	/*world.translation = {0,0,-distance};
	world.rotation = {0,0,0};
	world.Initialize();*/


	view.matView = view.matViewProjection = view.matView * view.matProjection;
	view.UpdateViewMatrix();
	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
}

void Camera::Update()
{
	Shake();
	view.matViewProjection = view.matView * view.matProjection;

	//view.UpdateViewMatrix();
}

void Camera::Movement(Vector3 move)
{
	
}





void Camera::MoveEyeVector(Vector3 move)
{
	Vector3 eye_= GetEye();
	eye_ += move;
	SetEye(eye_);
	view.UpdateViewMatrix();
}

void Camera::MoveTargetVector(Vector3 move)
{
	Vector3 target_= GetTarget();
	target_ += move;
	SetTarget(target_);
	view.UpdateViewMatrix();
}

void Camera::MoveVector(Vector3 move)
{
	Vector3 eye_ = GetEye();
	Vector3 target_ = GetTarget();
	eye_ += move;
	target_ += move;
	SetEye(eye_);
	SetTarget(target_);
	view.UpdateViewMatrix();
}

void Camera::RotVector(Vector3 rot)
{
	rot_ = {DirectX::XMConvertToRadians(rot.x),DirectX::XMConvertToRadians(rot.y),DirectX::XMConvertToRadians(rot.z)};

	// 追加回転分の回転行列を生成
	Matrix4x4 matRotNew = MakeIdentityMatrix();
	matRotNew *= MakeRotationZMatrix(-rot_.z);
	matRotNew *= MakeRotationXMatrix(-rot_.x);
	matRotNew *= MakeRotationYMatrix(-rot_.y);
	// 累積の回転行列を合成
	// ※回転行列を累積していくと、誤差でスケーリングがかかる危険がある為
	// クォータニオンを使用する方が望ましい
	matRot = matRotNew * matRot;

	// 注視点から視点へのベクトルと、上方向ベクトル
	Vector4 vTargetEye = {0.0f, 0.0f, -distance, 1.0f};
	Vector4 vUp = {0.0f, 1.0f, 0.0f, 0.0f};

	// ベクトルを回転
	vTargetEye.TransformVector3(vTargetEye, matRot);
	vUp.TransformVector3(vUp, matRot);

	// 注視点からずらした位置に視点座標を決定
	const Vector3& target = GetTarget();
	SetEye(
		{target.x + vTargetEye.x, target.y + vTargetEye.y,
		target.z + vTargetEye.z});
	SetUp({vUp.x, vUp.y, vUp.z});
	view.UpdateViewMatrix();
}

void Camera::Tracking(Vector3 target, bool isActive)
{
	if(isActive) return;

	Vector3 cameraPosXZ = view.eye - view.target;
	float height = cameraPosXZ.y;
	cameraPosXZ.y = 0.0f;
	float cameraPosXZLen = cameraPosXZ.length();
	cameraPosXZ.normalize();

	Vector3 ltarget = target;
	ltarget.y += 0.0f;

	Vector3 newCameraPos = view.target - ltarget;
	newCameraPos.y = 50.0f;
	newCameraPos.normalize();

	float weight = 0.0f;
	newCameraPos = newCameraPos * weight + cameraPosXZ * (1.0f - weight);
	newCameraPos.normalize();
	newCameraPos *= cameraPosXZLen;
	newCameraPos.y = height;
	Vector3 pos = ltarget + newCameraPos;

	view.target = ltarget;
	view.eye = pos;
	ViewUpdate();
}

void Camera::ShakeStart(int MaxFrame)
{
	ShakeFrame = MaxFrame;

	if(!IsShake){
		saveTarget = view.target;
		saveEye = view.eye;
	}

	IsShake = true;
}

void Camera::Reset()
{
	//カメラ
	RotVector({XMConvertToRadians(60.f), 0.f, 0.f});
	view.UpdateViewMatrix();
}

void Camera::Shake()
{
	if(!IsShake) return;

	if(frame >= ShakeFrame){
		view.target = saveTarget;
		view.eye = saveEye;
		frame = 0;
		IsShake = false;
		return;
	}

	view.target = saveTarget;
	view.eye = saveEye;

	Vector3 temp = {static_cast<float>(rand()%2-1),static_cast<float>(rand()%2-1),static_cast<float>(rand()%2-1)};
	MoveVector(temp);

	frame++;
	view.UpdateViewMatrix();
}


