#include "Geometry.hlsli"

//三角形の入力から、点を1つ出力サンプル
[maxvertexcount(6)]
void main(
	triangle VSOutput input[3] : SV_POSITION,
	//点ストリーム
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;	//出力頂点データ
		element.svpos = input[i].svpos;	//頂点座標をコピー
		element.normal = input[i].normal;	//法線をコピー
		element.uv = input[i].uv;	//ＵＶをコピー
		//頂点を1つ出力(出力リストに追加)
		output.Append(element);
	}
	//現在のストリップを終了
	output.RestartStrip();

	// 2つ目の三角形
    for (uint i = 0; i < 3; i++)    {
        GSOutput element;
        // X方向に20ずらす
        element.svpos = input[i].svpos + float4(20.0f, 0.0f, 0.0f, 0.0f);
        element.normal = input[i].normal;
        // UVを5倍に
        element.uv = input[i].uv * 5.0f;
        output.Append(element);
    }

}