#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "MapNode.h"
#include <queue>
#include <vector>

#include "AreaManager.h"

//前方宣言
class SkeltonStateManager;
class Skelton;

class SkeltonState
{
public:
	virtual void SetStateManager(SkeltonStateManager* stateManager)	{stateManager_ = stateManager;}

	void Initialize(Skelton* skelton);
	virtual void UpdateTrigger() = 0;
	virtual void Update() = 0;
	virtual void ParticleDraw() = 0;

protected:
	//借り物
	SkeltonStateManager* stateManager_ = nullptr;
	Skelton* skelton_ = nullptr;
};


//待機
class IdelSkeltonState : public SkeltonState{
private:
	void UpdateTrigger() override;
	void Update() override;
	void ParticleDraw() override;

private:
	//待機カウント最大数
	const int WaltCountMax = 2;

	//待機カウント
	int waitCount_ = 0;
};


//追跡
class TrackSkeltonState : public SkeltonState{
private:
	//エリアブロック数の半分
	const int AreaBlocksHalfNum = AreaManager::DIV_NUM_HALF;
	//エリアブロックサイズ
	const float AreaBlockSize = AreaManager::Block_Size;

	//マップ開始番号
	const int RootPathStartNumber = 9;
	//マップスケルトン番号
	const int RootPathSkeltonNumber = 5;
	//マッププレイヤー番号
	const int RootPathPlayerNumber = 6;
	//マップ プレイヤー到達時番号
	const int RootPathGoalNumber = 10;

	//移動方向数
	static const int PathDirection = 4;
	//角度
	const float LeftAngle  = DirectX::XMConvertToRadians(-90);
	const float RightAngle = DirectX::XMConvertToRadians(+90);
	const float DownAngle  = 0;
	const float UpAngle    = DirectX::XMConvertToRadians(+180);

	//イージング完了フレーム
	const float EasingFrameMax = 1.0f;
	//イージング移動時間
	const float EasingMoveTimeMax = 0.05f;


//エイリアス
private:
	template <class T> using vector = std::vector<T>;

private:
	void UpdateTrigger() override;
	void Update() override;
	void ParticleDraw() override;

private:
	//経路探索
	vector<MapNode*> path_;
	vector<vector<int>> mapPath_;

	int pathRoot_ = 10;

	//移動
	//イージングの移動開始座標
	Vector3 easigStartPos_ = {};
	//イージングの移動終了座標
	Vector3 easingEndPos_ = {};
	//イージングの移動タイム
	float easingMoveTime_ = 0;

	int eX_ = 0;
	int eY_ = 0;
	int pX_ = 0;
	int pY_ = 0;
};


//死亡
class DeadSkeltonState : public SkeltonState{
private:
	//パーティクル生存時間
	const int ParticleAliveFrameMax = 50;

	//パーティクル生成
	const size_t CreateNum = 10;
	//速度
	const float Rand_Vel = 0.08f;
	const float Rand_Vel_Half = 0.04f;
	const float VelY = 0.06f;
	//加速度
	const float AccY = -0.005f;
	//サイズ
	const float SizeStart = 0.4f;
	const float SizeEnd = 0.0f;
	//テクスチャ番号
	const uint32_t TextureNumber = 1;
	//色
	Vector4 Color = {1.0f,0.0f,0.0f,1.0f};

private:
	void UpdateTrigger() override;
	void Update() override;
	void ParticleDraw() override;

	void App();

private:
	//パーティクル生成
	//速度
	Vector3 vel{};
	//加速度
	Vector3 acc{};
};


//再出現
class PopSkeltonState : public SkeltonState{
private:
	//パーティクル生成時間
	const int ParticleCreateFrameMax = 180;
	//パーティクル生成待機時間
	const int ParticleCreateWaitFrameMax = 60;
	//パーティクル生存時間
	const int ParticleAliveFrameMax = 40;

	//パーティクル生成
	const size_t CreateNum = 10;
	//座標
	const float Rand_Pos = 1.5f;
	const float Rand_Pos_Half = 0.75f;
	const float PosY = -0.5f;
	//速度
	const float Rand_Vel = 0.025f;
	const float Rand_Vel_Half = 0.0125f;
	const float VelY = 0.05f;
	//加速度
	const float Rand_Acc = 0.001f;
	//サイズ
	const float SizeStart = 0.4f;
	const float SizeEnd = 0.0f;
	//テクスチャ番号
	const uint32_t TextureNumber = 1;
	//色
	Vector4 Color = {0.6f,0.3f,0.2f,0.4f};

private:
	void UpdateTrigger() override;
	void Update() override;
	void ParticleDraw() override;

	void App();

private:
	//パーティクル生成
	//座標
	Vector3 pos{};
	//速度
	Vector3 vel{};
	//加速度
	Vector3 acc{};
};