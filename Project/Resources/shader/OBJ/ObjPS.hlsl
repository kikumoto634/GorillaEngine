//#include "Obj.hlsli"
#include "ObjLight.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    float4 light = Light(input, texcolor);
        
    //return float4(light.xyz, 1);
    return float4(texcolor.xyz, 1);
}