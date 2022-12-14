#include "ObjModelMesh.h"

#include <d3dcompiler.h>
#include <cassert>
#include <DirectXMath.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
DirectXCommon* ObjModelMesh::dxCommon = nullptr;

void ObjModelMesh::StaticInitialize(DirectXCommon *dxCommon)
{
	assert(!ObjModelMesh::dxCommon);

	ObjModelMesh::dxCommon = dxCommon;
}

void ObjModelMesh::CreateBuffers()
{
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv)*vertices.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	HRESULT result = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short)*indices.size());
	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&indexBuff));

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void ObjModelMesh::Draw(ID3D12GraphicsCommandList *commandList)
{
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	commandList->IASetIndexBuffer(&ibView);

	// 描画コマンド
	commandList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

void ObjModelMesh::AddVertex(const VertexPosNormalUv &vertex)
{
	vertices.emplace_back(vertex);
}

void ObjModelMesh::AddIndex(unsigned short index)
{
	indices.emplace_back(index);
}

void ObjModelMesh::AddSmmpthData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData[indexPosition].emplace_back(indexVertex);
}

void ObjModelMesh::CalculateSmoothedVertexNormals()
{
	auto it = smoothData.begin();
	for(; it != smoothData.end(); ++it){
		//各面用の共通頂点コレクション
		std::vector<unsigned short>& v = it->second;
		//全頂点の法線を平均化する
		XMVECTOR normal = {};
		for(unsigned short index : v){
			normal += XMVectorSet(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z,0);
		}
		normal = XMVector3Normalize(normal/(float)v.size());
		//共通法線を使用するすべての頂点データに書き込む
		for(unsigned short index : v){
			vertices[index].normal = {normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2]};
		}
	}
}
