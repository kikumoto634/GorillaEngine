#include "Particle.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

ParticleGPU::ParticleCommon* ParticleGPU::common = nullptr;

void ParticleGPU::StaticInitialize()
{
    common = new ParticleCommon();
    common->window_ = Window::GetInstance();
    common->dxCommon_ = DirectXCommon::GetInstance();

	common->CommandBufferCounterOffset = common->AlignForUavCounter(common->CommandSizePerFrame);

    common->Initialize();
}

void ParticleGPU::StaticFinalize()
{
    if(!common) return;
    delete common;
    common = nullptr;
}

void ParticleGPU::SetPipelineState()
{
    //パイプラインステートの設定
    common->dxCommon_->GetCommandList()->SetPipelineState(common->pipelineState.Get());
    //ルートシグネチャの設定
	common->dxCommon_->GetCommandList()->SetGraphicsRootSignature(common->rootSignature.Get());
	//プリミティブ形状を設定
	common->dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

ParticleGPU *ParticleGPU::Create()
{
    ParticleGPU* temp = new ParticleGPU();
    if(!temp) return nullptr;

    if(!temp->Initialize()){
        delete temp;
        assert(0);
        return nullptr;
    }
    return temp;
}

ParticleGPU::ParticleGPU()
{
}

bool ParticleGPU::Initialize()
{
    HRESULT result = {};
    camera_ = Camera::GetInstance();

	constantBufferData.resize(common->TriangleCount);

	//デスクリプタ
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 3;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        common->dxCommon_->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap_)));

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        common->dxCommon_->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_)));

		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
		cbvSrvUavHeapDesc.NumDescriptors = 3 * 3;
		cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		common->dxCommon_->GetDevice()->CreateDescriptorHeap(&cbvSrvUavHeapDesc,IID_PPV_ARGS(&cbvSrvUavHeap_));
	
		rtvDescriptorSize_ = common->dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		cbvSrvUavDescriptorSize_ = common->dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//RTV
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap_->GetCPUDescriptorHandleForHeapStart());

		for(UINT n = 0; n < 3; n++){
			result = common->dxCommon_->GetSwapChain()->GetBuffer(n, IID_PPV_ARGS(&descHeapRTV_[n]));
			assert(SUCCEEDED(result));
			common->dxCommon_->GetDevice()->CreateRenderTargetView(descHeapRTV_[n].Get(), nullptr,rtvHandle);
			rtvHandle.Offset(1,rtvDescriptorSize_);
		}
	}

    //頂点
    {
        // Define the geometry for a triangle.
        Vertex triangleVertices[] =
        {
            { { 0.0f, common->TriangleHalfWidth, common->TriangleDepth } },
            { { common->TriangleHalfWidth, -common->TriangleHalfWidth, common->TriangleDepth } },
            { { -common->TriangleHalfWidth, -common->TriangleHalfWidth, common->TriangleDepth } }
        };

        const UINT vertexBuffSize = sizeof(triangleVertices);

        // ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
		  CD3DX12_RESOURCE_DESC::Buffer(vertexBuffSize);

        //生成
        result = common->dxCommon_->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertBuff_)
		);
		assert(SUCCEEDED(result));
    
        //転送
        result = vertBuff_->Map(0,nullptr,(void**)&vertMap_);
        assert(SUCCEEDED(result));
        memcpy(vertMap_,triangleVertices,sizeof(triangleVertices));
        vertBuff_->Unmap(0,nullptr);
;
        //頂点バッファビュー
        vbView_.BufferLocation =vertBuff_->GetGPUVirtualAddress();
        vbView_.SizeInBytes = vertexBuffSize;
        vbView_.StrideInBytes = sizeof(Vertex);
    }

    //定数
    {
        const UINT constBuffSize = (sizeof(Const) * (common->TriangleCount*3));

        // ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
		  CD3DX12_RESOURCE_DESC::Buffer(constBuffSize);

        //生成
        result = common->dxCommon_->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff_)
		);
		assert(SUCCEEDED(result));


		for(UINT i = 0; i < common->TriangleCount; i++){
			constantBufferData[i].velocity = Vector4(RandomFloat(0.01f,0.02f),0.f,0.f,0.f);
			constantBufferData[i].offset = Vector4(RandomFloat(-5.f,-1.5f),RandomFloat(-1.f,1.f),RandomFloat(0.f,2.f),0.f);
			constantBufferData[i].color = Vector4(RandomFloat(0.5f,1.f),RandomFloat(0.5f,1.f),RandomFloat(0.5f,1.f),1.f);
			constantBufferData[i].projection = camera_->GetViewProjectionMatrix();
		}

		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        result = (constBuff_->Map(0, &readRange, reinterpret_cast<void**>(&cbvDataBegin)));
        assert(SUCCEEDED(result));
        memcpy(cbvDataBegin, &constantBufferData[0], common->TriangleCount * sizeof(Const));

    }

	//深度
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
		  CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,Window::GetWindowWidth(),Window::GetWindowHeight(),1,0,1,0,D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		result = common->dxCommon_->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&depthBuff)
		);
		assert(SUCCEEDED(result));
		common->dxCommon_->GetDevice()->CreateDepthStencilView(depthBuff.Get(),&depthStencilDesc,dsvHeap_->GetCPUDescriptorHandleForHeapStart());
	}

	//コマンドシグネチャ
	{
		D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2] = {};
		argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
		argumentDescs[0].ConstantBufferView.RootParameterIndex = 0;
		argumentDescs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
	}

    return true;
}

void ParticleGPU::Update()
{
	for (UINT n = 0; n < common->TriangleCount; n++)
    {
        const float offsetBounds = 2.5f;

        // Animate the triangles.
        constantBufferData[n].offset.x += constantBufferData[n].velocity.x;
        if (constantBufferData[n].offset.x > offsetBounds)
        {
            constantBufferData[n].velocity.x = RandomFloat(0.01f, 0.02f);
            constantBufferData[n].offset.x = -offsetBounds;
        }
    }

    UINT8* destination = cbvDataBegin + (common->TriangleCount * common->dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex() * sizeof(Const));
    memcpy(destination, &constantBufferData[0], common->TriangleCount * sizeof(Const));
}

void ParticleGPU::Draw()
{
	UINT frameIndex = common->dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();

	common->dxCommon_->GetCommandList()->IASetVertexBuffers(0,1, &vbView_);

	common->dxCommon_->GetCommandList()->ExecuteIndirect(
		commandSignature.Get(),
		common->TriangleCount,
		commandBuffer.Get(),
		(common->TriangleCount * sizeof(IndirectCommand)),
		nullptr,
		0
	);

}

float ParticleGPU::RandomFloat(float min, float max)
{
	float scale = static_cast<float>(rand()) / RAND_MAX;
    float range = max - min;
    return scale * range + min;
}

void ParticleGPU::ParticleCommon::Initialize()
{
    HRESULT result = {};

    //シェーダ
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* csBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    //頂点シェーダーの読み込みコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/GPUParticleVS.hlsl",		//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main", "vs_5_0",					//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0,
		&vsBlob, &errorBlob);
	//エラーなら
	if(FAILED(result)){
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダーの読み込みコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/GPUParticlePS.hlsl",		//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main", "ps_5_0",					//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0,
		&psBlob, &errorBlob);
	//エラーなら
	if(FAILED(result)){
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//コンピュートシェーダーの読み込みコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/GPUParticleCS.hlsl",		//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能にする
		"main", "cs_5_0",					//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0,
		&csBlob, &errorBlob);
	//エラーなら
	if(FAILED(result)){
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	
		{//座標
			"POSITION",										//セマンティック名
			0,												//同じセマンティック名が複数あるときに使うインデックス
			DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す (XYZの3つでfloat型なのでR32G32B32_FLOAT)
			0,												//入力スロットインデックス
			D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット値 (D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別 (標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
			0												//一度に描画するインスタンス数
		},
	};

	//グラフィックス
	{
		//パイプラインステート
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
		pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob);
		pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob);
		pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineDesc.InputLayout = {inputLayout, _countof(inputLayout)};
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		pipelineDesc.SampleDesc.Count = 1;
		pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineDesc.DepthStencilState.DepthEnable = false;
		pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		//ルートパラメータ
		CD3DX12_ROOT_PARAMETER1 rootParam[1] = {};
		rootParam[0].InitAsConstantBufferView(0,0,D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,D3D12_SHADER_VISIBILITY_VERTEX);

		///テクスチャサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

		//ルートシグネチャ
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.Init_1_1(_countof(rootParam),rootParam,1,&samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//シグネチャ
		ID3DBlob* signature;
		result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,D3D_ROOT_SIGNATURE_VERSION_1_1,&signature,&errorBlob);
		assert(SUCCEEDED(result));
		result = dxCommon_->GetDevice()->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(), IID_PPV_ARGS(&common->rootSignature));
		assert(SUCCEEDED(result));
		pipelineDesc.pRootSignature = common->rootSignature.Get();

		//パイプライン生成
		result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc,IID_PPV_ARGS(&common->pipelineState));
		assert(SUCCEEDED(result));
	}

	//コンピュート
	{
		//パイプラインステート
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineDesc{};
		computePipelineDesc.CS = CD3DX12_SHADER_BYTECODE(csBlob);

		//レンジ
		CD3DX12_DESCRIPTOR_RANGE1 ranges[2]{};
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,2,0,0,D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,1,0,0,D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);

		//ルートパラメータ
		CD3DX12_ROOT_PARAMETER1 computeRootParam[2] = {};
		computeRootParam[0].InitAsDescriptorTable(2,ranges);
		computeRootParam[1].InitAsConstants(4,0);

		//ルートシグネチャ
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSignatureDesc{};
		computeRootSignatureDesc.Init_1_1(_countof(computeRootParam),computeRootParam);

		//シグネチャ
		ID3DBlob* signature;
		result = D3DX12SerializeVersionedRootSignature(&computeRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &errorBlob);
		assert(SUCCEEDED(result));
		result = dxCommon_->GetDevice()->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&common->computeRootSignature));
		assert(SUCCEEDED(result));
		computePipelineDesc.pRootSignature = common->computeRootSignature.Get();

		//パイプライン生成
		result = dxCommon_->GetDevice()->CreateComputePipelineState(&computePipelineDesc, IID_PPV_ARGS(&common->computePipelineState));
		assert(SUCCEEDED(result));
	}
}
