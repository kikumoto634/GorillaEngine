#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <stdint.h>


//前方宣言
class BlueSlimeStateManager;
class BlueSlime;

class BlueSlimeState
{
public:
	virtual void SetStateManager(BlueSlimeStateManager* stateManager)	{stateManager_ = stateManager;}

	void Initialize(BlueSlime* blueSlime);
	virtual void UpdateTrigger() = 0;
	virtual void Update() = 0;
	virtual void ParticleDraw() = 0;

protected:
	//借り物
	BlueSlimeStateManager* stateManager_ = nullptr;
	BlueSlime* blueSlime_ = nullptr;
};


//待機
class IdelBlueSlimeState : public BlueSlimeState{
private:
	//待機カウント最大数
	const int WaltCountMax = 1;

private:
	void UpdateTrigger() override;
	void Update() override;
	void ParticleDraw() override;

private:
	//待機カウント
	int waitCount_ = 0;
};


//死亡
class DeadBlueSlimeState : public BlueSlimeState{
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
class PopBlueSlimeState : public BlueSlimeState{
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