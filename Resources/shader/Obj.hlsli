cbuffer cbuff0 : register(b0)
{
	matrix mat; // ３Ｄ変換行列
};

cbuffer cbuff1 : register(b1)
{
	float3 m_ambient : packoffset(c0);	//アンビエント係数
	float3 m_diffuse : packoffset(c1);	//ディフューズ係数
	float3 m_specular : packoffset(c2);	//スペキュラ係数
	float m_alpha : packoffset(c2.w);
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	//float3 normal :NORMAL; // 法線ベクトル
	float4 color : COLOR;
	float2 uv  :TEXCOORD; // uv値
};
