#pragma once
#include "BaseScene.h"

#include "LightGroup.h"

#include "BaseSprites.h"
#include "BaseObjObject.h"

#include "GameManager.h"

#include <list>
#include <vector>

class TitleScene : public BaseScene
{
private:
	template <class T> using unique_ptr = std::unique_ptr<T>;
	template <class T> using list = std::list<T>;
	template <class T> using vector = std::vector<T>;

private:
	//ボタン
	const int buttonSpIndex = 2;

	//シーン遷移
	const float FadeSecond = 1.f;

	//各オブジェクト情報
	const Vector3 PlayerPos   = {-5.5f,-2.f,0.f};
	const Vector3 PlayerRot   = {0.f, DirectX::XMConvertToRadians(121),0.f};
	const Vector3 PlayerScale = {5,5,5};

	const Vector3 SlimePos   = {9.f,-3.f,-1.f};
	const Vector3 SlimeRot   = {DirectX::XMConvertToRadians(5), DirectX::XMConvertToRadians(220),0.f};
	const Vector3 SlimeScale = {3,3,3};

	const Vector3 SkeltonPos   = {7.f,-3.f,2.f};
	const Vector3 SkeltonRot   = {DirectX::XMConvertToRadians(5), DirectX::XMConvertToRadians(230),0.f};
	const Vector3 SkeltonScale = {5,5,5};

	const Vector3 WallBackPos  = {0,0,7};


	const Vector2 TitlePos  = {640,150};
	const Vector2 TitleSize = {800,160};
	const Vector2 TitleAnc  = {0.5f,0.5f};

	const Vector2 PushTextPos  = {550,600};
	const Vector2 PushTextSize = {180,60};
	const Vector2 PushtextAnc  = {0.5f,0.5f};

	const Vector2 ButtonPos  = {700,600};
	const Vector2 ButtonSize = {60,60};
	const Vector2 ButtonAnc  = {0.5f,0.5f};

	//ライト色
	const Vector3 LightColor = {0.15f,0.15f,0.15f};

public:
	
	//コンストラクタ
	TitleScene(DirectXCommon* dxCommon, Window* window);

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
	/// 3D描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 2D描画背景
	/// </summary>
	void DrawBack() override;

	/// <summary>
	/// 2D描画UI
	/// </summary>
	void DrawNear() override;

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize() override;

private:
	//シーン遷移
	void NextSceneChange();
	void DebugSceneChange();
	void SceneChange();

	//オブジェクト生成
	void ObjectInitialize();
	//スプライト生成
	void SpriteInitialize();

private:
	//オブジェクト
	list<unique_ptr<BaseObjObject>> objs_;

	//スプライト
	vector<unique_ptr<BaseSprites>> sp_;
	int buttonSpNumber = 0;


	//音声
	Audio* audio_ = nullptr;

	//ライト
	LightGroup* lightGroup_ = nullptr;

	//シーン遷移
	//Prev
	bool isPrevSceneChange_ = true;
	//Next
	bool isNextSceneChange_ = false;

	bool isDebugScene_= false;
};

