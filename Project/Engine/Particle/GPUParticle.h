#pragma once
#include "Vector3.h"
#include "Vector4.h"

#include <DirectXMath.h>
#include <d3dx12.h>
#include <DirectXCommon.h>

class GPUParticle{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//頂点
	struct Vertex{
		Vector3 pos;
	};

	//定数
	struct Const{
		Vector4 velocity;
		Vector4 offset;
		Vector4 color;
		DirectX::XMMATRIX projection;

		float padding[36];
	};

	//コンピュート
	struct Compute{
		float offSetX;
		float offSetY;
		float offSetCull;
		float commandCount;
	};

public:
	void Initialize();
	void Update();
	void Draw();
	void Finalize();

private:
	DirectXCommon* dxCommon_ = nullptr;

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12RootSignature> computeRootSignature;

	//パイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12PipelineState> computePipelineState;
};

