cbuffer SceneConstantBuffer : register(b0)
{
    float4 velocity;
    float4 offset;
    float4 color;
    matrix matBillboard;
    matrix mat;
};

struct Material
{
    float4 color : COLOR;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float scale : TEXCOORD;
};

struct GSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};