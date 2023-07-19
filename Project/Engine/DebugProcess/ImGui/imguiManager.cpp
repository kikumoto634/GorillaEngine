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

void imguiManager::Begin()
{
	//ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void imguiManager::End()
{
	//描画前準備
	ImGui::Render();
}

void imguiManager::Draw()
{
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();
	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = {srvHeap.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンド
	 ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void imguiManager::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

Vector4 imguiManager::ImGuiDragVector4(const char *name, Vector4 value, float speed, float min, float max)
{
	float tempPos[4] = {value.x,value.y,value.z,value.w};
	ImGui::DragFloat4(name, tempPos, speed, min, max);
	return Vector4({tempPos[0],tempPos[1],tempPos[2],tempPos[3]});
}

Vector3 imguiManager::ImGuiDragVector3(const char* name, Vector3 value, float speed, float min, float max)
{
	float tempPos[3] = {value.x,value.y,value.z};
	ImGui::DragFloat3(name, tempPos, speed, min, max);
	return Vector3({tempPos[0],tempPos[1],tempPos[2]});
}

Vector2 imguiManager::ImGuiDragVector2(const char *name, Vector2 value, float speed, float min, float max)
{
	float tempPos[2] = {value.x,value.y};
	ImGui::DragFloat2(name, tempPos, speed, min, max);
	return Vector2({tempPos[0],tempPos[1]});
}

float imguiManager::ImGuiDragFloat(const char *name, float* value, float speed, float min, float max)
{
	float *tempPos = value;
	ImGui::DragFloat(name, tempPos, speed, min, max);
	return *tempPos;
}

int imguiManager::ImGuiDrawInt(const char *name, int *value, float speed, int min, int max)
{
	int *tempPos = value;
	ImGui::DragInt(name, tempPos, speed, min, max);
	return *tempPos;
}
