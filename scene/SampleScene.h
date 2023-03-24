#pragma once
#include "BaseScene.h"

#include "../Engine/3D/TouchableObject.h"

#include "../Game/3D/Player/Player.h"

#include "../Game/2D/SampleSprite/SampleSprite.h"

#include "../Game/Collision/CollisionSystem/CollisionPrimitive.h"
#include "../Game/Collision/CollisionSystem/CollisionManager.h"

class SampleScene : public BaseScene
{
public:
	SampleScene(DirectXCommon* dxCommon, Window* window);

	/// <summary>
	/// 起動時
	/// </summary>
	void Application() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UI描画
	/// </summary>
	void UIDraw();

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize() override;

	//シーン遷移
	void NextSceneChange() override;

private:
    //Collision
    CollisionManager* collisionManager = nullptr;

    //player
    std::unique_ptr<Player> player;

    //Plane
    static const int DIV_NUM = 10;
    static const int Plane_Size = 2;
    std::unique_ptr<TouchableObject> plane[DIV_NUM][DIV_NUM];

    //box
    std::unique_ptr<TouchableObject> box;

    //pyramid
    std::unique_ptr<TouchableObject> pyramid;
    
    //sphere
    std::unique_ptr<SampleObjObject> sphere;

#ifdef _DEBUG
	//カメラ移動、回転変更フラグ
	bool IsCameraMovementChange = false;
#endif // _DEBUG
};

