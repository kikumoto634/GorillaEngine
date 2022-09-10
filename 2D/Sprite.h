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
	public:
		void InitializeGraphicsPipeline(const std::wstring &directoryPath);
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
	static void StaticInitialize(DirectXCommon* dxCommon,
		int window_width, int window_height,
		const std::wstring& directoryPath = L"Resources/");

	/// <summary>
	/// 静的解法
	/// </summary>
	static void StaticFinalize();

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

	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// コマンドリスト
	static ID3D12GraphicsCommandList* commandList;

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
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 座標設定
	/// </summary>
	void SetPosition(Vector2 pos);
	Vector2 GetPosition()	{return position;}

	/// <summary>
	/// 角度の設定
	/// </summary>
	/// <param name="rotation">角度</param>
	void SetRotation(float rotation);
	float GetRotation() { return rotation; }

	/// <summary>
	/// サイズ設定
	/// </summary>
	void SetSize(Vector2 size);
	Vector2 GetSize()	{return size;}

	/// <summary>
	/// アンカーポイント設定
	/// </summary>
	void SetAnchorpoint(Vector2 pos);
	Vector2 GetAnchorpoint()	{return anchorPoint;}

	/// <summary>
	/// テクスチャトリミング
	/// </summary>
	/// <param name="tex_x">左上X</param>
	/// <param name="tex_y">左上Y</param>
	/// <param name="tex_width">幅</param>
	/// <param name="tex_height">高さ</param>
	void SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height);

	/// <summary>
	/// 左右反転設定
	/// </summary>
	void SetIsFlipX(bool IsFlipX);
	bool GetIsFlipX()	{return isFlipX;}

	/// <summary>
	/// 上下反転設定
	/// </summary>
	void SetIsFlipY(bool IsFlipY);
	bool GetIsFlipY()	{return isFlipY;}

	/// <summary>
	/// 色設定
	/// </summary>
	void SetColor(DirectX::XMFLOAT4 color);
	DirectX::XMFLOAT4 GetColor()	{return color;}

private:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuffer;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffer;
	//頂点バッファマップ
	VertexPosUv* vertexMap = nullptr;
	//定数バッファマップ
	ConstBufferData* constMap = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	
	//テクスチャハンドル
	uint32_t textureHandle;
	//回転角
	float rotation = 0;
	//座標
	Vector2 position{};
	//サイズ
	Vector2 size = {100.f,100.f};
	//アンカーポイント
	Vector2 anchorPoint = {0.f,0.f};
	//ワールド座標
	DirectX::XMMATRIX matWorld{};
	//色
	DirectX::XMFLOAT4 color = {1,1,1,1};
	//左右反転
	bool isFlipX = false;
	//上下反転
	bool isFlipY = false;
	//切り抜き左上座標
	Vector2 texBase = {0.f,0.f};
	//切り抜きサイズ
	Vector2 texSize = {100.f,100.f};
	//非表示
	bool IsInvisible = false;
	//リソース設定
	//D3D12_RESOURCE_DESC resourceDesc;

private:
	//頂点情報転送
	void TransferVertices();
};

