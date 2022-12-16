#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <wrl.h>
#include <d3dx12.h>

#include <chrono>
#include <thread>

#include "Window.h"

/// <summary>
/// DirectXCommon
/// </summary>
class DirectXCommon
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static DirectXCommon* GetInstance();

	void Initialize(Window* window);
	void BeginDraw();
	void EndDraw();

	ID3D12Device* GetDevice()		{return device.Get();}
	ID3D12GraphicsCommandList* GetCommandList()		{return commandList.Get();}
	size_t GetBackBuffersCount()	{return backBuffers.size();}

private:
	/// <summary>
	/// 生成
	/// </summary>
	
	HRESULT CreateDevice();
	HRESULT CreateCommand();
	HRESULT CreateSwapChain();
	HRESULT CreateRTV();
	HRESULT CreateDSV();
	HRESULT CreateFence();

	void InitializeFixFPS();
	void UpdateFixFPS();

private:
	Window* window = nullptr;

	//DXGIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//デバイス
	ComPtr<ID3D12Device> device = nullptr;
	//グラフィックスコマンド
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain  = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//バックバッファ
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	//深度バッファ
	ComPtr<ID3D12Resource> depthBuffer = nullptr;
	//深度ステンシルビュー
	ComPtr<ID3D12DescriptorHeap> dsvHeap= nullptr;
	//レンダーターゲットビュー
	ComPtr<ID3D12DescriptorHeap> rtvHeap= nullptr;
	//フェンス
	ComPtr<ID3D12Fence> fence= nullptr;
	UINT64 fenceVal = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;

	// 記録時間(FPS固定用)
    std::chrono::steady_clock::time_point reference_;
};

