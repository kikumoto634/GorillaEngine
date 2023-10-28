#include "Application.h"

#include "AudioUtility.h"
#include "TextureUtility.h"

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
}

Application::~Application()
{
}

void Application::Run()
{
	MSG msg{};//メッセージ
	while (true)
	{
		Update();
		Draw();
		if(Input::GetInstance()->Trigger(DIK_ESCAPE)) break;
		if(PeekMessage(&msg, nullptr,0, 0,PM_REMOVE)){
			TranslateMessage(&msg);	//キー入力メッセージの処理
			DispatchMessage(&msg);	//プロシージャにメッセージを送る
		}
		if(msg.message == WM_QUIT) {
			break;
		}
	}
}

void Application::Initialize()
{
#pragma region 汎用機能初期化
	//Window生成
	window->Create("Engine", 1280, 720);

	//DirectXCommon
	dxCommon->Initialize(window);

	camera = Camera::GetInstance();
	camera->Initialize();

	//テクスチャ
	TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::Load(texFont_tex.number			,texFont_tex.path);
	TextureManager::Load(white1x1_tex.number		,white1x1_tex.path);
	TextureManager::Load(uvChecker_tex.number		,uvChecker_tex.path);


	//音声
	Audio::GetInstance()->Initialize();

#pragma endregion

	//スプライト静的初期化
	Sprite::StaticInitialize(dxCommon, window->GetWindowWidth(), window->GetWindowHeight());
	
	//FBX
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());
	FbxModelObject::StaticInitialize(dxCommon);

	//OBJ
	ObjModelManager::StaticInitialize(dxCommon);
	ObjModelObject::StaticInitialize(dxCommon);

	//Light
	LightGroup::StaticInitialize(dxCommon->GetDevice());

	// パーティクルマネージャ初期化
	ParticleManager::GetInstance()->Initialize(dxCommon);

#ifdef _DEBUG
	debugText = new DebugScreenText();
	debugText->Initialize(0);

	imgui = new imguiManager();
	imgui->Initialize(window, dxCommon);
#endif // _DEBUG

	sceneManager = SceneManager::GetInstance();
	BaseScene* scene = new BaseScene(dxCommon, window);

#ifdef _DEBUG
	scene->SetDebugText(debugText);
	scene->SetImGui(imgui);
#endif // _DEBUG

	sceneManager->SetNextScene(scene);

	//ポストエフェクト(必要なシーンでGetInstance()とInitialize()を呼び出す)
	postEffect_ = PostEffect::GetInstance();


	//オフセットスクリーン
	float color[4] = {1.f,1.f,1.f,1.f};
	offsetScreen = RenderTexture::Create(color);

	obj = new BaseObjObject();
	obj->Initialize("sphere", true);
	obj->SetPosition({0,5,-3});
	obj->SetScale({3,3,3});
	obj->SetTexture(offsetScreen->GetTexBuff().Get());
}

void Application::Update()
{
	camera->Update();

	obj->Update(camera);

#ifdef _DEBUG
	imgui->Begin();
#endif // _DEBUG
	sceneManager->Update();
	postEffect_->Update();

#ifdef _DEBUG
	imgui->End();
#endif // _DEBUG
}

void Application::Draw()
{
	//レンダーターゲットへの描画
	//postEffect_->PreDrawScene();
	//sceneManager->Draw();
	//postEffect_->PostDrawScene();

	offsetScreen->PreDraw();
	sceneManager->Draw();
	offsetScreen->PostDraw();

	//描画前処理
	dxCommon->BeginDraw();

	obj->Draw();
	sceneManager->Draw();
	//postEffect_->Draw();

	Sprite::SetPipelineState();
	sceneManager->DrawBack();
	sceneManager->DrawNear();

#ifdef _DEBUG
	debugText->DrawAll();
	imgui->Draw();
#endif // _DEBUG

	//描画後処理
	dxCommon->EndDraw();
	dxCommon->Reset();
}

void Application::Finalize()
{
	obj->Finalize();
	delete obj;

	delete postEffect_;

#ifdef _DEBUG
	imgui->Finalize();
	delete imgui;

	delete debugText;
	debugText = nullptr;
#endif // _DEBUG

	ObjModelObject::StaticFinalize();
	FbxModelObject::StaticFinalize();
	FbxLoader::GetInstance()->Finalize();
	Sprite::StaticFinalize();

	SceneManager::Delete();
	sceneManager = nullptr;

	DirectXCommon::Delete();
	dxCommon = nullptr;

	window->Finalize();
	Window::Delete();
	window = nullptr;
}
