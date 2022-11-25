#include "GeometryObjectManager.h"
#include <cassert>
#include "DirectXCommon.h"

using namespace std;
using namespace DirectX;

GeometryObjectManager *GeometryObjectManager::GetInstance()
{
	static GeometryObjectManager instance;
	return &instance;
}

void GeometryObjectManager::CreateBuffer()
{
	HRESULT result;
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

#pragma region 四角形情報
	//頂点データ
	VertexPos vertices[] = {
		//前
		{{0.0f, 0.0f, 0.0f}},//左下
	};
	copy(begin(vertices), end(vertices), this->vertices);

#pragma endregion

	//頂点バッファ
	//サイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPos)*_countof(vertices));
	{
		//ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
		//生成
		result = dxCommon->GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertBuffer)
		);
		assert(SUCCEEDED(result));

		//転送
		result = vertBuffer->Map(0, nullptr, (void**)&vertMap);
		if(SUCCEEDED(result)){
			memcpy(vertMap, vertices,sizeof(vertices));
			vertBuffer->Unmap(0,nullptr);
		}
	}

	//頂点バッファビューの生成
	vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(VertexPos);
}
