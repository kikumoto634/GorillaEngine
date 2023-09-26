cbuffer SceneConstantBuffer : register(b0)
{
    float4 velocity;
    float4 offset;
    float4 color;
    float4x4 projection;
    matrix matBillboard;
    matrix mat;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct GSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};