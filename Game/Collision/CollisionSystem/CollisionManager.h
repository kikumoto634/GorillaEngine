#pragma once
#include "CollisionPrimitive.h"
#include "../RaycastHit.h"

#include <d3d12.h>
#include <forward_list>

class BaseCollider;


class CollisionManager
{
//静的メンバ関数
public:
	static CollisionManager* GetInstance();

//メンバ関数
public:
	
	/// <summary>
	/// コライダーの追加
	/// </summary>
	/// <param name="collider">コライダー</param>
	inline void AddCollider(BaseCollider* collider){
		colliders.push_front(collider);
	}

	/// <summary>
	/// コライダー削除
	/// </summary>
	/// <param name="collider">コライダー</param>
	inline void RemoveCollider(BaseCollider* collider){
		colliders.remove(collider);
	}

	/// <summary>
	/// レイキャスト
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="hitinfo">衝突情報</param>
	/// <param name="maxDistance">最大距離</param>
	/// <returns>レイが任意のコライダーと交わる場合true, それ以外はfalse</returns>
	bool Raycast(const Ray& ray, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// すべての衝突チェック
	/// </summary>
	void CheckAllCollisions();

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
	//コライダーのリスト
	std::forward_list<BaseCollider*> colliders;
};

