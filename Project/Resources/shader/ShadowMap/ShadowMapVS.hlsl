#include "ShadowMap.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output;
    //output.svpos = mul(lightCamera, pos);
    output.svpos = mul(proj, mul(view, pos));
    output.uv = uv;
    return output;
}