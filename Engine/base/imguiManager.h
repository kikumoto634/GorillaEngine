#pragma once
#include "Window.h"
#include "DirectXCommon.h"

#include <wrl.h>
#include <d3d12.h>

class imguiManager
{
public:
	//初期化
	void Initialize(Window* winApp, DirectXCommon* dxCommon);

private:
	Window* winApp;
	DirectXCommon* dxCommon;

	//SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

