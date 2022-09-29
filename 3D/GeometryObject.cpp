#include "GeometryObject.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

GeometryObject::Common* GeometryObject::common = nullptr;

void GeometryObject::StaticInitialize(DirectXCommon *dxCommon)
{
	common = new Common();
	common->dxCommon = dxCommon;

	common->InitializeGraphicsPipeline();
	common->InitializeDescriptorHeap();

	common->geometryObjectManager = GeometryObjectManager::GetInstance();
	common->textureManager = TextureManager::GetInstance();
}

void GeometryObject::StaticFinalize()
{
	if(common != nullptr) return ;
	delete common;
	common = nullptr;
}

GeometryObject *GeometryObject::Create(UINT texNumber, XMFLOAT4 color)
{
	GeometryObject* object = new GeometryObject(texNumber, color);
	if(object == nullptr){
		return nullptr;
	}

	//初期化
	if(!object->Initialize(texNumber)){
		delete object;
		assert(0);
		return nullptr;
	}

	return object;
}

GeometryObject::GeometryObject()
{
}

GeometryObject::GeometryObject(UINT texNumber, XMFLOAT4 color)
{
	this->texNumber = texNumber;
	this->color = color;
}

bool GeometryObject::Initialize(UINT texNumber)
{
	HRESULT result;
	this->texNumber = texNumber;

	//定数バッファ生成
	{
		// ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc =
		  CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

		//生成
		result = common->dxCommon->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffer)
		);
		assert(SUCCEEDED(result));

		//定数バッファの転送
		result = constBuffer->Map(0, nullptr, (void**)&constMap);
		assert(SUCCEEDED(result));
		constBuffer->Unmap(0, nullptr);
	}
	return true;
}

void GeometryObject::Update(WorldTransform worldTransform, Camera* camera)
{
	//カメラの行列取得
	const XMMATRIX& matWorld = worldTransform.matWorld;
	const XMMATRIX& matView = camera->GetMatView();
	const XMMATRIX& matProjection = camera->GetMatProjection();

	constMap->color = color;
	constMap->mat = matWorld * matView * matProjection;
}

void GeometryObject::Draw()
{
#pragma region 共通描画コマンド
	//パイプラインステートの設定
	common->dxCommon->GetCommandList()->SetPipelineState(common->pipelineState.Get());
	//ルートシグネチャの設定
	common->dxCommon->GetCommandList()->SetGraphicsRootSignature(common->rootSignature.Get());
	//プリミティブ形状を設定
	common->dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion

#pragma region 個別描画コマンド
	//デスクリプタヒープのセット
	ID3D12DescriptorHeap* ppHeaps[] = {common->basicDescHeap.Get()};
	common->dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//定数バッファビュー(CBVの設定コマンド)
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffer->GetGPUVirtualAddress());
	//デスクリプタヒープの配列
	common->textureManager->SetDescriptorHeaps(common->dxCommon->GetCommandList());
	//頂点バッファの設定
	common->dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &common->geometryObjectManager->GetvbView());
	//インデックスバッファの設定
	common->dxCommon->GetCommandList()->IASetIndexBuffer(&common->geometryObjectManager->GetibView());

	//シェーダリソースビューをセット
	common->textureManager->SetShaderResourceView(common->dxCommon->GetCommandList(), 1, texNumber);

	//描画コマンド
	common->dxCommon->GetCommandList()->DrawIndexedInstanced(common->geometryObjectManager->GetIndices(),1, 0, 0, 0);
#pragma endregion
}

