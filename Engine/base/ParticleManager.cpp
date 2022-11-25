#include "ParticleManager.h"
#include <cassert>
#include "DirectXCommon.h"

using namespace std;
using namespace DirectX;

ParticleManager *ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::CreateBuffer()
{
	HRESULT result;
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

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

void ParticleManager::Update()
{
	//寿命がつきたパーティクルの全削除
	particle.remove_if([](Particle& x){
		return x.frame >= x.num_frame;
		}
	);

	//全パーティクル更新
	for(std::forward_list<Particle>::iterator it = particle.begin(); it != particle.end(); it++)
	{
		//経過フレーム数をカウント
		it->frame++;
		//速度に加速度を加算
		it->velocity = it->velocity + it->accel;
		//速度による移動
		it->position = it->position + it->velocity;

		//進行度を0~1の範囲でスケール計算
		float f = (float)it->frame / it->num_frame;
		//線形補完
		it->scale = (it->end_scale - it->start_scale)*f;
		it->scale += it->start_scale;
	}

	//頂点バッファへのデータ転送
	HRESULT result = vertBuffer->Map(0,nullptr,(void**)&vertMap);
	if(SUCCEEDED(result)){
		//パーティクルの情報を1ずつ反映
		for(std::forward_list<Particle>::iterator it = particle.begin(); it != particle.end(); it++){
			//座標
			vertMap->pos = it->position;
			//スケ―ル
			vertMap->scale = it->scale;
			//次の頂点
			vertMap++;
		}
		vertBuffer->Unmap(0,nullptr);
	}
}

void ParticleManager::Add(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale)
{
	//リストに要素を追加
	particle.emplace_front();
	//追加した要素の参照
	Particle& p = particle.front();
	//値のセット
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.start_scale = start_scale;
	p.end_scale = end_scale;
	p.num_frame = life;
}