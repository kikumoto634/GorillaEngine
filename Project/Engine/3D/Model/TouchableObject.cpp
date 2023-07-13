#include "TouchableObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"

void TouchableObject::Initialize(std::string filePath, bool IsSmmothing)
{
	BaseObjObject::Initialize(filePath, IsSmmothing);

	SetModel(model_);

	//コライダーの追加
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	//属性セット(自動的地形)
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	collider->ConstructTriangles(model_);
}
