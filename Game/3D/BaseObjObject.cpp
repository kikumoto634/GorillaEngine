#include "BaseObjObject.h"
#include "../Collision/CollisionSystem/BaseCollider.h"
#include "../Collision/CollisionSystem/CollisionManager.h"

BaseObjObject::~BaseObjObject()
{
	if(collider){
		//コリジョンマネージャーから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
}

void BaseObjObject::Initialize(std::string filePath, bool IsSmmothing)
{
	model = new ObjModelManager();
	model->CreateModel(filePath, IsSmmothing);
	object = ObjModelObject::Create(model);
	world.Initialize();

	//クラス名の文字列を取得
	name = typeid(*this).name();
}

void BaseObjObject::Update(Camera *camera)
{
	this->camera = camera;
	world.UpdateMatrix();
	object->Update(world, this->camera);

	//当たり判定更新
	if(collider){
		collider->Update();
	}
}

void BaseObjObject::Draw()
{
	object->Draw();
}

void BaseObjObject::Finalize()
{
	delete model;
	model = nullptr;
	delete object;
	object = nullptr;
	world = {};
}

void BaseObjObject::SetCollider(BaseCollider *collider)
{
	collider->SetObjObject(this);
	this->collider = collider;
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//行列の更新
	world.UpdateMatrix();
	//コライダーを更新しておく
	collider->Update();
}
