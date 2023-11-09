#include "ShadowMap.hlsli"

Texture2D<float4> depthTex : register(t0);
SamplerState smp : register(s0);


float4 main(VSOutput input) : SV_TARGET
{
    //ê[ìx
    float dep = pow(depthTex.Sample(smp, input.uv), 20);
    return float4(dep, dep, dep, 1);
}