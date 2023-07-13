#include "GameManager.h"
#include "ObjModelObject.h"

using namespace std;

void GameManager::Initialize()
{
	AudioInitialize();
	LightInitialize();
	SpriteInitialize();
}

void GameManager::Finalize()
{
	for(int i = 0; i < HpSpSize; i++){
		hpSp_[i]->Finalize();
	}

	combo_->Finalize();

	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_coin_[i]->Finalize();
	}
	coinSp_->Finalize();

	delete lightGroup_;
	lightGroup_ = nullptr;

	audio_->Finalize();
}

void GameManager::ComboIncrement()
{
	combo_->Increment();
}

void GameManager::ComboReset()
{
	AudioPlay(miss_audio.number,miss_audio.volume);
	combo_->Reset();
}

void GameManager::CoinIncrement()
{
	coinNum_ += 1;

	//スプライト更新
	int hundred = coinNum_/100;
	int ten = (coinNum_ - (hundred*100))/10;
	int one = (coinNum_ - (hundred*100) - (ten*10))/1;
	numberSp_coin_[0]->SetTexNumber(hundred + TexNumberBegin);
	numberSp_coin_[1]->SetTexNumber(ten + TexNumberBegin);
	numberSp_coin_[2]->SetTexNumber(one + TexNumberBegin);
}

void GameManager::InitializeSetHp(int _hp)
{
	hp_ = _hp;

	//体力
	damageHpSpriteIndex_ = hp_-1;
	for(int i = 0; i < HpSpSize; i++){
		std::unique_ptr<BaseSprites> Sp = make_unique<BaseSprites>();
		if(i <= damageHpSpriteIndex_){
			Sp->Initialize(heart_tex.number);
			Sp->SetSize({75,75});
		}
		else if(i > damageHpSpriteIndex_){
			Sp->Initialize(heartEmpty_tex.number);
			Sp->SetSize({50,50});
		}
		Sp->SetPosition({float(900 + (i*80)), 55});
		Sp->SetAnchorPoint({0.5f,0.5f});
		hpSp_.push_back(move(Sp));
	}
}

void GameManager::HpDecrement()
{
	if(damageHpSpriteIndex_ < 0)	return;

	hpSp_[damageHpSpriteIndex_]->SetTexNumber(15);
	hpSp_[damageHpSpriteIndex_]->SetSize({50,50});
	damageHpSpriteIndex_--;
}

void GameManager::HpIncrement()
{
	if(damageHpSpriteIndex_ >= 5)	return;

	hpSp_[damageHpSpriteIndex_]->SetSize({75,75});
	damageHpSpriteIndex_++;
	hpSp_[damageHpSpriteIndex_]->SetTexNumber(14);
}

void GameManager::AudioPlay(int number, float volume, bool loop)
{
	audio_->PlayWave(number, volume, loop);
}

void GameManager::AudioStop(int number)
{
	audio_->StopWave(number);
}

void GameManager::AudioRatio(int number, float ratio)
{
	audio_->SetRation(number, ratio);
}

void GameManager::LightUpdate(bool IsPlayerShadowDead)
{
	if(!isPlayerShadowAlive_ && !IsPlayerShadowDead){	
		//丸影
		lightGroup_->SetCircleShadowDir(0, {CircleShadowDir.x, CircleShadowDir.y, CircleShadowDir.z, 0});
		lightGroup_->SetCircleShadowAtten(0, CircleShadowAtten);
		lightGroup_->SetCircleShadowFactorAngle(0, CircleShadowFactorAngle);
	}

	if(IsPlayerShadowDead){
		isPlayerShadowAlive_ = true;
		lightGroup_->SetCircleShadowActive(0,false);
	}
	lightGroup_->Update();
}

void GameManager::PlayerCircleShadowSet(Vector3 pos)
{
	//プレイヤー、丸影座標
	lightGroup_->SetCircleShadowCasterPos(0, pos);
}

int GameManager::EnemyPopTurnCount()
{
	currentEnemyPopBeatTurn_++;
	if(currentEnemyPopBeatTurn_ >= EnemyPopBeatTurn){
		currentEnemyPopBeatTurn_ = 0;
		return EnemyPopCreateNum;
	}

	return 0;
}

Vector2 GameManager::EnemyRandomPos(const int groundWidth, const float Plane_Size)
{
	Vector2 lpos;
	lpos.x = static_cast<float>(rand() % (groundWidth) - 5) * Plane_Size;
	lpos.y = static_cast<float>(rand() % (groundWidth) - 5) * Plane_Size;

	return lpos;
}

Vector2 GameManager::EnemyRandomDir(Vector2 pos)
{
	Vector2 ldir;
	//0:{0,1}, 1:{0,-1}, 2:{1,0}, 3:{-1,0}
	int random = rand()%4;	//0~3
	
	//奥左
	if(pos.x < 0 && pos.y < 0){
		//右向き
		if(random >= 1){
			ldir = {1,0};
		}
		//手前向き
		else if(random <= 2){
			ldir = {0,1};
		}
	}
	//奥右
	else if(pos.x > 0 && pos.y < 0){
		//左向き
		if(random >= 1){
			ldir = {-1,0};
		}
		//手前向き
		else if(random <= 2){
			ldir = {0,1};
		}
	}
	//手前右
	else if(pos.x > 0 && pos.y > 0){
		//左向き
		if(random >= 1){
			ldir = {-1,0};
		}
		//奥向き
		else if(random <= 2){
			ldir = {0,-1};
		}
	}
	//手前左
	else if(pos.x < 0 && pos.y > 0){
		//右向き
		if(random >= 1){
			ldir = {1,0};
		}
		//奥向き
		else if(random <= 2){
			ldir = {0,-1};
		}
	}
	//中心地
	else {
		//左
		if(random == 0){
			ldir = {-1,0};
		}
		//右
		else if(random == 1){
			ldir = {1,0};
		}
		//奥
		else if(random == 2){
			ldir = {0,-1};
		}
		//手前
		else if(random == 3){
			ldir = {0,1};
		}
	}

	return ldir;
}

void GameManager::SpriteUpdate()
{
	combo_->Update();

	coinSp_->Update();
	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_coin_[i]->Update();
	}

	if(isHpScaleChange_){
		if(damageHpSpriteIndex_ < 0) return;
		if(hpSp_[damageHpSpriteIndex_]->ScaleChange({75,75}, {50,50})){
			isHpScaleChange_ = false;
		}
	}
	for(int i = 0; i < HpSpSize; i++){
		hpSp_[i]->Update();
	}
}

void GameManager::SpriteDraw()
{
	combo_->Draw();

	coinSp_->Draw();
	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_coin_[i]->Draw();
	}

	for(int i = 0; i < HpSpSize; i++){
		hpSp_[i]->Draw();
	}
}

void GameManager::AudioInitialize()
{
	audio_ = Audio::GetInstance();
}

void GameManager::LightInitialize()
{
	//ライト
	lightGroup_ = LightGroup::Create();
	//色設定
	lightGroup_->SetAmbientColor({0.15f, 0.15f, 0.15f});
	//3Dオブジェクト(.obj)にセット
	ObjModelObject::SetLight(lightGroup_);

	lightGroup_->SetDirLightActive(0, true);
	lightGroup_->SetDirLightActive(1, true);
	lightGroup_->SetDirLightActive(2, true);

	//丸影
	lightGroup_->SetCircleShadowActive(0, true);
}

void GameManager::SpriteInitialize()
{
	combo_ = make_unique<Combo>();
	combo_->Initialize();

	//コインテキスト
	coinSp_ = make_unique<BaseSprites>();
	coinSp_->Initialize(coin_tex.number);
	coinSp_->SetPosition({50,175});
	coinSp_->SetSize({75,75});

	//数字
	for(int i = 0;i < NumberSpSize; i++){
		numberSp_coin_[i] = make_unique<BaseSprites>();
		numberSp_coin_[i]->Initialize(TexNumberBegin + 0);
		numberSp_coin_[i]->SetPosition({float(125+(i*50)),175});
		numberSp_coin_[i]->SetSize({50,75});
	}
}
