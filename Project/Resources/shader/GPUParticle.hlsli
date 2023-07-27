cbuffer SceneConstantBuffer : register(b0)
{
    float4 velocity;
    float4 offset;
    float4 color;
    float4x4 projection;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};