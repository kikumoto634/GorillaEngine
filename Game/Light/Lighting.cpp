#include "Lighting.h"

#include "../../Engine/3D/ObjModelObject.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG


void Lighting::Initialize()
{
	lightGroup = LightGroup::Create();
    //LightColor
    lightGroup->SetAmbientColor({1,1,1});
    //LightSetting
    ObjModelObject::SetLight(lightGroup);

    lightGroup->SetDirLightActive(0, true);
    lightGroup->SetDirLightActive(1, true);
    lightGroup->SetDirLightActive(2, true);

    lightGroup->SetCircleShadowActive(0, true);
}

void Lighting::Update()
{
    //DirLightPositionSetting
    lightGroup->SetCircleShadowDir(0, DirectX::XMVECTOR({circleShadowDir[0], circleShadowDir[1], circleShadowDir[2], 0}));
    lightGroup->SetCircleShadowAtten(0, Vector3(circleShadowAtten[0], circleShadowAtten[1], circleShadowAtten[2]));
    lightGroup->SetCircleShadowFactorAngle(0, Vector2(circleShadowFactorAngle[0], circleShadowFactorAngle[1]));

    //CirclePositionSetting
    lightGroup->SetCircleShadowCasterPos(0, playerShadow);
    lightGroup->Update();

#ifdef _DEBUG
    {
        ImGui::SetNextWindowPos(ImVec2{0,250});
        ImGui::SetNextWindowSize(ImVec2{500,100});
        ImGui::Begin("CircleShadow");
        ImGui::DragFloat3("circleShadowDir", circleShadowDir, 0.1f);
        ImGui::DragFloat3("circleShadowAtten", circleShadowAtten, 0.1f);
        ImGui::DragFloat2("circleShadowFactorAngle", circleShadowFactorAngle, 0.1f);
        ImGui::End();
    }
#endif // _DEBUG
}

void Lighting::Finalize()
{
    delete lightGroup;
    lightGroup = nullptr;
}

void Lighting::SetPlayerShadow(Vector3 pos)
{
    playerShadow = pos;
}
