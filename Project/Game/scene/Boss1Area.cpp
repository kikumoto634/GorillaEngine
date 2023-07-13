#include "Boss1Area.h"
#include "HomeScene.h"
#include "SceneManager.h"

#include "Easing.h"

using namespace std;

Boss1Area::~Boss1Area()
{
}

Boss1Area::Boss1Area(DirectXCommon *dxCommon, Window *window, int saveHP)
	:BaseBattleScene(
		dxCommon,
		window)
{
}

void Boss1Area::NextSceneChange()
{
	sceneManager->SetNextScene(new HomeScene(dxCommon,window));
}

void Boss1Area::SceneChange()
{
	//PrevSceneからの移動後処理
	if(isPrevSceneChange_){
		if(!postEffect->FadeIn()) return;
		
		isPrevSceneChange_ = false;
		return;
	}
	//NextSceneへの移動
	else if(isNextSceneChange_ || isGameEnd_){
		if(!postEffect->FadeOut()) return;

		isDrawStop = true;
		
		camera->Reset();
		if(isNextSceneChange_)NextSceneChange();
		else if(isGameEnd_)SceneGameEnd();
	}
}

void Boss1Area::AddCommonInitialize()
{
	//ダンジョン
	areaManager_->CSVAreaInitialize("Boss1");
}

void Boss1Area::AddObject3DInitialize()
{
	indestructibleWallModel_ = new ObjModelManager();
	indestructibleWallModel_->CreateModel("GroundBlock3");
	indestructibleWallColliderModel_ = new ObjModelManager();
	indestructibleWallColliderModel_->CreateModel("GroundBlock2_Collider");
	EnterInitialize();
	ExitInitialize();

	BossInitialize();
	SkeltonInitialize();

	for(int i = 0; i < coinPopNumMax_; i++){
		unique_ptr<Coins> newObj = make_unique<Coins>();
		newObj->Initialize("Coins");
		obj_.push_back(move(newObj));
	}
}

void Boss1Area::AddObject2DInitialize()
{
	CutInInitialize();
}

void Boss1Area::AddCommonUpdate()
{
	areaManager_->CSVAreaUpdate(camera, player_->GetPosition());

	cutInInput();
	EventUpdate();
}

void Boss1Area::AddObject3DUpdate()
{
	exit_->SetExitOpenNeedCoin(0);
	exit_->NeedCoinSpriteUpdate();

	EnterUpdate();
	ExitUpdate();

	BossUpdate();
	SkeltonUpdate();

	for(auto it = obj_.begin(); it != obj_.end(); it++){
		if((*it)->GetIsContactTrigger()){
			gameManager_->CoinIncrement();
			gameManager_->AudioPlay(coinGet_audio.number,coinGet_audio.volume);
		}

		if(!coinDropPos_.empty() && (*it)->GetIsPopsPmposibble_()){
			(*it)->Pop(coinDropPos_.front());
			coinDropPos_.pop();
		}

		(*it)->Update(this->camera);
	}
}

void Boss1Area::AddObject2DUpdate()
{
	CutInUpdate();
}

void Boss1Area::AddBeatEndUpdate()
{
	BossBeatEnd();
	SkeltonBeatEnd();

	if(isEventBGM_){
		gameManager_->AudioPlay(bpm120Game_audio.number, bpm120Game_audio.volume, true);
		isEventBGM_ = false;
	}

	for(auto it = obj_.begin(); it != obj_.end(); ++it){
		if(!(*it)->GetIsAlive()) continue;
		(*it)->IsBeatEndOn();
	}
}

void Boss1Area::AddObject3DDraw()
{
	areaManager_->CSVAreaDraw();

	EnterDraw();
	ExitDraw();

	BossDraw();
	SkeltonDraw();

	for(auto it = obj_.begin(); it != obj_.end(); ++it){
		(*it)->Draw();
	}
}

void Boss1Area::AddParticleDraw()
{
	BossParticleDraw();
	SkeltonParticleDraw();
}

void Boss1Area::AddFrontUIDraw()
{
}

void Boss1Area::AddBackUIDraw()
{
	CutInDraw();
}

void Boss1Area::AddObjectFinalize()
{
	for(auto it = obj_.begin(); it != obj_.end(); ++it){
		(*it)->Finalize();
	}

	CutInFinalize();
	EnterFinalize();
	ExitFinalize();
	delete indestructibleWallModel_;
	delete indestructibleWallColliderModel_;
	BossFinalize();
	SkeltonFinalize();
}

void Boss1Area::AddCommonFinalize()
{
	areaManager_->CSVAreaFinalize();
}

#pragma region カットイン
void Boss1Area::CutInInitialize()
{
	//センター
	unique_ptr<BaseSprites> center = make_unique<BaseSprites>();
	center->Initialize(cutInBoss_tex.number);
	center->SetAnchorPoint(CutInSpAnc);
	center->SetSize(CutInSpCenterSize);

	//上
	unique_ptr<BaseSprites> up = make_unique<BaseSprites>();
	up->Initialize(cutInParts2_tex.number);
	up->SetAnchorPoint(CutInSpAnc);
	up->SetSize(CutInSpUpDownSize);

	//下
	unique_ptr<BaseSprites> down = make_unique<BaseSprites>();
	down->Initialize(cutInParts1_tex.number);
	down->SetAnchorPoint(CutInSpAnc);
	down->SetSize(CutInSpUpDownSize);

	//名前
	unique_ptr<BaseSprites> name = make_unique<BaseSprites>();
	name->Initialize(bossName_tex.number);
	name->SetAnchorPoint(CutInSpAnc);
	name->SetSize(CutInSpNameSize);

	//追加
	cutInSp_.push_back(move(center));
	cutInSp_.push_back(move(up));
	cutInSp_.push_back(move(down));
	cutInSp_.push_back(move(name));

	cutInSpPos_.resize(CutInSpNum);

	//イベント状態
	player_->EventBegin();
}

void Boss1Area::cutInInput()
{
	if(!isCutInAlive_) return;
	
	bool isDirKey = !input->Trigger(DIK_UP) && !input->Trigger(DIK_LEFT) && !input->Trigger(DIK_DOWN) && !input->Trigger(DIK_RIGHT);
	bool isWASD = !input->Trigger(DIK_W) && !input->Trigger(DIK_A) && !input->Trigger(DIK_S) && !input->Trigger(DIK_D);
	bool isEnter = !input->Trigger(DIK_Z) && !input->Trigger(DIK_SPACE) && !input->Trigger(DIK_RETURN);
	bool isPadDir = !input->PadButtonPush(XINPUT_GAMEPAD_DPAD_UP) && !input->PadButtonPush(XINPUT_GAMEPAD_DPAD_DOWN) && 
		!input->PadButtonPush(XINPUT_GAMEPAD_DPAD_RIGHT) && !input->PadButtonPush(XINPUT_GAMEPAD_DPAD_LEFT);
	bool isPadEnter = !input->PadButtonPush(XINPUT_GAMEPAD_A) && !input->PadButtonPush(XINPUT_GAMEPAD_B) && 
		!input->PadButtonPush(XINPUT_GAMEPAD_X) && !input->PadButtonPush(XINPUT_GAMEPAD_Y);

	if(isDirKey && isWASD && isEnter && isPadDir && isPadEnter) return;
		

	isBossAppUIFlag_ = false;
	//リズム
	rhythmManager_->TimeStart();
	//イベント
	player_->EventEnd();
}

void Boss1Area::CutInUpdate()
{
	if(!isCutInAlive_) return;
	//開始時
	if(isBossAppUIFlag_ && !isCutInUIMove_){
		//時間
		cutInMoveFrame_ = Time_OneWay(cutInMoveFrame_, CutInMoveSecondMax);

		//移動
		cutInSpPos_[CutInSpCenterNumber] = Easing_Point2_Linear<Vector2>(CutInSpCenterBeginPos, CutInSpCenterEndPos, cutInMoveFrame_);
		cutInSpPos_[CutInSpUpNumber]     = Easing_Point2_Linear<Vector2>(CutInSpUpBeginPos, CutInSpUpEndPos, cutInMoveFrame_);
		cutInSpPos_[CutInSpDownNumber]   = Easing_Point2_Linear<Vector2>(CutInSpDownBeginPos, CutInSpDownEndPos, cutInMoveFrame_);
		cutInSpPos_[CutInSpNameNumber]   = Easing_Point2_Linear<Vector2>(CutInSpNameBeginPos, CutInSpNameEndPos, cutInMoveFrame_);

		//目標値
		if(cutInMoveFrame_ >= 1.0){
			//音声
			gameManager_->AudioPlay(cutIn_audio.number, cutIn_audio.volume);

			cutInSpPos_[CutInSpCenterNumber] = CutInSpCenterEndPos;
			cutInSpPos_[CutInSpUpNumber]     = CutInSpUpEndPos;
			cutInSpPos_[CutInSpDownNumber]   = CutInSpDownEndPos;
			cutInSpPos_[CutInSpNameNumber]   = CutInSpNameEndPos;
			cutInMoveFrame_ = 0.f;

			//移動完了
			isCutInUIMove_ = true;
		}
	}
	//UI引き
	else if(!isBossAppUIFlag_ && isCutInUIMove_){
		//時間
		cutInMoveFrame_ = Time_OneWay(cutInMoveFrame_, CutInMoveSecondMax);

		//移動
		cutInSpPos_[CutInSpCenterNumber] = Easing_Point2_Linear<Vector2>(CutInSpCenterEndPos, CutInSpCenterBeginPos, cutInMoveFrame_);
		cutInSpPos_[CutInSpUpNumber]     = Easing_Point2_Linear<Vector2>(CutInSpUpEndPos, CutInSpUpBeginPos, cutInMoveFrame_);
		cutInSpPos_[CutInSpDownNumber]   = Easing_Point2_Linear<Vector2>(CutInSpDownEndPos, CutInSpDownBeginPos, cutInMoveFrame_);
		cutInSpPos_[CutInSpNameNumber]   = Easing_Point2_Linear<Vector2>(CutInSpNameEndPos, CutInSpNameBeginPos, cutInMoveFrame_);

		//目標値
		if(cutInMoveFrame_ >= 1.0){
			cutInSpPos_[CutInSpCenterNumber] = CutInSpCenterBeginPos;
			cutInSpPos_[CutInSpUpNumber]     = CutInSpUpBeginPos;
			cutInSpPos_[CutInSpDownNumber]   = CutInSpDownBeginPos;
			cutInSpPos_[CutInSpNameNumber]   = CutInSpNameBeginPos;
			isCutInAlive_ = false;
		}
	}

	//更新
	int i = 0;
	for(const auto& it : cutInSp_){
		it->SetPosition(cutInSpPos_[i]);
		it->Update();
		i++;
	}
}

void Boss1Area::CutInDraw()
{
	if(!isCutInAlive_) return;
	for(const auto& it : cutInSp_){
		it->Draw();
	}
}

void Boss1Area::CutInFinalize()
{
	for(const auto& it : cutInSp_){
		it->Finalize();
	}
}
#pragma endregion


#pragma region 入口
void Boss1Area::EnterInitialize()
{
	for(int i = 0; i < BlocksNum; i++){
		unique_ptr<IndestructibleWall> obj = make_unique<IndestructibleWall>();
		obj->Initialize(indestructibleWallModel_,indestructibleWallColliderModel_);
		obj->SetPosition({EnterWallBasePos.x + BlockSize*i, EnterWallBasePos.y, EnterWallBasePos.z});
		enterWall_.push_back(move(obj));
	}
}

void Boss1Area::EnterUpdate()
{
	if(!isEnterBlocksAlive_){
		if(player_->GetPosition().z < EnterCloseBorderPosZ) return;
		camera->ShakeStart();
		gameManager_->AudioPlay(gateEnter_audio.number);
		isEnterBlocksAlive_ = true;

		EventStart();

		return;
	}

	for(const auto& it : enterWall_){
		it->Update(camera);
		it->SetPlayerPos(player_->GetPosition());
	}
}

void Boss1Area::EnterDraw()
{
	if(!isEnterBlocksAlive_) return;
	for(const auto& it : enterWall_){
		it->Draw();
	}
}

void Boss1Area::EnterFinalize()
{
	for(const auto& it : enterWall_){
		it->Finalize();
	}
}
#pragma endregion


#pragma region 出口
void Boss1Area::ExitInitialize()
{
	for(int i = 0; i < BlocksNum; i++){
		unique_ptr<IndestructibleWall> obj = make_unique<IndestructibleWall>();
		obj->Initialize(indestructibleWallModel_,indestructibleWallColliderModel_);
		obj->SetPosition({ExitWallBasePos.x + BlockSize*i, ExitWallBasePos.y, ExitWallBasePos.z});
		exitWall_.push_back(move(obj));
	}
}

void Boss1Area::ExitUpdate()
{
#ifdef _DEBUG
	if(input->Trigger(DIK_2) && isExitBlocksAlive_){
		ExitOpen();
	}
#endif // _DEBUG

	if(!boss_->GetIsAlive() && isExitBlocksAlive_){
		ExitOpen();
	}

	if(!isExitBlocksAlive_) return;
	for(const auto& it : exitWall_){
		it->Update(camera);
		it->SetPlayerPos(player_->GetPosition());
	}
}

void Boss1Area::ExitDraw()
{
	if(!isExitBlocksAlive_) return;
	for(const auto& it : exitWall_){
		it->Draw();
	}
}

void Boss1Area::ExitFinalize()
{
	for(const auto& it : exitWall_){
		it->Finalize();
	}
}

void Boss1Area::ExitOpen()
{
	for(const auto& it : exitWall_){
		it->ColliderRemove();
	}

	isExitBlocksAlive_ = false;
	camera->ShakeStart();
	gameManager_->AudioPlay(gateEnter_audio.number);
}
#pragma endregion


#pragma region ボス
void Boss1Area::BossInitialize()
{
	boss_ = make_unique<Boss1>();
	boss_->Initialize("necromancer");
	Vector3 lpos = areaManager_->GetCSVObjectPopPosition(0);
	lpos.y = -3.f;
	boss_->BossPopInit(lpos);
}

void Boss1Area::BossUpdate()
{
	if(!isBossAlive_) return;
	if(boss_->GetIsContactTrigger()){
		gameManager_->AudioPlay(damage_audio.number, damage_audio.volume);
	}
	boss_->SetMapInfo(areaManager_->GetMapInfo());
	boss_->Update(camera, player_->GetPosition());
}

void Boss1Area::BossBeatEnd()
{
	if(!isBossAlive_) return;
	boss_->IsBeatEndOn();

	if(boss_->GetIsEvent()) return;
	SkeltonCreate();

	if(boss_->GetIsBpmChange()&&Time_OneWay(changeTime, ChangeTime)){
		changeTime = 0;
		gameManager_->AudioPlay(roar_audio.number, roar_audio.volume);

		if(boss_->GetBPMValue().BPM == 90){
			rhythmManager_->BPMLowSet();
			gameManager_->AudioRatio(bpm120Game_audio.number, 0.75f);
		}
		else if(boss_->GetBPMValue().BPM == 120){
			rhythmManager_->BPMNormalSet();
			gameManager_->AudioRatio(bpm120Game_audio.number, 1.f);
		}
		else if(boss_->GetBPMValue().BPM == 150){
			rhythmManager_->BPMHighSet();
			gameManager_->AudioRatio(bpm120Game_audio.number, 1.25f);
		}
	}
}

void Boss1Area::BossDraw()
{
	if(!isBossAlive_) return;
	boss_->Draw();
}

void Boss1Area::BossParticleDraw()
{
	if(!isBossAlive_) return;
	boss_->ParticleDraw();
}

void Boss1Area::BossFinalize()
{
	boss_->Finalize();
}
#pragma endregion

#pragma region スケルトン
void Boss1Area::SkeltonCreate()
{
	if(boss_->GetIsSummon()) return;
	for(const auto& it : enemys_){
		if(!it->GetIsPopsPmposibble_()) continue;

		it->Pop(boss_->GetSummonObjPos());
		it->SetRotation({0,DirectX::XMConvertToRadians(180),0.f});
		break;
	}
}

void Boss1Area::SkeltonInitialize()
{
	for(int i = 0; i < boss_->GetSummonMax(); i++){
		unique_ptr<Skelton> obj = make_unique<Skelton>();
		obj->Initialize("Skeleton");

		enemys_.push_back(move(obj));
	}
}

void Boss1Area::SkeltonUpdate()
{
	for(const auto& it : enemys_){
		if(!boss_->GetIsAlive()){
			it->SetIsAlive(false);
		}

		if(it->GetIsContactTrigger()){
			gameManager_->AudioPlay(damage_audio.number, damage_audio.volume);
			coinDropPos_.push(it->GetPopPosition());
		}
		it->SetMapInfo(areaManager_->GetMapInfo());
		it->Update(camera,player_->GetPosition());
	}
}

void Boss1Area::SkeltonBeatEnd()
{
	for(const auto& it : enemys_){
		it->IsBeatEndOn();
	}
}

void Boss1Area::SkeltonDraw()
{
	for(const auto& it : enemys_){
		it->Draw();
	}
}

void Boss1Area::SkeltonParticleDraw()
{
	for(const auto& it : enemys_){
		it->ParticleDraw();
	}
}

void Boss1Area::SkeltonFinalize()
{
	for(const auto& it : enemys_){
		it->Finalize();
	}
}
#pragma endregion

#pragma region イベント
void Boss1Area::EventStart()
{
	isEventActive_ = true;
	isBossAlive_  = true;

	player_->EventBegin();
	boss_->EventBegin();

	targetSaveValue = camera->GetTarget();
	eyeSaveValue = camera->GetEye();
}

void Boss1Area::EventUpdate()
{
	if(!isEventActive_) return;
	switch (event_)
	{
	case Boss1Area::EventState::Approach:

		//Target
		targetValue = Easing_Point2_EaseInCubic<Vector3>(
			targetSaveValue, TargetEnd,
			eventSecond);

		//Eye
		eyeValue = Easing_Point2_EaseInCubic<Vector3>(
			eyeSaveValue, EyeEnd,
			eventSecond);

		if(Time_OneWay(eventSecond, EventApproachSecond) < 1.0f) break;
		targetValue = TargetEnd;
		eyeValue = EyeEnd;
		eventSecond = 0;
		event_ = EventState::Roar;

		gameManager_->AudioPlay(roar_audio.number, roar_audio.volume);
		isRoar_ = true;
		postEffect->BlurStart();
		camera->ShakeStart();
		input->PadVibrationLeap(2.0f);

		break;
	case Boss1Area::EventState::Roar:
		if(!isRoar_) break;
		postEffect->Blur(eventSecond, EventRoarSecond);

		if(Time_OneWay(eventSecond, EventRoarSecond) < 1.0f) break;

		postEffect->BlurEnd();
		isEventBGM_ = true;
		isRoar_ = false;
		eventSecond = 0.f;
		event_ = EventState::Return;

		break;
	case Boss1Area::EventState::Return:
	default:

		//Target
		targetValue = Easing_Point2_EaseInCubic<Vector3>(
			TargetEnd,targetSaveValue,
			eventSecond);

		//Eye
		eyeValue = Easing_Point2_EaseInCubic<Vector3>(
			EyeEnd, eyeSaveValue,
			eventSecond);

		if(Time_OneWay(eventSecond, EventApproachSecond) < 1.0f) break;
		
		targetValue = targetSaveValue;
		eyeValue = eyeSaveValue;
		eventSecond = 0;

		isEventActive_ = false;
		player_->EventEnd();
		boss_->EventEnd();
		break;
	}

	camera->SetTarget(targetValue);
	camera->SetEye(eyeValue);
}
#pragma endregion