#include "LightGroup.h"
#include <cassert>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG


using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* LightGroup::device = nullptr;

void LightGroup::StaticInitialize(ID3D12Device *device)
{
	// 再初期化チェック
	assert(!LightGroup::device);

	// nullptrチェック
	assert(device);

	LightGroup::device = device;
}

LightGroup *LightGroup::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	LightGroup* instance = new LightGroup();

	// 初期化
	instance->Initialize();

	return instance;
}

void LightGroup::Initialize()
{
	// nullptrチェック
	assert(device);

	DefaultLightSetting();

	HRESULT result;
	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)){
		assert(0);
	}

	// 定数バッファへデータ転送
	TransferConstBuffer();
}

void LightGroup::Update()
{
	// 値の更新があった時だけ定数バッファに転送する
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void LightGroup::DebugUpdate()
{
	//座標
	ImGui::SetNextWindowPos(ImVec2{980,0});
	//サイズ
	ImGui::SetNextWindowSize(ImVec2{300,700});
	ImGui::Begin("Lights");
	

	ImGui::Text("Ambient");
	{
		static float col[3] = {1.0f,1.0f,1.0f};
		ImGui::ColorEdit3("col", col);
		SetAmbientColor({col[0],col[1],col[2]});
	}

	ImGui::Text("Directnal");
	{
		static bool isActive = true;
		static float col[3] = {1.0f,1.0f,1.0f};
		static float dir[3] = {0.0f,-1.0f,0.0f};

		ImGui::BeginChild(ImGui::GetID((void**)0), ImVec2(275,82), ImGuiWindowFlags_NoTitleBar);
		ImGui::Checkbox("act 1", &isActive);
		ImGui::ColorEdit3("color 1", col);
		ImGui::DragFloat3("dir 1", dir, 0.01f,-10.f,10.f);
		ImGui::EndChild();
	
		SetDirLightActive(isActive);
		SetDirLightColor({col[0],col[1],col[2]});
		SetDirLightDir({dir[0],dir[1],dir[2]});
	}

	ImGui::Text("Point [1 ~ 3]");
	{
		static bool isActive[3] = {false,false,false};
		static float col[3][3] = {{1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}};
		static float pos[3][3] = {{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}};
		static float att[3][3] = {{1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}};

		ImGui::BeginChild(ImGui::GetID((void**)1), ImVec2(275,105), ImGuiWindowFlags_NoTitleBar);
		ImGui::Checkbox("act 1", &isActive[0]);
		ImGui::ColorEdit3("color 1", col[0]);
		ImGui::DragFloat3("pos 1", pos[0], 0.5f,-100.f,100.f);
		ImGui::DragFloat3("att 1", att[0], 0.1f,0.f,3.f);
	
		ImGui::Checkbox("act 2", &isActive[1]);
		ImGui::ColorEdit3("color 2", col[1]);
		ImGui::DragFloat3("pos 2", pos[1], 0.5f,-100.f,100.f);
		ImGui::DragFloat3("att 2", att[1], 0.01f,0.f,3.f);

		ImGui::Checkbox("act 3", &isActive[2]);
		ImGui::ColorEdit3("color 3", col[2]);
		ImGui::DragFloat3("pos 3", pos[2], 0.5f,-100.f,100.f);
		ImGui::DragFloat3("att 3", att[2], 0.01f,0.f,3.f);
		ImGui::EndChild();
	
		for(int i = 0; i < 3; i++){
			SetPointLightActive(i, isActive[i]);
			SetPointLightColor(i,{col[i][0],col[i][1],col[i][2]});
			SetPointLightPos(i, {pos[i][0],pos[i][1],pos[i][2]});
			SetPointLightAtten(i,{att[i][0],att[i][1],att[i][2]});
		}
	}

	ImGui::Text("Spot [1 ~ 3]");
	{
		static bool isActive[3] = {false,false,false};
		static float col[3][3] = {{1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}};
		static float pos[3][3] = {{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}};
		static float dir[3][3] = {{1.0f,-1.0f,1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,-1.0f,0.0f}};
		static float att[3][3] = {{1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f}};
		static float ang[3][2] = {{0.5f,0.2f},{0.5f,0.2f},{0.5f,0.2f}};

		ImGui::BeginChild(ImGui::GetID((void**)2), ImVec2(275,150), ImGuiWindowFlags_NoTitleBar);
		ImGui::Checkbox("act 1", &isActive[0]);
		ImGui::ColorEdit3("color 1", col[0]);
		ImGui::DragFloat3("pos 1", pos[0], 0.5f,-100.f,100.f);
		ImGui::DragFloat3("dir 1", dir[0], 1.f,-360.f,360.f);
		ImGui::DragFloat3("att 1", att[0], 0.01f,0.f,3.f);
		ImGui::DragFloat2("ang 1", ang[0], 0.1f,0.f,180.f);
	
		ImGui::Checkbox("act 2", &isActive[1]);
		ImGui::ColorEdit3("color 2", col[1]);
		ImGui::DragFloat3("pos 2", pos[1], 0.5f,-100.f,100.f);
		ImGui::DragFloat3("dir 2", dir[1], 1.f,-360.f,360.f);
		ImGui::DragFloat3("att 2", att[1], 0.01f,0.f,3.f);
		ImGui::DragFloat2("ang 2", ang[1], 0.1f,0.f,180.f);

		ImGui::Checkbox("act 3", &isActive[2]);
		ImGui::ColorEdit3("color 3", col[2]);
		ImGui::DragFloat3("pos 3", pos[2], 0.5f,-100.f,100.f);
		ImGui::DragFloat3("dir 3", dir[2], 1.f,-360.f,360.f);
		ImGui::DragFloat3("att 3", att[2], 0.01f,0.f,3.f);
		ImGui::DragFloat2("ang 3", ang[2], 0.1f,0.f,180.f);
		ImGui::EndChild();
	
		for(int i = 0; i < 3; i++){
			SetSpotLightActive(i, isActive[i]);
			SetSpotLightColor(i,{col[i][0],col[i][1],col[i][2]});
			SetSpotLightPos(i, {pos[i][0],pos[i][1],pos[i][2]});
			SetSpotLightDir(i, {dir[i][0],dir[i][1],dir[i][2]});
			SetSpotLightAtten(i,{att[i][0],att[i][1],att[i][2]});
			SetSpotLightFactorAngleCos(i,{ang[i][0],ang[i][1]});
		}
	}

	ImGui::Text("CircleShadow");
	{
		ImGui::BeginChild(ImGui::GetID((void**)3), ImVec2(275,150), ImGuiWindowFlags_NoTitleBar);
		for(int i = 0; i < CircleShadowNum; i++){
			if(!circleShadows[i].GetActive()) continue;
			ImGui::DragFloat3("dir "+i, circleShadowDir[i], 0.01f,-10.f,10.f);
			ImGui::DragFloat3("att "+i, circleShadowAtten[i], 0.01f,-1.f,1.f);
			ImGui::DragFloat2("ang "+i, circleShadowAngle[i], 0.01f,-10.f,10.f);

			SetCircleShadowDir(i, {circleShadowDir[i][0],circleShadowDir[i][1],circleShadowDir[i][2]});
			SetCircleShadowAtten(i, {circleShadowAtten[i][0],circleShadowAtten[i][1],circleShadowAtten[i][2]});
			SetCircleShadowFactorAngle(i, {circleShadowAngle[i][0],circleShadowAngle[i][1]});
			SetCircleShadowCasterPos(i, {circleShadowPos[i][0],circleShadowPos[i][1],circleShadowPos[i][2]});
		}
		ImGui::EndChild();
	}

	ImGui::End();
}

void LightGroup::Draw(ID3D12GraphicsCommandList *cmdList, UINT rootParameterIndex)
{
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff->GetGPUVirtualAddress());
}


void LightGroup::TransferConstBuffer()
{
	HRESULT result;
	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		// 環境光
		constMap->ambientColor = ambientColor;
		// 平行光源
		// ライトが有効なら設定を転送
		if (dirLights.GetIsActive()) {
			constMap->dirLights.active = 1;
			constMap->dirLights.lightv = -dirLights.GetLightDir();
			constMap->dirLights.lightcolor = dirLights.GetLightColor();
		}
		// ライトが無効ならライト色を0に
		else {
			constMap->dirLights.active = 0;
		}
		// 点光源
		for (int i = 0; i < PointLightNum; i++) {
			// ライトが有効なら設定を転送
			if (pointLights[i].GetIsActive()) {
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor = pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights[i].GetLightAten();
			}
			// ライトが無効ならライト色を0に
			else {
				constMap->pointLights[i].active = 0;
			}
		}
		//スポットライト
		for(int i = 0; i < SpotLightNum; i++){
			//ライトが有効なら設定を転送
			if(spotLights[i].GetActive()){
				constMap->spotLights[i].active = 1;
				constMap->spotLights[i].lightv = -spotLights[i].GetLightDir();
				constMap->spotLights[i].lightpos = spotLights[i].GetLightPos();
				constMap->spotLights[i].lightcolor = spotLights[i].GetLightColor();
				constMap->spotLights[i].lightatten = spotLights[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos = spotLights[i].GetLightFactorAngleCos();
			}
			else{
				constMap->spotLights[i].active = 0;
			}
		}
		//丸影
		for(int i = 0; i < CircleShadowNum; i++){
			//有効なら設定を転送
			if(circleShadows[i].GetActive()){
				constMap->circleShadows[i].active = 1;
				constMap->circleShadows[i].dir = -circleShadows[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight = circleShadows[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten = circleShadows[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos = circleShadows[i].GetFactorAngleCos();
			}
			else{
				constMap->circleShadows[i].active = 0;
			}
		}
		constBuff->Unmap(0, nullptr);
	}
}

void LightGroup::DefaultLightSetting()
{
	dirLights.SetIsActive(true);
	dirLights.SetLightColor({0.0f, 1.0f, 1.0f});
	dirLights.SetLightDir({0.0f, -1.0f, 0.0f});
}

void LightGroup::SetAmbientColor(const Vector3 &color)
{
	ambientColor = color;
	dirty = true;
}

void LightGroup::SetDirLightActive(bool active)
{
	dirLights.SetIsActive(active);
}

void LightGroup::SetDirLightDir(const Vector3 &lightdir)
{
	dirLights.SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetDirLightColor(const Vector3 &lightcolor)
{
	dirLights.SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetIsActive(active);
}

void LightGroup::SetPointLightPos(int index, const Vector3 &lightpos)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightPos(lightpos);
	dirty = true;
}

void LightGroup::SetPointLightColor(int index, const Vector3 &lightcolor)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::SetPointLightAtten(int index, const Vector3 &lightAtten)
{
	assert(0 <= index && index < PointLightNum);

	pointLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

void LightGroup::SetSpotLightActive(int index, bool active)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetActive(active);
}

void LightGroup::SetSpotLightDir(int index, const DirectX::XMVECTOR &lightdir)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetSpotLightPos(int index, const Vector3 &lightpos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightPos(lightpos);
	dirty = true;
}

void LightGroup::SetSpotLightColor(int index, const Vector3 &lightcolor)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::SetSpotLightAtten(int index, const Vector3 &lightAtten)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightAtten(lightAtten);
	dirty = true;
}

void LightGroup::SetSpotLightFactorAngleCos(int index, const Vector2 &lightFactorAngleCos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights[index].SetLightFactorAngleCos(lightFactorAngleCos);
	dirty = true;
}

void LightGroup::CircleShadowSet(int index, Vector3 pos, Vector3 dir, Vector3 atten, Vector2 angle)
{
	assert(0 <= index && index < CircleShadowNum);

	SetCircleShadowActive(index,true);
	circleShadowDir[index][0] = dir.x;
	circleShadowDir[index][1] = dir.y;
	circleShadowDir[index][2] = dir.z;
	SetCircleShadowDir(index, {dir.x,dir.y,dir.z});
	circleShadowDir[index][0] = dir.x;
	circleShadowDir[index][1] = dir.y;
	circleShadowDir[index][2] = dir.z;
	SetCircleShadowAtten(index, atten);
	circleShadowAtten[index][0] = atten.x;
	circleShadowAtten[index][1] = atten.y;
	circleShadowAtten[index][2] = atten.z;
	SetCircleShadowFactorAngle(index, angle);
	circleShadowAngle[index][0] = angle.x;
	circleShadowAngle[index][1] = angle.y;
	SetCircleShadowCasterPos(index, pos);
	circleShadowPos[index][0] = pos.x;
	circleShadowPos[index][1] = pos.y;
	circleShadowPos[index][2] = pos.z;
}

void LightGroup::SetCircleShadowActive(int index, bool active)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetActive(active);
}

void LightGroup::SetCircleShadowCasterPos(int index, const Vector3 &casterPos)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetCasterPos(casterPos);
	dirty = true;
}

void LightGroup::SetCircleShadowDir(int index, const DirectX::XMVECTOR &lightdir)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetDir(lightdir);
	dirty = true;
}

void LightGroup::SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetDistanceCasterLight(distanceCasterLight);
	dirty = true;
}

void LightGroup::SetCircleShadowAtten(int index, const Vector3 &lightAtten)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetAtten(lightAtten);
	dirty = true;
}

void LightGroup::SetCircleShadowFactorAngle(int index, const Vector2 &lightFactorAngle)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows[index].SetFactorAngleCos(lightFactorAngle);
	dirty = true;
}
