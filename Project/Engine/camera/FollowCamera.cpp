#include "FollowCamera.h"
#include "Utility.h"

using namespace DirectX;

FollowCamera *FollowCamera::GetInstance()
{
	static FollowCamera instance;
	return &instance;
}

void FollowCamera::Initialize(Vector3 pos, Vector3 rot)
{
	input_ = Input::GetInstance();

	Camera::Initialize(pos,rot);

	rotation = {world.rotation.y, world.rotation.x};
}

void FollowCamera::Update(WorldTransform target, Vector3 offset)
{
	target_ = target.translation;
	offset_ = offset;

	Rotation();
	Movement();

	world.UpdateMatrix();

	Camera::Update();
}

void FollowCamera::Movement()
{
	Vector3 cameraPosXZ = view.eye - view.target;
	float height = cameraPosXZ.y;
	cameraPosXZ.y = 0.0f;
	float cameraPosXZLen = cameraPosXZ.length();
	cameraPosXZ.normalize();

	Vector3 ltarget = target_;
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

	SetPosition(pos + offset_);
}

void FollowCamera::Rotation()
{
	if(!input_->GetIsPadConnect()) return;

	float speed = 0.05f;
	
	rotation.x += input_->PadRStick().x * speed;
	rotation.y -= input_->PadRStick().y * speed;
	world.rotation.y = rotation.x;
	world.rotation.x = rotation.y;

	//回転行列
	XMMATRIX matRot;
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(world.rotation.z);
	matRot *= XMMatrixRotationX(world.rotation.x);
	matRot *= XMMatrixRotationY(world.rotation.y);

	offset_ = TransformNormal(offset_, matRot);

	world.UpdateMatrix();
}
