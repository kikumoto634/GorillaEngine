cbuffer cbuff0 : register(b0)
{
	matrix viewproj;	//ビュープロジェクション行列
	matrix world;		//ワールド行列
	float3 cameraPos;	//カメラ座標(ワールド座標)
};

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos : POSITION;	//位置
	float3 normal : NORMAL;	//頂点法線
	float2 uv  : TEXCOORD;	//テクスチャ―座標
};

//頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION;	//システム用頂点座標
	float3 normal : NORMAL;		//法線
	float2 uv : TEXCOORD;		//uv値
};