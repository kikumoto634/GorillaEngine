#include "Particle.hlsli"

//四角形の超点数
static const uint vnum = 4;

//センターからオフセット
static const float4 offset_array[vnum] = 
{
	float4(-0.5f, -0.5f, 0, 0),//左下
	float4(-0.5f, +0.5f, 0, 0),//左上
	float4(+0.5f, -0.5f, 0, 0),//右下
	float4(+0.5f, +0.5f, 0, 0),//右上
};
static const float2 uv_array[vnum] = 
{
	float2(0, 1),//左下
	float2(0, 0),//左上
	float2(1, 1),//右下
	float2(1, 0),//右上
};

//三角形の入力から、点を1つ出力サンプル
[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION,
	//点ストリーム
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;
	//4頂点分
	for(uint i = 0; i < vnum; i++){
		//中心からオフセットをスケーリング
		float4 offset = offset_array[i] * input[0].scale;
		//中心からオフセットをビルボード回転
		offset = mul(matBillboard, offset);
		//オフセット分ずらす
		element.svpos = input[0].svpos + offset;
		//ビュー、射影変換
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
		output.Append(element);
	}
}