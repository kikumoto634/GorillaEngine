#include "Particle.hlsli"

VSOutput main( float4 pos : POSITION, float scale : TEXCOORD)
{
	VSOutput output;	//ピクセルシェーダーに渡す値
	output.svpos = pos;
	output.scale = scale;
	return output;
}