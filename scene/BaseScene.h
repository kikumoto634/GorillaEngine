#pragma once
#include "../Engine/base/DirectXCommon.h"
#include "../Engine/base/Window.h"
#include "../Engine/input/Input.h"
#include "../camera/Camera.h"

#include "../Game/3D/SampleObjObject/SampleObjObject.h"

#include "../Game/Light/Lighting.h"

#ifdef _DEBUG
#include "../Engine/debugProcess/DebugText.h"
#include "../Engine/base/imguiManager.h"

#include <imgui.h>
#endif // _DEBUG


//前方宣言
class SceneManager;

class BaseScene
{
//メンバ関数
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseScene(DirectXCommon* dxCommon, Window* window);

	/// <summary>
	/// 起動時
	/// </summary>
	virtual void Application();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 後処理
	/// </summary>
	virtual void Finalize();

	//シーン遷移
	virtual void NextSceneChange();

	/// <summary>
	/// 後更新
	/// </summary>
	void EndUpdate();

#ifdef _DEBUG
	void SetDebugText(DebugText* text)	{this->debugText = text;}
	void SetImGui(imguiManager* imgui)	{this->imgui = imgui;}

	DebugText* GetDebugText()	{return debugText;}
	imguiManager* GetImGui()	{return imgui;}
#endif // _DEBUG

	//Setter
	void SetSceneManager(SceneManager* lsceneManager)	{sceneManager = lsceneManager;}

//メンバ変数
protected:

#pragma region 汎用機能
	//借りもの
	DirectXCommon* dxCommon = nullptr;
	Window* window = nullptr;
	
	//作成
	Input* input = nullptr;
	Camera* camera = nullptr;

	//借り物
	//シーンマネージャー
	SceneManager* sceneManager = nullptr;


	//skydome
    std::unique_ptr<SampleObjObject> skydome;

	//Light
	std::unique_ptr<Lighting> lightObject;

#ifdef _DEBUG
	//借り物
	DebugText* debugText = nullptr;
	imguiManager* imgui = nullptr;
	bool show_demo_window = false;
#endif // _DEBUG

};

