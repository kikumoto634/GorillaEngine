#include "Obj.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//VSOutput output; // ピクセルシェーダーに渡す値
	//output.svpos = mul(mat, pos);
	//output.normal = normal;
	//output.uv = uv;
	//return output;
	float3 lightdir = float3(1,-1,1);
	lightdir = normalize(lightdir);

	float3 lightcolor = float3(1,1,1);

	VSOutput output;
	output.svpos = mul(mat,pos);

	float3 ambient = m_ambient;
	float3 diffuse = dot(-lightdir,normal) * m_diffuse;
	const float3 eye = float3(0,0,-10);
	const float shininess = 4.0f;
	float3 eyedir = normalize(eye - pos.xyz);
	float3 reffect = normalize(lightdir + 2 * dot(-lightdir, normal)*normal);
	float3 specular = pow(saturate(dot(reffect, eyedir)),shininess)*m_specular;

	output.color.rgb = (ambient + diffuse + specular)*lightcolor;
	output.color.a = m_alpha;
	output.uv = uv;
	return output;
}