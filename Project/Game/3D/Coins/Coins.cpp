#include "Coins.h"
#include "SphereCollider.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"


void Coins::Initialize(std::string filePath, bool IsSmoothing)
{
	BaseObjObject::Initialize(filePath, IsSmoothing);

	SetPosition(DeadPos);
	
	//コライダー
	ColliderSet();
}

void Coins::Update(Camera *camera)
{
	this->camera_ = camera;

	if(!isAlive_)return;

	//ビート処理
	if(isBeatEnd_){
		
		if(loatCount_ >= LostCountMax){
			loatCount_ = 0;
			isAlive_ = false;
		}

		loatCount_++;
		isBeatEnd_ = false;
	}

	//コライダー更新
	baseCollider_->Update();
	BaseObjObject::Update(this->camera_);
}

void Coins::Draw()
{
	if(!isAlive_) return;

	BaseObjObject::Draw();
}


void Coins::OnCollision(const CollisionInfo &info)
{
	if(!isAlive_) return;

	if(info.collider->GetAttribute() == COLLISION_ATTR_ALLIES){
		ContactUpdate();
	}
}


void Coins::Pop(Vector3 pos)
{
	BaseObjObject::Pop({pos.x,PositionY,pos.z});
	isAlive_ = true;
	loatCount_ = 0;
}


void Coins::ColliderSet()
{
	SetCollider(new SphereCollider(DirectX::XMVECTOR{0,SphereColliderRadius,0,0}, SphereColliderRadius));
	baseCollider_->SetAttribute(COLLISION_ATTR_ITEMS);
}

void Coins::ContactUpdate()
{
	isContactTrigger_ = true;
	isPopsPosibble_ = true;
	SetPosition(DeadPos);
	world_.UpdateMatrix();
	baseCollider_->Update();
	isAlive_ = false;
}
