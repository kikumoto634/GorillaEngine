#include "Application.h"
#include "SampleScene.h"

Application* Application::app = nullptr;

using namespace std;

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
	window = Window::GetInstance();
	dxCommon = DirectXCommon::GetInstance();

	scene = make_unique<SampleScene>(dxCommon, window);
	scene->Application();
	sceneName = "Title";
}

Application::~Application()
{
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
#pragma region 汎用機能初期化
	//Window生成
	window->Create("GiliraEngine", 1280, 720);

	//DirectXCommon
	dxCommon->Initialize(window);

	//テクスチャ
	TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::Load(0, "white1x1.png");
#pragma endregion

	//スプライト静的初期化
	Sprite::StaticInitialize(dxCommon, window->GetWindowWidth(), window->GetWindowHeight());
	
	//Geometry
	GeometryObjectManager::GetInstance()->CreateBuffer();
	GeometryObject::StaticInitialize(dxCommon);
	
	//FBX
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());
	FbxModelObject::StaticInitialize(dxCommon);

	scene->Initialize();
}

void Application::Update()
{
	scene->Update();
	scene->EndUpdate();
}

void Application::Draw()
{
	//描画前処理
	dxCommon->BeginDraw();

	Sprite::SetPipelineState();
	scene->Draw();

	//描画後処理
	dxCommon->EndDraw();
}

void Application::Finalize()
{
	FbxModelObject::StaticFinalize();
	FbxLoader::GetInstance()->Finalize();
	GeometryObject::StaticFinalize();
	Sprite::StaticFinalize();

	scene->Finalize();
	delete scene.get();

	window->Finalize();
}
