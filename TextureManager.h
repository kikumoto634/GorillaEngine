#pragma once
#include "DirectXCommon.h"
#include "DirectXTex.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <string>

/// <summary>
/// テクスチャ
/// </summary>
class TextureManager
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//テクスチャ枚数(デスクリプタ数)
	static const int maxTextureCount = 512;
	using string = std::string;

	/// <summary>
	/// テクスチャ
	/// </summary>
	struct Texture {
		// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
		// 名前
		std::string name;
	};

public:

	/// <summary>
	/// シングルトン
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectXCommon 共通処理</param>
	void Initialize(DirectXCommon* dxCommon, string directoryPath = "Resources/");

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="number">番号</param>
	/// <param name="filename">ファイル名</param>
	void LoadTexture(uint32_t number, const string filename);

	/// <summary>
	/// 全テクスチャクリア
	/// </summary>
	void ResetAll();

	///Getter
	
	/// <summary>
	/// テクスチャバッファ取得
	/// </summary>
	/// <param name="number">番号</param>
	/// <returns></returns>
	ID3D12Resource* GetTextureBuffer(uint32_t number);

	/// <summary>
	/// デスクリプタヒープ取得
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetDescriptorHeap();


	///Setter

	/// <summary>
	/// デスクリプタヒープセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// SRVをセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="RootParameterIndex">ルートパラメータ番号</param>
	/// <param name="texnumber">テクスチャ番号</param>
	void SetShaderResourceView(ID3D12GraphicsCommandList* commandList, UINT RootParameterIndex, uint32_t texnumber);


private:
	DirectXCommon* dxCommon = nullptr;

	//テクスチャ用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	//テクスチャバッファの配列
	ComPtr<Texture> textureBuffer[maxTextureCount];
	//ディレクトリパス
	string directoryPath;
};

