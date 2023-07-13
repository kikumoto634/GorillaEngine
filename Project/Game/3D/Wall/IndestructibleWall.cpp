#include "IndestructibleWall.h"
#include "SphereCollider.h"
#include "CollisionManager.h"

using namespace DirectX;

IndestructibleWall::~IndestructibleWall()
{
	//Finalize();
}

void IndestructibleWall::Initialize(ObjModelManager* model, ObjModelManager* collider)
{
	BaseObjObject::Initialize(model);

	isAlive_ = true;

	colliderModel_ = collider;
	ColliderInitialize();
}

void IndestructibleWall::Update(Camera *camera)
{
	this->camera_ = camera;

	isReflect_ = false;
	if(!isAlive_) return;
	Vector3 pos = PlayerPos_ - world_.translation;
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

void IndestructibleWall::Draw()
{
	if(!isAlive_) return;
	if(!isHide_) return;
	BaseObjObject::Draw();
}

void IndestructibleWall::OnCollision(const CollisionInfo &info)
{
	if(!isAlive_) return;
	if(!isHide_) return;

}

void IndestructibleWall::ContactUpdate()
{
	if(isReflect_)return;
	isReflect_ = true;
}

void IndestructibleWall::ColliderInitialize()
{
	//コライダー追加
	collider_ = new MeshCollider;
}

void IndestructibleWall::ColliderSet()
{
	//属性セット
	SetCollider(collider_);
	collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	collider_->ConstructTriangles(colliderModel_);
}

void IndestructibleWall::ColliderRemove()
{
	if(!collider_) return;
	if(!isCollision_) return;

	//コリジョンマネージャーから登録を解除する
	CollisionManager::GetInstance()->RemoveCollider(collider_);
	isCollision_ = false;
}
