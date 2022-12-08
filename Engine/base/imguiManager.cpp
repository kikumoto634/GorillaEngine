#include "imguiManager.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <cassert>

void imguiManager::Initialize(Window* winApp, DirectXCommon* dxCommon)
{
	assert(winApp);
	this->winApp = winApp;
	this->dxCommon = dxCommon;

	//ImGuiのコンテキストを生成
	ImGui::CreateContext();
	//ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(this->winApp->GetHwnd());

	// デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープ生成
	HRESULT result = this->dxCommon->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	ImGui_ImplDX12_Init(
    this->dxCommon->GetDevice(),
    static_cast<int>(this->dxCommon->GetBackBuffersCount()),
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap.Get(),
    srvHeap->GetCPUDescriptorHandleForHeapStart(),
    srvHeap->GetGPUDescriptorHandleForHeapStart()
    );

	ImGuiIO& io = ImGui::GetIO();
	// 標準フォントを追加する
	io.Fonts->AddFontDefault();
}
