#include "TextureManager.h"
#include <assert.h>

void TextureManager::Load(UINT texnumber, const std::string &filename)
{
	TextureManager::GetInstance()->LoadTexture(texnumber, filename);
}

TextureManager *TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon)
{
	//メンバ関数に記録
	this->dxCommon = dxCommon;

	descriptorhandleIncrementSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//テクスチャリセット
	ResetAll();
}

void TextureManager::ResetAll()
{
	HRESULT result;

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc= {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors= maxTextureCount;
	result = this->dxCommon->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
	assert(SUCCEEDED(result));

	//全テクスチャ初期化
	for(size_t i = 0; i < maxTextureCount; i++){
		textures[i].resource.Reset();
		textures[i].cpuDescHandleSRV.ptr = 0;
		textures[i].gpuDescHandleSRV.ptr = 0;
	}
}

void TextureManager::LoadTexture(UINT texnumber, const std::string& filename)
{
	//異常な番号の指定を検出
	assert(texnumber <= maxTextureCount - 1);

	HRESULT result;

	//フルパス
	std::string fullPath = directoryPath + filename;
	//ユニコード文字列に変換
	wchar_t wfilePath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));

	///画像ファイルの用意
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICテクスチャデータのロード
	result = LoadFromWICFile(
		wfilePath,
		WIC_FLAGS_NONE,
		&metadata, scratchImg);
	assert(SUCCEEDED(result));


	//ミップマップの生成
	ScratchImage mipChain{};
	//生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), 
		scratchImg.GetImageCount(), 
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 
		0, 
		mipChain
	);
	if(SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	//フォーマットを書き換える
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	///テクスチャバッファ設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHandleProp{};
	textureHandleProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHandleProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHandleProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D
		(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT16)metadata.arraySize,
			(UINT16)metadata.mipLevels
		);

	//テクスチャバッファの生成
	result= dxCommon->GetDevice()->CreateCommittedResource
		(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textures[texnumber].resource)
		);
	assert(SUCCEEDED(result));


	//テクスチャバッファへのデータ転送
	//全ミップマップについて
	for(size_t i = 0; i < metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textures[texnumber].resource->WriteToSubresource(
			(UINT)i,				
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//一ラインサイズ
			(UINT)img->slicePitch	//一枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	///シェーダリソースビューの作成
	//設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビューの作成
	textures[texnumber].cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descHeap->GetCPUDescriptorHandleForHeapStart(), texnumber, 
		descriptorhandleIncrementSize);
	textures[texnumber].gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descHeap->GetGPUDescriptorHandleForHeapStart(), texnumber, 
		descriptorhandleIncrementSize);

	dxCommon->GetDevice()->CreateShaderResourceView
	(
		textures[texnumber].resource.Get(),
		&srvDesc,
		textures[texnumber].cpuDescHandleSRV
	);
}

ID3D12Resource *TextureManager::GetSpriteTexBuffer(UINT texnumber)
{
	//配列オーバーフロー防止
	assert(texnumber < maxTextureCount);

	return textures[texnumber].resource.Get();
}

void TextureManager::SetDescriptorHeaps(ID3D12GraphicsCommandList *commandList)
{
	//テスクチャ用デスクリプタヒープの設定
	ID3D12DescriptorHeap* ppHeaps[] = {descHeap.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps),ppHeaps);
}

void TextureManager::SetShaderResourceView(ID3D12GraphicsCommandList *commandList, UINT RootParameterIndex, UINT texnumber)
{
	//シェーダーリソースビュー(SRV)をセット
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, textures[texnumber].gpuDescHandleSRV);
}