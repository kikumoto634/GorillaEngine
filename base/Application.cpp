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
	window = new Window("ゴリラエンジン", 1280, 720);
	dxCommon = new DirectXCommon();
	sceneManager = new SceneManager();
}

Application::~Application()
{
	delete window;
	delete dxCommon;
	delete sceneManager;
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
		if(sceneManager->GetIsAlive()) break;

		Update();
		Draw();
	}
}

void Application::Initialize()
{
	//Window生成
	window->Create();

	//DirectXCommon
	dxCommon->Initialize(window);

	//シーン追加
	sceneManager->Add("Area01", new GameScene01(window));

	//シーン指定
	sceneManager->Change("Area01");

	//テクスチャマネージャー
	TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::Load(0,"white1x1.png");

	//スプライト
	Sprite::StaticInitialize(dxCommon, window->GetWindowWifth(), window->GetWindowHeight());
}

void Application::Update()
{
	sceneManager->Update();
}

void Application::Draw()
{
	dxCommon->BeginDraw();
	Sprite::PreDraw();

	sceneManager->Draw();

	dxCommon->EndDraw();
}

void Application::Finalize()
{
	window->Finalize();
}
