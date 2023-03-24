#pragma once
#include "../Engine/light/LightGroup.h"

class Lighting
{
public:
    void Initialize();
    void Update();
    void Finalize();

    void SetPlayerShadow(Vector3 pos);

private:
	//Light
    LightGroup* lightGroup = nullptr;

    //Shadow
    float circleShadowDir[3] = { 0,-1,0 };
    float circleShadowAtten[3] = { 0.5f,0.6f,0.0f };
    float circleShadowFactorAngle[2] = { 0.0f, 0.5f };

    Vector3 playerShadow = {};
    bool IsShadowSet = false;
};

