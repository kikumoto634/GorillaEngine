﻿#pragma once
#include<wrl.h>
#include <DirectXMath.h>

#include "DirectXCommon.h"
#include "Vector4.h"
#include "Vector3.h"

#include "Camera.h"

class GPUParticleManager
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMFLOAT4 = DirectX::XMFLOAT4;

	//サブクラス
public:
	class ParticleCommon{
		friend GPUParticleManager;

	private:
		//三角形
		const UINT TriangleCount = 10000;
		const float TriangleHalfWidth = 0.05f;
		const float TriangleDepth = 1.0f;
		const float CullingCutoff = 0.5f;

		const UINT CommandSizePerFrame = TriangleCount*sizeof(IndirectCommand);
		UINT CommandBufferCounterOffset = 0;

		const UINT ComputeThreadBlockSize = 128;

		Window* window_ = nullptr;
		DirectXCommon* dxCommon_ = nullptr;

		UINT FrameCount = 2;

		ComPtr<ID3D12CommandAllocator> computeCommandAllocators;
		ComPtr<ID3D12CommandQueue> computeCommandQueue_;
		ComPtr<ID3D12GraphicsCommandList> computeCommandList_;

		ComPtr<ID3D12PipelineState> pipelineState;
		ComPtr<ID3D12PipelineState> computePipelineState;

		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12RootSignature> computeRootSignature;

	public:
		void Initialize();

		UINT AlignForUavCounter(UINT bufferSize)
		{
			const UINT alignment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
			return (bufferSize + (alignment - 1)) & ~(alignment - 1);
		}
	};

	struct Vertex{
		Vector3 pos;
	};

	struct Const{
		Vector4 velocity;
		Vector4 offset;
		Vector4 color;
		XMMATRIX matBillboard;
		XMMATRIX mat;
		float padding[36];
	};

	struct Compute{
		float offsetX;
        float offsetZ;
        float cullOffset;
        float commandCount;
	};

	//コマンドシグネチャ
	struct IndirectCommand{
		D3D12_GPU_VIRTUAL_ADDRESS cbv;
		D3D12_DRAW_ARGUMENTS drawArguments;
	};

public:
	static void StaticInitialize();
	static void StaticFinalize();
	static void SetPipelineState();
	static GPUParticleManager* Create();

private:
	static ParticleCommon* common;

public:
	GPUParticleManager();
	bool Initialize();
	void Update(WorldTransform world,Camera* camera);
	void Draw();
private:
	float RandomFloat(float min, float max);

private:
	//頂点
	ComPtr<ID3D12Resource> vertBuff_;
	Vertex* vertMap_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	ComPtr<ID3D12Resource> constBuff_;
	//三角形の定数情報
	std::vector<Const> constantBufferData;
	UINT8* cbvDataBegin;

	//深度バッファ
	ComPtr<ID3D12Resource> depthBuff;

	//デスクリプタ
	ComPtr<ID3D12Resource> descHeapRTV_[3];
	ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap_;

	UINT rtvDescriptorSize_;
    UINT cbvSrvUavDescriptorSize_;

	//コマンドシグネチャ
	ComPtr<ID3D12CommandSignature> commandSignature_;
	//コマンドバッファ
	ComPtr<ID3D12Resource> commandBuffer_;
	ComPtr<ID3D12Resource> processedCommandBuffers[3];
	ComPtr<ID3D12Resource> processedCommandBuffersCounterReset;

	ComPtr<ID3D12Resource> vertexBufferUpload;
	ComPtr<ID3D12Resource> commandBufferUpload;

	Compute compute;

    ComPtr<ID3D12Fence> computeFence_;
    UINT64 fenceValues_;
    HANDLE fenceEvent_;
};

