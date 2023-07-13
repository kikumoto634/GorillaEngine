#include "FbxModelManager.h"

FbxModelManager::~FbxModelManager()
{
	//fbxScene->Destory();
}

void FbxModelManager::CreateBuffer(ID3D12Device *device)
{
	HRESULT result = S_FALSE;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin)*vertices.size());
	{
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

		result = device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertBuffer)
		);
		assert(SUCCEEDED(result));

		result = vertBuffer->Map(0, nullptr, (void**)&vertMap);
		if(SUCCEEDED(result)){
			std::copy(vertices.begin(), vertices.end(), vertMap);
			vertBuffer->Unmap(0, nullptr);
		}
	}
	vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
	{
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);

		result = device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)
		);
		assert(SUCCEEDED(result));

		result = indexBuffer->Map(0, nullptr, (void**)&indexMap);
		if(SUCCEEDED(result)){
			std::copy(indices.begin(), indices.end(), indexMap);
			indexBuffer->Unmap(0, nullptr);
		}
	}
	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;


	const DirectX::Image* img = scratchImage.GetImage(0,0,0);
	assert(img);

	{
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

		CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			metaData.format,
			metaData.width,
			(UINT)metaData.height,
			(UINT16)metaData.arraySize,
			(UINT16)metaData.mipLevels
		);

		result = device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texBuffer)
		);
		assert(SUCCEEDED(result));
	}

	result = texBuffer->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);
	assert(SUCCEEDED(result));


	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 1;
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeapSRV));
	assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texBuffer->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(
		texBuffer.Get(),
		&srvDesc,
		descriptorHeapSRV->GetCPUDescriptorHandleForHeapStart()
	);
}

void FbxModelManager::Draw(ID3D12GraphicsCommandList *commandList)
{
	ID3D12DescriptorHeap* ppHeaps[] = {descriptorHeapSRV.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->IASetVertexBuffers(0,1,&vbView);
	commandList->IASetIndexBuffer(&ibView);
	commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapSRV->GetGPUDescriptorHandleForHeapStart());
	commandList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}
