#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <forward_list>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Camera.h"
#include "WorldTransform.h"

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

	//定数バッファ用データ構造体(3D変換行列
	struct ConstBufferData{
		DirectX::XMMATRIX mat;	//3D変換行列
		DirectX::XMMATRIX matBillboard;	//ビルボード行列
		Vector4 color;
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
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(WorldTransform worldTransform, Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// パーティクル追加
	/// </summary>
	/// <param name="life">生存時間</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="start_scale">開始時スケール</param>
	/// <param name="end_scale">終了時スケール</param>
	void Add(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale, UINT texNumber = 1);

	//Get
	int GetMaxObjectCount()	{return maxObjectCount;}
	//頂点情報
	int Getvertices()	{return _countof(vertices);}
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW GetvbView()	{return vbView;}
	//パーティクル描画数
	UINT GetParticle()	{return (UINT)std::distance(particle.begin(), particle.end());}

	//Setter
	void SetColor(Vector4 color)	{this->color = color;}

private:
	/// <summary>
	/// グラフィックパイプライン初期化
	/// </summary>
	void InitializeGraphicsPipeline();

	/// <summary>
	/// デストラクタヒープ初期化
	/// </summary>
	void InitializeDescriptorHeap();

private:
	//DirectXCommon
	DirectXCommon* dxCommon = nullptr;

	//パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	//デスクリプタヒープ(定数バッファビュー用)
	ComPtr<ID3D12DescriptorHeap> basicDescHeap;

	TextureManager* textureManager;

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuffer;
	//頂点マップ
	VertexPos* vertMap = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//頂点データ
	VertexPos vertices[vertexCount];

	//定数バッファ(行列)
	ComPtr<ID3D12Resource> constBuffer;
	//マッピング用ポインタ
	ConstBufferData* constMap = nullptr;

	//パーティクル配列
	std::forward_list<Particle> particle;

	//テクスチャデータ
	UINT texNumber = 0;

	Vector4 color = {0,1,0,0.5};
};

