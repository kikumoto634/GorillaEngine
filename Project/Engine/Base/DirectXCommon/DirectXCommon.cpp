#include "DirectXCommon.h"
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

DirectXCommon* DirectXCommon::instance = nullptr;

DirectXCommon *DirectXCommon::GetInstance()
{
	if(!instance){
		instance = new DirectXCommon();
	}
	return instance;
}

void DirectXCommon::Delete()
{
	//if(instance){
	//	//delete instance;
	//	//instance = nullptr;
	//}
}

void DirectXCommon::Initialize(Window *window)
{
	this->window = window;

	InitializeFixFPS();

	if(FAILED(CreateDevice())) assert(0);
	if(FAILED(CreateCommand())) assert(0);
	if(FAILED(CreateSwapChain())) assert(0);
	if(FAILED(CreateRTV())) assert(0);
	if(FAILED(CreateDSV())) assert(0);
	if(FAILED(CreateFence())) assert(0);
}

void DirectXCommon::BeginDraw()
{
	//リソースバリア
	UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

	//リソースバリア書き込み可能
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//描画先の変更
	//リソースバリアビュー
	rtvH = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//深度バッファビュー
	dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvH, false, &dsvH);


	//クリアコマンド
	//色
	FLOAT clearColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	//画面クリア
	commandList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	//深度クリア
	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH,1.f,0,0,nullptr);

	//ビューポート
	commandList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, (float)window->GetWindowWidth(), (float)window->GetWindowHeight()));
	//シザー矩形
	commandList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, (LONG)window->GetWindowWidth(), (LONG)window->GetWindowHeight()));
}

void DirectXCommon::EndDraw()
{
	//リソースバリア
	UINT bbIndex = swapChain->GetCurrentBackBufferIndex();
	commandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//コマンドリスト
	//命令クローズ
	commandList->Close();
	//コマンドリストの実行
	ID3D12CommandList* commandLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(1, commandLists);

	//コマンド完了まち
	commandQueue->Signal(fence.Get(), ++fenceVal);
	if(fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	UpdateFixFPS();

	//キュークリア
	commandAllocator->Reset();
	//再びコマンドリストを溜める準備
	commandList->Reset(commandAllocator.Get(), nullptr);

	//画面に表示するバッファをフリップ(表裏の入れ替え)
	swapChain->Present(1, 0);
}

HRESULT DirectXCommon::CreateDevice()
{
	HRESULT result;

	///デバックレイヤー
#ifdef _DEBUG
	//デバックレイヤーをオンに
	ID3D12Debug* debugController= nullptr;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))){
		debugController->EnableDebugLayer();
	}
#endif // _DEBUG

	///デバイスの生成(1ゲームに一つ)
	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = 
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	///アダプタ列挙
	//DXGIファクトリー
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	if(FAILED(result)){
		return result;
	}

	//アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//パフォーマンスが高いものから順に、すべてのアダプタを列挙する
	for(UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		//動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	///アダプタの選別
	//打倒なアダプタを選別する
	for(size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if(!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[i].Get();
			break;
		}
	}

	D3D_FEATURE_LEVEL featureLevel;
	for(size_t i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
		if(SUCCEEDED(result))
		{
			//デバイスを生成出来た時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// 抑制するエラー
		D3D12_MESSAGE_ID denyIds[] = {
		  /*
		   * Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
		   * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
		   */
		  D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE};
		// 抑制する表示レベル
		D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したエラーの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		// エラー時にブレークを発生させる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	}
#endif

	return result;
}

HRESULT DirectXCommon::CreateCommand()
{
	HRESULT result;

	///コマンドリスト(GPUに、まとめて命令を送るためのコマンド)
	//コマンドアロケータを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator)
	);
	if(FAILED(result)){
		return result;
	}

	//コマンドリストを生成
	result = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList)
	);
	if(FAILED(result)){
		return result;
	}


	///コマンドキュー(コマンドリストをGPUに順位実行させていく仕組み)
	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	if(FAILED(result)){
		return result;
	}

	return result;
}

HRESULT DirectXCommon::CreateSwapChain()
{
	HRESULT result;

	///スワップチェーン(フロントバッファ、バックバッファを入れ替えてパラパラ漫画を作る)
	//設定
	swapChainDesc.Width = window->GetWindowWidth();
	swapChainDesc.Height = window->GetWindowHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//色情報書式
	swapChainDesc.SampleDesc.Count = 1;								//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;				//バックバッファ用
	swapChainDesc.BufferCount = 2;									//バッファ数を二つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		//フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//生成
	//IDXGISwapChain1のComPtr用意
	ComPtr<IDXGISwapChain1> swapchain1;
	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		window->GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapchain1
	);


	//生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapchain1.As(&swapChain);
	if(FAILED(result)){
		return result;
	}

	return result;
}

HRESULT DirectXCommon::CreateRTV()
{
	HRESULT result;

	///レンダーターゲットビュー(バックバッファを描画キャンバスとして扱うオブジェクト)
	//デスクリプタヒープ生成(レンダーターゲットビューはデスクリプタヒープに生成するので準備)
	//設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;	//表裏の二つ(ダブルバッファリング)
	//生成
	result = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	if(FAILED(result)){
		return result;
	}

	//バックバッファ(スワップチェーン内で生成されたバックバッファのアドレス収容用)
	backBuffers.resize(swapChainDesc.BufferCount);

	//レンダーターゲットビュー(RTV)生成
	//スワップチェーンのすべてのバッファについて処理する
	for(size_t i = 0; i < backBuffers.size(); i++)
	{
		//スワップチェーンからバッファを取得
		result = swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		if(FAILED(result)){
			return result;
		}

		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビューの生成
		device->CreateRenderTargetView
		(
			backBuffers[i].Get(),
			&rtvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE//デスクリプタヒープのハンドルを取得
			(//表か裏でアドレスがずれる
				rtvHeap->GetCPUDescriptorHandleForHeapStart(),
				INT(i),
				device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type)
			)
		);
	}
	return result;
}

HRESULT DirectXCommon::CreateDSV()
{
	HRESULT result;

	///深度バッファのリソース(テクスチャの一種)
	//リソース設定
	CD3DX12_RESOURCE_DESC depthReourceDesc = CD3DX12_RESOURCE_DESC::Tex2D
		(
			DXGI_FORMAT_D32_FLOAT,
			window->GetWindowWidth(),
			window->GetWindowHeight(),
			1,0,
			1,0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	//深度バッファの生成
	result = device->CreateCommittedResource
		(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthReourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに使用
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
			IID_PPV_ARGS(&depthBuffer)
		);
	if(FAILED(result)){
		return result;
	}
	
	//深度ビュー用デスクリプタヒープ作成(DSV)
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;			//深度ビューは一つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//デプスステンシルビュー
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	if(FAILED(result)){
		return result;
	}
	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	return result;
}

HRESULT DirectXCommon::CreateFence()
{
	HRESULT result;

	///フェンス(CPUとGPUで同期をとるための仕組み)
	//生成
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if(FAILED(result)){
		return result;
	}
	return result;
}

void DirectXCommon::InitializeFixFPS()
{
    // 現在時間を記録する
    reference_ = std::chrono::steady_clock::now();
}

void DirectXCommon::UpdateFixFPS()
{
    // 1/60秒ぴったりの時間
    const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
    // 1/60秒よりわずかに短い時間
    const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

    // 現在時間を取得する
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // 前回記録からの経過時間を取得する
    std::chrono::microseconds elapsed = 
    std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    // 1/60秒（よりわずかに短い時間）経っていない場合
    if (elapsed < kMinCheckTime) {
        // 1/60秒経過するまで微小なスリープを繰り返す
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            // 1マイクロ秒スリープ
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
    // 現在の時間を記録する
    reference_ = std::chrono::steady_clock::now();

}
