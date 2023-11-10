#include "Action/Action.h"
#include "Utility.h"
#include <DirectXMath.h>
#include <cmath>

#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace std;
using namespace DirectX;

Action::~Action()
{
	CollisionManager::GetInstance()->RemoveCollider(meshCollider);
}

void Action::Initialize()
{
	input = Input::GetInstance();

	BaseObjObject::Initialize("sphere", false);

	/*meshCollider = new MeshCollider();
	SetCollider(meshCollider);
	meshCollider->SetAttribute(COLLISION_ATTR_ALLIES);
	meshCollider->ConstructTriangles(model_);*/

	SetPosition({0,0,-3});
}

void Action::Update(Camera *camera)
{
	this->camera_ = camera;
	Rotation();
	Movement();

	BaseObjObject::Update(this->camera_);
}

void Action::Rotation()
{
	//回転
	//解法1
	/*world_.rotation.y = atan2(moveValue.x, moveValue.z);
	XMMATRIX mat = XMMatrixRotationY(-world_.rotation.y);
	Vector3 velocityZ = TransformNormal(moveValue,mat);
	world_.rotation.x = atan2(-velocityZ.y, velocityZ.z);*/
	//解法2
	world_.rotation.y = std::atan2(moveValue.x, moveValue.z);
	Vector3 velocityXZ = Vector3{moveValue.x, 0, moveValue.z};
	world_.rotation.x = std::atan2(-moveValue.y, velocityXZ.length());
}

void Action::Movement()
{
	if(!input->GetIsPadConnect()) return;

	float speed = 0.5f;
	moveValue = {input->PadLStick().x, 0, input->PadLStick().y};

	moveValue = moveValue.normalize() * speed;

	//カメラの方向へと動く
	XMMATRIX matRot;
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(camera_->GetWorld().rotation.z);
	matRot *= XMMatrixRotationX(camera_->GetWorld().rotation.x);
	matRot *= XMMatrixRotationY(camera_->GetWorld().rotation.y);
	moveValue = TransformNormal(moveValue, matRot);

	//移動
	world_.translation+=moveValue;
}
