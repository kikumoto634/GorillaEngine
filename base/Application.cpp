#include "Application.h"
#include "GameScene01.h"

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
	input = Input::GetInstance();
}

Application::~Application()
{
	delete sprite;

	delete objectFbx;
	delete modelFbx;
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

	//Input初期化
	input->Initialize(window->GetHwnd());

	////テクスチャ
	TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::Load(0, "Texture.jpg");

	//カメラ
	camera = Camera::GetInstance();
	camera->Initialize();

	//FBXマネージャー
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());

#pragma endregion

#pragma region スプライト初期化
	//スプライト静的初期化
	Sprite::StaticInitialize(dxCommon, window->GetWindowWidth(), window->GetWindowHeight());
	//生成
	sprite = Sprite::Create(0, Vector2(100.f,100.f));
	sprite->SetSize(Vector2(100,100));
#pragma endregion

#pragma region オブジェクト初期化

	////幾何学オブジェクト静的初期化
	//GeometryObjectManager::GetInstance()->CreateBuffer();
	////生成
	//worldTransform.Initialize();
	//GeometryObject::StaticInitialize(dxCommon);
	//object = GeometryObject::Create(0);

	//Fbx
	modelFbx = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	FbxModelObject::SetDevice(dxCommon->GetDevice());
	FbxModelObject::SetCamera(camera);
	FbxModelObject::CreateGraphicsPipeline();

	objectFbx = new FbxModelObject();
	objectFbx->Initialize();
	objectFbx->SetModel(modelFbx);

#pragma endregion

#ifdef _DEBUG
	//一時停止
	sceneStopper = SceneStopper::GetInstance();

#endif // _DEBUG
}

void Application::Update()
{
#pragma region 汎用機能更新
	//入力情報更新
	input->Update();
	//カメラ
	camera->Update();
#pragma endregion

#ifdef _DEBUG

#pragma region 一時停止
	//入力
	if(input->Trigger(DIK_F1)){
		if(!sceneStopper->GetIsSceneStop()){
			sceneStopper->SetIsSceneStop(true);
		}
		else if(sceneStopper->GetInstance()){
			sceneStopper->SetIsSceneStop(false);
		}
	}
	//停止
	if(sceneStopper->GetIsSceneStop()) return;
#pragma endregion

#endif // _DEBUG

#pragma region 入力処理
	/*{
		if(input->Push(DIK_SPACE)){
			sprite->SetColor({1,0,0,1});
		}
		else{
			sprite->SetColor({1,1,1,1});
		}
	}
	{
		if(input->Push(DIK_RIGHT)){
			worldTransform.rotation.y += XMConvertToRadians(1.f);
		}
		else if(input->Push(DIK_LEFT)){
			worldTransform.rotation.y += XMConvertToRadians(-1.f);
		}
		worldTransform.UpdateMatrix();
	}*/
#pragma endregion

#pragma region スプライト更新


#pragma endregion

#pragma region オブジェクト更新
	//object->Update(worldTransform, camera);
	objectFbx->Update();
#pragma endregion

}

void Application::Draw()
{
	//描画前処理
	dxCommon->BeginDraw();
	//object->Draw();
	objectFbx->Draw(dxCommon->GetCommandList());

	Sprite::SetPipelineState();
	sprite->Draw();

	//描画後処理
	dxCommon->EndDraw();
}

void Application::Finalize()
{
	FbxLoader::GetInstance()->Finalize();
	GeometryObject::StaticFinalize();
	Sprite::StaticFinalize();
	window->Finalize();
}
