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
	window = new Window("GiliraEngine", 1280, 720);
	dxCommon = new DirectXCommon();
	input = Input::GetInstance();
}

Application::~Application()
{
	delete window;
	delete dxCommon;
	delete sprite;
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
	window->Create();

	//DirectXCommon
	dxCommon->Initialize(window);

	//Input初期化
	input->Initialize(window->GetHwnd());

	////テクスチャ
	TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::Load(0, "white1x1.png");

	////スプライト
	Sprite::StaticInitialize(dxCommon, window->GetWindowWidth(), window->GetWindowHeight());

	sprite = Sprite::Create(0, Vector2(100.f,100.f));
	sprite->SetSize(Vector2(100,100));
}

void Application::Update()
{
	//入力情報更新
	input->Update();

	if(input->Push(DIK_SPACE)){
		sprite->SetColor({1,0,0,1});
	}
	else{
		sprite->SetColor({1,1,1,1});
	}
}

void Application::Draw()
{
	//描画前処理
	dxCommon->BeginDraw();
	Sprite::SetPipelineState();

	sprite->Draw();

	//描画後処理
	dxCommon->EndDraw();
}

void Application::Finalize()
{
	Sprite::StaticFinalize();
	window->Finalize();
}
