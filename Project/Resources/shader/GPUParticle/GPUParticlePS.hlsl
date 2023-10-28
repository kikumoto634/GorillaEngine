#include "GPUParticle.hlsli"

PSOutput main(VSOutput output) : SV_TARGET
{    
    PSOutput ps;
    
    Material input;
    input.color = color;
    
    ps.target0 = input.color;
    ps.target1 = float4(1 - (input.color).xyz, 1);
    
    return ps;
}