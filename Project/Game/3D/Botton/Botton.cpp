#include "Botton.h"
#include "SphereCollider.h"
#include "CollisionManager.h"


Botton::~Botton()
{
	audio_->Finalize();
}

void Botton::Initialize(std::string filePath, bool IsSmoothing)
{
	BaseObjObject::Initialize(filePath, IsSmoothing);
	isAlive_ = true;

	ColliderInitialize();
	IntervalNumSet();

	audio_ = Audio::GetInstance();
}

void Botton::Update(Camera *camera, Vector3 playerPos)
{
	this->camera_ = camera;
	this->playerPos_ = playerPos;

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
	
	ReturnButton();

	if(!isHide_) return;
	BaseObjObject::Update(this->camera_);
}

void Botton::Draw()
{
	if(!isAlive_) return;
	if(!isHide_) return;
	BaseObjObject::Draw();
}

void Botton::OnCollision(const CollisionInfo &info)
{
	if(!isAlive_) return;
	if(!isHide_) return;	

	if(info.collider->GetAttribute() == COLLISION_ATTR_ALLIES){
		ContactUpdate();
	}
}

void Botton::ContactUpdate()
{
	if(isPush_) return;
	audio_->PlayWave(push_audio.number, push_audio.volume);
	isPush_ = true;

	Vector3 lpos = GetPosition();
	lpos.y += PushDownPosY;
	SetPosition(lpos);

	if(isBeatEnd_){
		RhythmChange();
	}
}

void Botton::ReturnButton()
{
	if(!isPush_)return;

	if(isBeatEnd_) {
		intervalBeatCount++;
		isBeatEnd_ = false;
	};

	if(intervalBeatCount >= intervalBeatCountMax){
		intervalBeatCount = 0;
		audio_->PlayWave(push_audio.number, push_audio.volume);
		isPush_ = false;

		Vector3 lpos = GetPosition();
		lpos.y -= PushDownPosY;
		SetPosition(lpos);

		RhythmNormalChange();
	}
}

void Botton::RhythmNormalChange()
{
	rhythm_->BPMNormalSet();
	audio_->SetRation(bpm120Game_audio.number, 1.0f);
}

void Botton::ColliderInitialize()
{
	//コライダー追加
	collider_ = new MeshCollider;
}

void Botton::ColliderSet()
{
	SetCollider(collider_);
	//属性セット
	collider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	collider_->ConstructTriangles(model_);
}

void Botton::ColliderRemove()
{
	if(!collider_) return;
	//コリジョンマネージャーから登録を解除する
	CollisionManager::GetInstance()->RemoveCollider(collider_);
}

void Botton::IntervalNumSet()
{
}

void Botton::RhythmChange()	{}
