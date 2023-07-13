#pragma once
#include "CollisionPrimitive.h"
#include "RaycastHit.h"
#include "QueryCallback.h"

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
	/// レイキャスト(属性あり)
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="attribute">対象の衝突属性</param>
	/// <param name="hitinfo">衝突情報</param>
	/// <param name="maxDistance">最大距離</param>
	/// <returns>レイが任意のコライダーと交わる場合true, それ以外はfalse</returns>
	bool Raycast(const Ray& ray, unsigned short attribute, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	/// <summary>
	/// レイキャスト(属性なし)
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

	/// <summary>
	/// 球による衝突全検索
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="callback">衝突時コールバック</param>
	/// <param name="attribute">対象の衝突属性</param>
	void QuerySphere(const Sphere& sphere, QueryCallback* callback, unsigned short attribute = (unsigned short)0xffffffff);

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
	//コライダーのリスト
	std::forward_list<BaseCollider*> colliders;
};

