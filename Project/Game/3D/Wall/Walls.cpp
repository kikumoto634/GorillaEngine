#include "Walls.h"
#include "SphereCollider.h"
#include "CollisionManager.h"

using namespace DirectX;

Walls::~Walls()
{
}

void Walls::Initialize(ObjModelManager* model, ObjModelManager* collider)
{
	BaseObjObject::Initialize(model);

	isAlive_ = true;

	colliderModel_ = collider;
	ColliderInitialize();
}

void Walls::Update(Camera *camera)
{
	this->camera_ = camera;

	isDig_ = false;
	if(!isAlive_) return;
	Vector3 pos = playerPos_ - world_.translation;
	distance_ = pos.length();

	if(isLightCal_){
		if(-DrawingRange_Half <= distance_ && distance_ <= DrawingRange_Half){
			object_->OnLighting();
		}
		else if(-DrawingRange_Half > distance_ || distance_ > DrawingRange_Half){
			object_->OffLighting();
		}
	}
	else if(!isLightCal_){
		object_->OnLighting();
	}

	if(-DrawingRange <= distance_ && distance_ <= DrawingRange)		{
		isHide_ = true;

		if(!isCollision_){
			ColliderSet();
			isCollision_ = true;
		}
	}
	else if(-DrawingRange > distance_ || distance_ > DrawingRange){
		isHide_ = false;

		if(isCollision_){
			ColliderRemove();
			isCollision_ = false;
		}
	}
	

	if(!isHide_) return;
	BaseObjObject::Update(this->camera_);
}

void Walls::Draw()
{
	if(!isAlive_) return;
	if(!isHide_) return;
	BaseObjObject::Draw();
}

void Walls::OnCollision(const CollisionInfo &info)
{
	if(!isAlive_) return;
	if(!isHide_) return;	
}

void Walls::ContactUpdate()
{
	isAlive_ = false;
	isDig_ = true;
	digPosition_ = GetPosition();
	world_.translation = NotAlivePos;
	world_.UpdateMatrix();
	collider_->Update();
}

void Walls::ColliderInitialize()
{
	//コライダー追加
	collider_ = new MeshCollider;
}

void Walls::ColliderSet()
{
	SetCollider(collider_);
	//属性セット
	collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	collider_->ConstructTriangles(colliderModel_);
}

void Walls::ColliderRemove()
{
	if(!collider_) return;
	//コリジョンマネージャーから登録を解除する
	CollisionManager::GetInstance()->RemoveCollider(collider_);
}

