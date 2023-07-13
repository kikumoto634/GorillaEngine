#include "PostEffect.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = tex.Sample(smp, input.uv);

	//フェード
	if(isFadeActive){
		return float4(texcolor.rgb*fadeColor,1);
	}
	//ぼかし
	else if(isBlurActive){
		float4 col = {0,0,0,0};
		float2 symmetryUV = input.uv - 0.5f;
		float distance = length(symmetryUV);
		float factor = BlurStrength / BlurCount * distance;
		for(int j = 0; j < BlurCount; j++){
			float uvOffset = 1 - factor * j;
			col += tex.Sample(smp, symmetryUV * uvOffset + 0.5);
		}

		col /= 10;
		return col;
	}

	return float4(texcolor.rgb,1);
}