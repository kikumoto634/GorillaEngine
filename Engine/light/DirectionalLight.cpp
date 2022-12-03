#include "DirectionalLight.h"

DirectXCommon* DirectionalLight::dxCommon = nullptr;

using namespace DirectX;

DirectionalLight *DirectionalLight::Create()
{
	//3Dオブジェクトのインスタンス化
	DirectionalLight* instance = new DirectionalLight();
	//初期化
	instance->Initialize();
	//生成したインスタンスを返す
	return instance;
}

void DirectionalLight::StaticInitialize(DirectXCommon* dxCommon)
{
	assert(!DirectionalLight::dxCommon);
	assert(dxCommon);
	DirectionalLight::dxCommon = dxCommon;
}

void DirectionalLight::Initialize()
{
	HRESULT result;
	//定数バッファ生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	assert(SUCCEEDED(result));

	//転送
	TransferConstBuffer();
}

void DirectionalLight::Update()
{
	//値の更新があった時だけに定数バッファに転送する
	if(IsDirty){
		TransferConstBuffer();
		IsDirty = false;
	}
}

void DirectionalLight::Draw(UINT rootParameterIndex)
{
	//定数バッファビューをセット
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		rootParameterIndex, 
		constBuff->GetGPUVirtualAddress()
	);
}

void DirectionalLight::TransferConstBuffer()
{
	HRESULT result;
	//定数バッファ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0,nullptr, (void**)&constMap);
	if(SUCCEEDED(result))
	{
		constMap->lightv = {-lightdir.m128_f32[0], -lightdir.m128_f32[1], -lightdir.m128_f32[2],-lightdir.m128_f32[3]};
		constMap->lightcolor = lightcolor;
		constBuff->Unmap(0,nullptr);
	}
}

void DirectionalLight::SetLightDir(const DirectX::XMVECTOR &lightdir)
{
	this->lightdir = XMVector3Normalize(lightdir);
	IsDirty = true;
}

void DirectionalLight::SetLightColor(const Vector3 lightcolor)
{
	this->lightcolor = lightcolor;
	IsDirty = true;
}
