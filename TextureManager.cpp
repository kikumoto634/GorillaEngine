#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

TextureManager *TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(DirectXCommon *dxCommon, string directoryPath)
{
	assert(dxCommon);

	this->dxCommon = dxCommon;
	this->directoryPath = directoryPath;

	ResetAll();
}

void TextureManager::ResetAll()
{
	HRESULT result;

	//デスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NumDescriptors = maxTextureCount;

	result = dxCommon->GetDevice()->CreateDescriptorHeap(&descriptorHeap,IID_PPV_ARGS(&descriptorHeap));
}
