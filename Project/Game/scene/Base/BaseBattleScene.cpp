#include "BaseBattleScene.h"
#include "Easing.h"

#include "Collision.h"
#include "SphereCollider.h"
#include "MeshCollider.h"

#include "SceneManager.h"
#include "TitleScene.h"
#include "HomeScene.h"

#include <thread>

using namespace std;

BaseBattleScene::BaseBattleScene(DirectXCommon *dxCommon, Window *window, int saveHP, int floorVaule)
	:BaseScene(
		dxCommon,
		window)
{
	this->saveHP_ = saveHP;
	this->floorValue_ = floorValue_;
}

BaseBattleScene::~BaseBattleScene()
{
	Finalize();
}

void BaseBattleScene::Application()
{
	BaseScene::Application();
}

void BaseBattleScene::Initialize()
{
	BaseScene::Initialize();

	CommonInitialize();
	//オブジェクト3D
	Object3DInitialize();
	//2D
	Object2DInitialize();

	//ポストエフェクト
	postEffect->FadeStart();
}

void BaseBattleScene::Update()
{
	BaseScene::Update();

#ifdef _DEBUG
	//if(input->Push(DIK_LSHIFT)){
		
	if(input->Push(DIK_A)){
		camera->MoveVector({-1.f, 0.f, 0.f});
	}
	else if(input->Push(DIK_D)){
		camera->MoveVector({1.f, 0.f, 0.f});
	}

	if(input->Push(DIK_W)){
		//camera->MoveVector({0.f, 0.f, 1.f});
		camera->RotVector({XMConvertToRadians(1.0f),0.f,0.f});
	}
	else if(input->Push(DIK_S)){
		//camera->MoveVector({0.f, 0.f, -1.f});
		camera->RotVector({XMConvertToRadians(-1.0f),0.f,0.f});
	}
	//}
#endif // _DEBUG

	//シーン更新
	SceneChange();

	//リズム
	rhythmManager_->PreUpdate();
	RhythmJudgeUpdate();
	BeatEndUpdate();

	//更新
	CommonUpdate();

	rhythmManager_->PostUpdate();

#ifdef _DEBUG
	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{0,100});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{300,125});
		ImGui::Begin("Camera");
		//カメラ 回転:false , 移動:true
		ImGui::Text("Camera");
		if(ImGui::Button("Shake")){
			camera->ShakeStart();
		}
		ImGui::End();
	}

	{
		ImGui::SetNextWindowPos(ImVec2{0,225});
		ImGui::SetNextWindowSize(ImVec2{300,125});
		ImGui::Begin("Rhythm");
		if(ImGui::Button("Low BPM90")){
			rhythmManager_->BPMLowSet();
			gameManager_->AudioRatio(bpm120Game_audio.number, 0.75f);
		}
		if(ImGui::Button("Normal BPM120")){
			rhythmManager_->BPMNormalSet();
			gameManager_->AudioRatio(bpm120Game_audio.number, 1.f);
		}
		if(ImGui::Button("High BPM150")){
			rhythmManager_->BPMHighSet();
			gameManager_->AudioRatio(bpm120Game_audio.number, 1.25f);
		}
		ImGui::End();
	}

	//Scene
	{
		//座標
		ImGui::SetNextWindowPos(ImVec2{1000,40});
		//サイズ
		ImGui::SetNextWindowSize(ImVec2{280,150});
		ImGui::Begin("SCENE");

		ImGui::Text("Now:Home   Next:Game");
		if(!isPrevSceneChange_ && ImGui::Button("NextScene")){
			isNextSceneChange_ = true;
			postEffect->FadeStart();
		}
		ImGui::Text("Now:Home   Next:Title");
		if(!isPrevSceneChange_ && ImGui::Button("GameEnd")){
			isGameEnd_ = true;
			postEffect->FadeStart();
		}

		ImGui::End();
	}

#endif // _DEBUG

	BaseScene::EndUpdate();
}

void BaseBattleScene::Draw()
{
	Object3DDraw();

	ParticleDraw();
	player_->ParticleDraw();
}

void BaseBattleScene::DrawBack()
{
}

void BaseBattleScene::DrawNear()
{
	Sprite::SetPipelineState();
	UIDraw();

#ifdef _DEBUG
	debugText->Printf(0,400,1.f,"Camera Target  X:%f, Y:%f, Z:%f", camera->GetTarget().x, camera->GetTarget().y, camera->GetTarget().z);
	debugText->Printf(0,420,1.f,"Camera Eye  X:%f, Y:%f, Z:%f", camera->GetEye().x, camera->GetEye().y, camera->GetEye().z);


	debugText->Printf(0,560,1.f,"Player Pos X:%f, Y:%f, Z:%f", player_->GetPosition().x, player_->GetPosition().y,player_->GetPosition().z);

	debugText->Printf(0,580, 1.f,     "Time					: %lf[ms]", rhythmManager_->GetCalTime());
	debugText->Printf(0, 600, 1.f,    "JudgeTimeBase		: %lf[ms]", rhythmManager_->GetInputTimeTarget());
	debugText->Printf(0, 620, 1.f,	  "InputJudgeTimeBase	: %lf[ms]", rhythmManager_->GetInputTimet());
	
	debugText->Printf(000, 640, 1.f, "RhytjmSUB			: %lf[ms]", rhythmManager_->GetCalTime() - rhythmManager_->GetInputTimet());

	//debugText->Printf(200, 640, 1.f, "COMBO	: %d", gameManager_->GetComboNum());
	debugText->Printf(200, 660, 1.f, "COIN	: %d",player_->GetCoinNum());


	//debugText->Printf(0, 640, 1.f, "IsBeat : %d", rhythmManager->GetIsRhythmEnd());
	debugText->Printf(300, 300, 1.f, "Stick : %f,%f", input->PadLStickPre().x, input->PadLStickPre().y);

#endif // _DEBUG
}

void BaseBattleScene::Finalize()
{
	ObjectFinaize();

	CommonFinalize();

	BaseScene::Finalize();
}


void BaseBattleScene::CommonInitialize()
{
	//衝突マネージャー
	collisionManager_ = CollisionManager::GetInstance();

	//リズムマネージャー
	rhythmManager_ = make_unique<RhythmManager>();
	rhythmManager_->Initialize();

	//ゲームマネージャー
	gameManager_ = make_unique<GameManager>();
	gameManager_->Initialize();

	areaManager_ = make_unique<AreaManager>();

	//カメラ
	camera->RotVector(CameraRotValueIni);
	camera->Update();

	//追加コモン初期化
	AddCommonInitialize();
}

void BaseBattleScene::Object3DInitialize()
{
	//blenderでの保存スケールは 2/10(0.2)でのエクスポート
	player_ = make_unique<Player>();
	player_->Initialize("human1");
	player_->Pop(areaManager_->GetPlayerPosition());
	player_->SetWeaponModelPos(WeaponOffsetValueIni);
	player_->SetRotation(PlayerRotValueIni);
	player_->SetHp(saveHP_);
	gameManager_->InitializeSetHp(player_->GetHp());

	//出口
	exit_ = make_unique<Exit>();
	exit_->Initialize("Exit");
	exit_->SetPosition(areaManager_->GetExitPosition());
	exit_->SetExitOpenNeedCoin(needCoin_);
	exit_->NeedCoinSpriteUpdate();

	AddObject3DInitialize();
}

void BaseBattleScene::Object2DInitialize()
{
	judgeLoca_ = make_unique<JudgeLocation>();
	judgeLoca_->Initialize();

	AddObject2DInitialize();

	for(int i = 0; i < notesNum; i++){
		unique_ptr<LNotes> newsp_L = make_unique<LNotes>();
		newsp_L->Initialize(1);
		lNotes.push_back(move(newsp_L));

		unique_ptr<RNotes> newsp_R = make_unique<RNotes>();
		newsp_R->Initialize(1);
		rNotes.push_back(move(newsp_R));
	}

	{
		unique_ptr<BaseSprites> sp1 = make_unique<BaseSprites>();
		sp1->Initialize(resultTextLobby_tex.number);
		sp1->SetPosition(ResultTextPos);
		sp1->SetSize(ResultTextSize);
		sp1->SetAnchorPoint(ResultTextAnch);
		resultText.push_back(move(sp1));

		unique_ptr<BaseSprites> sp2 = make_unique<BaseSprites>();
		sp2->Initialize(resultTextTitle_tex.number);
		sp2->SetPosition({ResultTextPos.x, ResultTextPos.y+ResultTextSize.y});
		sp2->SetSize(ResultTextSize);
		sp2->SetAnchorPoint(ResultTextAnch);
		resultText.push_back(move(sp2));

		unique_ptr<BaseSprites> sp3 = make_unique<BaseSprites>();
		sp3->Initialize(playResult_tex.number);
		sp3->SetPosition(PlayResultPos);
		sp3->SetSize(PlayResultSize);
		sp3->SetAnchorPoint(ResultTextAnch);
		resultText.push_back(move(sp3));

		backSp = make_unique<BaseSprites>();
		backSp->Initialize(white1x1_tex.number);
		backSp->SetSize(ResultBackSize);
		backSp->SetColor({ResultBackColor.x,ResultBackColor.y,ResultBackColor.z,0.5f});

		resultText[select]->SetColor({ResultColor.x,ResultColor.y,ResultColor.z,1.0f});
	}
}

void BaseBattleScene::Object3DUpdate()
{
	//プレイヤー
	//小節終了時に入力可能状態に変更
	if(rhythmManager_->GetIsMeasureUp()){
		player_->InputPossible();
	}
	if(player_->GetIsInput()){
		rhythmManager_->InputTime();
		isRhythmInput_ = true;
		isNoteInput_ = true;
	}
	if(player_->GetIsDamage())	{
		gameManager_->AudioPlay(damage_audio.number,damage_audio.volume);
		gameManager_->HpDecrement();
		input->PadVibrationLeap(1.0f);
	}
	if(player_->GetIsRecover()){
		gameManager_->AudioPlay(recover_audio.number,recover_audio.volume);
		gameManager_->HpIncrement();
	}
	if(player_->GetIsDead())	{
		gameManager_->AudioPlay(damage_audio.number,damage_audio.volume);
		camera->ShakeStart(5);
		isResult = true;
	}
	player_->Update(camera);
	//出口
	exit_->Update(camera);
	{
		Vector3 target = player_->GetPosition() + ExitSpLocalPos;
		Vector2 pos = exit_->GetCoinSp()->ChangeTransformation(target, this->camera);
		exit_->SetCoinSpPosition(pos);
	}
}

void BaseBattleScene::Object2DUpdate()
{
	gameManager_->SpriteUpdate();

	ResultUpdate();

	judgeLoca_->Update(isNoteInput_);

	if(isNoteInput_){
		//左
		for(auto it = lNotes.begin(); it != lNotes.end(); it++){

			//前提条件
			if(!(*it)->GetIsNoteAlive()) continue;
			
			//ハートとノーツの当たり判定
			if(judgeLoca_->GetPosition().x-judgeLoca_->GetSize().x/2 <= (*it)->GetPosition().x+(*it)->GetSize().x/2 && 
				(*it)->GetPosition().x-(*it)->GetSize().x/2 <= judgeLoca_->GetPosition().x+judgeLoca_->GetSize().x/2){
				(*it)->InputUpdate();
			}
		}
		//右
		for(auto it = rNotes.begin(); it != rNotes.end(); it++){

			//前提条件
			if(!(*it)->GetIsNoteAlive()) continue;
			
			//ハートとノーツの当たり判定
			if(judgeLoca_->GetPosition().x-judgeLoca_->GetSize().x/2 <= (*it)->GetPosition().x+(*it)->GetSize().x/2 && 
				(*it)->GetPosition().x-(*it)->GetSize().x/2 <= judgeLoca_->GetPosition().x+judgeLoca_->GetSize().x/2){
				(*it)->InputUpdate();
			}
		}
	}

	for(auto it = lNotes.begin(); it != lNotes.end();it++){
		if((*it)->GetIsNoteAlive()){
			(*it)->Update((float)rhythmManager_->GetBPMTime() + rhythmManager_->GetNoteSpeedAdjustment());
		}
	}
	for(auto it = rNotes.begin(); it != rNotes.end();it++){
		if((*it)->GetIsNoteAlive()){
			(*it)->Update((float)rhythmManager_->GetBPMTime() + rhythmManager_->GetNoteSpeedAdjustment());
		}
	}

}

void BaseBattleScene::CommonUpdate()
{
	//オブジェクト3D
	Object3DUpdate();
	AddObject3DUpdate();
	//2D
	Object2DUpdate();
	AddObject2DUpdate();

	gameManager_->PlayerCircleShadowSet(player_->GetPosition());
	//地面
	gameManager_->LightUpdate(player_->GetIsDead());

	//出口
	if(gameManager_->GetCoinNum() >= exit_->GetExitNeedCoinNum() && exit_->GetIsPlayerContact()){
		exit_->ExitOpen();
		player_->SetIsExitOpen(true);
	}
	else if(!exit_->GetIsPlayerContact()){
		exit_->ExitClose();
		player_->SetIsExitOpen(false);
	}

	//カメラ追従
	camera->Tracking(player_->GetPosition(), player_->GetIsEvent());

	//シーン遷移
	if(player_->GetIsNextScene())	{
		isNextSceneChange_ = true;
		postEffect->FadeStart();
		if(!exit_->GetIsOpenAudioOnce()){
			gameManager_->AudioPlay(openExit_audio.number, openExit_audio.volume);
			camera->ShakeStart();
			exit_->ModelChange();
		}
	}

	//すべての衝突をチェック
	collisionManager_->CheckAllCollisions();

	//追加コモン更新
	AddCommonUpdate();
}

void BaseBattleScene::RhythmJudgeUpdate()
{
	if(isPrevSceneChange_) return;

	//リズム判別
	if(isRhythmInput_){
		isRhythmInput_ = false;

		if(rhythmManager_->IsInputRhythmJudge()){
			gameManager_->ComboIncrement();
			player_->SetInputJudge(true);
		}
		else if(!rhythmManager_->IsInputRhythmJudge()){
			gameManager_->ComboReset();
			player_->SetInputJudge(false);
		}
	}
}

void BaseBattleScene::BeatEndUpdate()
{
	if(isPrevSceneChange_) return;

	//リズム終了時処理
	if(rhythmManager_->GetIsJustRhythm() && !isGameEnd_){
		
		//SE
		gameManager_->AudioPlay(rhythm_audio.number, rhythm_audio.volume);

		//各オブジェクト処理
		if(!player_->GetIsDead())player_->IsBeatEndOn();

		areaManager_->BeatEndUpdate(gameManager_.get());

		exit_->IsBeatEndOn();
		gameManager_->IsBeatEndOn();

		//ビート目視用
		for(auto it = lNotes.begin(); it != lNotes.end(); it++){
			if(!(*it)->GetIsNoteAlive()){
				(*it)->BeatUpdate();
				break;
			}
		}
		for(auto it = rNotes.begin(); it != rNotes.end(); it++){
			if(!(*it)->GetIsNoteAlive()){
				(*it)->BeatUpdate();
				break;
			}
		}

		AddBeatEndUpdate();
	}
}

void BaseBattleScene::Object3DDraw()
{
	if(isDrawStop) return;

	player_->Draw();
	exit_->Draw();
	
	AddObject3DDraw();
}

void BaseBattleScene::ParticleDraw()
{
	if(isDrawStop) return;

	areaManager_->ParticleDraw();

	AddParticleDraw();
}

void BaseBattleScene::UIDraw()
{
	if(isDrawStop) return;

	AddFrontUIDraw();

	//出口
	exit_->Draw2D();

	//リザルト
	ResultDraw2D();
	
	judgeLoca_->Draw();
	for(auto it = lNotes.begin(); it != lNotes.end(); it++){
		(*it)->Draw();
	}
	for(auto it = rNotes.begin(); it != rNotes.end(); it++){
		(*it)->Draw();
	}
	

	gameManager_->SpriteDraw();

	AddBackUIDraw();
}

void BaseBattleScene::SceneGameEnd()
{
	if(isHome)		sceneManager->SetNextScene(new HomeScene(dxCommon,window));
	else if(isTitle)sceneManager->SetNextScene(new TitleScene(dxCommon,window));
	else			sceneManager->SetNextScene(new HomeScene(dxCommon,window));
}

void BaseBattleScene::SceneChange()
{
	//PrevSceneからの移動後処理
	if(isPrevSceneChange_){
		if(!postEffect->FadeIn()) return;
		
		isPrevSceneChange_ = false;
		rhythmManager_->TimeStart();
		gameManager_->AudioPlay(bpm120Game_audio.number, bpm120Game_audio.volume, true);
		return;
	}
	//NextSceneへの移動
	else if(isNextSceneChange_ || isGameEnd_){
		if(!postEffect->FadeOut()) return;

		isDrawStop = true;

		gameManager_->AudioStop(bpm120Game_audio.number);
		
		camera->Reset();
		if(isNextSceneChange_)NextSceneChange();
		else if(isGameEnd_)SceneGameEnd();
	}
}

void BaseBattleScene::ObjectFinaize()
{
#pragma region _3D解放
	player_->Finalize();
	exit_->Finalize();
#pragma endregion _3D解放

#pragma region _2D解放
	for(auto it = lNotes.begin(); it != lNotes.end(); it++){
		(*it)->Finalize();
	}
	for(auto it = rNotes.begin(); it != rNotes.end(); it++){
		(*it)->Finalize();
	}
	judgeLoca_->Finalize();

	for(const auto& it : resultText){
		it->Finalize();
	}

#pragma endregion _2D解放

	AddObjectFinalize();
}

void BaseBattleScene::CommonFinalize()
{
	gameManager_->Finalize();

	rhythmManager_ = nullptr;

	AddCommonFinalize();
}


void BaseBattleScene::ResultUpdate()
{
	if(!isResult) return;
	if(isHome || isTitle) return;

	bool isUp = (input->Trigger(DIK_W)||input->Trigger(DIK_UP) ||
		input->PadButtonTrigger(XINPUT_GAMEPAD_DPAD_UP) || input->PadLStickUpTrigger());
	bool isDown = (input->Trigger(DIK_S)||input->Trigger(DIK_DOWN) ||
		input->PadButtonTrigger(XINPUT_GAMEPAD_DPAD_DOWN) || input->PadLStickDownTrigger());

	//入力
	if(isUp) {
		resultText[select]->SetColor({1.f,1.f,1.f,1.0f});
		select--;
		select = max(select,0);
		resultText[select]->SetColor({ResultColor.x,ResultColor.y,ResultColor.z,1.0f});
	}
	else if(isDown) {
		resultText[select]->SetColor({1.f,1.f,1.f,1.0f});
		select++;
		select = min(select,1);
		resultText[select]->SetColor({ResultColor.x,ResultColor.y,ResultColor.z,1.0f});
	}

	//決定
	bool isEnter = (input->Trigger(DIK_Z) || input->Trigger(DIK_SPACE) || input->Trigger(DIK_RETURN) || 
		input->PadButtonTrigger(XINPUT_GAMEPAD_A));

	if(isEnter){
		if(select == 0)		 isHome = true;
		else if(select == 1) isTitle = true;
	}

	backSp->Update();
	for(const auto& it : resultText){
		it->Update();
	}

	if(!isHome && !isTitle) return;

	isGameEnd_ = true;
	postEffect->FadeStart();
}

void BaseBattleScene::ResultDraw2D()
{
	if(!isResult) return;

	backSp->Draw();
	for(const auto& it : resultText){
		it->Draw();
	}
}

