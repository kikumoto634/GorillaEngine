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

struct PSOutput
{
    float4 target0 : SV_Target0;
    float4 target1 : SV_Target1;
};