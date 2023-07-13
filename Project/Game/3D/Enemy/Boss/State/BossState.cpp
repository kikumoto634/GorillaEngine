#include "BossState.h"
#include "BossStateManager.h"
#include "Boss1.h"
#include "Easing.h"
#include <cassert>

using namespace std;

void BossState::Initialize(Boss1 *boss)
{
	assert(boss);
	boss_ = boss;
}



void IdelBossState::UpdateTrigger()
{
}

void IdelBossState::Update()
{
	if(boss_->GetIsEvent()) return;
	if(!boss_->isBeatEnd_) return;

	waitCount_++;
	if(waitCount_ <= WaitCountMax) return;
	waitCount_ = 0;
	if(!boss_->isSummonComp_ || boss_->summonNum_ >= boss_->SummonMax) {
		stateManager_->SetNextState(new SummonBossState);
		return;
	}
	stateManager_->SetNextState(new TrackBossState);
}

void IdelBossState::ParticleDraw()
{
}



void SummonBossState::UpdateTrigger()
{
	boss_->SetRotation({0,DirectX::XMConvertToRadians(180),0.f});
}

void SummonBossState::Update()
{
	App();


	if(isStateIntarval_){
		if(Time_OneWay(stateInterval_,StateIntarvalMax) >= 1.f){
			queue<Vector3> empty;
			boss_->summonObjPos.swap(empty);
			stateManager_->SetNextState(new IdelBossState);
		}
		return;
	}

	//召喚終了
	if(boss_->isSummon_){
		isStateIntarval_ = true;
		boss_->summonNum_ = 0;
		boss_->isSummon_ = false;
		boss_->isSummonComp_ = true;
		return;
	}

	//召喚座標
	if(boss_->isSummon_) return;
	if(boss_->summonNum_ >= boss_->SummonMax){
		boss_->isSummon_ = true;
	}

	Vector3 pos = {};
	int X = 0;
	int Y = 0;
	do{
		pos = boss_->GetPosition();
		pos = {pos.x + (BlockSize*(rand()%7-3)), pos.y, pos.z + (BlockSize*(rand()%4-1))};

		X = +int(pos.x/AreaBlockSize)+AreaBlocksHalfNum;
		Y = -int(pos.z/AreaBlockSize)+AreaBlocksHalfNum;
	}while(boss_->mapInfo_[Y][X] != 1);
	boss_->summonObjPos.push(pos);
	boss_->summonNum_++;
}

void SummonBossState::ParticleDraw()
{
	boss_->summonParticle_->Draw();
}

void SummonBossState::App()
{
	pos.x = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos/2.0f;
	pos.y = PosY;
	pos.z = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos/2.0f;
	pos += boss_->GetPosition();

	vel.x = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel / 2.0f;
	vel.y = VelY;
	vel.z = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel / 2.0f;

	acc.y = AccY;

	boss_->summonParticle_->ParticleSet(ParticleAliveFrameMax,pos,vel,acc,SizeStart,SizeEnd,TextureNumber,Color);
	boss_->summonParticle_->ParticleAppearance();
}



void TrackBossState::UpdateTrigger()
{
	easigStartPos_ = boss_->world_.translation;
	//移動先計算
	mapPath_ = boss_->mapInfo_;
	eX_ = +int(boss_->world_.translation.x/AreaBlockSize)+AreaBlocksHalfNum;
	eY_ = -int(boss_->world_.translation.z/AreaBlockSize)+AreaBlocksHalfNum;
	
	if(boss_->moveCount < MoveCountMax){
		pX_ = +int(boss_->playerPos_.x/AreaBlockSize)+AreaBlocksHalfNum;
		pY_ = -int(boss_->playerPos_.z/AreaBlockSize)+AreaBlocksHalfNum;
	}
	else if(boss_->moveCount >= MoveCountMax){
		pX_ = TrackGoalMapIndexX;
		pY_ = TrackGoalMapIndexY;
	}

	path_ = boss_->PathSearch(boss_->mapInfo_, eX_,eY_, pX_,pY_);

	int root = RootPathStartNumber;
	for(auto it = path_.begin(); it != path_.end(); it++){
		mapPath_[(*it)->y][(*it)->x] = root;
		root++;
	}
	mapPath_[eY_][eX_] = RootPathSkeltonNumber;
	mapPath_[pY_][pX_] = RootPathGoalNumber;

	//上下左右のルート先
    int dx[PathDirection] = {-1, 1, 0, 0};
    int dy[PathDirection] = {0, 0, -1, 1};

    //四方向をforで調べる
    for (int j = 0; j < PathDirection; j++) {
        //dx,dy方向のルート移動
        int next_x = eX_ + dx[j];
        int next_y = eY_ + dy[j];
		if(next_x < 0 || next_y < 0 || 31 <= next_x || 31 <= next_y) return;

        if(mapPath_[next_y][next_x] == pathRoot_ || mapPath_[next_y][next_x] == RootPathGoalNumber){
			easingEndPos_ = boss_->world_.translation + Vector3{dx[j]*AreaBlockSize, 0.f, -dy[j]*AreaBlockSize};
			eX_ = next_x;
			eY_ = next_y;

			//回転
			if(j == 0){//Left
				boss_->world_.rotation.y = LeftAngle;
			}
			else if(j == 1){//Right
				boss_->world_.rotation.y = RightAngle;
			}
			else if(j == 2){//down
				boss_->world_.rotation.y = DownAngle;
			}
			else if(j == 3){//up
				boss_->world_.rotation.y = UpAngle;
			}

			if(mapPath_[next_y][next_x] == RootPathGoalNumber) {
				pathRoot_ = RootPathNumber;

				boss_->isSummonComp_ = false;
				boss_->moveCount = 0;
				stateManager_->SetNextState(new IdelBossState);
			}
			else if(mapPath_[next_y][next_x] == pathRoot_){
				pathRoot_++;
			}
			break;
		}

		easingEndPos_ = boss_->world_.translation;
    }
}

void TrackBossState::Update()
{
	//移動処理
	boss_->world_.translation = Easing_Point2_Linear(easigStartPos_, easingEndPos_, Time_OneWay(easingMoveTime_, EasingMoveTimeMax));

	//移動完了時
	if(easingMoveTime_ >= EasingFrameMax){
		boss_->world_.translation = easingEndPos_;
		easigStartPos_ = {};
		easingEndPos_ = {};
		easingMoveTime_ = 0.f;

		if(boss_->moveCount >= MoveCountMax*2){
			boss_->isSummonComp_ = false;
			boss_->moveCount = 0;
			stateManager_->SetNextState(new IdelBossState);
		}
		
		stateManager_->SetNextState(new IdelBossState);
		boss_->moveCount++;
	}
}

void TrackBossState::ParticleDraw()
{
}



void RunAwayBossState::UpdateTrigger()
{
	startPos = boss_->GetPosition();
	boss_->ColliderRemove();
}

void RunAwayBossState::Update()
{
	App();
	boss_->world_.translation = Easing_Point2_Linear(startPos,GoalPos,Time_OneWay(moveSecond, MoveSecondMax));

	if(moveSecond >= 1.0f){
		boss_->SetPosition(GoalPos);
		boss_->ColliderSet();

		

		if(boss_->hp_%2 == 0){
			stateManager_->SetNextState(new RhythmChangeBossState);
			return;
		}
		
		stateManager_->SetNextState(new SummonBossState);
	}
}

void RunAwayBossState::ParticleDraw()
{
	boss_->runAwayParticle_->Draw();
}

void RunAwayBossState::App()
{
	pos.x = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos/2.0f;
	pos.y = PosY;
	pos.z = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos/2.0f;
	pos += boss_->GetPosition();

	vel.x = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel / 2.0f;
	vel.y = VelY;
	vel.z = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel / 2.0f;

	acc.y = AccY;

	boss_->runAwayParticle_->ParticleSet(ParticleAliveFrameMax,pos,vel,acc,SizeStart,SizeEnd,TextureNumber,Color);
	boss_->runAwayParticle_->ParticleAppearance();
}



void RhythmChangeBossState::UpdateTrigger()
{
	boss_->isBpmChange = true;
	if(boss_->currentBPM.BPM == boss_->Normal.BPM){
		boss_->currentBPM = boss_->Fast;
	}
	else if(boss_->currentBPM.BPM == boss_->Fast.BPM){
		boss_->currentBPM = boss_->Normal;
	}
}

void RhythmChangeBossState::Update()
{
	App();

	if(Time_OneWay(changeTime, ChangeTime)){
		stateManager_->SetNextState(new IdelBossState);
	}
}

void RhythmChangeBossState::ParticleDraw()
{
}

void RhythmChangeBossState::App()
{
	for(int i = 0; i < 100; i++){
		pos.x = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos/2.0f;
		pos.y = PosY;
		pos.z = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos/2.0f;
		pos += boss_->GetPosition();

		vel.x = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel / 2.0f;
		vel.y = VelY;
		vel.z = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel / 2.0f;

		acc.x = AccY;
		acc.z = AccY;

		boss_->rhythmChangeParticle_->ParticleSet(ParticleAliveFrameMax,pos,vel,acc,SizeStart,SizeEnd,TextureNumber,Color);
		boss_->rhythmChangeParticle_->ParticleAppearance();
	}
}



void DeadBossState::UpdateTrigger()
{
	boss_->particlePos_ = boss_->GetPosition();
	boss_->SetPosition(boss_->NotAlivePos);
	boss_->world_.UpdateMatrix();
	boss_->baseCollider_->Update();

	App();
}

void DeadBossState::Update()
{
	boss_->particleAliveFrame_++;

	if(boss_->particleAliveFrame_ < ParticleAliveFrameMax) return;
	boss_->isAlive_ = false;
	queue<Vector3> empty;
	boss_->summonObjPos.swap(empty);
}

void DeadBossState::ParticleDraw()
{
	boss_->deadParticle_->Draw();
}

void DeadBossState::App()
{
	for (int i = 0; i < CreateNum; i++) {
		//速度
		vel.x = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel_Half;
		vel.y = VelY;
		vel.z = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel_Half;

		//加速度
		acc.y = AccY;

		//生成
		boss_->deadParticle_->ParticleSet(
			ParticleAliveFrameMax,
			boss_->particlePos_,
			vel,
			acc,
			SizeStart,
			SizeEnd,
			TextureNumber,
			Color
		);
		boss_->deadParticle_->ParticleAppearance();
	}
}

