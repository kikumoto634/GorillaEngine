#include "Particle.hlsli"

VSOutput main( float4 pos : POSITION)
{
	VSOutput output;	//ピクセルシェーダーに渡す値
	output.svpos = pos;
	return output;
}