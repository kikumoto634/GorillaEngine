#pragma once
#include "ParticleManager.h"

/// <summary>
/// パーティクル属性
/// ・生存時間
/// ・座標
/// ・速度
/// ・加速度
/// ・初期サイズ
/// ・終期サイズ
/// ・テクスチャ番号
/// ・色
/// </summary>
struct Particle{
	int life;
	Vector3 position;
	Vector3 velocity; 
	Vector3 accel;
	float start_scale; 
	float end_scale;
	UINT texNumber;
	Vector4 color;
};

class ParticleObject
{
//メンバ関数
public:
	ParticleObject(){};
	ParticleObject(
		int life, 
		Vector3 position, 
		Vector3 velocity, 
		Vector3 accel, 
		float start_scale, 
		float end_scale, 
		UINT texNumber = 1,
		Vector4 color = {1,1,1,1}
	);
	ParticleObject(Particle obj);

	~ParticleObject();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// パーティクル発生
	/// </summary>
	void ParticleAppearance();

	/// <summary>
	/// パーティクル変更
	/// </summary>
	void ParticleSet(Particle obj);

//メンバ変数
public:
	//パーティクル
	ParticleManager* particle = nullptr;
	WorldTransform world = {};

	//各属性
	int life = 0;
	Vector3 position = {};
	Vector3 velocity = {};
	Vector3 accel = {};
	float start_scale = 0.f;
	float end_scale = 0.f;
	UINT texNumber = 0;
};

