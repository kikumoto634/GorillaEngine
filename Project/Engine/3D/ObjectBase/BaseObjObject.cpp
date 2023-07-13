#include "BaseObjObject.h"
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "Easing.h"

BaseObjObject::~BaseObjObject()
{
	//if(collider){
	//	//コリジョンマネージャーから登録を解除する
	//	CollisionManager::GetInstance()->RemoveCollider(collider);
	//	delete collider;
	//}
}

void BaseObjObject::Initialize(std::string filePath, bool IsSmmothing)
{
	model_ = new ObjModelManager();
	model_->CreateModel(filePath, IsSmmothing);
	object_ = ObjModelObject::Create(model_);
	world_.Initialize();
	world_.UpdateMatrix();

	//クラス名の文字列を取得
	name = typeid(*this).name();
}

void BaseObjObject::Initialize(ObjModelManager *model)
{
	isLendModel_ = true;
	this->model_ = model;
	object_ = ObjModelObject::Create(model);
	world_.Initialize();
	world_.UpdateMatrix();

	//クラス名の文字列を取得
	name = typeid(*this).name();
}


void BaseObjObject::Update(Camera *camera)
{
	this->camera_ = camera;
	world_.UpdateMatrix();
	object_->Update(world_, this->camera_);

	//当たり判定更新
	if(baseCollider_){
		baseCollider_->Update();
	}
}

void BaseObjObject::Draw()
{
	object_->Draw();
}

void BaseObjObject::Finalize()
{
	if(baseCollider_){
		//コリジョンマネージャーから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(baseCollider_);
		delete baseCollider_;
	}

	if(!isLendModel_){
		delete model_;
		model_ = nullptr;
	}

	delete object_;
	object_ = nullptr;

	world_ = {};
}

void BaseObjObject::Pop(Vector3 pos)
{
	SetPosition(pos);
	isPopsPosibble_ = false;
}

void BaseObjObject::ContactUpdate()
{
}

bool BaseObjObject::GetIsContactTrigger()
{
	if(isContactTrigger_){
		isContactTrigger_ = false;
		return true;
	}
	return false;
}

void BaseObjObject::SetPosition(const Vector3 &position)
{
	world_.translation = position;
	world_.UpdateMatrix();
}

void BaseObjObject::SetCollider(BaseCollider *collider)
{
	collider->SetObjObject(this);
	this->baseCollider_ = collider;
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//行列の更新
	world_.UpdateMatrix();
	//コライダーを更新しておく
	collider->Update();
}

bool BaseObjObject::ScaleChange(Vector3 &sizeMax, Vector3 &sizeMin, const float &EndTime)
{
	float ease = -(cosf(3.14159265f * scaleCurrentTime_) - 1.f)/2.f;
	scale_ = Easing_Point2_Linear(sizeMin, sizeMax, ease);
	SetScale(scale_);

	if(scaleCurrentTime_ >= 1.0f){
		scale_ = scaleMax_;
		scaleCurrentTime_ = 0.f;
		return true;
	}

	scaleCurrentTime_ += 1.f/(60*EndTime);
	return false;
}

void BaseObjObject::DistanceUpdate()
{
	if(isLightCal_){
		object_->OnLighting();
		return;
	}
	
	object_->OffLighting();
}
