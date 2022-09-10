#include "TextureManager.h"
#include <cassert>

using namespace DirectX;
using namespace std;

TextureManager *TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}

void TextureManager::Load(uint32_t number, const string &fileName)
{
	TextureManager::GetInstance()->LoadTexture(number, fileName);
}

void TextureManager::Initialize(DirectXCommon *dxCommon, string directoryPath)
{
	assert(dxCommon);

	this->dxCommon = dxCommon;
	this->directoryPath = directoryPath;

	//デスクリプタサイズ設定
	descriptorHandleIncrementSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//リセット
	ResetAll();
}

void TextureManager::LoadTexture(uint32_t number, const string filename)
{
	assert(number <= maxTextureCount - 1);

	HRESULT result;
	uint32_t handle = indexNextDescriptorHeap;
	
	//ディレクトリパスとファイル名の連結
	string fullPath = directoryPath + filename;

	//ロード用にstringをユニコード文字列に変換
	wchar_t wfilwPath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilwPath, _countof(wfilwPath));

	//画像ファイル用意
	TexMetadata metadata{};
	ScratchImage scratchImage{};
	//WICテクスチャデータのロード
	result = LoadFromWICFile(wfilwPath, WIC_FLAGS_NONE, &metadata, scratchImage);
	assert(SUCCEEDED(result));

	//ミップマップの生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), 
		scratchImage.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
	if(SUCCEEDED(result)){
		scratchImage = move(mipChain);
		metadata = scratchImage.GetMetadata();
	}

	//読み込んどディヒューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	//テクスチャバッファ設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHandleProp{};
	textureHandleProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHandleProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHandleProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//テクスチャバッファ生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textures[number].resource)
	);
	assert(SUCCEEDED(result));

	//テクスチャバッファへのデータ転送
	//全ミップマップ
	for(size_t i = 0; i < metadata.mipLevels; i++){
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImage.GetImage(i,0,0);
		//テクスチャバッファに転送
		result = textures[number].resource->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域コピー
			img->pixels,			//データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	//シェーダリソースビュー生成
	textures[number].cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descriptorHeap->GetCPUDescriptorHandleForHeapStart(), number, descriptorHandleIncrementSize
	);
	textures[number].gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descriptorHeap->GetGPUDescriptorHandleForHeapStart(), number, descriptorHandleIncrementSize
	);

	//リソース設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resourceDesc = textures[number].resource->GetDesc();
	srvDesc.Format = resourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//ハンドルの指す位置にシェーダリソースビュー作成
	dxCommon->GetDevice()->CreateShaderResourceView(
		textures[number].resource.Get(),
		&srvDesc,
		textures[number].cpuDescHandleSRV
	);
	indexNextDescriptorHeap++;
}

void TextureManager::ResetAll()
{
	HRESULT result;

	//デスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NumDescriptors = maxTextureCount;

	result = dxCommon->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc,IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(result));

	indexNextDescriptorHeap = 0;

	//全テクスチャ初期化
	for(size_t i = 0; i < maxTextureCount; i++){
		textures[i].resource.Reset();
		textures[i].cpuDescHandleSRV.ptr = 0;
		textures[i].gpuDescHandleSRV.ptr = 0;
		textures[i].name.clear();
	}
}

ID3D12Resource *TextureManager::GetTextureBuffer(uint32_t number)
{
	assert(number < maxTextureCount);
	return textures[number].resource.Get();
}

const D3D12_RESOURCE_DESC TextureManager::GetResoureDesc(uint32_t number)
{
	assert(number < textures.size());
	Texture& texture = textures.at(number);
	return texture.resource->GetDesc();
}


void TextureManager::SetDescriptorHeaps(ID3D12GraphicsCommandList *commandList)
{
	//テクスチャ用デスクリプタヒープの設定
	ID3D12DescriptorHeap* ppHeaps[] = {descriptorHeap.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps),ppHeaps);
}

void TextureManager::SetShaderResourceView(ID3D12GraphicsCommandList *commandList, UINT RootParameterIndex, uint32_t texnumber)
{
	//SRVのサイズを取得
	UINT sizeSRV = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//デスクリプタテーブルの先頭を取得
	D3D12_GPU_DESCRIPTOR_HANDLE start = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

	//SRVのGPUハンドル取得
	CD3DX12_GPU_DESCRIPTOR_HANDLE handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(start, texnumber,sizeSRV);

	//シェーダーリソースビュー(SRV)をセット
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, handle);
}
