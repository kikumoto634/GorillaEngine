#include "RenderTexture.h"
#include <cassert>
#include "Window.h"

using namespace std;

RenderTexture::RenderTexture(DirectXCommon* dxCommon, const float clearColor[]):
	dxCommon(dxCommon)
{
	for(int i = 0; i < 4; i++){
		this->clearColor[i] = clearColor[i];
	}
}

void RenderTexture::TextureInitialize()
{
	HRESULT result{};
	//テクスチャリソース
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		Window::GetWindowWidth(),
		(UINT)Window::GetWindowHeight(),
		1,0,1,0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);
	for(int i = 0; i < 2; i++){
		//テクスチャバッファ
		result = dxCommon->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,clearColor),
			IID_PPV_ARGS(&texBuff_[i])
		);
		assert(SUCCEEDED(result));

		//イメージデータ転送
		///画素数
		const UINT pixelCount = Window::GetWindowWidth()*Window::GetWindowHeight();
		//画像1行のデータ
		const UINT rowPitch = sizeof(UINT) * Window::GetWindowWidth();
		//画像全体サイズ
		const UINT depthPitch = rowPitch * Window::GetWindowHeight();
		//イメージ
		UINT* img = new UINT[pixelCount];
		for(UINT j = 0; j < pixelCount; j++)	{img[j] = 0xff0000ff;}

		//転送
		result = texBuff_[i]->WriteToSubresource(
			0,
			nullptr,
			img,
			rowPitch,
			depthPitch
		);
		assert(SUCCEEDED(result));
		delete[] img;
	}
}

void RenderTexture::RTVInitialize()
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 2;
	//生成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(
		&rtvDescHeapDesc,
		IID_PPV_ARGS(&descHeapRTV)
	);
	assert(SUCCEEDED(result));

	/*//設定
	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	//シェーダーの計算結果をSRGBに変換と書き込み
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;*/

	//作成
	for(int i = 0; i < 2; i++){
		dxCommon->GetDevice()->CreateRenderTargetView(
			texBuff_[i].Get(),
			nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV->GetCPUDescriptorHandleForHeapStart(),i,
				dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			)
		);
	}
}

void RenderTexture::DepthInitialize()
{
	HRESULT result;
	//設定
	CD3DX12_RESOURCE_DESC depthResDesc = 
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			Window::GetWindowWidth(),
			Window::GetWindowHeight(),
			1,0,
			1,0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

	//生成
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT,1.0f, 0),
		IID_PPV_ARGS(&depthBuff)
	);
	assert(SUCCEEDED(result));
}

void RenderTexture::DSVInitialize()
{
	HRESULT result;
	//設定
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;

	//作成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(
		&DescHeapDesc,
		IID_PPV_ARGS(&descHeapDSV)
	);
	assert(SUCCEEDED(result));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dxCommon->GetDevice()->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart()
	);
}

void RenderTexture::PreDraw()
{
	//リソースバリア
	for(int i = 0; i < 2; i++){
		dxCommon->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				texBuff_[i].Get(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			)
		);
	}

	//レンダーターゲットビュー
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH[2];
	for(int i = 0; i < 2; i++){
		rtvH[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
			dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
	}
	
	//深度ステンシル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();

	//レンダーターゲットをセット
	dxCommon->GetCommandList()->OMSetRenderTargets(2,rtvH,false,&dsvH);

	CD3DX12_VIEWPORT viewports[2];
	CD3DX12_RECT scissorRects[2];
	for(int i = 0; i < 2; i++){
		viewports[i] = CD3DX12_VIEWPORT(0.0F,0.0F,(FLOAT)Window::GetWindowWidth(),(FLOAT)Window::GetWindowHeight());
		scissorRects[i] = CD3DX12_RECT(0,0,Window::GetWindowWidth(),Window::GetWindowHeight());
	}
	//ビューポート
	dxCommon->GetCommandList()->RSSetViewports(2,viewports);
	//シザー
	dxCommon->GetCommandList()->RSSetScissorRects(2,scissorRects);

	//画面クリア
	for(int i = 0; i < 2; i++){
		dxCommon->GetCommandList()->ClearRenderTargetView(rtvH[i],clearColor,0,nullptr);
	}

	//深度バッファクリア
	dxCommon->GetCommandList()->ClearDepthStencilView(dsvH,D3D12_CLEAR_FLAG_DEPTH,1.0f,0,0,nullptr);
}

void RenderTexture::PostDraw()
{
	//リソースバリア
	for(int i = 0; i < 2; i++){
		dxCommon->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				texBuff_[i].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			)
		);
	}
}
