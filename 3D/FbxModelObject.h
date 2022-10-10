#pragma once

#include <Window.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

#include "FbxModelManager.h"
#include "Camera.h"
#include "Vector2.h"
#include "Vector3.h"

class FbxModelObject{
protected:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	struct ConstBufferDataTransform{
		XMMATRIX viewproj;
		XMMATRIX world;
		Vector3 cameraPos;
	};

private:
	static ID3D12Device* device;
	static Camera* camera;

	static ComPtr<ID3D12RootSignature> rootSignature;
	static ComPtr<ID3D12PipelineState> pipelineState;

public:
	static void SetDevice(ID3D12Device* device)	{FbxModelObject::device = device;}
	static void SetCamera(Camera* camera)	{FbxModelObject::camera = camera;}
	static void CreateGraphicsPipeline();

public:
	void Initialize();
	void Update();
	void Draw(ID3D12GraphicsCommandList* commandList);

	void SetModel(FbxModelManager* model)	{this->model = model;}

protected:
	ComPtr<ID3D12Resource> constBufferTransform;

	Vector3 scale = {1,1,1};
	Vector3 rotation = {0,0,0};
	Vector3 position = {0,0,0};
	XMMATRIX matWorld;
	FbxModelManager* model = nullptr;
};