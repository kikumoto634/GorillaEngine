#include "Application.h"
#include "GameScene01.h"

Application* Application::app = nullptr;

Application *Application::GetInstance()
{
	if(!app)
	{
		app = new Application();
	}
	return app;
}

void Application::Delete()
{
	if(app){
		delete app;
		app = nullptr;
	}
}

Application::Application()
{
	window = new Window(1280, 720);
	dxCommon = new DirectXCommon();
}

Application::~Application()
{
	delete window;
	delete dxCommon;
}

void Application::Run()
{
	MSG msg{};//メッセージ
	while (true)
	{
		if(PeekMessage(&msg, nullptr,0, 0,PM_REMOVE)){
			TranslateMessage(&msg);	//キー入力メッセージの処理
			DispatchMessage(&msg);	//プロシージャにメッセージを送る
		}
		if(msg.message == WM_QUIT) {
			break;
		}

		Update();
		Draw();
	}
}

void Application::Initialize()
{
	//Window生成
	window->Create(L"GoliraEngine");

	//DirectXCommon
	dxCommon->Initialize(window);

	//テクスチャ
	textureManager = std::make_unique<TextureManager>();
	textureManager->Initialize(dxCommon);

	//スプライト
	Sprite::StaticInitialize(dxCommon, textureManager.get(), window->GetWindowWidth(), window->GetWindowHeight());

	textureManager->LoadTexture(0, L"Resources/Texture.jpg");
	sprite = std::make_unique<Sprite>();
	sprite->Initialize(0);
}

void Application::Update()
{
}

void Application::Draw()
{
	dxCommon->BeginDraw();

	Sprite::SetPipelineState(this->dxCommon->GetCommandList());
	sprite->Draw(dxCommon->GetCommandList());

	dxCommon->EndDraw();
}

void Application::Finalize()
{
	Sprite::StaticFinalize();
	window->Finalize();
}
