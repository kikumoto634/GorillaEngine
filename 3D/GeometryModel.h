#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

#include "GeometryManager.h"
#include "DirectXCommon.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class GeometryModel
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT4 = DirectX::XMFLOAT4;

public:
	static const int maxObjectCount = 512;

public:
	//定数バッファ用データ構造体(3D変換行列
	struct ConstBufferData{
		DirectX::XMMATRIX mat;	//3D変換行列
		XMFLOAT4 color;	//色(RGBA)
	};

private:
	//共通データ
	class Common
	{
		friend class GeometryModel;
		
	public:
		/// <summary>
		/// グラフィックスパイプライン初期化
		/// </summary>
		void InitializeGraphicsPipeline();

		/// <summary>
		/// デスクリプタヒープ初期化
		/// </summary>
		void InitializeDescriptorHeap();

		/// <summary>
		/// カメラの初期化
		/// </summary>
		//void InitializeCamera();

	private:
		//DirectX12ベース
		DirectXCommon* dxCommon = nullptr;
		//パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate;
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature;
		//デスクリプタヒープ(定数バッファビュー用)
		ComPtr<ID3D12DescriptorHeap> basicDescHeap;
		//次に使うデスクリプタヒープの番号
		int descHeapIndex = 0;
		//モデルデータ
		GeometryManager* model = nullptr;
	};

/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void StaticInitialize(GeometryManager* model);

	/// <summary>
	/// デスクリプタヒープリセット
	/// </summary>
	static void ResetDescriptorHeap();

	/// <summary>
	/// 解放
	/// </summary>
	static void StaticFinalize();

/// <summary>
/// 静的メンバ変数
/// </summary>
private:
	static Common* common;

/// <summary>
/// メンバ関数
/// </summary>
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">グラフィックスコマンド</param>
	void Draw(ID3D12GraphicsCommandList* commandList);



	//setter
	void SetTexNumber(UINT texNumber)	{this->texNumber = texNumber;	}

	void SetColor(const XMFLOAT4& color)	{this->color = color; }

	/*void SetScale(const Vector3& scale)	{this->scale = scale; }

	void SetRotation(const Vector3& rotation)	{this->rotation = rotation; }

	void SetPosition(const Vector3& position)	{this->position = position; }*/


	//getter
	/*const XMFLOAT4& GetColor()	{return this->color; }
	const Vector3& GetScale() {return this->scale; }
	const Vector3& GetRotation()	{return this->rotation; }
	const Vector3& GetPosition()	{return this->position; }

	const XMMATRIX& GetWorld()	{return this->matWorld;}*/

/// <summary>
/// メンバ変数
/// </summary>
private:

	UINT texNumber = 1;

	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuff;
	//マッピング用ポインタ
	ConstBufferData* constBuffer = nullptr;

	XMFLOAT4 color = {1,1,1,1};

	////定数バッファビューのCPUアドレス
	//CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
	////定数バッファビューのGPUアドレス
	//CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;

	//トランスフォーム
	////アフィン変換
	//Vector3 scale = {1.0f, 1.0f, 1.0f};
	//Vector3 rotation = {0.0f, 0.0f, 0.0f};
	//Vector3 position = {0.0f, 0.0f, 0.0f};
	////ワールド変換行列
	//DirectX::XMMATRIX matWorld;
	////親オブジェクトへのポインタ
	//GeometryModel* parent = nullptr;
};

