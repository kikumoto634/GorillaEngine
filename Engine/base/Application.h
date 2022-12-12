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


#include "../../scene/BaseScene.h"
#include "../Engine/2D/Sprite.h"




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

	std::unique_ptr<BaseScene> scene;
	std::string sceneName = "";
};