#include "GPUParticle.hlsli"

PSInput main(float4 position : POSITION)
{
    PSInput result;

    result.position = mul(mul(mat, projection), position + offset);

    float intensity = saturate((4.0f - result.position.z) / 2.0f);
    result.color = float4(color.xyz * intensity, 1.0f);

    return result;
}