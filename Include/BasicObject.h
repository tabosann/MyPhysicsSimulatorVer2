#pragma once

#include"../MyHelpers/MyVector3.h"
#include"../MyHelpers/MyChar32.h"

class DX12Wrapper;
class MySphere;

//���ׂĂ̕��̂ɋ��ʂ���v�f���l�ߍ��񂾃N���X
//�E���_���
//�E���_�o�b�t�@�r���[
//�E�C���f�b�N�X�o�b�t�@�r���[
//�E�A�t�B���ϊ��s��
class BasicObject {
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//�t���[�����ɃA�b�v�f�[�g�����f�[�^�������\����
	struct Transform {
		//NOTE: DirectX::XMMATRIX��16�̔{���o�C�g�݂̂������̂�
		//    : 16�̔{���ɃA���C�����g���Ȃ���΂����Ȃ��B
		void* operator new(size_t size);
		DirectX::XMMATRIX world;
		DirectX::XMFLOAT4 color;
	};

public:
	//�P�̒��_�ɍڂ�����
	//NOTE: Vertex�\���̂�ύX������'_inputLayout'�z����ύX���邱��
	struct Vertex {
		DirectX::XMFLOAT3 pos;	  //�ʒu[m]
		DirectX::XMFLOAT3 normal; //�@���x�N�g��
		DirectX::XMFLOAT3 color;  //�F(R, G, B)
	};

	//���̍\���̂���'world'�s��ɃA�N�Z�X���A�A�t�B���ϊ����s���B
	//e.g. _mappedTransform->world = XMMatrixTranslation();
	Transform* _mappedTransform;

	Char32 _name;
	MyVec3 _size;
	float  _m; //����[kg]
	float  _e; //�����W��
	MyVec3 _color;

	//@param dx12 : DX12Wrapper�N���X�I�u�W�F�N�g
	BasicObject(DX12Wrapper* dx12, const Char32& name, const MyVec3& size, float m, float e, const MyVec3& col);
	virtual ~BasicObject();

	//�p�ӂ��ꂽ���_�f�[�^���V�F�[�_�ɓn������������
	HRESULT CreateVertexBufferView(Vertex* begin, Vertex* end, size_t sizeOfVertices);
	//�p�ӂ��ꂽ���_�f�[�^�̃C���f�b�N�X�Ńv���~�e�B�u�`��̏��Ԃ����肷��B
	HRESULT CreateIndexBufferView(unsigned short* begin, unsigned short* end, size_t sizeOfIndices);

	virtual void Update();
	virtual void Render() const;
	virtual void Adjuster() = 0;

private:
	//�R�}���h���X�g�ƃf�o�C�X�̎擾�Ɏg�p
	DX12Wrapper*                 _dx12;
	//���̂��\�����钸�_�ƃC���f�b�N�X�̏���
	ComPtr<ID3D12Resource>       _vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW     _vertexBufferView = {};
	ComPtr<ID3D12Resource>       _indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW      _indexBufferView = {};
	//�A�t�B���ϊ��s��̏���
	ComPtr<ID3D12Resource>       _transformBuffer;
	ComPtr<ID3D12DescriptorHeap> _transformDescriptorHeap;

	//�A�t�B���ϊ��s����V�F�[�_�ɓn�����߂̏���
	HRESULT CreateTransformViewWithCBV();
};
