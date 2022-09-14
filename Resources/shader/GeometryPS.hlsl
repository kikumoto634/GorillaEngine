#include "Geometry.hlsli"

Texture2D<float4> tex : register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);		//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float3 light = normalize(float3(1,-1,1));	//右下奥向きのライト
	float diffuse = saturate(dot(-light,input.normal));	//diffuseを[0,1]の範囲にClampする
	float brightness = diffuse + 0.3f;	//アンビエント光を0.3として計算
	float4 texcolor = float4(tex.Sample(smp,input.uv));

	//テクスチャレンダリング
	return float4(texcolor.rgb * brightness, texcolor.a)*color;

	//テクスチャマッピング
	//return float4(brightness, brightness, brightness, 1);

	//色指定
	//return color;
}