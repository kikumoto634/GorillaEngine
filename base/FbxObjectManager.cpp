#include "FbxObjectManager.h"

FbxObjectManager::~FbxObjectManager()
{
}

void FbxObjectManager::CreateBuffers(ID3D12Device *device)
{
	HRESULT result = S_FALSE;

	//頂点データの全体サイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin)*vertices.size());
	//頂点バッファの生成
	result = device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));
	//頂点バッファへのデータ転送
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if(SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}
	//頂点バッファビュー(VBV)の作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//頂点インデックス全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	//インデックスバッファ生成
	result = device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	assert(SUCCEEDED(result));
	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr,(void**)&indexMap);
	if(SUCCEEDED(result))
	{
		std::copy(indices.begin(),indices.end(),indexMap);
		indexBuff->Unmap(0, nullptr);
	}
	//インデックスバッファビュー(IBV)の作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes= sizeIB;

	//テクスチャ画像データ
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);//生データ抽出
	assert(img);
	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D
		(
			metadata.format,
			metadata.width,
			(UINT)metadata.height,
			(UINT)metadata.arraySize,
			(UINT)metadata.mipLevels
		);
	//テクスチャ用バッファの生成
	result = device->CreateCommittedResource
		(
			&CD3DX12_HEAP_PROPERTIES
			(
				D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0
			),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,	//テクスチャ用指定
			nullptr,
			IID_PPV_ARGS(&texBuff)
		);
	assert(SUCCEEDED(result));
	//テクスチャバッファにデータ転送
	result = texBuff->WriteToSubresource
		(
			0,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//1枚サイズ
		);
	assert(SUCCEEDED(result));
	
	//SRV用デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	//シェーダーから見えるように
	descHeapDesc.NumDescriptors = 1;	//テクスチャ枚数
	result = device->CreateDescriptorHeap(&descHeapDesc,IID_PPV_ARGS(&descHeapSRV));//生成
	assert(SUCCEEDED(result));
	//シェーダーリソースビュー(SRV)生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	D3D12_RESOURCE_DESC resDesc= texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension =D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView
	(
		texBuff.Get(),	//ビューと関連付けるバッファ
		&srvDesc,	//テクスチャ設定情報
		descHeapSRV->GetCPUDescriptorHandleForHeapStart()	//ヒープの先頭アドレス
	);
}

void FbxObjectManager::Draw(ID3D12GraphicsCommandList *commandList)
{
	//デスクリプタヒープのセット
	ID3D12DescriptorHeap* ppHeaps[] = {descHeapSRV.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//頂点バッファをセット
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファをセット
	commandList->IASetIndexBuffer(&ibView);

	//シェーダリソースビューをセット
	commandList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());

	//描画コマンド
	commandList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}
