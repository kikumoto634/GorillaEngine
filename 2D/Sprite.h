#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include<wrl.h>
#include <string>

#include "Window.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Vector2.h"
#include "Vector3.h"

/// <summary>
/// スプライト
/// </summary>
class Sprite
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	class Common{
		friend class Sprite;

	private:
		DirectXCommon* dxCommon = nullptr;
		//パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate;
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature;
		//射影行列
		DirectX::XMMATRIX matProjection{};
		//テクスチャマネージャー
		TextureManager* textureManager = nullptr;

	public:
		void InitializeGraphicsPipeline();
	};

	//頂点データ
	struct VertexPosUv{
		Vector3 pos;
		Vector2 uv;
	};

	//定数バッファ用
	struct ConstBufferData{
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};

public:

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	/// <param name="directoryPath">シェーダーパス</param>
	static void StaticInitialize(ID3D12Device* device, 
		int window_width, int window_height,
		const std::wstring& directoryPath = L"Resources/");

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="textureNumber">番号</param>
	/// <param name="pos">座標</param>
	/// <param name="color">色</param>
	/// <param name="anchorpoint">アンカーポイント</param>
	/// <param name="isFlipX">左右反転</param>
	/// <param name="isFlipY">上下反転</param>
	/// <returns>スプライト</returns>
	static Sprite* Create(uint32_t textureNumber, Vector2 pos, DirectX::XMFLOAT4 color = {1,1,1,1},
		Vector2 anchorpoint = {0.f,0.f}, bool isFlipX = false, bool isFlipY = false);

private:

	//頂点数
	static const int VertNum = 4;

	//サブクラス(共通処理)
	static Common* common;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Sprite();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="textureNumber">番号</param>
	/// <param name="pos">座標</param>
	/// <param name="size">サイズ</param>
	/// <param name="color">色</param>
	/// <param name="anchorpoint">アンカーポイント</param>
	/// <param name="isFlipX">左右反転</param>
	/// <param name="isFlipY">上下反転</param>
	Sprite(uint32_t textureNumber, Vector2 pos, Vector2 size, DirectX::XMFLOAT4 color,
		Vector2 anchorpoint, bool isFlipX, bool isFlipY);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>bool</returns>
	bool Initialize();

	/// <summary>
	/// テクスチャ番号設定
	/// </summary>
	/// <param name="textureNumber">テクスチャ番号</param>
	void SetTextureHandle(uint32_t textureNumber);

	uint32_t GetTextureHandle()	{return textureHandle;}

private:
	ComPtr<ID3D12Resource> vertBuffer;
	ComPtr<ID3D12Resource> constBuffer;

	VertexPosUv* vertexMap = nullptr;
	ConstBufferData* constMap = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView{};

	uint32_t textureHandle;
	float rotation = 0;
	Vector2 position{};
	Vector2 size = {100.f,100.f};
	Vector2 anchorPoint = {0.f,0.f};
	DirectX::XMMATRIX matWorld{};
	DirectX::XMFLOAT4 color = {1,1,1,1};
	bool isFlipX = false;
	bool isFlipY = false;
	Vector2 texBase = {0.f,0.f};
	Vector2 texSize = {100.f,100.f};
	D3D12_RESOURCE_DESC resourceDesc;

private:
	void TransferVertices();
};

