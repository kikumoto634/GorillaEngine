#include "GPUParticle.hlsli"

float4 main(VSOutput output) : SV_TARGET
{    
    Material input;
    input.color = color;
    
    return input.color;
}