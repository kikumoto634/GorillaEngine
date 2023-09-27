#include "GPUParticle.hlsli"

VSOutput main(float4 position : POSITION)
{
    VSOutput result;

    result.svpos = mul(mat, position + offset);
    result.scale = 1.f;

    return result;
}