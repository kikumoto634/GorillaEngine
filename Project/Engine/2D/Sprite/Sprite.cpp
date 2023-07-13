#include "Sprite.h"

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

Sprite::Common* Sprite::common = nullptr;

void Sprite::StaticInitialize(DirectXCommon* dxCommon,
	int window_width, int window_height, const std::string& directoryPath)
{
	common = new Common();

	common->dxCommon = dxCommon;

	//グラフィックスパイプライン生成
	common->InitializeGraphicsPipeline(directoryPath);

	//並行投影の射影行列生成
	common->matProjection = XMMatrixOrthographicOffCenterLH
	(
		0.f, (float)window_width,
		(float)window_height, 0.f,
		0.f, 1.f
	);
}

void Sprite::StaticFinalize()
{
	//解放
	if(common != nullptr)
	{
		delete common;
		common = nullptr;
	}
}

void Sprite::SetPipelineState()
{
	//パイプラインステートの設定
	common->dxCommon->GetCommandList()->SetPipelineState(common->pipelinestate.Get());
	//ルートシグネチャの設定
	common->dxCommon->GetCommandList()->SetGraphicsRootSignature(common->rootsignature.Get());
	//プリミティブ形状を設定
	common->dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

Sprite *Sprite::Create(UINT texNumber, Vector2 pos, XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY)
{
	// 仮サイズ
	Vector2 size = {100.0f, 100.0f};

	{
		// テクスチャ情報取得
		//const D3D12_RESOURCE_DESC& resDesc = common->textureManager->GetSpriteTexBuffer(texNumber)->GetDesc();
		const D3D12_RESOURCE_DESC& resDesc = TextureManager::GetInstance()->GetSpriteTexBuffer(texNumber)->GetDesc();
		// スプライトのサイズをテクスチャのサイズに設定
		size = {(float)resDesc.Width, (float)resDesc.Height};
	}

	// Spriteのインスタンスを生成
	Sprite* sprite =
		new Sprite(texNumber, {pos.x,pos.y,0.f}, size, color, anchorpoint, isFlipX, isFlipY);
	if (sprite == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!sprite->Initialize(texNumber)) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return sprite;
}

Sprite::Sprite()
{
}

Sprite::Sprite(UINT texnumber, Vector3 pos, Vector2 size, XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY)
{
	this->texNumber = texnumber;
	this->position = pos;
	this->size = size;
	this->color = color;
	this->matWorld = XMMatrixIdentity();
	this->anchorpoint = anchorpoint;
	this->IsFlipX = isFlipX;
	this->IsFlipY = isFlipY;
	this->texSize = size;
}

bool Sprite::Initialize(UINT texNumber)
{
	HRESULT result;

	//テクスチャ番号コピー
	this->texNumber = texNumber;

	//指定番号の画像が読込落ちなら
	if(TextureManager::GetInstance()->GetSpriteTexBuffer(texNumber))
	{
		//テクスチャ情報取得
		resourceDesc = TextureManager::GetInstance()->GetSpriteTexBuffer(texNumber)->GetDesc();

		//スプライトの大きさを画像の解像度に合わせる
		this->size = {(float)resourceDesc.Width, (float)resourceDesc.Height};
	}

	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
		  CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * common->vertNum);

		//頂点バッファ生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&this->vertBuff)
		);
		assert(SUCCEEDED(result));
	}

	//頂点バッファへのデータ転送
	SpriteTransferVertexBuffer();

	//頂点バッファビューの作成
	this->vbView.BufferLocation = this->vertBuff->GetGPUVirtualAddress();
	this->vbView.SizeInBytes = sizeof(VertexPosUv) * common->vertNum;
	this->vbView.StrideInBytes = sizeof(VertexPosUv);

	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
		  CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

		//定数バッファの生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&this->constBuffData)
		);
		assert(SUCCEEDED(result));
	}

	//定数バッファのデータ転送
	result = this->constBuffData->Map(0, nullptr, (void**)&constMap);
	if(SUCCEEDED(result)){
		constMap->color = this->color;
		constMap->mat = common->matProjection;
		this->constBuffData->Unmap(0, nullptr);
	}

	return true;
}

//スプライト単体頂点バッファの転送
void Sprite::SpriteTransferVertexBuffer()
{
	HRESULT result = S_FALSE;

	enum {LB, LT, RB, RT};

	float left = (0.f - this->anchorpoint.x)* this->size.x;
	float right = (1.f - this->anchorpoint.x)* this->size.x;
	float top = (0.f - this->anchorpoint.y)* this->size.y;
	float bottom = (1.f - this->anchorpoint.y)* this->size.y;

	if(this->IsFlipX)
	{//左右入れ替え
		left = -left;
		right = -right;
	}
	if(this->IsFlipY)
	{//上下反転
		top = -top;
		bottom = -bottom;
	}

	//頂点データ
	VertexPosUv vertices[4];

	vertices[LB].pos = {left, bottom, 0.f};
	vertices[LT].pos = {left, top, 0.f};
	vertices[RB].pos = {right, bottom, 0.f};
	vertices[RT].pos = {right, top, 0.f};

	//UV計算
	//指定番号の画像が読込済みなら
	if(TextureManager::GetInstance()->GetSpriteTexBuffer(texNumber))
	{
		float tex_left = this->texLeftTop.x / resourceDesc.Width;
		float tex_right = (this->texLeftTop.x + this->texSize.x) / resourceDesc.Width;
		float tex_top = this->texLeftTop.y / resourceDesc.Height;
		float tex_bottom = (this->texLeftTop.y + this->texSize.y) / resourceDesc.Height;
	
		vertices[LB].uv = {tex_left, tex_bottom};
		vertices[LT].uv = {tex_left, tex_top};
		vertices[RB].uv = {tex_right, tex_bottom};
		vertices[RT].uv = {tex_right, tex_top};
	}

	//頂点バッファのデータ転送
	result = this->vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	memcpy(vertMap, vertices,sizeof(vertices));
	this->vertBuff->Unmap(0,nullptr);
}

void Sprite::Draw()
{
	//ワールド行列の更新
	this->matWorld = XMMatrixIdentity();
	//Z軸回転
	this->matWorld *= XMMatrixRotationZ(XMConvertToRadians(this->rotation));
	//平行移動
	this->matWorld *= XMMatrixTranslation(this->position.x, this->position.y, this->position.z);

	//定数バッファの転送
	HRESULT result;
	result = this->constBuffData->Map(0,nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->mat = this->matWorld * common->matProjection;
	constMap->color = this->color;
	this->constBuffData->Unmap(0, nullptr);

	if(this->IsInvisible){
		return ;
	}

	//頂点バッファのセット
	common->dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&this->vbView);
	//定数バッファをセット
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, this->constBuffData->GetGPUVirtualAddress());
	//テスクチャ用デスクリプタヒープの設定
	TextureManager::GetInstance()->SetDescriptorHeaps(common->dxCommon->GetCommandList());
	//シェーダーリソースビューをセット
	TextureManager::GetInstance()->SetShaderResourceView(common->dxCommon->GetCommandList(), 1, this->texNumber);
	//ポリゴンの描画
	common->dxCommon->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

void Sprite::SetPosition(Vector2 pos)
{
	this->position.x = pos.x;
	this->position.y = pos.y;

	//頂点情報の転送
	SpriteTransferVertexBuffer();
}

void Sprite::SetSize(Vector2 size)
{
	this->size.x = size.x;
	this->size.y = size.y;
	
	//頂点情報の転送
	SpriteTransferVertexBuffer();
}

void Sprite::SetAnchorpoint(Vector2 pos)
{
	this->anchorpoint.x = pos.x;
	this->anchorpoint.y = pos.y;

	SpriteTransferVertexBuffer();
}

void Sprite::SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height)
{
	this->texLeftTop = {tex_x, tex_y};
	this->texSize = {tex_width, tex_height};

	SpriteTransferVertexBuffer();
}

void Sprite::SetIsFlipX(bool IsFlipX)
{
	this->IsFlipX = IsFlipX;

	SpriteTransferVertexBuffer();
}

void Sprite::SetIsFlipY(bool IsFlipY)
{
	this->IsFlipY = IsFlipY;

	SpriteTransferVertexBuffer();
}

void Sprite::SetColor(XMFLOAT4 color)
{
	this->color = color;
	SpriteTransferVertexBuffer();
}

void Sprite::SetTexNumber(UINT texNumber)
{
	this->texNumber = texNumber;
	SpriteTransferVertexBuffer();
}

void Sprite::Common::InitializeGraphicsPipeline(const std::string& directoryPath)
{
	HRESULT result;

	///頂点シェーダーfileの読み込みとコンパイル
	ID3DBlob* vsBlob ;			//頂点シェーダーオブジェクト
	ID3DBlob* psBlob ;			//ピクセルシェーダーオブジェクト
	ID3DBlob* errorBlob ;		//エラーオブジェクト

	std::string fullPathVS = directoryPath + "/SpriteVS.hlsl";
	wchar_t wFullPathVS[256];
	MultiByteToWideChar(CP_ACP, 0, fullPathVS.c_str(), -1, wFullPathVS, _countof(wFullPathVS));

	//頂点シェーダーの読み込みコンパイル
	result = D3DCompileFromFile(
		wFullPathVS,		//シェーダーファイル名
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

	std::string fullPathPS = directoryPath + "/SpritePS.hlsl";
	wchar_t wFullPathPS[256];
	MultiByteToWideChar(CP_ACP, 0, fullPathPS.c_str(), -1, wFullPathPS, _countof(wFullPathPS));

	//ピクセルシェーダーの読み込みコンパイル
	result = D3DCompileFromFile(
		wFullPathPS,		//シェーダーファイル名
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
	
		{//xyz座標
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

	///ルートパラメータ
	//デスクリプタレンジの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);


	//設定
	////定数バッファ 0番
	CD3DX12_ROOT_PARAMETER rootParam[2] = {};
	////定数　0番 material
	rootParam[0].InitAsConstantBufferView(0);
	////テクスチャレジスタ 0番
	rootParam[1].InitAsDescriptorTable(1, &descRangeSRV);


	///<summmary>
	///グラフィックスパイプライン
	///<summary/>
	
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
	//ブレンドステート
	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBAすべてのチャンネルを描画
	//共通設定
	blenddesc.BlendEnable = true;						//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;		//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;			//ソースの値を100% 使う	(ソースカラー			 ： 今から描画しようとしている色)
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;		//デストの値を  0% 使う	(デスティネーションカラー： 既にキャンバスに描かれている色)
	//各種設定
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//設定
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースの値を 何% 使う
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//デストの値を 何% 使う
	//頂点レイアウト設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	//図形の形状設定 (プリミティブトポロジー)
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//その他設定
	pipelineDesc.NumRenderTargets = 1;		//描画対象は一つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;	//1ピクセルにつき1回サンプリング
	//デプスステンシルステートの設定	(深度テストを行う、書き込み許可、深度がちいさければ許可)
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pipelineDesc.DepthStencilState.DepthEnable = false;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

	///テクスチャサンプラー
	//設定
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);


	//ルートシグネチャ (テクスチャ、定数バッファなどシェーダーに渡すリソース情報をまとめたオブジェクト)
	//設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0(_countof(rootParam), rootParam,1, &samplerDesc,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	//シリアライズ
	ID3DBlob* rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob,&errorBlob);
	assert(SUCCEEDED(result));
	result = dxCommon->GetDevice()->CreateRootSignature(0,rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),IID_PPV_ARGS(&common->rootsignature));
	assert(SUCCEEDED(result));
	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = common->rootsignature.Get();

	//パイプラインステート (グラフィックスパイプラインの設定をまとめたのがパイプラインステートオブジェクト(PSO))
	//パイプラインステートの生成
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&common->pipelinestate));
	assert(SUCCEEDED(result));
}
