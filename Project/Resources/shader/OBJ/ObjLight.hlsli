#include "Obj.hlsli"


float4 Light(VSOutput input, float4 texcolor)
{
	////光沢度
    const float shiness = 4.0f;
	////頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	// 環境反射光
    float3 ambient = m_ambient;
	// シェーディングによる色
    float4 shadecolor = float4(ambientColor * ambient, m_alpha);

    if (lightLayer)
        return (shadecolor * texcolor);
	
	// 平行光源
    if (dirLights.active)
    {
		// ライトに向かうベクトルと法線の内積
        float3 dotlightnormal = dot(dirLights.lightv, input.normal);
		// 反射光ベクトル
        float3 reflect = normalize(-dirLights.lightv + 2 * dotlightnormal * input.normal);
		// 拡散反射光
        float3 diffuse = dotlightnormal * m_diffuse;
        //diffuse /= 3.1415926f;
		// 鏡面反射光
        float3 specular = pow(saturate(dot(reflect, eyedir)), shiness) * m_specular;
		// 全て加算する
        shadecolor.rgb += (diffuse + specular) * dirLights.lightcolor;
    }

	// 点光源
    for (int i = 0; i < POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
			// ライトへのベクトル
            float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			// ベクトルの長さ
            float d = length(lightv);
			// 正規化し、単位ベクトルにする
            lightv = normalize(lightv);
			// 距離減衰係数
            float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);
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

	//スポットライト
    for (i = 0; i < SPOTLIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
			//ライトへの方向ベクトル
            float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
			//距離減衰係数
            float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d + spotLights[i].lightatten.z * d * d));
			//角度減衰
            float cos = dot(lightv, spotLights[i].lightv);
			//減衰開始角度から、減衰終了角度にかけて減衰
			//減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
			//角度減衰を乗算
            atten *= angleatten;
			//ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal);
			//反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			//拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
            //diffuse /= 3.1415926f;
			//鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shiness) * m_specular;
			//すべて加算
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
        }
    }

	//丸影
    for (i = 0; i < CIRCLESHADOW_NUM; i++)
    {
        if (circleShadows[i].active)
        {
			//オブジェクト表面からキャスターへのベクトル
            float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			//投影方向での距離
            float d = dot(casterv, circleShadows[i].dir);
			//距離減衰係数
            float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			//距離がminusなら0にする
            atten *= step(0, d);
			//仮想ライトの座標
            float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			//オブジェクト表面からライトへのベクトル(単位ベクトル)
            float3 lightv = normalize(lightpos - input.worldpos.xyz);
			//角度減衰
            float cos = dot(lightv, circleShadows[i].dir);
			//減衰開始角度から、減衰終了角度にかけて減衰
			//減衰開始角度の内側は1倍 減衰終了角度は0倍の輝度
            float angleatten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);
			//角度減衰を乗算
            atten *= angleatten;
			//すべて減算
            shadecolor.rgb -= atten;
        }
    }

    return (shadecolor * texcolor);
}