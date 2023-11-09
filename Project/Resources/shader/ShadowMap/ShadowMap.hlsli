cbuffer SceneBuffer : register(b0)
{
    matrix view;            //ビュー
    matrix proj;            //プロジェクション
    matrix shadow;          //影行列
    matrix lightCamera;
    float3 eye;             //視点
}

//頂点シェーダーからの出力構造体
struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};