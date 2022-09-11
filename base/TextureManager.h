#pragma once

#include "DirectXCommon.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include "DirectXTex.h"

using namespace DirectX;

//テクスチャマネージャー
class TextureManager
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//定数
	//テクスチャの最大枚数
	static const int maxTextureCount = 512;

	//ディレクトリパス
	const std::string directoryPath = "Resources/";

public:
	/// <summary>
	/// テクスチャ
	/// </summary>
	struct Texture {
		// テクスチャリソース
		ComPtr<ID3D12Resource> resource;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	};

public://静的

	static void Load(UINT texnumber, const std::string& filename);

	static TextureManager* GetInstance();

public:	//メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectXCommon->device.Get()</param>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// クリア
	/// </summary>
	void ResetAll();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	/// <param name="filename">テクスチャファイル名</param>
	void LoadTexture(UINT texnumber, const std::string& filename);

	/// <summary>
	/// テクスチャバッファ取得
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	/// <returns></returns>
	ID3D12Resource* GetSpriteTexBuffer(UINT texnumber);

	/// <summary>
	/// デスクリプタヒープをセット
	/// </summary>
	/// <param name="commandList">グラフィックスコマンド</param>
	void SetDescriptorHeaps (ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// SRVをセット
	/// </summary>
	/// <param name="commandList">グラフィックスコマンド</param>
	/// <param name="RootParameterIndex">ルートパラメータ番号</param>
	/// <param name="texnumber">テクスチャ番号</param>
	void SetShaderResourceView(ID3D12GraphicsCommandList* commandList, UINT RootParameterIndex, UINT texnumber);

	/// <summary>
	/// デスクリプタヒープ取得
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetDescHeap() {return descHeap.Get(); }

private:	//メンバ変数
	//テクスチャ用デスクリプタヒープの生成
	ComPtr<ID3D12DescriptorHeap> descHeap;
	//テクスチャ情報配列
	Texture textures[maxTextureCount];
	//デスクリプタサイズ
	UINT descriptorhandleIncrementSize = 0u;
	//DirectXCommon
	DirectXCommon* dxCommon = nullptr;
};

