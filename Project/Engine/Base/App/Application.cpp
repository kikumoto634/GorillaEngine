#include "Application.h"
#include "TitleScene.h"

#include "HomeScene.h"
#include "GameScene.h"
#include "Boss1Area.h"
#include "TutorialScene.h"

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
	window->Create("CAVERHYTHM", 1280, 720);

	//DirectXCommon
	dxCommon->Initialize(window);

	//テクスチャ
	TextureManager::GetInstance()->Initialize(dxCommon);
	TextureManager::Load(texFont_tex.number			,texFont_tex.path);
	TextureManager::Load(white1x1_tex.number		,white1x1_tex.path);
	TextureManager::Load(combo_tex.number			,combo_tex.path);
	TextureManager::Load(number0_tex.number			,number0_tex.path);
	TextureManager::Load(number1_tex.number			,number1_tex.path);
	TextureManager::Load(number2_tex.number			,number2_tex.path);
	TextureManager::Load(number3_tex.number			,number3_tex.path);
	TextureManager::Load(number4_tex.number			,number4_tex.path);
	TextureManager::Load(number5_tex.number			,number5_tex.path);
	TextureManager::Load(number6_tex.number			,number6_tex.path);
	TextureManager::Load(number7_tex.number			,number7_tex.path);
	TextureManager::Load(number8_tex.number			,number8_tex.path);
	TextureManager::Load(number9_tex.number			,number9_tex.path);
	TextureManager::Load(coin_tex.number			,coin_tex.path);
	TextureManager::Load(heart_tex.number			,heart_tex.path);
	TextureManager::Load(heartEmpty_tex.number		,heartEmpty_tex.path);
	TextureManager::Load(cross_tex.number			,cross_tex.path);
	TextureManager::Load(buttonZ_tex.number			,buttonZ_tex.path);
	TextureManager::Load(titleLogo_tex.number		,titleLogo_tex.path);
	TextureManager::Load(pushText_tex.number		,pushText_tex.path);
	TextureManager::Load(bossName_tex.number		,bossName_tex.path);
	TextureManager::Load(notes_tex.number			,notes_tex.path);
	TextureManager::Load(rhythmHeart_tex.number		,rhythmHeart_tex.path);
	TextureManager::Load(tutorialText1_tex.number	,tutorialText1_tex.path);
	TextureManager::Load(tutorialText2_tex.number	,tutorialText2_tex.path);
	TextureManager::Load(goText.number				,goText.path);
	TextureManager::Load(training_tex.number		,training_tex.path);
	TextureManager::Load(depthValue_tex.number		,depthValue_tex.path);
	TextureManager::Load(cutInBoss_tex.number		,cutInBoss_tex.path);
	TextureManager::Load(cutInParts1_tex.number		,cutInParts1_tex.path);
	TextureManager::Load(cutInParts2_tex.number		,cutInParts2_tex.path);
	TextureManager::Load(rhythmHeart2_tex.number	,rhythmHeart2_tex.path);
	TextureManager::Load(resultTextTitle_tex.number	,resultTextTitle_tex.path);
	TextureManager::Load(resultTextLobby_tex.number	,resultTextLobby_tex.path);
	TextureManager::Load(playResult_tex.number		,playResult_tex.path);
	TextureManager::Load(PadA_tex.number			,PadA_tex.path);
	TextureManager::Load(buttonCross_tex.number		,buttonCross_tex.path);
	TextureManager::Load(PadLStick_tex.number		,PadLStick_tex.path);

	//音声
	Audio::GetInstance()->Initialize();
	Audio::Load(rhythm_audio.number		,rhythm_audio.path);
	Audio::Load(miss_audio.number		,miss_audio.path);
	Audio::Load(damage_audio.number		,damage_audio.path);
	Audio::Load(exBPM90_audio.number	,exBPM90_audio.path);
	Audio::Load(exBPM120_audio.number	,exBPM120_audio.path);
	Audio::Load(exBPM180_audio.number	,exBPM180_audio.path);
	Audio::Load(openExit_audio.number	,openExit_audio.path);
	Audio::Load(coinGet_audio.number	,coinGet_audio.path);
	Audio::Load(bpm120Game_audio.number	,bpm120Game_audio.path);
	Audio::Load(bpm120Home_audio.number	,bpm120Home_audio.path);
	Audio::Load(dig_audio.number		,dig_audio.path);
	Audio::Load(gateEnter_audio.number	,gateEnter_audio.path);
	Audio::Load(cutIn_audio.number		,cutIn_audio.path);
	Audio::Load(reflected_audio.number	,reflected_audio.path);
	Audio::Load(roar_audio.number		,roar_audio.path);
	Audio::Load(recover_audio.number	,recover_audio.path);
	Audio::Load(push_audio.number		,push_audio.path);

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
	debugText = new DebugText();
	debugText->Initialize(0);

	imgui = new imguiManager();
	imgui->Initialize(window, dxCommon);
#endif // _DEBUG

	sceneManager = SceneManager::GetInstance();
	BaseScene* scene = new HomeScene(dxCommon, window);

#ifdef _DEBUG
	scene->SetDebugText(debugText);
	scene->SetImGui(imgui);
#endif // _DEBUG

	sceneManager->SetNextScene(scene);

	
	//ポストエフェクト
	postEffect_ = PostEffect::GetInstance();
}

void Application::Update()
{
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
	postEffect_->PreDrawScene();
	sceneManager->Draw();
	postEffect_->PostDrawScene();


	//描画前処理
	dxCommon->BeginDraw();

	sceneManager->DrawBack();
	postEffect_->Draw();
	sceneManager->DrawNear();

#ifdef _DEBUG
	debugText->DrawAll();
	imgui->Draw();
#endif // _DEBUG

	//描画後処理
	dxCommon->EndDraw();
}

void Application::Finalize()
{
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
