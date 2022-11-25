//3D変換行列
cbuffer ConstBufferData : register(b0)
{
	matrix mat;	//3D変換行列
	matrix matBillboard;	//ビルボード行列
};

//頂点シェーダーの出力構造体
//(頂点シェーダーからピクセルシェーダーへのより取りに使用する)
struct VSOutput
{
	//システム用頂点座標
	float4 svpos : SV_POSITION;
	float scale : TEXCOORD;
};

//ジオメトリシェーダーからピクセルシェーダーへの出力
struct GSOutput
{
	//システム用頂点座標
	float4 svpos : SV_POSITION;
	//uv値
	float2 uv : TEXCOORD;
};