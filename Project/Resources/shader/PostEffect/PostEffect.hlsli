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

//���_�V�F�[�_�[����̏o�͍\����
struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};