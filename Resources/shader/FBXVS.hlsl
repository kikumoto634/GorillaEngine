#include "FBX.hlsli"

VSOutput main( VSInput input )
{
	float4 wnormal = normalize(mul(world, float4(input.normal, 0)));
	//ピクセルシェーダーに渡す値
	VSOutput output;
	//行列による座標変換
	output.svpos = mul(shadow, input.pos);
	output.svpos = mul(mul(viewproj, world), output.svpos);
	//ワールド法線を次のステージに渡す
	output.normal = wnormal.xyz;
	//入力値をそのまま次のステージを渡す
	output.uv = input.uv;

	return output;
}
