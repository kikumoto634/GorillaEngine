#include "GeometryManager.h"

using namespace std;

void GeometryManager::Initialize()
{
	HRESULT result;
	dxCommon = DirectXCommon::GetInstance();

#pragma region 四角形データ
	//頂点データ
	VertexPosNormal vertices[24] = 
	{
		//前
		{{-5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}},
		{{-5.0f, +5.0f, -5.0f}, {}, {0.0f, 0.0f}},
		{{+5.0f, -5.0f, -5.0f}, {}, {1.0f, 1.0f}},
		{{+5.0f, +5.0f, -5.0f}, {}, {1.0f, 0.0f}},
		//後
		{{-5.0f, -5.0f, +5.0f}, {}, {0.0f, 1.0f}},
		{{-5.0f, +5.0f, +5.0f}, {}, {0.0f, 0.0f}},
		{{+5.0f, -5.0f, +5.0f}, {}, {1.0f, 1.0f}},
		{{+5.0f, +5.0f, +5.0f}, {}, {1.0f, 0.0f}},
		//左
		{{-5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}},
		{{-5.0f, -5.0f, +5.0f}, {}, {0.0f, 0.0f}},
		{{-5.0f, +5.0f, -5.0f}, {}, {1.0f, 1.0f}},
		{{-5.0f, +5.0f, +5.0f}, {}, {1.0f, 0.0f}},
		//右
		{{+5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}},
		{{+5.0f, -5.0f, +5.0f}, {}, {0.0f, 0.0f}},
		{{+5.0f, +5.0f, -5.0f}, {}, {1.0f, 1.0f}},
		{{+5.0f, +5.0f, +5.0f}, {}, {1.0f, 0.0f}},
		//下
		{{-5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}},
		{{+5.0f, -5.0f, -5.0f}, {}, {0.0f, 0.0f}},
		{{-5.0f, -5.0f, +5.0f}, {}, {1.0f, 1.0f}},
		{{+5.0f, -5.0f, +5.0f}, {}, {1.0f, 0.0f}},
		//上
		{{-5.0f, +5.0f, -5.0f}, {}, {0.0f, 1.0f}},
		{{+5.0f, +5.0f, -5.0f}, {}, {0.0f, 0.0f}},
		{{-5.0f, +5.0f, +5.0f}, {}, {1.0f, 1.0f}},
		{{+5.0f, +5.0f, +5.0f}, {}, {1.0f, 0.0f}},
	};
	for(int i = 0; i < _countof(vertices); i++)
	{
		this->vertices[i] = vertices[i];
	}

	///インデックスデータ
	unsigned short indices[] = 
	{
		//前
		0, 1, 2,
		2, 1, 3,
		//後
		5, 4, 6,
		5, 6, 7,
		//左
		8, 9, 10,
		10, 9, 11,
		//右
		13, 12, 14,
		13, 14, 15,
		//下
		16, 17, 18,
		18, 17, 19,
		//上
		21, 20, 22,
		21, 22, 23,
	};
	for(int i = 0; i < _countof(indices); i++)
	{
		this->indices[i] = indices[i];
	}

	///法線計算
	for(int i = 0; i < _countof(indices)/3; i++){
		//三角形一つごとに計算していく
		//三角形にインデックスを取り出して、一時的な変数を入れる
		unsigned short index0 = indices[i*3+0];
		unsigned short index1 = indices[i*3+1];
		unsigned short index2 = indices[i*3+2];
		//p0->p1ベクトル、p0->p2ベクトルの計算	(ベクトル減算)
		Vector3 v1 = vertices[index0].pos - vertices[index1].pos;
		Vector3 v2 = vertices[index0].pos - vertices[index2].pos;
		//外積は両方から垂直なベクトル
		Vector3 normal = v1.cross(v2);
		//正規化(長さを1にする)
		normal = normal.normalize();
		//求めた法線を頂点データに代入
		vertices[index0].normal = normal;
		vertices[index1].normal = normal;
		vertices[index2].normal = normal;
	}
#pragma endregion

	//頂点バッファ
	//頂点サイズ
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormal) * _countof(vertices));

	//頂点バッファ
	{
		//ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
		//生成
		result = dxCommon->GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertBuffer)
		);
		assert(SUCCEEDED(result));

		//転送
		VertexPosNormal* vertMap = nullptr;
		result = vertBuffer->Map(0, nullptr, (void**)&vertMap);
		if(SUCCEEDED(result)){
			copy(this->vertices.begin(), this->vertices.end(), vertMap);
			vertBuffer->Unmap(0, nullptr);
		}
	}

	//頂点バッファビュー
	vbView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(VertexPosNormal);

	//頂点インデックス
	//インデックスデータサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));

	//頂点インデックス
	{
		//ヒーププロパティ
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//リソース設定
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
		//生成
		result = dxCommon->GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)
		);
		//転送
		unsigned short* indexMap = nullptr;
		result = indexBuffer->Map(0, nullptr, (void**)&indexBuffer);
		if(SUCCEEDED(result)){
			copy(this->indices.begin(), this->indices.end(), indexMap);
			indexBuffer->Unmap(0, nullptr);
		}
	}

	//インデックスバッファビュー
	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}
