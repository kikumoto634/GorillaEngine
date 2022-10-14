#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "DirectXCommon.h"
#include "GeometryObjectManager.h"
#include "TextureManager.h"
#include "Camera.h"
#include "WorldTransform.h"

/// <summary>
/// 幾何学オブジェクト
/// </summary>
class GeometryObject
{
/// <summary>
/// エイリアス
/// </summary>
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// インナークラス
/// </summary>
public:
	class CommonGeometry{
		friend class GeometryObject;

	public:
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

		GeometryObjectManager* geometryObjectManager;
		TextureManager* textureManager;

	};

	//定数バッファ用データ構造体(3D変換行列
	struct ConstBufferData{
		DirectX::XMMATRIX mat;	//3D変換行列
		DirectX::XMFLOAT4 color;	//色(RGBA)
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void StaticInitialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 静的メンバ解法
	/// </summary>
	static void StaticFinalize();

	static GeometryObject* Create(UINT texNumber, DirectX::XMFLOAT4 color = {1,1,1,1});

private:
	static CommonGeometry* common;

public:
	GeometryObject();
	GeometryObject(UINT texNumber, DirectX::XMFLOAT4 color = {1,1,1,1});

	/// <summary>
	/// 初期化
	/// </summary>
	bool Initialize(UINT texNumber);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(WorldTransform worldTransform, Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	//テクスチャデータ
	UINT texNumber = 0;

	//定数バッファ(行列)
	ComPtr<ID3D12Resource> constBuffer;
	//マッピング用ポインタ
	ConstBufferData* constMap = nullptr;

	XMFLOAT4 color = {1,1,1,1};
};

