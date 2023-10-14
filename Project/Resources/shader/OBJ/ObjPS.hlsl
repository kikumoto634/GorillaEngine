//#include "Obj.hlsli"
#include "ObjLight.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

PSOutput main(VSOutput input)
{
    PSOutput output;
    
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    float4 light = Light(input, texcolor);
    
    output.target0 = light;
    output.target1 = light;
    
    return output;
}