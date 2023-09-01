#include "GPUParticle.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

const UINT GPUParticle::CommandSizePerFrame = TriangleCount * sizeof(IndirectCommand);
const UINT GPUParticle::CommandBufferCounterOffset = AlignForUavCounter(GPUParticle::CommandSizePerFrame);

const float GPUParticle::TriangleHalfWidth = 0.05f;
const float GPUParticle::TriangleDepth = 1.0f;

void GPUParticle::Initialize()
{
    window_ = Window::GetInstance();
    dxCommon_ = DirectXCommon::GetInstance();

    HRESULT result = {};
	UINT dxgiFactoryFlags = 0;

    frameIndex = 0;
    //rtvDescriptorSize = 0;
    cbvSrvUavDescriptorSize = 0;

    m_enableCulling = true;

    constantBufferData.resize(TriangleCount);

    csRootConstants.offSetX = TriangleHalfWidth;
    csRootConstants.offSetY = TriangleDepth;
    csRootConstants.offSetCull = 0.5f;
    csRootConstants.commandCount = TriangleCount;

    float center = window_->GetWindowWidth() /2.f;
    //cullingScissorRect.left = (LONG)(center - (center*0.5));
    //cullingScissorRect.right = (LONG)(center+(center*0.5));
    //cullingScissorRect.bottom = (LONG)(window_->GetWindowHeight());

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    result = (dxCommon_->GetDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    assert(SUCCEEDED(result));

    D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
    computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

    result = (dxCommon_->GetDevice()->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeCommandQueue)));
    assert(SUCCEEDED(result));
    //NAME_D3D12_OBJECT(computeCommandQueue);
 
    frameIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();


    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        /*D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        result = (dxCommon_->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
        assert(SUCCEEDED(result));*/

        // Describe and create a depth stencil view (DSV) descriptor heap.
        /*D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        result = (dxCommon_->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));
        assert(SUCCEEDED(result));*/

        // Describe and create a constant buffer view (CBV), Shader resource
        // view (SRV), and unordered access view (UAV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
        cbvSrvUavHeapDesc.NumDescriptors = CbvSrvUavDescriptorCountPerFrame * FrameCount;
        cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        result = (dxCommon_->GetDevice()->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&cbvSrvUavHeap)));
        assert(SUCCEEDED(result));
       // NAME_D3D12_OBJECT(cbvSrvUavHeap);

        //rtvDescriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        cbvSrvUavDescriptorSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    // Create frame resources.
    {
        //CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV and command allocators for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
           // NAME_D3D12_OBJECT_INDEXED(renderTargets, n);

            result = (dxCommon_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[n])));
            assert(SUCCEEDED(result));
            result = (dxCommon_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&computeCommandAllocators[n])));
            assert(SUCCEEDED(result));
        }
    }







     // Create the root signatures.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(dxCommon_->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_ROOT_PARAMETER1 rootParameters[GraphicsRootParametersCount];
        rootParameters[Cbv].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        result = (D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        assert(SUCCEEDED(result));
        result = (dxCommon_->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
        assert(SUCCEEDED(result));
        //NAME_D3D12_OBJECT(rootSignature);


        // Create compute signature.
        CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

        CD3DX12_ROOT_PARAMETER1 computeRootParameters[ComputeRootParametersCount];
        computeRootParameters[SrvUavTable].InitAsDescriptorTable(2, ranges);
        computeRootParameters[RootConstants].InitAsConstants(4, 0);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSignatureDesc;
        computeRootSignatureDesc.Init_1_1(_countof(computeRootParameters), computeRootParameters);

        result = (D3DX12SerializeVersionedRootSignature(&computeRootSignatureDesc, featureData.HighestVersion, &signature, &error));
        assert(SUCCEEDED(result));
        result = (dxCommon_->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&computeRootSignature)));
        assert(SUCCEEDED(result));
        //NAME_D3D12_OBJECT(computeRootSignature);
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;
        ComPtr<ID3DBlob> computeShader;
        ComPtr<ID3DBlob> error;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        result = D3DCompileFromFile(
		L"Resources/shader/GPUParticleVS.hlsl",    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vertexShader, &error);
        assert(SUCCEEDED(result));

        result = D3DCompileFromFile(
		L"Resources/shader/GPUParticlePS.hlsl",    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",    // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&pixelShader, &error);
        assert(SUCCEEDED(result));

        result = D3DCompileFromFile(
		L"Resources/shader/GPUParticleCS.hlsl",    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "cs_5_0",    // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&computeShader, &error);
        assert(SUCCEEDED(result));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        // Describe and create the graphics pipeline state objects (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;

        result = (dxCommon_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
        assert(SUCCEEDED(result));
        //NAME_D3D12_OBJECT(pipelineState);

        // Describe and create the compute pipeline state object (PSO).
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePsoDesc = {};
        computePsoDesc.pRootSignature = computeRootSignature.Get();
        computePsoDesc.CS = CD3DX12_SHADER_BYTECODE(computeShader.Get());

        result = (dxCommon_->GetDevice()->CreateComputePipelineState(&computePsoDesc, IID_PPV_ARGS(&computePipelineState)));
        assert(SUCCEEDED(result));
        //NAME_D3D12_OBJECT(computePipelineState);
    }

    // Create the command list.
    result = (dxCommon_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList)));
    assert(SUCCEEDED(result));
    result = (dxCommon_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, computeCommandAllocators[frameIndex].Get(), computePipelineState.Get(), IID_PPV_ARGS(&computeCommandList)));
    assert(SUCCEEDED(result));
    result = (computeCommandList->Close());
    assert(SUCCEEDED(result));

    //NAME_D3D12_OBJECT(commandList);
    //NAME_D3D12_OBJECT(computeCommandList);

    // Note: ComPtr's are CPU objects but these resources need to stay in scope until
    // the command list that references them has finished executing on the GPU.
    // We will flush the GPU at the end of this method to ensure the resources are not
    // prematurely destroyed.
    ComPtr<ID3D12Resource> vertexBufferUpload;
    ComPtr<ID3D12Resource> commandBufferUpload;

    // Create the vertex buffer.
    {
        // Define the geometry for a triangle.
        Vertex triangleVertices[] =
        {
            { { 0.0f, TriangleHalfWidth, TriangleDepth } },
            { { TriangleHalfWidth, -TriangleHalfWidth, TriangleDepth } },
            { { -TriangleHalfWidth, -TriangleHalfWidth, TriangleDepth } }
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);

        result = (dxCommon_->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&vertBuffer)));
        assert(SUCCEEDED(result));

        result = (dxCommon_->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBufferUpload)));
        assert(SUCCEEDED(result));

        //NAME_D3D12_OBJECT(vertBuffer);

        // Copy data to the intermediate upload heap and then schedule a copy
        // from the upload heap to the vertex buffer.
        D3D12_SUBRESOURCE_DATA vertexData = {};
        vertexData.pData = reinterpret_cast<UINT8*>(triangleVertices);
        vertexData.RowPitch = vertexBufferSize;
        vertexData.SlicePitch = vertexData.RowPitch;

        UpdateSubresources<1>(commandList.Get(), vertBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

        // Initialize the vertex buffer view.
        vertBufferView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
        vertBufferView.StrideInBytes = sizeof(Vertex);
        vertBufferView.SizeInBytes = sizeof(triangleVertices);
    }

    // Create the depth stencil view.
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        result = (dxCommon_->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, window_->GetWindowWidth(), window_->GetWindowHeight(), 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&depthStencil)
            ));
        assert(SUCCEEDED(result));

        //NAME_D3D12_OBJECT(depthStencil);

        //dxCommon_->GetDevice()->CreateDepthStencilView(depthStencil.Get(), &depthStencilDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
    }

    // Create the constant buffers.
    {
        const UINT constantBufferDataSize = TriangleResourceCount * sizeof(Const);

        result = (dxCommon_->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(constantBufferDataSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&constBuffer)));
        assert(SUCCEEDED(result));

        //NAME_D3D12_OBJECT(constBuffer);

        // Initialize the constant buffers for each of the triangles.
        for (UINT n = 0; n < TriangleCount; n++)
        {
            constantBufferData[n].velocity = Vector4(GetRandomFloat(0.01f, 0.02f), 0.0f, 0.0f, 0.0f);
            constantBufferData[n].offset = Vector4(GetRandomFloat(-5.0f, -1.5f), GetRandomFloat(-1.0f, 1.0f), GetRandomFloat(0.0f, 2.0f), 0.0f);
            constantBufferData[n].color = Vector4(GetRandomFloat(0.5f, 1.0f), GetRandomFloat(0.5f, 1.0f), GetRandomFloat(0.5f, 1.0f), 1.0f);
            DirectX::XMStoreFloat4x4(&constantBufferData[n].projection, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)(window_->GetWindowWidth()/window_->GetWindowHeight()), 0.01f, 20.0f)));
        }

        // Map and initialize the constant buffer. We don't unmap this until the
        // app closes. Keeping things mapped for the lifetime of the resource is okay.
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        result = (constBuffer->Map(0, &readRange, reinterpret_cast<void**>(&cbvDataBegin)));
        assert(SUCCEEDED(result));
        memcpy(cbvDataBegin, &constantBufferData[0], TriangleCount * sizeof(Const));

        // Create shader resource views (SRV) of the constant buffers for the
        // compute shader to read from.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = TriangleCount;
        srvDesc.Buffer.StructureByteStride = sizeof(Const);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvOffset, cbvSrvUavDescriptorSize);
        for (UINT frame = 0; frame < FrameCount; frame++)
        {
            srvDesc.Buffer.FirstElement = frame * TriangleCount;
            dxCommon_->GetDevice()->CreateShaderResourceView(constBuffer.Get(), &srvDesc, cbvSrvHandle);
            cbvSrvHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
        }
    }

    // Create the command signature used for indirect drawing.
    {
        // Each command consists of a CBV update and a DrawInstanced call.
        D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2] = {};
        argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
        argumentDescs[0].ConstantBufferView.RootParameterIndex = Cbv;
        argumentDescs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

        D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
        commandSignatureDesc.pArgumentDescs = argumentDescs;
        commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
        commandSignatureDesc.ByteStride = sizeof(IndirectCommand);

        result = (dxCommon_->GetDevice()->CreateCommandSignature(&commandSignatureDesc, rootSignature.Get(), IID_PPV_ARGS(&commandSignature)));
        assert(SUCCEEDED(result));
        //NAME_D3D12_OBJECT(commandSignature);
    }

    // Create the command buffers and UAVs to store the results of the compute work.
    {
        std::vector<IndirectCommand> commands;
        commands.resize(TriangleResourceCount);
        const UINT commandBufferSize = CommandSizePerFrame * FrameCount;

        D3D12_RESOURCE_DESC commandBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(commandBufferSize);
        result = (dxCommon_->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &commandBufferDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&commandBuffer)));
        assert(SUCCEEDED(result));

        result = (dxCommon_->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(commandBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&commandBufferUpload)));
        assert(SUCCEEDED(result));

        //NAME_D3D12_OBJECT(commandBuffer);

        D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = constBuffer->GetGPUVirtualAddress();
        UINT commandIndex = 0;

        for (UINT frame = 0; frame < FrameCount; frame++)
        {
            for (UINT n = 0; n < TriangleCount; n++)
            {
                commands[commandIndex].cbv = gpuAddress;
                commands[commandIndex].drawArguments.VertexCountPerInstance = 3;
                commands[commandIndex].drawArguments.InstanceCount = 1;
                commands[commandIndex].drawArguments.StartVertexLocation = 0;
                commands[commandIndex].drawArguments.StartInstanceLocation = 0;

                commandIndex++;
                gpuAddress += sizeof(Const);
            }
        }

        // Copy data to the intermediate upload heap and then schedule a copy
        // from the upload heap to the command buffer.
        D3D12_SUBRESOURCE_DATA commandData = {};
        commandData.pData = reinterpret_cast<UINT8*>(&commands[0]);
        commandData.RowPitch = commandBufferSize;
        commandData.SlicePitch = commandData.RowPitch;

        UpdateSubresources<1>(commandList.Get(), commandBuffer.Get(), commandBufferUpload.Get(), 0, 0, 1, &commandData);
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(commandBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

        // Create SRVs for the command buffers.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = TriangleCount;
        srvDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE commandsHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), CommandsOffset, cbvSrvUavDescriptorSize);
        for (UINT frame = 0; frame < FrameCount; frame++)
        {
            srvDesc.Buffer.FirstElement = frame * TriangleCount;
            dxCommon_->GetDevice()->CreateShaderResourceView(commandBuffer.Get(), &srvDesc, commandsHandle);
            commandsHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
        }

        // Create the unordered access views (UAVs) that store the results of the compute work.
        CD3DX12_CPU_DESCRIPTOR_HANDLE processedCommandsHandle(cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(), ProcessedCommandsOffset, cbvSrvUavDescriptorSize);
        for (UINT frame = 0; frame < FrameCount; frame++)
        {
            // Allocate a buffer large enough to hold all of the indirect commands
            // for a single frame as well as a UAV counter.
            commandBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(CommandBufferCounterOffset + sizeof(UINT), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            result = (dxCommon_->GetDevice()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &commandBufferDesc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&processedCommandBuffers[frame])));
            assert(SUCCEEDED(result));

            //NAME_D3D12_OBJECT_INDEXED(processedCommandBuffers, frame);

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
                processedCommandsHandle);

            processedCommandsHandle.Offset(CbvSrvUavDescriptorCountPerFrame, cbvSrvUavDescriptorSize);
        }

        // Allocate a buffer that can be used to reset the UAV counters and initialize
        // it to 0.
        result = (dxCommon_->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT)),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&processedCommandBufferCounterReset)));
        assert(SUCCEEDED(result));

        UINT8* pMappedCounterReset = nullptr;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        result = (processedCommandBufferCounterReset->Map(0, &readRange, reinterpret_cast<void**>(&pMappedCounterReset)));
        assert(SUCCEEDED(result));
        ZeroMemory(pMappedCounterReset, sizeof(UINT));
        processedCommandBufferCounterReset->Unmap(0, nullptr);
    }

    // Close the command list and execute it to begin the vertex buffer copy into
    // the default heap.
    result = (commandList->Close());
    assert(SUCCEEDED(result));
    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        result = (dxCommon_->GetDevice()->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        assert(SUCCEEDED(result));
        result = (dxCommon_->GetDevice()->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&computeFence)));
        fenceValues[frameIndex]++;

        // Create an event handle to use for frame synchronization.
        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEvent == nullptr)
        {
            result = (HRESULT_FROM_WIN32(GetLastError()));
            assert(SUCCEEDED(result));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForGpu();
    }

}

void GPUParticle::Update()
{
	for (UINT n = 0; n < TriangleCount; n++)
    {
        const float offsetBounds = 2.5f;

        // Animate the triangles.
        constantBufferData[n].offset.x += constantBufferData[n].velocity.x;
        if (constantBufferData[n].offset.x > offsetBounds)
        {
            constantBufferData[n].velocity.x = GetRandomFloat(0.01f, 0.02f);
            constantBufferData[n].offset.x = -offsetBounds;
        }
    }

    UINT8* destination = cbvDataBegin + (TriangleCount * frameIndex * sizeof(Const));
    memcpy(destination, &constantBufferData[0], TriangleCount * sizeof(Const));
}

void GPUParticle::Draw()
{
    HRESULT result = {};

    //try
    //{
        // Record all the commands we need to render the scene into the command list.
        // Command list allocators can only be reset when the associated 
        // command lists have finished execution on the GPU; apps should use 
        // fences to determine GPU execution progress.
        result = (computeCommandAllocators[frameIndex]->Reset());
        assert(SUCCEEDED(result));
        result = (commandAllocators[frameIndex]->Reset());
        assert(SUCCEEDED(result));

        // However, when ExecuteCommandList() is called on a particular command 
        // list, that command list can then be reset at any time and must be before 
        // re-recording.
        result = (computeCommandList->Reset(computeCommandAllocators[frameIndex].Get(), computePipelineState.Get()));
        assert(SUCCEEDED(result));
        result = (commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get()));
        assert(SUCCEEDED(result));

        // Record the compute commands that will cull triangles and prevent them from being processed by the vertex shader.
        if (m_enableCulling)
        {
            UINT frameDescriptorOffset = frameIndex * CbvSrvUavDescriptorCountPerFrame;
            D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvUavHandle = cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();

            computeCommandList->SetComputeRootSignature(computeRootSignature.Get());

            ID3D12DescriptorHeap* ppHeaps[] = { cbvSrvUavHeap.Get() };
            computeCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            computeCommandList->SetComputeRootDescriptorTable(
                SrvUavTable,
                CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvUavHandle, CbvSrvOffset + frameDescriptorOffset, cbvSrvUavDescriptorSize));

            computeCommandList->SetComputeRoot32BitConstants(RootConstants, 4, reinterpret_cast<void*>(&csRootConstants), 0);

            // Reset the UAV counter for this frame.
            computeCommandList->CopyBufferRegion(processedCommandBuffers[frameIndex].Get(), CommandBufferCounterOffset, processedCommandBufferCounterReset.Get(), 0, sizeof(UINT));

            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(processedCommandBuffers[frameIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            computeCommandList->ResourceBarrier(1, &barrier);

            computeCommandList->Dispatch(static_cast<UINT>(ceil(TriangleCount / float(ComputeThreadBlockSize))), 1, 1);
        }

        result = (computeCommandList->Close());
        assert(SUCCEEDED(result));

        // Record the rendering commands.
        {
            commandList->SetPipelineState(pipelineState.Get());
            // Set necessary state.
            commandList->SetGraphicsRootSignature(rootSignature.Get());

            ID3D12DescriptorHeap* ppHeaps[] = { cbvSrvUavHeap.Get() };
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            //CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
            //CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
            //commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

            // Record commands.
            //const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
            //commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
            //commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            commandList->IASetVertexBuffers(0, 1, &vertBufferView);

            if (m_enableCulling)
            {
                //PIXBeginEvent(commandList.Get(), 0, L"Draw visible triangles");

                // Draw the triangles that have not been culled.
                commandList->ExecuteIndirect(
                    commandSignature.Get(),
                    TriangleCount,
                    processedCommandBuffers[frameIndex].Get(),
                    0,
                    processedCommandBuffers[frameIndex].Get(),
                    CommandBufferCounterOffset);
            }
            else
            {
                //PIXBeginEvent(commandList.Get(), 0, L"Draw all triangles");

                // Draw all of the triangles.
                commandList->ExecuteIndirect(
                    commandSignature.Get(),
                    TriangleCount,
                    commandBuffer.Get(),
                    CommandSizePerFrame * frameIndex,
                    nullptr,
                    0);
            }

            result = (commandList->Close());
            assert(SUCCEEDED(result));
        }



        // Execute the compute work.
        if (m_enableCulling)
        {
            //PIXBeginEvent(commandQueue.Get(), 0, L"Cull invisible triangles");

            ID3D12CommandList* ppCommandLists[] = { computeCommandList.Get() };
            computeCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

            //PIXEndEvent(commandQueue.Get());

            computeCommandQueue->Signal(computeFence.Get(), fenceValues[frameIndex]);

            // Execute the rendering work only when the compute work is complete.
            commandQueue->Wait(computeFence.Get(), fenceValues[frameIndex]);
        }

        //PIXBeginEvent(commandQueue.Get(), 0, L"Render");

        // Execute the rendering work.
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        //PIXEndEvent(m_commandQueue.Get());

        MoveToNextFrame();
    //}
    //catch (HRESULT& e)
    //{
    //    if (e == DXGI_ERROR_DEVICE_REMOVED || e == DXGI_ERROR_DEVICE_RESET)
    //    {
    //        // Give GPU a chance to finish its execution in progress.
    //        try
    //        {
    //            WaitForGpu();
    //        }
    //        catch (HRESULT&)
    //        {
    //            // Do nothing, currently attached adapter is unresponsive.
    //        }
    //        fence.Reset();
    //        Initialize();
    //    }
    //    else
    //    {
    //        throw;
    //    }
    //}
}

void GPUParticle::Finalize()
{
    WaitForGpu();

    CloseHandle(fenceEvent);
}

float GPUParticle::GetRandomFloat(float min, float max)
{
	float scale = static_cast<float>(rand()) / RAND_MAX;
    float range = max - min;
    return scale * range + min;
}

void GPUParticle::WaitForGpu()
{
    HRESULT result = {};

    // Schedule a Signal command in the queue.
    result = (commandQueue->Signal(fence.Get(), fenceValues[frameIndex]));
    assert(SUCCEEDED(result));

    // Wait until the fence has been processed.
    result = (fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
    assert(SUCCEEDED(result));
    WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

    // Increment the fence value for the current frame.
    fenceValues[frameIndex]++;
}

void GPUParticle::MoveToNextFrame()
{
    HRESULT result = {};

    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = fenceValues[frameIndex];
    result = (commandQueue->Signal(fence.Get(), currentFenceValue));
    assert(SUCCEEDED(result));

    // Update the frame index.
    frameIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (fence->GetCompletedValue() < fenceValues[frameIndex])
    {
        result = (fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
        assert(SUCCEEDED(result));
        WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    fenceValues[frameIndex] = currentFenceValue + 1;
}
