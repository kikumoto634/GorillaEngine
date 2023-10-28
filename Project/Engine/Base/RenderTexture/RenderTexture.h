#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <vector>

#include "DirectXCommon.h"

class RenderTexture
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	static RenderTexture* Create(const float clearColor[]);

public:
	void PreDraw();
	void PostDraw();

	//Getter
	ComPtr<ID3D12Resource> GetTexBuff()	{return texBuff_;}

private:
	RenderTexture(const float clearColor[]);

	void TextureInitialize();
	void RTVInitialize();
	void DepthInitialize();
	void DSVInitialize();

private:
	DirectXCommon* dxCommon;

	ComPtr<ID3D12Resource> texBuff_;
	//リソース情報
	D3D12_RESOURCE_DESC resourceDesc;

	ComPtr<ID3D12Resource> depthBuff;
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

	float clearColor[4];
};

