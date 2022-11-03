#include "PostEffect.h"

PostEffect* PostEffect::Create(UINT texNumber, Vector2 pos, XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY)
{
	// 仮サイズ
	Vector2 size = {100.0f, 100.0f};

	{
		// テクスチャ情報取得
		//const D3D12_RESOURCE_DESC& resDesc = common->textureManager->GetSpriteTexBuffer(texNumber)->GetDesc();
		const D3D12_RESOURCE_DESC& resDesc = TextureManager::GetInstance()->GetSpriteTexBuffer(texNumber)->GetDesc();
		// スプライトのサイズをテクスチャのサイズに設定
		size = {(float)resDesc.Width, (float)resDesc.Height};
	}

	// Spriteのインスタンスを生成
	PostEffect* post =
		new PostEffect(texNumber, {pos.x,pos.y,0.f}, size, color, anchorpoint, isFlipX, isFlipY);
	if (post == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!post->Initialize(texNumber)) {
		delete post;
		assert(0);
		return nullptr;
	}

	return post;
}


PostEffect::PostEffect(UINT texnumber, Vector3 pos, Vector2 size, XMFLOAT4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY)
	:Sprite(
		texnumber,
		pos,
		size,
		color,
		anchorpoint,
		isFlipX,
		IsFlipY)
{
}

void PostEffect::Draw()
{
	//ワールド行列の更新
	this->matWorld = XMMatrixIdentity();
	//Z軸回転
	this->matWorld *= XMMatrixRotationZ(XMConvertToRadians(this->rotation));
	//平行移動
	this->matWorld *= XMMatrixTranslation(this->position.x, this->position.y, this->position.z);

	//定数バッファの転送
	HRESULT result;
	result = this->constBuffData->Map(0,nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->mat = this->matWorld * common->matProjection;
	constMap->color = this->color;
	this->constBuffData->Unmap(0, nullptr);

	if(this->IsInvisible){
		return ;
	}

	//パイプラインステートの設定
	common->dxCommon->GetCommandList()->SetPipelineState(common->pipelinestate.Get());
	//ルートシグネチャの設定
	common->dxCommon->GetCommandList()->SetGraphicsRootSignature(common->rootsignature.Get());
	//プリミティブ形状を設定
	common->dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//頂点バッファのセット
	common->dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&this->vbView);
	//定数バッファをセット
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, this->constBuffData->GetGPUVirtualAddress());
	//テスクチャ用デスクリプタヒープの設定
	TextureManager::GetInstance()->SetDescriptorHeaps(common->dxCommon->GetCommandList());
	//シェーダーリソースビューをセット
	TextureManager::GetInstance()->SetShaderResourceView(common->dxCommon->GetCommandList(), 1, this->texNumber);
	//ポリゴンの描画
	common->dxCommon->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
