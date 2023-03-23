#pragma once
#include "Window.h"
#include "DirectXCommon.h"

#include "TextureManager.h"

#include "ParticleManager.h"

#include "../Engine/loader/FbxLoader.h"
#include "FbxModelManager.h"
#include "../Engine/3D/FbxModelObject.h"

#include "../Engine/base/ObjModelManager.h"
#include "../Engine/3D/ObjModelObject.h"

#include "../light/DirectionalLight.h"

#include "../scene/SceneManager.h"
#include "../Engine/2D/Sprite.h"

#ifdef _DEBUG
#include "../debugProcess/DebugText.h"
#include "../Engine/base/imguiManager.h"
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

};