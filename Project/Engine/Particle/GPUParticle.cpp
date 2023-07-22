#include "GPUParticle.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

void GPUParticle::Initialize()
{
	HRESULT result = {};

	dxCommon_ = DirectXCommon::GetInstance();

	//コマンドキュー
	{
		//グラフィックス
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		result = dxCommon_->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
		assert(SUCCEEDED(result));

		//コンピュート
		D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
		computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		result = dxCommon_->GetDevice()->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeCommandQueue));
		assert(SUCCEEDED(result));
	}

	//コマンドアロケータ
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	}


	InitializeDescriptorHeap();
	InitializeRootSignature();
	InitializePipeline();
}

void GPUParticle::Update()
{
}

void GPUParticle::Draw()
{
}

void GPUParticle::Finalize()
{
}

void GPUParticle::InitializeRootSignature()
{
	HRESULT result = {};
	//ルートシグネチャの最新バージョン
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	result = dxCommon_->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,&featureData,sizeof(featureData));
	if(FAILED(result)){
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}


	//シグネチャ
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	//グラフィックス
	CD3DX12_ROOT_PARAMETER1 rootParameters[GraphicsRootParamtersCount];
	rootParameters[Cbv].InitAsConstantBufferView(0,0,D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters),rootParameters,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,featureData.HighestVersion,&signature,&error);
	assert(SUCCEEDED(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//コンピュート
	CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	CD3DX12_ROOT_PARAMETER1 computeRootParameters[ComputeRootParametersCount];
	computeRootParameters[SrvUavTable].InitAsDescriptorTable(2, ranges);
	computeRootParameters[RootConstants].InitAsConstants(4,0);
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSignatureDesc;
	computeRootSignatureDesc.Init_1_1(_countof(computeRootParameters), computeRootParameters);

	result = D3DX12SerializeVersionedRootSignature(&computeRootSignatureDesc, featureData.HighestVersion, &signature, &error);
	assert(SUBLANGID(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&computePipelineState));
	assert(SUCCEEDED(result));
}

void GPUParticle::InitializeDescriptorHeap()
{
	//RTV
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dxCommon_->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc,IID_PPV_ARGS(&rtvHeap));

	//DSV
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dxCommon_->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc,IID_PPV_ARGS(&dsvHeap));

	//CBV,SRV,UAV
	D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
	cbvSrvUavHeapDesc.NumDescriptors = CbvSrvUavDescriptorCountPerFrame * FrameCount;
	cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dxCommon_->GetDevice()->CreateDescriptorHeap(&cbvSrvUavHeapDesc,IID_PPV_ARGS(&cbvSrvUavHeap));

	//デスクリプタサイズ
	rtvDescriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	cbvSrvUavDescriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GPUParticle::InitializePipeline()
{
	HRESULT result;

	//パイプライン
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> csBlob;
	ComPtr<ID3DBlob> errorBlob;

	//読み込み
	{
		result = D3DCompileFromFile(
			L"Resources/shader/GPUParticleVS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "vs_5_0",
			D3DCOMPILE_DEBUG| D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vsBlob, &errorBlob
		);

		result = D3DCompileFromFile(
			L"Resources/shader/GPUParticlePS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "ps_5_0",
			D3DCOMPILE_DEBUG| D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vsBlob, &errorBlob
		);

		result = D3DCompileFromFile(
			L"Resources/shader/GPUParticleCS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "cs_5_0",
			D3DCOMPILE_DEBUG| D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vsBlob, &errorBlob
		);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	};

	//グラフィックスパイプライン(頂点、ピクセル)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	pipelineDesc.InputLayout = {inputLayout, _countof(inputLayout)};
	pipelineDesc.pRootSignature = rootSignature.Get();
	pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.SampleMask = UINT_MAX;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineDesc.SampleDesc.Count = 1;
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));


	//コンピュートパイプライン
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineDesc = {};
	computePipelineDesc.pRootSignature = computeRootSignature.Get();
	computePipelineDesc.CS = CD3DX12_SHADER_BYTECODE(csBlob.Get());
	result = dxCommon_->GetDevice()->CreateComputePipelineState(&computePipelineDesc, IID_PPV_ARGS(&computePipelineState));
	assert(SUCCEEDED(result));
}
