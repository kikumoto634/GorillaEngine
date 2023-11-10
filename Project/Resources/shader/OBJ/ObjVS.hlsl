#include "Obj.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint instNo : SV_InstanceID)
{
	//法線にワールド行列によるスケーリング・回転
	float4 wnormal = normalize(mul(world, float4(normal,0)));
	float4 wpos = mul(world,pos);

	VSOutput output;
    if (instNo == 0)
    {
        output.svpos = mul(mul(viewproj, world),pos);
    }
    else if (instNo == 1)
    {
        output.svpos = mul(mul(viewproj, world), mul(shadow, pos));
    }
	output.worldpos = wpos;
	output.normal = wnormal.xyz;
	output.uv = uv;
    output.instNo = instNo;
	return output;
}