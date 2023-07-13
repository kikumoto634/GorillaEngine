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

	//ImGui受付開始
	void Begin();

	//ImGui受付終了
	void End();

	//描画
	void Draw();

	//後処理
	void Finalize();

private:
	Window* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;

	//SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

