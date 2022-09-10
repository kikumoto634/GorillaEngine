#include "Sprite.h"
#include <d3dcompiler.h>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

Sprite::Common* Sprite::common = nullptr;
UINT Sprite::descriptorHandleIncrementSize;
ID3D12GraphicsCommandList* Sprite::commandList = nullptr;

void Sprite::Common::InitializeGraphicsPipeline(const std::wstring &directoryPath)
{
	HRESULT result;

	//シェーダーの読み込みコンパイル
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errorBlob;

	//頂点シェーダー
	wstring vsFile = directoryPath + L"/shader/SpriteVS.hlsl";
	result = D3DCompileFromFile(
		vsFile.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能
		"main", "vs_5_0",	//エントリーポイント、シェーダーモデル
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0, &vsBlob, &errorBlob
	);
	if(FAILED(result)){
		string error;
		error.resize(errorBlob->GetBufferSize());
		copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), error.begin());
		error += "\n";
		//エラー内容を出力に表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点シェーダー
	wstring psFile = directoryPath + L"/shader/SpritePS.hlsl";
	result = D3DCompileFromFile(
		psFile.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//インクルード可能
		"main", "ps_5_0",	//エントリーポイント、シェーダーモデル
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバック用設定
		0, &psBlob, &errorBlob
	);
	if(FAILED(result)){
		string error;
		error.resize(errorBlob->GetBufferSize());
		copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), error.begin());
		error += "\n";
		//エラー内容を出力に表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{//xy座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//uv座標
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//グラフィックスパイプライン
	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineDesc{};
	gpipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipelineDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//サンプルマスク
	gpipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定

	//ラスタライザ設定		背面カリング　ポリゴン内塗りつぶし　深度クリッピング有効
	gpipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	//デプスステンシルステートの設定	(深度テストを行う、書き込み許可、深度がちいさければ許可)
	gpipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gpipelineDesc.DepthStencilState.DepthEnable = false;

	//深度バッファフォーマット
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

	//頂点レイアウト
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	gpipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形設定
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//その他設定
	gpipelineDesc.NumRenderTargets = 1;		//描画対象は一つ
	gpipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0~255指定のRGBA
	gpipelineDesc.SampleDesc.Count = 1;	//1ピクセルにつき1回サンプリング

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	//ルートパラメータ
	////定数バッファ 0番
	CD3DX12_ROOT_PARAMETER rootParam[2] = {};
	rootParam[0].InitAsConstantBufferView(0);
	rootParam[1].InitAsDescriptorTable(1, &descRangeSRV);

	//テクスチャサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャ
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0(_countof(rootParam), rootParam,1, &samplerDesc,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//シリアライズ
	//バージョン自動設定
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob,&errorBlob);
	assert(SUCCEEDED(result));

	//ルートシグネチャ生成
	result = dxCommon->GetDevice()->CreateRootSignature(0,rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),IID_PPV_ARGS(&common->rootsignature));
	assert(SUCCEEDED(result));

	//パイプラインにシグネチャをセット
	gpipelineDesc.pRootSignature = common->rootsignature.Get();

	//ブレンドステート
	// //レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	gpipelineDesc.BlendState.RenderTarget[0] = blenddesc;
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

	//グラフィックパイプライン生成
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&gpipelineDesc, IID_PPV_ARGS(&common->pipelinestate));
	assert(SUCCEEDED(result));
}

void Sprite::StaticInitialize(DirectXCommon* dxCommon, int window_width, int window_height, const std::wstring &directoryPath)
{
	common = new Common();

	assert(dxCommon);

	common->dxCommon = dxCommon;
	descriptorHandleIncrementSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//パイプライン生成
	common->InitializeGraphicsPipeline(directoryPath);

	//射影行列生成
	common->matProjection = XMMatrixOrthographicOffCenterLH(
		0.f, (float)window_width,
		(float)window_height, 0.f,
		0.f, 1.f
	);
}

void Sprite::StaticFinalize()
{
	if(common != nullptr){
		delete common;
		common = nullptr;
	}
}

void Sprite::PreDraw(ID3D12GraphicsCommandList *commandList)
{
	assert(Sprite::commandList = nullptr);
	assert(commandList);

	Sprite::commandList = commandList;

	//パイプラインステート設定
	Sprite::commandList->SetPipelineState(common->pipelinestate.Get());
	//ルートシグネチャ設定
	Sprite::commandList->SetGraphicsRootSignature(common->rootsignature.Get());
	//プリミティブ形状設定
	Sprite::commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::PostDraw()
{
	Sprite::commandList = nullptr;
}

Sprite *Sprite::Create(uint32_t textureNumber, Vector2 pos, DirectX::XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY)
{
	//仮サイズ
	Vector2 size = {100,100};
	
	if(TextureManager::GetInstance()->GetTextureBuffer(textureNumber))
	{
		//テクスチャ情報取得
		const D3D12_RESOURCE_DESC& resourceDesc = TextureManager::GetInstance()->GetResoureDesc(textureNumber);
		//スプライトのサイズをテクスチャのサイズ設定
		size = {(float)resourceDesc.Width, (float)resourceDesc.Height};
	}

	//インスタンス生成
	Sprite* sprite = new Sprite(textureNumber, pos, size, color, anchorpoint, isFlipX, isFlipY);
	if(!sprite) return nullptr;

	//初期化(失敗なら削除)
	if(!sprite->Initialize()){
		delete sprite;
		assert(0);
		return nullptr;
	}
	return sprite;
}

Sprite::Sprite()
{
}

Sprite::Sprite(uint32_t textureNumber, Vector2 pos, Vector2 size, DirectX::XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY)
{
	this->textureHandle = textureNumber;
	this->position = pos;
	this->size = size;
	this->anchorPoint = anchorPoint;
	this->color = color;
	this->matWorld = XMMatrixIdentity();
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;
	this->texSize = size;
}

bool Sprite::Initialize()
{
	HRESULT result;

	//頂点バッファ
	{
		//ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv)*VertNum);
		//生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuffer)
		);
		assert(SUCCEEDED(result));
	}
	//転送
	TransferVertices();
	
	//頂点バッファビュー生成
	vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv)*VertNum;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	//定数バッファ
	{
		//ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);
		//生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuffer)
		);
		assert(SUCCEEDED(result));
	}
	//転送
	result = constBuffer->Map(0, nullptr, (void**)&constMap);
	if(SUCCEEDED(result)){
		constMap->color = XMFLOAT4(1,1,1,1);
		constMap->mat = common->matProjection;
		this->constBuffer->Unmap(0, nullptr);
	}

	return true;
}

void Sprite::Draw()
{
	//ワールド座標更新
	matWorld = XMMatrixIdentity();
	matWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));
	matWorld *= XMMatrixTranslation(position.x, position.y, 0.f);

	//定数バッファ転送
	HRESULT result;
	result = constBuffer->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->color = color;
	constMap->mat = matWorld * common->matProjection;
	constBuffer->Unmap(0, nullptr);

	//非表示
	if(IsInvisible){
		return;
	}

	//頂点バッファ
	commandList->IASetVertexBuffers(0,1,&vbView);
	//定数バッファ
	commandList->SetGraphicsRootConstantBufferView(0, constBuffer->GetGPUVirtualAddress());
	//テクスチャ用デスクリプタヒープ
	TextureManager::GetInstance()->SetDescriptorHeaps(commandList);
	//シェーダーリソースビュー
	TextureManager::GetInstance()->SetShaderResourceView(commandList, 1, textureHandle);
	//描画
	commandList->DrawInstanced(4,1,0,0);
}

void Sprite::SetPosition(Vector2 pos)
{
	position = pos;

	TransferVertices();
}

void Sprite::SetRotation(float rotation)
{
	this->rotation = rotation;

	TransferVertices();
}

void Sprite::SetSize(Vector2 size)
{
	this->size = size;

	TransferVertices();
}

void Sprite::SetAnchorpoint(Vector2 pos)
{
	anchorPoint = pos;

	TransferVertices();
}

void Sprite::SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height)
{
	texBase = {tex_x, tex_y};
	texSize = {tex_width, tex_height};

	TransferVertices();
}

void Sprite::SetIsFlipX(bool IsFlipX)
{
	isFlipX = IsFlipX;

	TransferVertices();
}

void Sprite::SetIsFlipY(bool IsFlipY)
{
	isFlipY = isFlipY;

	TransferVertices();
}

void Sprite::SetColor(DirectX::XMFLOAT4 color)
{
	this->color = color;

	TransferVertices();
}

void Sprite::TransferVertices()
{
	HRESULT result = S_FALSE;

	enum {LB, LT, RB, RT};

	float left = (0.f - this->anchorPoint.x)* this->size.x;
	float right = (1.f - this->anchorPoint.x)* this->size.x;
	float top = (0.f - this->anchorPoint.y)* this->size.y;
	float bottom = (1.f - this->anchorPoint.y)* this->size.y;

	if(this->isFlipX)
	{//左右入れ替え
		left = -left;
		right = -right;
	}
	if(this->isFlipY)
	{//上下反転
		top = -top;
		bottom = -bottom;
	}

	//頂点データ
	VertexPosUv vertices[VertNum];
	vertices[LB].pos = {left, bottom, 0.f};
	vertices[LT].pos = {left, top, 0.f};
	vertices[RB].pos = {right, bottom, 0.f};
	vertices[RT].pos = {right, top, 0.f};

	//UV計算
	//指定番号の画像が読込済みなら
	if(TextureManager::GetInstance()->GetTextureBuffer(this->textureHandle))
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = TextureManager::GetInstance()->GetTextureBuffer(this->textureHandle)->GetDesc();

		float tex_left = this->texBase.x / resDesc.Width;
		float tex_right = (this->texBase.x + this->texSize.x) / resDesc.Width;
		float tex_top = this->texBase.y / resDesc.Height;
		float tex_bottom = (this->texBase.y + this->texSize.y) / resDesc.Height;
	
		vertices[LB].uv = {tex_left, tex_bottom};
		vertices[LT].uv = {tex_left, tex_top};
		vertices[RB].uv = {tex_right, tex_bottom};
		vertices[RT].uv = {tex_right, tex_top};
	}

	//頂点バッファのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = this->vertBuffer->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices,sizeof(vertices));
	this->vertBuffer->Unmap(0,nullptr);
}
