#pragma once
#include "Sprite.h"
#include "RenderTexture.h"

#include "Matrix4x4.h"
#include <DirectXMath.h>

#include "LightGroup.h"
#include "Camera.h"

class ShadowMap : public Sprite
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	struct SceneMatrix{
		DirectX::XMMATRIX view;				//ビュー
		DirectX::XMMATRIX proj;				//プロジェクション
		DirectX::XMMATRIX lightCamera;
		DirectX::XMMATRIX shadow;			//影行列
		Vector3 eye;				//視点
	};

public:
	static ShadowMap* GetInstance();
	static void Delete();

	static ShadowMap* Create(UINT texNumber, Vector2 pos, Vector2 size, XMFLOAT4 color = {1,1,1,1},
		Vector2 anchorpoint = {0.f,0.f}, bool isFlipX = false, bool isFlipY = false);

public:
	ShadowMap() {};

	bool Initialize();

	void Update(LightGroup* light, Camera* camera);

	void Draw();

	//シーン描画前描画
	void PreDrawScene();
	//シーン描画後処理
	void PostDrawScene();

private:
	//スプライト共通初期化
	void SpriteInitialize();
	//SRV初期化
	void SRVInitialize();
	//定数
	void CreateConst();

	/// <summary>
	/// パイプライン生成
	/// </summary>
	void CreateGraphicsPipelineState();


private:
	//シングルトン
	static ShadowMap* instance;

	//画面クリアカラー
	static const float clearColor[4];

	static const int VertNum = 4;

private:
	//レンダーテクスチャ
	RenderTexture* renderTexture_ = nullptr;
	//SRVデスクリ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;


	ComPtr<ID3D12Resource> mapped_;
};

