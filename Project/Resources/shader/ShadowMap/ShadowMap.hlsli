cbuffer ModelCb : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};

// 頂点シェーダーへの入力
struct SVSIn
{
    float4 pos : POSITION; // モデルの頂点座標
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION; // スクリーン空間でのピクセルの座標
};
