#include "TouchableObject.h"
#include "../../Game/Collision/MeshCollider.h"
#include "../../Game/Collision/CollisionSystem/CollisionAttribute.h"

void TouchableObject::Initialize(std::string filePath, bool IsSmmothing)
{
	BaseObjObject::Initialize(filePath, IsSmmothing);

	SetModel(model);

	//コライダーの追加
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider);
	collider->ConstructTriangles(model);
	//属性セット(自動的地形)
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
}
