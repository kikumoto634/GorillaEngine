#pragma once
#include "BaseScene.h"
#include "../Game/3D/SampleFbxObject/SampleFbxObject.h"
#include "../Game/2D/SampleSprite/SampleSprite.h"

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

private:
	float time = 0.f;

	std::unique_ptr<SampleFbxObject> obj;
	std::unique_ptr<SampleSprite> sp;
};

