#include "Obj.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = tex.Sample(smp, input.uv);

	////シェーディングによる色
	float4 shadecolor;
	////光沢度
	const float shiness = 4.0f;
	////頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	////ライトに向かうベクトルと法線の内積
	//float3 dotlightnormal = dot(lightv, input.normal);
	////反射光ベクトル
	//float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	////環境反射光
	//float3 ambient = m_ambient;
	////拡散反射光
	//float3 diffuse = dotlightnormal * m_diffuse;
	////鏡面反射光
	//float3 specular = pow(saturate(dot(reflect, eyedir)), shiness) * m_specular;
	////すべて加算
	//shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;

	// 点光源
	for (int i = 0; i < POINTLIGHT_NUM; i++) {
		if (pointLights[i].active) {
			// ライトへのベクトル
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			// ベクトルの長さ
			float d = length(lightv);
			// 正規化し、単位ベクトルにする
			lightv = normalize(lightv);
			// 距離減衰係数
			float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z *d*d);
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shiness) * m_specular;
			// 全て加算する
			shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
		}
	}

	shadecolor.a = m_alpha;

	return shadecolor * texcolor;
}