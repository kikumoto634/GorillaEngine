#pragma once
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

