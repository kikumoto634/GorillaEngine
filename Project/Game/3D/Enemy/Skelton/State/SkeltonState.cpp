#include "SkeltonState.h"
#include "SkeltonStateManager.h"
#include "Easing.h"
#include "Skelton.h"
#include <cassert>

using namespace std;

void SkeltonState::Initialize(Skelton *skelton)
{
	assert(skelton);
	skelton_ = skelton;
}




void IdelSkeltonState::UpdateTrigger()
{
}

void IdelSkeltonState::Update()
{
	//死亡時
	if(!skelton_->isAlive_){
		skelton_->popPosition_ = skelton_->world_.translation;
		stateManager_->SetNextState(new DeadSkeltonState);
	}

	//ビート時
	if(!skelton_->isBeatEnd_) return;

	//待機カウント終了時
	if(waitCount_ > WaltCountMax){
		waitCount_ = 0;
		stateManager_->SetNextState(new TrackSkeltonState);
	}

	waitCount_++;
}

void IdelSkeltonState::ParticleDraw()
{
}




void TrackSkeltonState::UpdateTrigger()
{
	easigStartPos_ = skelton_->world_.translation;
	//移動先計算
	mapPath_ = skelton_->mapInfo_;
	eX_ = +int(skelton_->world_.translation.x/AreaBlockSize)+AreaBlocksHalfNum;
	eY_ = -int(skelton_->world_.translation.z/AreaBlockSize)+AreaBlocksHalfNum;
	pX_ = +int(skelton_->playerPos_.x/AreaBlockSize)+AreaBlocksHalfNum;
	pY_ = -int(skelton_->playerPos_.z/AreaBlockSize)+AreaBlocksHalfNum;
	path_ = skelton_->PathSearch(skelton_->mapInfo_, eX_,eY_, pX_,pY_);

	int root = RootPathStartNumber;
	for(auto it = path_.begin(); it != path_.end(); it++){
		mapPath_[(*it)->y][(*it)->x] = root;
		root++;
	}
	mapPath_[eY_][eX_] = RootPathSkeltonNumber;
	mapPath_[pY_][pX_] = RootPathPlayerNumber;

	//上下左右のルート先
    int dx[PathDirection] = {-1, 1, 0, 0};
    int dy[PathDirection] = {0, 0, -1, 1};

    //四方向をforで調べる
    for (int j = 0; j < PathDirection; j++) {
        //dx,dy方向のルート移動
        int next_x = eX_ + dx[j];
        int next_y = eY_ + dy[j];
		if(next_x < 0 || next_y < 0 || 31 <= next_x || 31 <= next_y) return;

        if(mapPath_[next_y][next_x] == pathRoot_ || mapPath_[next_y][next_x] == RootPathPlayerNumber){
			easingEndPos_ = skelton_->world_.translation + Vector3{dx[j]*AreaBlockSize, 0.f, -dy[j]*AreaBlockSize};
			eX_ = next_x;
			eY_ = next_y;

			//回転
			if(j == 0){//Left
				skelton_->world_.rotation.y = LeftAngle;
			}
			else if(j == 1){//Right
				skelton_->world_.rotation.y = RightAngle;
			}
			else if(j == 2){//down
				skelton_->world_.rotation.y = DownAngle;
			}
			else if(j == 3){//up
				skelton_->world_.rotation.y = UpAngle;
			}

			if(mapPath_[next_y][next_x] == RootPathPlayerNumber) {
				pathRoot_ = RootPathGoalNumber;

			}
			else if(mapPath_[next_y][next_x] == pathRoot_){
				pathRoot_++;
			}
			break;
		}

		easingEndPos_ = skelton_->world_.translation;
    }
}

void TrackSkeltonState::Update()
{
	//死亡時
	if(!skelton_->isAlive_){
		skelton_->popPosition_ = easigStartPos_;
		stateManager_->SetNextState(new DeadSkeltonState);
	}

	//移動処理
	skelton_->world_.translation = Easing_Point2_Linear(easigStartPos_, easingEndPos_, Time_OneWay(easingMoveTime_, EasingMoveTimeMax));

	//移動完了時
	if(easingMoveTime_ >= EasingFrameMax){
		skelton_->world_.translation = easingEndPos_;
		easigStartPos_ = {};
		easingEndPos_ = {};
		easingMoveTime_ = 0.f;
		
		stateManager_->SetNextState(new IdelSkeltonState);
	}
}

void TrackSkeltonState::ParticleDraw()
{
}




void DeadSkeltonState::UpdateTrigger()
{
	skelton_->particlePos_ = skelton_->GetPosition();
	skelton_->SetPosition(skelton_->NotAlivePos);
	skelton_->world_.UpdateMatrix();
	skelton_->baseCollider_->Update();

	skelton_->isContactTrigger_ = true;

	App();
}

void DeadSkeltonState::Update()
{
	//更新処理
	skelton_->deadParticle_->Update(skelton_->camera_);
	skelton_->particleAliveFrame_++;


	if(skelton_->particleAliveFrame_ < ParticleAliveFrameMax) return;

	skelton_->isPopsPosibble_ = true;	
	skelton_->particleAliveFrame_ = 0;

	skelton_->world_.UpdateMatrix();
	stateManager_->SetNextState(new PopSkeltonState);
}

void DeadSkeltonState::ParticleDraw()
{
	skelton_->deadParticle_->Draw();
}

void DeadSkeltonState::App()
{
	for (int i = 0; i < CreateNum; i++) {
		//速度
		vel.x = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel_Half;
		vel.y = VelY;
		vel.z = (float)rand() / RAND_MAX * Rand_Vel - Rand_Vel_Half;

		//加速度
		acc.y = AccY;

		//生成
		skelton_->deadParticle_->ParticleSet(
			ParticleAliveFrameMax,
			skelton_->particlePos_,
			vel,
			acc,
			SizeStart,
			SizeEnd,
			TextureNumber,
			Color
		);
		skelton_->deadParticle_->ParticleAppearance();
	}
}




void PopSkeltonState::UpdateTrigger()
{

}

void PopSkeltonState::Update()
{
	if(skelton_->isPopsPosibble_) return;
	//更新処理
	App();
	skelton_->popParticle_->Update(skelton_->camera_);
	skelton_->particleAliveFrame_++;


	if(skelton_->particleAliveFrame_ < ParticleCreateFrameMax) return;

	skelton_->isAlive_ = true;
	skelton_->world_.translation = skelton_->particlePos_;
	skelton_->particleAliveFrame_ = 0;

	skelton_->world_.UpdateMatrix();
	stateManager_->SetNextState(new IdelSkeltonState);
}

void PopSkeltonState::ParticleDraw()
{
	if(skelton_->particleAliveFrame_ < ParticleCreateWaitFrameMax) return;
	skelton_->popParticle_->Draw();
}

void PopSkeltonState::App()
{
	for (int i = 0; i < CreateNum; i++) {

		//座標
		pos.x = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos_Half;
		pos.y = PosY;
		pos.z = (float)rand()/RAND_MAX * Rand_Pos - Rand_Pos_Half;
		pos += skelton_->particlePos_;

		//速度
		vel.x = (float)rand()/RAND_MAX*Rand_Vel - Rand_Vel_Half;
		vel.y = VelY;
		vel.z = (float)rand()/RAND_MAX*Rand_Vel - Rand_Vel_Half;

		//加速度
		acc.y = -(float)rand()/RAND_MAX*Rand_Acc;

		//生成
		skelton_->popParticle_->ParticleSet(
			ParticleAliveFrameMax,
			pos,
			vel,
			acc,
			SizeStart,
			SizeEnd,
			TextureNumber,
			Color
		);
		skelton_->popParticle_->ParticleAppearance();
	}
}
