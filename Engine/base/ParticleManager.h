#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <forward_list>

#include "../Engine/math/Vector/Vector2.h"
#include "../Engine/math/Vector/Vector3.h"

class ParticleManager
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//定数
	//テクスチャの最大枚数
	static const int maxObjectCount = 512;
	//頂点数
	static const int vertexCount = 1024;
public:
	struct VertexPos{
		Vector3 pos;
		float scale;
	};

	//パーティクル粒
	struct Particle
	{
		//座標
		Vector3 position = {};
		//速度
		Vector3 velocity = {};
		//加速度
		Vector3 accel = {};

		//スケール
		float scale = 1.f;
		//初期値
		float start_scale = 1.f;
		//最終値
		float end_scale = 0.f;

		//現在のフレーム
		int frame = 0;
		//終了フレーム
		int num_frame = 0;
	};

public:
	static ParticleManager* GetInstance();

public:
	/// <summary>
	/// バッファ生成
	/// </summary>
	void CreateBuffer();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// パーティクル追加
	/// </summary>
	/// <param name="life">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="start_scale">開始時スケール</param>
	/// <param name="end_scale">終了時スケール</param>
	void Add(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale);

	//Get
	int GetMaxObjectCount()	{return maxObjectCount;}
	//頂点情報
	int Getvertices()	{return _countof(vertices);}
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW GetvbView()	{return vbView;}
	//パーティクル描画数
	UINT GetParticle()	{return (UINT)std::distance(particle.begin(), particle.end());}

private:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuffer;
	//頂点マップ
	VertexPos* vertMap = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//頂点データ
	VertexPos vertices[vertexCount];

	//パーティクル配列
	std::forward_list<Particle> particle;
};

