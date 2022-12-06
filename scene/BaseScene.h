#pragma once
#include "../Engine/base/DirectXCommon.h"
#include "../Engine/base/Window.h"
#include "../Engine/input/Input.h"
#include "../camera/Camera.h"

#ifdef _DEBUG
#include "../Engine/debugProcess/DebugText.h"
#endif // _DEBUG



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
	/// 後更新
	/// </summary>
	void EndUpdate();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 後描画
	/// </summary>
	void EndDraw();

	/// <summary>
	/// 後処理
	/// </summary>
	virtual void Finalize();

	//Getter
	bool GetIsSceneChange()	{return IsSceneChange;}
	//Reset
	void ResetIsSceneChange()	{IsSceneChange = false;}

//メンバ変数
protected:

#pragma region 汎用機能
	//借りもの
	DirectXCommon* dxCommon = nullptr;
	Window* window = nullptr;
	
	//作成
	Input* input = nullptr;
	Camera* camera = nullptr;

	//シーン遷移
	bool IsSceneChange = false;

#ifdef _DEBUG
	std::unique_ptr<DebugText> debugText;
#endif // _DEBUG

};

