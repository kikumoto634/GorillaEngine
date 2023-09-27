#include "GPUParticle.hlsli"

VSOutput main(float4 position : POSITION)
{
    VSOutput result;

    result.svpos = position + offset;
    result.scale = 0.1f;

    return result;
}