#include "PostEffect.h"
#include "Window.h"

#include "Easing.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

PostEffect* PostEffect::instance = nullptr;
const float PostEffect::clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};


PostEffect *PostEffect::GetInstance()
{
	if(!instance)
	{
		instance = new PostEffect();
	}
	return instance;
}

void PostEffect::Delete()
{
	if(instance){
		delete instance;
		instance = nullptr;
	}
}

PostEffect *PostEffect::Create(UINT texNumber, Vector2 pos, Vector2 size, XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY)
{
	if (instance== nullptr) {
		return nullptr;
	}
	instance->texNumber = texNumber;
	instance->position = {pos.x,pos.y,0};
	instance->size = size;
	instance->anchorpoint = anchorpoint;
	instance->color = color;
	instance->IsFlipX = isFlipX;
	instance->IsFlipY = isFlipY;

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
		return nullptr;
	}

	return instance;
}

bool PostEffect::Initialize()
{
	SpriteInitialize();
	TextureInitialize();
	SRVInitialize();
	RTVInitialize();
	DepthInitialize();
	DSVInitialize();

	HRESULT result;
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData_Blur) + 0xff) & ~0xff);

		// 定数バッファの生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&heapProps, // アップロード可能
			D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&constBuff_Blur));
		assert(SUCCEEDED(result));
	}
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData_Fade) + 0xff) & ~0xff);

		// 定数バッファの生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&heapProps, // アップロード可能
			D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&constBuff_Fade));
		assert(SUCCEEDED(result));
	}

	//パイプライン
	CreateGraphicsPipelineState();

	return true;
}

void PostEffect::Update()
{
}

void PostEffect::Draw()
{
	if(!instance) return;

	//ワールド行列の更新
	this->matWorld = XMMatrixIdentity();
	//Z軸回転
	this->matWorld *= XMMatrixRotationZ(XMConvertToRadians(this->rotation));
	//平行移動
	this->matWorld *= XMMatrixTranslation(this->position.x, this->position.y, this->position.z);

	//定数バッファの転送
	HRESULT result;
	{
		ConstBufferData_Blur* constMap = nullptr;
		result = this->constBuff_Blur->Map(0,nullptr, (void**)&constMap);
		assert(SUCCEEDED(result));
		constMap->isActive = isBlurActive_;
		constMap->count = blurCount;
		constMap->strength= blurStrength;
		constBuff_Blur->Unmap(0, nullptr);
	}
	{
		ConstBufferData_Fade* constMap = nullptr;
		result = this->constBuff_Fade->Map(0,nullptr, (void**)&constMap);
		assert(SUCCEEDED(result));
		constMap->isActive = isFadeActive;
		constMap->color = fadeColor;
		constBuff_Fade->Unmap(0, nullptr);
	}


	//パイプラインステートの設定
	common->dxCommon->GetCommandList()->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	common->dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状を設定
	common->dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	if(this->IsInvisible){
		return ;
	}

	//頂点バッファのセット
	common->dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&this->vbView);
	//定数バッファをセット
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff_Blur->GetGPUVirtualAddress());
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuff_Fade->GetGPUVirtualAddress());
	//テスクチャ用デスクリプタヒープの設定
	ID3D12DescriptorHeap* ppHeaps[] = {descHeapSRV.Get()};
	common->dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps),ppHeaps);
	//シェーダーリソースビュー(SRV)をセット
	common->dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());
	//ポリゴンの描画
	common->dxCommon->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}



void PostEffect::SpriteInitialize()
{
	HRESULT result;
	
	//頂点
	{
		//生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * VertNum),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertBuff)
		);
		assert(SUCCEEDED(result));

		//データ
		VertexPosUv vertices[VertNum] = {
			{{-1.0f, -1.0f, +0.0f}, {0.0f, 1.0f}},
			{{-1.0f, +1.0f, +0.0f}, {0.0f, 0.0f}},
			{{+1.0f, -1.0f, +0.0f}, {1.0f, 1.0f}},
			{{+1.0f, +1.0f, +0.0f}, {1.0f, 0.0f}},
		};

		//転送
		VertexPosUv* vertMap = nullptr;
		result = vertBuff->Map(0,nullptr, (void**)&vertMap);
		if(SUCCEEDED(result)){
			memcpy(vertMap, vertices, sizeof(vertices));
			vertBuff->Unmap(0, nullptr);
		}

		//VBV(頂点バッファビュー)
		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = sizeof(VertexPosUv) * VertNum;
		vbView.StrideInBytes = sizeof(VertexPosUv);
	}
}

void PostEffect::TextureInitialize()
{
	HRESULT result;
	//テクスチャリソース
	{
		CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			Window::GetWindowWidth(),
			(UINT)Window::GetWindowHeight(),
			1,0,1,0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		//テクスチャバッファ
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,clearColor),
			IID_PPV_ARGS(&texbuff)
		);
		assert(SUCCEEDED(result));
	}

	//イメージデータ転送
	{
		///画素数
		const UINT pixelCount = Window::GetWindowWidth()*Window::GetWindowHeight();
		//画像1行のデータ
		const UINT rowPitch = sizeof(UINT) * Window::GetWindowWidth();
		//画像全体サイズ
		const UINT depthPitch = rowPitch * Window::GetWindowHeight();
		//イメージ
		UINT* img = new UINT[pixelCount];
		for(UINT i = 0; i < pixelCount; i++)	{img[i] = 0xff0000ff;}

		//転送
		result = texbuff->WriteToSubresource(
			0,
			nullptr,
			img,
			rowPitch,
			depthPitch
		);
		assert(SUCCEEDED(result));
		delete[] img;
	}
}

void PostEffect::SRVInitialize()
{
	HRESULT result;
	//SRVデスクリ
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
		srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvDescHeapDesc.NumDescriptors = 1;
		//生成
		result = common->dxCommon->GetDevice()->CreateDescriptorHeap(
			&srvDescHeapDesc,
			IID_PPV_ARGS(&descHeapSRV)
		);
		assert(SUCCEEDED(result));

		//SRV作成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		//デスクリプタヒープにSRV作成
		common->dxCommon->GetDevice()->CreateShaderResourceView(
			texbuff.Get(),
			&srvDesc,
			descHeapSRV->GetCPUDescriptorHandleForHeapStart()
		);
	}
}

void PostEffect::RTVInitialize()
{
	HRESULT result;
	//RTV
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
		rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescHeapDesc.NumDescriptors = 1;
		//生成
		result = common->dxCommon->GetDevice()->CreateDescriptorHeap(
			&rtvDescHeapDesc,
			IID_PPV_ARGS(&descHeapRTV)
		);
		assert(SUCCEEDED(result));

		//設定
		D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		//シェーダーの計算結果をSRGBに変換と書き込み
		renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		//作成
		common->dxCommon->GetDevice()->CreateRenderTargetView(
			texbuff.Get(),
			&renderTargetViewDesc,
			descHeapRTV->GetCPUDescriptorHandleForHeapStart()
		);
	}
}

void PostEffect::DepthInitialize()
{
	HRESULT result;
	//深度バッファ
	{
		//設定
		CD3DX12_RESOURCE_DESC depthResDesc = 
			CD3DX12_RESOURCE_DESC::Tex2D(
				DXGI_FORMAT_D32_FLOAT,
				Window::GetWindowWidth(),
				Window::GetWindowHeight(),
				1,0,
				1,0,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
			);

		//生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT,1.0f, 0),
			IID_PPV_ARGS(&depthBuff)
		);
		assert(SUCCEEDED(result));
	}
}

void PostEffect::DSVInitialize()
{
	HRESULT result;
	//DSV
	{
		//設定
		D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
		DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		DescHeapDesc.NumDescriptors = 1;

		//作成
		result = common->dxCommon->GetDevice()->CreateDescriptorHeap(
			&DescHeapDesc,
			IID_PPV_ARGS(&descHeapDSV)
		);
		assert(SUCCEEDED(result));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		common->dxCommon->GetDevice()->CreateDepthStencilView(
			depthBuff.Get(),
			&dsvDesc,
			descHeapDSV->GetCPUDescriptorHandleForHeapStart()
		);
	}
}

void PostEffect::CreateGraphicsPipelineState()
{
	HRESULT result;

	///頂点シェーダーfileの読み込みとコンパイル
	ID3DBlob* vsBlob = nullptr;			//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;			//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob = nullptr;		//エラーオブジェクト

	//頂点シェーダーの読み込みコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/PostEffectVS.hlsl",		//シェーダーファイル名
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
		L"Resources/shader/PostEffectPS.hlsl",		//シェーダーファイル名
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

	///頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	
		{//xy座標
			"POSITION",										//セマンティック名
			0,												//同じセマンティック名が複数あるときに使うインデックス
			DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す (XYZの3つでfloat型なのでR32G32B32_FLOAT)
			0,												//入力スロットインデックス
			D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット値 (D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別 (標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
			0												//一度に描画するインスタンス数
		},
		{//uv座標
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	//シェーダー設定
	pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob);
	pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob);
	
	//サンプルマスク設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定
	//ラスタライザ設定 背面カリング	ポリゴン内塗りつぶし	深度クリッピング有効
	pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//デプスステンシル
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBAすべてのチャンネルを描画
	//共通設定
	blenddesc.BlendEnable = true;						//ブレンドを有効にする
	//各種設定
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//設定
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースの値を 何% 使う
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//デストの値を 何% 使う

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;		//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;			//ソースの値を100% 使う	(ソースカラー			 ： 今から描画しようとしている色)
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;		//デストの値を  0% 使う	(デスティネーションカラー： 既にキャンバスに描かれている色)

	//ブレンドステート設定
	pipelineDesc.BlendState.RenderTarget[0] = blenddesc;

	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

	//頂点レイアウト設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	//図形の形状設定 (プリミティブトポロジー)
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//その他設定
	pipelineDesc.NumRenderTargets = 1;		//描画対象は一つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;	//1ピクセルにつき1回サンプリング

	//デスクリプタレンジの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//設定
	CD3DX12_ROOT_PARAMETER rootParam[3] = {};
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParam[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootParam[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	///スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0,D3D12_FILTER_MIN_MAG_MIP_POINT);
	//UVずらし防止
	//samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	//samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;

	//ルートシグネチャ (テクスチャ、定数バッファなどシェーダーに渡すリソース情報をまとめたオブジェクト)
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0(_countof(rootParam), rootParam,1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//シリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, 
		D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob,&errorBlob);
	assert(SUCCEEDED(result));

	//ルートシグネチャ
	result = common->dxCommon->GetDevice()->CreateRootSignature(0,rootSigBlob->GetBufferPointer(), 
		rootSigBlob->GetBufferSize(),IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();

	//パイプラインステート (グラフィックスパイプラインの設定をまとめたのがパイプラインステートオブジェクト(PSO))
	//パイプラインステートの生成
	result = common->dxCommon->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}



void PostEffect::PreDrawScene()
{
	if(!instance) return;
	//リソースバリア
	common->dxCommon->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			texbuff.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

	//レンダーターゲットビュー
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = 
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	
	//深度ステンシル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = 
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();

	//レンダーターゲットをセット
	common->dxCommon->GetCommandList()->OMSetRenderTargets(1,&rtvH,false,&dsvH);

	//ビューポート
	common->dxCommon->GetCommandList()->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0F,0.0F,(FLOAT)Window::GetWindowWidth(),(FLOAT)Window::GetWindowHeight()));

	//シザー
	common->dxCommon->GetCommandList()->RSSetScissorRects(1, &CD3DX12_RECT(0,0,Window::GetWindowWidth(),Window::GetWindowHeight()));

	//画面クリア
	common->dxCommon->GetCommandList()->ClearRenderTargetView(rtvH,clearColor,0,nullptr);

	//深度バッファクリア
	common->dxCommon->GetCommandList()->ClearDepthStencilView(dsvH,D3D12_CLEAR_FLAG_DEPTH,1.0f,0,0,nullptr);
}

void PostEffect::PostDrawScene()
{
	if(!instance) return;
	//リソースバリア
	common->dxCommon->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			texbuff.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		)
	);
}

void PostEffect::Blur(float second, const float Second)
{
	if(!isBlurActive_) return;

	blurStrength = BlurStrength*((Second-second)/Second);
}

bool PostEffect::FadeIn()
{
	if(!isFadeActive) return false;
	fadeColor = Easing_Point2_Linear<Vector3>({0,0,0}, {1,1,1}, Time_OneWay(fadeFrame, FadeSecond));

	if(fadeColor.x == 1) {
		isFadeActive = false;
		fadeFrame = 0;
		return true;
	}

	return false;
}

bool PostEffect::FadeOut()
{
	if(!isFadeActive) return false;
	fadeColor = Easing_Point2_Linear<Vector3>({1,1,1}, {0,0,0}, Time_OneWay(fadeFrame, FadeSecond));

	if(fadeColor.x == 0) {
		isFadeActive = false;
		fadeFrame = 0;
		return true;
	}

	return false;
}
