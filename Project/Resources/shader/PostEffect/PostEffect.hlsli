cbuffer cbuff0 : register(b0)
{
	bool isBlurActive;
	int BlurCount;
	float BlurStrength;
};

cbuffer cbuff1 : register(b1)
{
	bool isFadeActive;
	float3 fadeColor;
};

//頂点シェーダーからの出力構造体
struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float Gaussian(float2 drawUV, float2 pickUV, float sigma){
    float d = distance(drawUV, pickUV);
    return exp(-(d*d) / (2*sigma * sigma));
}