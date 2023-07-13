#include "BaseEnemy.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

using namespace std;

void BaseEnemy::Initialize(std::string filePath, bool IsSmoothing)
{
	BaseObjObject::Initialize(filePath, IsSmoothing);

	isAlive_ = false;

	//拡縮最小値
	scaleMin_ = SizeMin;

    //コライダー
    ColliderInitialize();

    //パーティクル
    ParticleInitialize();

	AddInitialize();
}

void BaseEnemy::Update(Camera *camera, const Vector3 &playerPos)
{
	this->camera_ = camera;
	playerPos_ = playerPos;

	AddUpdate();

    if(!isAlive_) return;
	//距離に応じた更新
	DistanceUpdate();
	//アクション更新
	ActionUpdate();
	//ビート更新
	BeatUpdate();

	//高さ
	if(world_.translation.y != PositionY){
		world_.translation.y = PositionY;
	}

	BaseObjObject::Update(camera);
}

void BaseEnemy::Draw()
{
	if(isPopsPosibble_ || isInvisible_ || !isAlive_) return;

	AddDraw();

	BaseObjObject::Draw();
}

void BaseEnemy::ParticleDraw()
{
	AddParticleDraw();
}

void BaseEnemy::Finalize()
{
	AddFinalize();

	deadParticle_->Finalize();
    popParticle_->Finalize();

	BaseObjObject::Finalize();
}

void BaseEnemy::OnCollision(const CollisionInfo &info)
{
	if(!isAlive_) return;
    if(info.collider->GetAttribute() == COLLISION_ATTR_ALLIES){
        ContactUpdate();
    }
}

void BaseEnemy::Pop(Vector3 pos)
{
	particlePos_ = {pos.x, PositionY, pos.z};
	isPopsPosibble_ = false;
}

void BaseEnemy::ContactUpdate()
{
	AddContactUpdate();
}


void BaseEnemy::ActionUpdate()
{
	//拡縮
	if(isScaleChange_ && ScaleChange(scaleMax_, scaleMin_, ScaleEndTime)){
		isScaleChange_ = false;
	}

    //コライダー
    if(baseCollider_){
        sphereCollider_->Update();
    }
}

void BaseEnemy::BeatUpdate()
{
	if(!isBeatEnd_) return;
    isBeatEnd_ = false;

    //拡縮
    isScaleChange_ = true;
}

void BaseEnemy::DistanceUpdate()
{
	//距離
	Vector3 sub = playerPos_ - world_.translation;
	distance_ = sub.length();

	//光計算
	if(isLightCal_){
		//距離範囲内
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

	//範囲が非表示
	if(-DrawingRange_Not <= distance_ && distance_ <= DrawingRange_Not)		{
		isInvisible_ = false;
	}
	else if(-DrawingRange_Not > distance_ || distance_ > DrawingRange_Not)	{
		isInvisible_ = true;
	}
}

void BaseEnemy::ColliderInitialize()
{
	SetCollider(new SphereCollider(XMVECTOR{0,0,0,0}, colliderRadius_));
    baseCollider_->SetAttribute(COLLISION_ATTR_ENEMYS);
    //球コライダー取得
	sphereCollider_ = dynamic_cast<SphereCollider*>(baseCollider_);
	assert(sphereCollider_);
    baseCollider_->Update();
}

void BaseEnemy::ColliderSet()
{
	SetCollider(new SphereCollider(XMVECTOR{0,0,0,0}, colliderRadius_));
	baseCollider_->SetAttribute(COLLISION_ATTR_ENEMYS);
}

void BaseEnemy::ColliderRemove()
{
	//コリジョンマネージャーから登録を解除する
	CollisionManager::GetInstance()->RemoveCollider(baseCollider_);
}

void BaseEnemy::ParticleInitialize()
{
	deadParticle_ = make_unique<ParticleObject>();
    deadParticle_->Initialize();

    popParticle_ = make_unique<ParticleObject>();
    popParticle_->Initialize();
}
