#include "Application.h"
#include "SafeDelete.h"

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
	camera = Camera::GetInstance();
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

	//Input初期化
	input->Initialize(window->GetHwnd());

	//テクスチャ
	TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::Load(0, "Texture.jpg");
	TextureManager::Load(1, "white1x1.png");

	//カメラ
	camera->Initialize();

	//Geometry
	GeometryObjectManager::GetInstance()->CreateBuffer();
	GeometryObject::StaticInitialize(dxCommon);
	
	//FBX
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());
	FbxModelObject::StaticInitialize(dxCommon);

#pragma endregion

#pragma region スプライト初期化
	//スプライト静的初期化
	Sprite::StaticInitialize(dxCommon, window->GetWindowWidth(), window->GetWindowHeight());
	//生成
	sprite = (Sprite::Create(1, Vector2(100.f,100.f)));
	sprite->SetSize(Vector2(100,100));

	//ポストエフェクト
	postEffect = (PostEffect::Create(0, Vector2(0.f, 0.f)));

#pragma endregion

#pragma region オブジェクト初期化

	//Geometry
	worldTransform.Initialize();
	object = GeometryObject::Create(0);

	//Fbx
	worldTransformFbx.Initialize();
	modelFbx = FbxLoader::GetInstance()->LoadModeFromFile("boneTest");
	objectFbx = FbxModelObject::Create(modelFbx);
	objectFbx->PlayAnimation();

#pragma endregion

#pragma region カメラ初期化


#pragma endregion

#ifdef _DEBUG
	//一時停止
	sceneStopper = SceneStopper::GetInstance();

#endif // _DEBUG
}

void Application::Update()
{
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

	worldTransformFbx.translation = {0,-2.5f,0};
	worldTransformFbx.rotation = {0,XMConvertToRadians(90),0};
	worldTransformFbx.UpdateMatrix();

#pragma region 入力処理

	//スプライト
	{
		if(input->Push(DIK_SPACE)){
			sprite->SetColor({1,0,0,1});
		}
		else{
			sprite->SetColor({1,1,1,1});
		}
	}

	//幾何学
	{
		if(input->Push(DIK_RIGHT)){
			worldTransform.rotation.y += XMConvertToRadians(1.f);
		}
		else if(input->Push(DIK_LEFT)){
			worldTransform.rotation.y += XMConvertToRadians(-1.f);
		}
		worldTransform.UpdateMatrix();
	}

	//カメラ
	if(input->Push(DIK_D)){
		//座標移動
		//eye.x += 1.f;
		//target.x += 1.f;
	}
	else if(input->Push(DIK_A)){
		//座標移動
		//eye.x += -1.f;
		//target.x += -1.f;
	}

#pragma endregion

#pragma region スプライト更新


#pragma endregion

#pragma region オブジェクト更新
	object->Update(worldTransform, camera);
	objectFbx->Update(worldTransformFbx, camera);
#pragma endregion

#pragma region 汎用機能更新
	//入力情報更新
	input->Update();
	//カメラ
	camera->Update();
#pragma endregion
}

void Application::Draw()
{
	//描画前処理
	dxCommon->BeginDraw();
	//object->Draw();
	objectFbx->Draw();

#pragma region 2D描画
	postEffect->Draw();

	Sprite::SetPipelineState();
	sprite->Draw();
#pragma endregion 2D描画終了

	//描画後処理
	dxCommon->EndDraw();
}

void Application::Finalize()
{
	FBXDelete(objectFbx, modelFbx, worldTransformFbx);
	FbxModelObject::StaticFinalize();
	FbxLoader::GetInstance()->Finalize();

	GeometryDelete(object, worldTransform);
	GeometryObject::StaticFinalize();

	SpriteDelete(sprite);
	Sprite::StaticFinalize();

	window->Finalize();
}
