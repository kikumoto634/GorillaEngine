#pragma once
#include "ParticleManager.h"

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
	void ParticleSet(int life, 
		Vector3 position, 
		Vector3 velocity, 
		Vector3 accel, 
		float start_scale, 
		float end_scale, 
		UINT texNumber,
		Vector4 color
	);

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

