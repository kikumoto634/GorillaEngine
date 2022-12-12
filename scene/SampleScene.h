#pragma once
#include "BaseScene.h"
#include "../Engine/light/LightGroup.h"

#include "../Game/3D/SampleFbxObject/SampleFbxObject.h"
#include "../Game/3D/SampleObjObject/SampleObjObject.h"
#include "../Engine/base/ParticleManager.h"

#include "../Game/3D/Player/Player.h"

#include "../Game/2D/SampleSprite/SampleSprite.h"

#include "../Game/Collision/CollisionPrimitive.h"
#include "../Game/Collision/CollisionManager.h"

#ifdef _DEBUG
#include "../Engine/base/imguiManager.h"
#endif // _DEBUG


class SampleScene : public BaseScene
{
public:
	
	//コンストラクタ
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
	/// 後処理
	/// </summary>
	void Finalize() override;

	void CreateParticles();

private:
	float time = 0.f;

	//ライト
	LightGroup* lightGroup = nullptr;

	//丸影
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.6f,0.0f };
	float circleShadowFactorAngle[2] = { 0.0f, 0.5f };

	//衝突マネージャー
	CollisionManager* collisionManager = nullptr;

	//パーティクル
	ParticleManager* particle = nullptr;

	//3Dオブジェクト
	std::unique_ptr<Player> player;
	std::unique_ptr<SampleObjObject> obj3_2;
	std::unique_ptr<SampleObjObject> obj3_3;
	std::unique_ptr<SampleObjObject> obj3_4;
#ifdef _DEBUG
	imguiManager* imgui;

	bool show_demo_window = false;
#endif // _DEBUG
};
