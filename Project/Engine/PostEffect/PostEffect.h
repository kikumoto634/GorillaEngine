#pragma once
#include "Sprite.h"

class PostEffect : public Sprite
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//ぼかし
	struct ConstBufferData_Blur{
		bool isActive;
		int count;
		float strength;
	};

	//フェード
	struct ConstBufferData_Fade{
		bool isActive;
		Vector3 color;
	};

private:
	//ブラー回数
	const int BlurCont = 16;
	const float BlurStrength = 1.f;

	//フェード時間
	const float FadeSecond = 1.f;
	
public:
	static PostEffect* GetInstance();
	static void Delete();

	static PostEffect* Create(UINT texNumber, Vector2 pos, Vector2 size, XMFLOAT4 color = {1,1,1,1},
		Vector2 anchorpoint = {0.f,0.f}, bool isFlipX = false, bool isFlipY = false);

public:
	PostEffect() {};

	bool Initialize();

	void Update();

	void Draw();

	//シーン描画前描画
	void PreDrawScene();
	//シーン描画後処理
	void PostDrawScene();

private:
	//スプライト共通初期化
	void SpriteInitialize();

	//テクスチャ初期化
	void TextureInitialize();
	//SRV初期化
	void SRVInitialize();
	//RTV初期化
	void RTVInitialize();
	//深度バッファ初期化
	void DepthInitialize();
	//DSV初期化
	void DSVInitialize();

	/// <summary>
	/// パイプライン生成
	/// </summary>
	void CreateGraphicsPipelineState();


private:
	//シングルトン
	static PostEffect* instance;

	//画面クリアカラー
	static const float clearColor[4];

	static const int VertNum = 4;

private:
	//テクスチャ
	ComPtr<ID3D12Resource> texbuff;
	//SRVデスクリ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
};

