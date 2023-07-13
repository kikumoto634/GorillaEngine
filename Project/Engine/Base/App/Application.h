#pragma once
#include "Window.h"
#include "DirectXCommon.h"

#include "TextureManager.h"

#include "ParticleManager.h"

#include "Audio.h"

#include "FbxLoader.h"
#include "FbxModelManager.h"
#include "FbxModelObject.h"

#include "ObjModelManager.h"
#include "ObjModelObject.h"

#include "DirectionalLight.h"

#include "BaseScene.h"
#include "SceneManager.h"
#include "Sprite.h"

#include "PostEffect.h"

#ifdef _DEBUG

#include "imguiManager.h"
#include <imgui.h>
#endif // _DEBUG

class Application
{
private:
	//シングルトン
	static Application* app;

	Application();
	~Application();

public:
	//シングルトン(Applicationを複数持つ意味はないため、単一の存在)
	static Application* GetInstance();
	static void Delete();

	void Initialize();
	void Run();
	void Finalize();

private:
	void Update();
	void Draw();

private:
#pragma region 汎用機能
	Window* window;
	DirectXCommon* dxCommon;
#pragma endregion
	SceneManager* sceneManager = nullptr;

#ifdef _DEBUG
	DebugText* debugText = nullptr;

	imguiManager* imgui = nullptr;
	bool IsSceneChange_ImGui = false;
#endif // _DEBUG

	//ポストエフェクト
	PostEffect* postEffect_ = nullptr;

	bool isEnd = false;
};