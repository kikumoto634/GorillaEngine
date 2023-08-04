#include "GPUParticle.h"
#include "Window.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

const UINT GPUParticle::CommandSizePerFrame = TriangleCount * sizeof(IndirectCommand);
const UINT GPUParticle::CommandBufferCounterOffset = AlignForUavCounter(GPUParticle::CommandSizePerFrame);

const float GPUParticle::TriangleHalfWidth = 0.05f;
const float GPUParticle::TriangleDepth = 1.0f;

void GPUParticle::Initialize(Camera* camera)
{
	HRESULT result = {};

	dxCommon_ = DirectXCommon::GetInstance();


	csRootConstants.offSetX = TriangleHalfWidth;
	csRootConstants.offSetY = TriangleDepth;
	csRootConstants.offSetCull = 0.5f;
	csRootConstants.commandCount = TriangleCount;


	//コマンドキュー
	{
		//グラフィックス
		/*D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		result = dxCommon_->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
		assert(SUCCEEDED(result));*/

		//コンピュート
		D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
		computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		result = dxCommon_->GetDevice()->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeCommandQueue));
		assert(SUCCEEDED(result));
	}

	//コマンドアロケータ
	{
		for(UINT i = 0; i < FrameCount; ++i){
			result = dxCommon_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&computeCommandAllocators[i]));
		}
	}

	//コマンドキュー
	D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	result = dxCommon_->GetDevice()->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeCommandQueue));
	assert(SUCCEEDED(result));


	//バックバッファ
	frameIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();

	InitializeDescriptorHeap();
	InitializeRootSignature();
	InitializePipeline();

	//コマンドリスト
	result = dxCommon_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, computeCommandAllocators[frameIndex].Get(), computePipelineState.Get(), IID_PPV_ARGS(&computeCommandList));
	assert(SUCCEEDED(result));
	result = computeCommandList->Close();
	assert(SUCCEEDED(result));


	ComPtr<ID3D12Resource> vertexBufferUpload;
    ComPtr<ID3D12Resource> commandBufferUpload;
	//頂点バッファ
	{
		//三角形頂点情報
		Vertex triangleVertices[] = {
			{ {0.0f, TriangleHalfWidth,TriangleDepth,} },
			{ {TriangleHalfWidth, -TriangleHalfWidth, TriangleDepth} },
			{ {-TriangleHalfWidth,-TriangleHalfWidth, TriangleDepth} }
		};

		//頂点バッファサイズ
		const UINT vertexBufferSize = sizeof(triangleVertices);

		//生成
		result = dxCommon_->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&vertBuffer));
		assert(SUCCEEDED(result));
		result = dxCommon_->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBufferUpload));
		assert(SUCCEEDED(result));

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<UINT8*>(triangleVertices);
		vertexData.RowPitch = vertexBufferSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources<1>(dxCommon_->GetCommandList(), vertBuffer.Get(), vertexBufferUpload.Get(),0,0,1,&vertexData);
		dxCommon_->GetCommandList()->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(vertBuffer.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		vertBufferView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
		vertBufferView.StrideInBytes = sizeof(Vertex);
		vertBufferView.SizeInBytes = sizeof(triangleVertices);
	}

	//深度ステンシル
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		result = dxCommon_->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Window::GetWindowWidth(), Window::GetWindowHeight(),
				1,0,1,0,D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&depthStencil)
		);
		assert(SUCCEEDED(result));

		dxCommon_->GetDevice()->CreateDepthStencilView(depthStencil.Get(),&depthStencilDesc,dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	//定数バッファ
	{
		UINT constantBufferDataSize = TriangleResourceCount * sizeof(Const);

		result = dxCommon_->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constantBufferDataSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffer)
		);
		assert(SUCCEEDED(result));

		constantBufferData.resize(TriangleCount);
		for(UINT i = 0; i < TriangleCount; i++){
			constantBufferData[i].velocity = Vector4(GetRandomFloat(0.01f,0.02f),0,0,0);
			constantBufferData[i].offset = Vector4(GetRandomFloat(-5.f,5.f),GetRandomFloat(-1.f,1.f),GetRandomFloat(0.f,2.f),0);
			constantBufferData[i].color = Vector4(GetRandomFloat(0.5f,1.f),GetRandomFloat(0.5f,1.f),GetRandomFloat(0.5f,1.f),1.0f);
			constantBufferData[i].projection = camera->GetViewProjectionMatrix();
		}

		CD3DX12_RANGE readRange(0,0);
		constBuffer->Map(0,&readRange, reinterpret_cast<void**>(&cbvDataBegin));
		memcpy(cbvDataBegin, &constantBufferData[0],TriangleCount*sizeof(Const));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = TriangleCount;
        srvDesc.Buffer.StructureByteStride = sizeof(Const);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvOffset, cbvSrvUavDescriptorSize);
		for(UINT frame = 0; frame < FrameCount; frame++){
			srvDesc.Buffer.FirstElement = frame*TriangleCount;
			dxCommon_->GetDevice()->CreateShaderResourceView(constBuffer.Get(), &srvDesc, cbvSrvHandle);
			cbvSrvHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
		}
	}

	//コマンドシグネチャ
	{
		D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2] = {};
		argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
		argumentDescs[0].ConstantBufferView.RootParameterIndex = Cbv;
		argumentDescs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

		D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
		commandSignatureDesc.pArgumentDescs = argumentDescs;
		commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
		commandSignatureDesc.ByteStride = sizeof(IndirectCommand);

		result = dxCommon_->GetDevice()->CreateCommandSignature(&commandSignatureDesc, rootSignature.Get(), IID_PPV_ARGS(&commandSignature));
		assert(SUCCEEDED(result));
	}

	//コマンドバッファ
	{
		std::vector<IndirectCommand> commands;
		commands.resize(TriangleResourceCount);
		const UINT commandBufferSize = CommandSizePerFrame * FrameCount;

		D3D12_RESOURCE_DESC commandBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(commandBufferSize);
		result = dxCommon_->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&commandBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&commandBuffer)
		);
		assert(SUCCEEDED(result));

		result = dxCommon_->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(commandBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&commandBufferUpload)
		);
		assert(SUCCEEDED(result));

		//GPUアドレス
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddres = constBuffer->GetGPUVirtualAddress();
		UINT commandIndex = 0;
		for(UINT frame = 0; frame < FrameCount; frame++){
			for(UINT i = 0; i < TriangleCount; i++){
				commands[commandIndex].cbv = gpuAddres;
				commands[commandIndex].drawArguments.VertexCountPerInstance = 3;
				commands[commandIndex].drawArguments.InstanceCount = 1;
				commands[commandIndex].drawArguments.StartVertexLocation = 0;
				commands[commandIndex].drawArguments.StartInstanceLocation = 0;

				commandIndex++;
				gpuAddres += sizeof(Const);
			}
		}

		//コピー
		D3D12_SUBRESOURCE_DATA commandData = {};
		commandData.pData = reinterpret_cast<UINT8*>(&commands[0]);
		commandData.RowPitch = commandBufferSize;
		commandData.SlicePitch = commandData.RowPitch;

		UpdateSubresources<1>(dxCommon_->GetCommandList(),commandBuffer.Get(), commandBufferUpload.Get(), 0, 0, 1, &commandData);
		dxCommon_->GetCommandList()->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(commandBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
	
		//SRV生成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = TriangleCount;
		srvDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		CD3DX12_CPU_DESCRIPTOR_HANDLE commandsHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(),CommandsOffset,cbvSrvUavDescriptorSize);
		for(UINT frame =0; frame < FrameCount; frame++){
			srvDesc.Buffer.FirstElement = frame*TriangleCount;
			dxCommon_->GetDevice()->CreateShaderResourceView(commandBuffer.Get(), &srvDesc, commandsHandle);
			commandsHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE processedCommandsHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(),ProcessedCommandsOffset, cbvSrvUavDescriptorSize);
		for(UINT frame = 0; frame <FrameCount; frame++){
			commandBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(CommandBufferCounterOffset+sizeof(UINT), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
			result = dxCommon_->GetDevice()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&commandBufferDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&processedCommandBuffers[frame])
			);
			assert(SUCCEEDED(result));

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = TriangleCount;
			uavDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
			uavDesc.Buffer.CounterOffsetInBytes = CommandBufferCounterOffset;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

			dxCommon_->GetDevice()->CreateUnorderedAccessView(
				processedCommandBuffers[frame].Get(),
				processedCommandBuffers[frame].Get(),
				&uavDesc,
				processedCommandsHandle
			);
			processedCommandsHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
		}

		//オールケートバッファ
		result = dxCommon_->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&processedCommandBufferCounterReset)
		);
		assert(SUCCEEDED(result));

		UINT8* mappedCounterReset = nullptr;
		CD3DX12_RANGE readRange(0,0);
		result = processedCommandBufferCounterReset->Map(0,&readRange,reinterpret_cast<void**>(&mappedCounterReset));
		assert(SUCCEEDED(result));
		processedCommandBufferCounterReset->Unmap(0,nullptr);
	}

	//GPU 同期オブジェクト フェンス
	{
		result = dxCommon_->GetDevice()->CreateFence(fenceValues[frameIndex],D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&computeFence));
		assert(SUCCEEDED(result));
		fenceValues[frameIndex]++;
	}
}

void GPUParticle::Update()
{
	for(UINT i = 0; i < TriangleCount; i++){
		const float offsetBounds = 2.5f;

		constantBufferData[i].offset.x += constantBufferData[i].velocity.x;
		if(constantBufferData[i].offset.x > offsetBounds){
			constantBufferData[i].velocity.x = GetRandomFloat(0.01f,0.02f);
			constantBufferData[i].offset.x = -offsetBounds;
		}
	}

	UINT8* destination = cbvDataBegin + (TriangleCount * frameIndex * sizeof(Const));
	memcpy(destination, &constantBufferData[0], TriangleCount*sizeof(Const));
}

void GPUParticle::Draw()
{
	HRESULT result = {};

	frameIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();

	//try{
		//コマンドリスト
		result = computeCommandAllocators[frameIndex]->Reset();
		assert(SUCCEEDED(result));
		result = computeCommandList->Reset(computeCommandAllocators[frameIndex].Get(), computePipelineState.Get());
		assert(SUCCEEDED(result));
		
		UINT frameDescriptorOffset = frameIndex*CbvSrvUavDescriptorCountPerFrame;
		D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvUavHandle = cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();
		
		computeCommandList->SetComputeRootSignature(computeRootSignature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = {cbvSrvUavHeap.Get()};
		computeCommandList->SetDescriptorHeaps(_countof(ppHeaps),ppHeaps);

		computeCommandList->SetComputeRootDescriptorTable(
			SrvUavTable,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvUavHandle, CbvSrvOffset+frameDescriptorOffset, cbvSrvUavDescriptorSize)
		);

		computeCommandList->SetComputeRoot32BitConstants(RootConstants,4,reinterpret_cast<void*>(&csRootConstants),0);
	
		computeCommandList->CopyBufferRegion(processedCommandBuffers[frameIndex].Get(), CommandBufferCounterOffset, processedCommandBufferCounterReset.Get(), 0, sizeof(UINT));

		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(processedCommandBuffers[frameIndex].Get(),D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		computeCommandList->ResourceBarrier(1,&barrier);

		computeCommandList->Dispatch(static_cast<UINT>(ceil(TriangleCount/float(ComputeThreadBlockSize))),1,1);

		result = computeCommandList->Close();
		assert(SUCCEEDED(result));

		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		dxCommon_->GetCommandList()->SetPipelineState(pipelineState.Get());

		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertBufferView);

		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffer->GetGPUVirtualAddress());

		dxCommon_->GetCommandList()->IASetVertexBuffers(0,1,&vertBufferView);

		dxCommon_->GetCommandList()->ExecuteIndirect(
                commandSignature.Get(),
                TriangleCount,
                processedCommandBuffers[frameIndex].Get(),
                0,
                processedCommandBuffers[frameIndex].Get(),
                CommandBufferCounterOffset);
		// Draw all of the triangles.
        /*dxCommon_->GetCommandList()->ExecuteIndirect(
            commandSignature.Get(),
            TriangleCount,
            commandBuffer.Get(),
            CommandSizePerFrame * frameIndex,
            nullptr,
            0);*/


		ID3D12CommandList* ppCommandLists[] = {computeCommandList.Get()};
		computeCommandQueue->ExecuteCommandLists(_countof(ppCommandLists),ppCommandLists);

		computeCommandQueue->Signal(computeFence.Get(), fenceValues[frameIndex]);

		computeCommandQueue->Wait(computeFence.Get(), fenceValues[frameIndex]);
	//}
	//catch(int a){
	//	a;
	//	throw;
	//}	

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
	CD3DX12_ROOT_PARAMETER1 rootParameters[GraphicsRootParamtersCount] = {};
	rootParameters[Cbv].InitAsConstantBufferView(0,0,D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters),rootParameters,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,featureData.HighestVersion,&signature,&error);
	assert(SUCCEEDED(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//コンピュート
	CD3DX12_DESCRIPTOR_RANGE1 ranges[2] = {};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	CD3DX12_ROOT_PARAMETER1 computeRootParameters[ComputeRootParametersCount] = {};
	computeRootParameters[SrvUavTable].InitAsDescriptorTable(2, ranges);
	computeRootParameters[RootConstants].InitAsConstants(4,0);
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSignatureDesc;
	computeRootSignatureDesc.Init_1_1(_countof(computeRootParameters), computeRootParameters);

	result = D3DX12SerializeVersionedRootSignature(&computeRootSignatureDesc, featureData.HighestVersion, &signature, &error);
	assert(SUCCEEDED(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&computeRootSignature));
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
			&psBlob, &errorBlob
		);

		result = D3DCompileFromFile(
			L"Resources/shader/GPUParticleCS.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "cs_5_0",
			D3DCOMPILE_DEBUG| D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&csBlob, &errorBlob
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
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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

float GPUParticle::GetRandomFloat(float min, float max)
{
	float scale = static_cast<float>(rand()) / RAND_MAX;
    float range = max - min;
    return scale * range + min;
}
