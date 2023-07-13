#include "Exit.h"
#include "TextureUtility.h"

using namespace std;


void Exit::Initialize(std::string filePath, bool IsSmmothing)
{
	BaseObjObject::Initialize(filePath, IsSmmothing);

	SetModel(model_);

	//コライダー
	ColliderSet();

	//スプライト
	sp_ = make_unique<ExitSprite>();
	sp_->Initialize();

	//階段モデル
	stairsModel_ = new ObjModelManager();
	stairsModel_->CreateModel("Stairs");
}

void Exit::Update(Camera *camera)
{
	this->camera_ = camera;

	//拍終わり時
	if(isBeatEnd_){
		
		//プレイヤー接触時
		if(isPlayerContact_){
			//サイズ変更
			if(ScaleChange(scaleMax_, scaleMin_, ScaleEndTime)){
				isBeatEnd_ = false;
				isPlayerContact_ = false;
			}
		}
	}

	sp_->Update(isOpen_, isPlayerContact_);

	BaseObjObject::Update(this->camera_);
}


void Exit::Draw2D()
{
	sp_->Draw();
}

void Exit::Finalize()
{
	delete stairsModel_;
	stairsModel_ = nullptr;;

	sp_->Finalize();

	BaseObjObject::Finalize();
}

void Exit::OnCollision(const CollisionInfo &info)
{
	if(info.collider->GetAttribute() == COLLISION_ATTR_ALLIES){
		ContactUpdate();
	}
}

void Exit::ContactUpdate()
{
	isPlayerContact_ = true;
}


void Exit::ColliderSet()
{
	meshCollider_ = new MeshCollider;
	SetCollider(meshCollider_);
	//属性セット
	meshCollider_->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	meshCollider_->ConstructTriangles(model_);
}

