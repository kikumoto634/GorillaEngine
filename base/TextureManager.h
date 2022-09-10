#pragma once
#include "DirectXCommon.h"
#include "DirectXTex.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <string>
#include <array>


/// <summary>
/// テクスチャマネージャー
/// </summary>
class TextureManager
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using string = std::string;
public:
	//テクスチャ枚数(デスクリプタ数)
	static const size_t maxTextureCount = 512;

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
		// 名前
		string name;
	};

public:

	/// <summary>
	/// シングルトン
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	static void Load(uint32_t number, const string& fileName);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectXCommon 共通処理</param>
	void Initialize(DirectXCommon* dxCommon, string directoryPath = "Resources/");

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
	/// リソース情報取得
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>リソース情報</returns>
	const D3D12_RESOURCE_DESC GetResoureDesc(uint32_t number);

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
	std::array<Texture, maxTextureCount> textures;
	//デスクリプタサイズ
	UINT descriptorHandleIncrementSize = 0u;
	//次のデスクリプタヒープ番号
	uint32_t indexNextDescriptorHeap = 0u;

	//ディレクトリパス
	string directoryPath;

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="number">番号</param>
	/// <param name="filename">ファイル名</param>
	void LoadTexture(uint32_t number, const string filename);
};

