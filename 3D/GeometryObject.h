#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "DirectXCommon.h"
#include "GeometryObjectManager.h"
#include "TextureManager.h"
#include "ViewProjection.h"

/// <summary>
/// �􉽊w�I�u�W�F�N�g
/// </summary>
class GeometryObject
{
/// <summary>
/// �G�C���A�X
/// </summary>
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// �C���i�[�N���X
/// </summary>
public:
	class Common{
		friend class GeometryObject;

	public:
		/// <summary>
		/// �O���t�B�b�N�p�C�v���C��������
		/// </summary>
		void InitializeGraphicsPipeline();

		/// <summary>
		/// �f�X�g���N�^�q�[�v������
		/// </summary>
		void InitializeDescriptorHeap();

	private:
		//DirectXCommon
		DirectXCommon* dxCommon = nullptr;

		//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelineState;
		//���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootSignature;
		//�f�X�N���v�^�q�[�v(�萔�o�b�t�@�r���[�p)
		ComPtr<ID3D12DescriptorHeap> basicDescHeap;

		GeometryObjectManager* geometryObjectManager;
		TextureManager* textureManager;

		ViewProjection viewProjevtion;
	};

	//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��
	struct ConstBufferData{
		DirectX::XMMATRIX mat;	//3D�ϊ��s��
		DirectX::XMFLOAT4 color;	//�F(RGBA)
	};

public:
	/// <summary>
	/// ������
	/// </summary>
	static void StaticInitialize(DirectXCommon* dxCommon);

	/// <summary>
	/// �ÓI�����o��@
	/// </summary>
	static void StaticFinalize();

	static GeometryObject* Create(UINT texNumber, DirectX::XMFLOAT4 color = {1,1,1,1});

private:
	static Common* common;

public:
	GeometryObject();
	GeometryObject(UINT texNumber, DirectX::XMFLOAT4 color = {1,1,1,1});

	/// <summary>
	/// ������
	/// </summary>
	bool Initialize(UINT texNumber);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

private:
	//�e�N�X�`���f�[�^
	UINT texNumber = 0;

	//�萔�o�b�t�@(�s��)
	ComPtr<ID3D12Resource> constBuffer;
	//�}�b�s���O�p�|�C���^
	ConstBufferData* constMap = nullptr;

	XMFLOAT4 color = {1,1,1,1};

	//�g�����X�t�H�[��
	//�A�t�B���ϊ�
	Vector3 scale = {1.0f, 1.0f, 1.0f};
	Vector3 rotation = {0.0f, 0.0f, 0.0f};
	Vector3 position = {0.0f, 0.0f, 0.0f};
	//���[���h�ϊ��s��
	DirectX::XMMATRIX matWorld;
	//�e�I�u�W�F�N�g�ւ̃|�C���^
	GeometryObject* parent = nullptr;
};
