#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "../Engine/base/TextureManager.h"
#include "../Engine/math/Vector/Vector2.h"
#include "../Engine/math/Vector/Vector3.h"

/// <summary>
/// スプライト
/// </summary>
class Sprite
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT4 = DirectX::XMFLOAT4;

public://サブクラス
	class Common{
		friend class Sprite;
		friend class PostEffect;

	private:
		//頂点数
		const int vertNum = 4;
		//DirectXCommon
		DirectXCommon* dxCommon = nullptr;
		//パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate;
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature;
		//射影行列
		XMMATRIX matProjection{};


		//テクスチャマネージャー
		//TextureManager* textureManager = nullptr;

	public:
		void InitializeGraphicsPipeline(const std::string& directoryPath);
	};

	//スプライトデータ構造
	struct VertexPosUv
	{
		Vector3 pos;
		Vector2 uv;
	};

	//定数バッファ用データ構造体(3D変換行列
	struct ConstBufferData{
		XMMATRIX mat;	//3D変換行列
		XMFLOAT4 color;	//色(RGBA)
	};

public://静的メンバ関数
	/// <summary>
	/// 静的メンバの初期化
	/// </summary>
	static void StaticInitialize(DirectXCommon* dxCommon,
		int window_width, int window_height, const std::string& directoryPath = "Resources/shader");

	/// <summary>
	/// 静的メンバの解放
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// グラフィックスパイプラインのセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	static void SetPipelineState();


	static Sprite* Create(UINT texNumber, Vector2 pos, XMFLOAT4 color = {1,1,1,1},
		Vector2 anchorpoint = {0.f,0.f}, bool isFlipX = false, bool isFlipY = false);

protected://静的メンバ変数
	static Common* common;


public://メンバ関数

	Sprite();
	Sprite(UINT texnumber, Vector3 pos, Vector2 size, XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY);

	/// <summary>
	/// 初期化
	/// </summary>
	bool Initialize(UINT texNumber);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//スプライト単体頂点バッファの転送
	void SpriteTransferVertexBuffer();


	/// <summary>
	/// 座標取得
	/// </summary>
	Vector3 GetPosition()	{return position;}

	/// <summary>
	/// 座標設定
	/// </summary>
	void SetPosition(Vector2 pos);

	/// <summary>
	/// サイズ取得
	/// </summary>
	Vector2 GetSize()	{return size;}

	/// <summary>
	/// サイズ設定
	/// </summary>
	void SetSize(Vector2 size);

	/// <summary>
	/// アンカーポイント設定
	/// </summary>
	void SetAnchorpoint(Vector2 pos);

	/// <summary>
	/// 切り抜きサイズ設定
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

	/// <summary>
	/// 上下反転設定
	/// </summary>
	void SetIsFlipY(bool IsFlipY);

	/// <summary>
	/// 色設定
	/// </summary>
	void SetColor(XMFLOAT4 color);

protected://メンバ変数
	///頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffData;
	//頂点バッファマップ
	VertexPosUv* vertMap = nullptr;
	//定数バッファマップ
	ConstBufferData* constMap = nullptr;
	///頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//テクスチャ番号
	UINT texNumber = 0;

	//座標
	Vector3 position = {0, 0, 0};
	//Z軸周りの回転角
	float rotation = 0.f;
	//大きさ
	Vector2 size = {100, 100};

	//アンカーポイント
	Vector2 anchorpoint = {0.0f, 0.0f};
	//左右反転
	bool IsFlipX = false;
	//上下反転
	bool IsFlipY = false;
	//テクスチャ左上座標
	Vector2 texLeftTop = {0,0};
	//テクスチャ切り出しサイズ
	Vector2 texSize = {100, 100};
	//非表示
	bool IsInvisible = false;

	//ワールド行列
	XMMATRIX matWorld;

	//色
	XMFLOAT4 color = {1, 1, 1, 1};

	//リソース情報
	D3D12_RESOURCE_DESC resourceDesc;
};

