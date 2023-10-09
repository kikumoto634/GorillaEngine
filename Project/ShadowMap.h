#pragma once
#include <DirectXMath.h>

#include "Sprite.h"
#include "Camera.h"

class ShadowMap : public Sprite
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;
public:
	struct ConstShadowMap{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	static ShadowMap* Create(UINT texNumber, Vector2 pos, Vector2 size, XMFLOAT4 color = {1,1,1,1},
		Vector2 anchorpoint = {0.f,0.f}, bool isFlipX = false, bool isFlipY = false);

public:
	ShadowMap(UINT texnumber, Vector3 pos, Vector2 size, XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY);
	
	bool Initialize();
	void Update();
	void Draw();
	void Finalize();

	//影用モデル描画前処理
	void PreShadowDraw();
	//影用モデル描画後処理
	void PostShadowDraw();

private:
	void SpriteInitialize();
	void TextureInitialize();
	void SRVInitialize();
	void RTVInitialize();
	void DepthInitialize();
	void DSVInitialize();

	void CreateGraphicsPipelineState();

private:
	//画面クリアカラー
	static const float clearColor[4];
	static const int VertNum = 4;

private:
	ComPtr<ID3D12Resource> texBuff;
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	ComPtr<ID3D12Resource> depthBuff;
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//定数
	ComPtr<ID3D12Resource> constBuff;

	//ライトカメラ
	Camera* lightCamera = nullptr;
};

