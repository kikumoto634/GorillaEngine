#include "ViewProjection.h"
#include "DirectXCommon.h"
#include <d3dx12.h>
#include <cassert>

using namespace DirectX;

void ViewProjection::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void ViewProjection::CreateConstBuffer()
{
	HRESULT result;

	//ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
	  CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataViewProjection) + 0xff) & ~0xff);
	//定数バッファ生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
	  &heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuffer));
	assert(SUCCEEDED(result));
}

void ViewProjection::Map()
{
	//定数バッファとのデータリンク
	HRESULT result;
	result = constBuffer->Map(0, nullptr, (void**)&constBuffer);
	assert(SUCCEEDED(result));
}

void ViewProjection::UpdateMatrix()
{
	// ビュー行列の生成
	XMFLOAT3 leye = {eye.x,eye.y,eye.z};
	XMFLOAT3 ltarget = {target.x,target.y,target.z};
	XMFLOAT3 lup = {up.x,up.y,up.z};
	matView = XMMatrixLookAtLH(XMLoadFloat3(&leye), XMLoadFloat3(&ltarget), XMLoadFloat3(&lup));

	// 透視投影による射影行列の生成
	matProjection = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);

	// 定数バッファに書き込み
	constMap->view = matView;
	constMap->projection = matProjection;
	constMap->cameraPos = eye;
}
