#pragma once
#include "Window.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "Sprite.h"

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
	Window* window;
	DirectXCommon* dxCommon;
	Input* input;

	Sprite* sprite = nullptr;
};