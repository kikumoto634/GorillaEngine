#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

//通常
float4 Normal(VSOutput input)
{
    float4 texcolor = tex.Sample(smp, input.uv);

	return float4(texcolor.rgb, 1.0f);
}

//平均ぼかし
float4 AverageBlur(VSOutput input, int blurValue)
{
    float2 texelSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);
    float4 color = (0, 0, 0, 0);

    for (int x = 0; x < blurValue; x++)
    {
        for (int y = 0; y < blurValue; y++)
        {
            color += tex.Sample(smp, input.uv + float2(x, y) * texelSize);
        }
    }
    color /= (blurValue * blurValue); // 周囲9ピクセルの平均値を計算

    return float4(color.rgb, 1);
}

//ネガポジ反転
float4 Negative(VSOutput input)
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float3 nega = float3(1 - texcolor.x, 1 - texcolor.y, 1 - texcolor.z);
    return float4(nega, 1.0f);
}

float4 main(VSOutput input) : SV_TARGET
{
	//return Normal(input);
    return AverageBlur(input, 16);
    //return Negative(input);
}