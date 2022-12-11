#include "CollisionManager.h"
#include "BaseCollider.h"
#include "Collision.h"

CollisionManager *CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	//すべての組合せについて総当たりチェック
	itA = colliders.begin();
	for(; itA != colliders.end(); ++itA){
		itB = itA;
		++itB;

		for(; itB != colliders.end(); ++itB){
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;

			//ともに球
			if(colA->GetShapeType() == COLLIDIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLIDIONSHAPE_SPHERE){
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Sphere* SphereB = dynamic_cast<Sphere*>(colB);
				DirectX::XMVECTOR inter;
				if(Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter)){
					colA->OnCollision(CollisionInfo(colB->GetObjObject(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObjObject(), colA, inter));
				}
			}
		}
	}
}
