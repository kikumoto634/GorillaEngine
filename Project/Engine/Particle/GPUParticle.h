#pragma once
#include "Vector3.h"
#include "Vector4.h"

#include <DirectXMath.h>
#include <d3dx12.h>
#include <DirectXCommon.h>

class GPUParticle{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//頂点
	struct Vertex{
		Vector3 pos;
	};

	//定数
	struct Const{
		Vector4 velocity;
		Vector4 offset;
		Vector4 color;
		DirectX::XMMATRIX projection;

		float padding[36];
	};

	//コンピュート
	struct Compute{
		float offSetX;
		float offSetY;
		float offSetCull;
		float commandCount;
	};


	//グラフィックスルートシグネチャ
	enum GraphicsRootParameters{
		Cbv,
		GraphicsRootParamtersCount
	};

	//コンピュートシグネチャ
	enum ComputeRootParameters{
		SrvUavTable,
		RootConstants,
		ComputeRootParametersCount,
	};


	//デスクリプタヒープ(CBV/SRV/UAV)作成のオフセット
	enum HeapOffset{
		CbvSrvOffset = 0,
		CommandsOffset = CbvSrvOffset + 1,
		ProcessedCommandsOffset = CommandsOffset + 1,
		CbvSrvUavDescriptorCountPerFrame = ProcessedCommandsOffset + 1
	};

public:
	void Initialize();
	void Update();
	void Draw();
	void Finalize();

private:
	void InitializeRootSignature();
	void InitializeDescriptorHeap();
	void InitializePipeline();

private:
	//ポリゴン数
    static const UINT FrameCount = 3;

private:
	DirectXCommon* dxCommon_ = nullptr;

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12RootSignature> computeRootSignature;

	//パイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12PipelineState> computePipelineState;

	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap;
	//デスクリプタサイズ
	UINT rtvDescriptorSize;
	UINT cbvSrvUavDescriptorSize;


	//GPUパーティクル用のコマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocators[FrameCount];
	ComPtr<ID3D12CommandAllocator> computeCommandAllocators[FrameCount];

	//GPUパーティクル用のコマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<ID3D12CommandQueue> computeCommandQueue;

	//GPUパーティクル用のフェンス
	ComPtr<ID3D12Fence> fence;
    ComPtr<ID3D12Fence> computeFence;

	//GPUパーティクル用のコマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12GraphicsCommandList> computeCommandList;
};

