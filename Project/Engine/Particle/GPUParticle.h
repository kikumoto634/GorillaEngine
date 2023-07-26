#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Camera.h"

#include <DirectXMath.h>
#include <d3dx12.h>
#include <DirectXCommon.h>
#include <vector>

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

	//コマンドシグネチャ
	struct IndirectCommand{
		D3D12_GPU_VIRTUAL_ADDRESS cbv;
		D3D12_DRAW_ARGUMENTS drawArguments;
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
	void Initialize(Camera* camera);
	void Update();
	void Draw();
	void Finalize();

private:
	void InitializeRootSignature();
	void InitializeDescriptorHeap();
	void InitializePipeline();

	float GetRandomFloat(float min, float max);

private:
	//スワップ枚数
    static const UINT FrameCount = 2;

	//コマンド
	static const UINT CommandSizePerFrame;

	//三角形
	static const UINT TriangleCount = 1024;
    static const UINT TriangleResourceCount = TriangleCount * FrameCount;

	//三角形情報
	static const float TriangleHalfWidth;
	static const float TriangleDepth;    

private:
	DirectXCommon* dxCommon_ = nullptr;

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12RootSignature> computeRootSignature;
	//コマンドシグネチャ
	ComPtr<ID3D12CommandSignature> commandSignature;


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
	ComPtr<ID3D12CommandAllocator> computeCommandAllocators[FrameCount];

	//GPUパーティクル用のコマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<ID3D12CommandQueue> computeCommandQueue;

	//GPUパーティクル用のフェンス
	ComPtr<ID3D12Fence> fence;
    ComPtr<ID3D12Fence> computeFence;

	//GPUパーティクル用のコマンドリスト
	ComPtr<ID3D12GraphicsCommandList> computeCommandList;

	//バックバッファインデックス
	UINT frameIndex;


	//三角形の定数情報
	std::vector<Const> constantBufferData;
	UINT8* cbvDataBegin;

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuffer;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertBufferView{};

	//深度ステンシル
	ComPtr<ID3D12Resource> depthStencil;

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffer;

	//コマンドバッファ
	ComPtr<ID3D12Resource> commandBuffer;
};

