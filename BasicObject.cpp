#include<DX12Wrapper.h>
#include<DX12Utility.h>
#include<BasicObject.h>

using namespace std;
using namespace DX12Util;

BasicObject::BasicObject(DX12Wrapper* dx12, const Char32& name, const MyVec3& size,  float m, float e, const MyVec3& col)
	:_dx12(dx12), _name(name), _size(size), _m(m), _e(e), _color(col)
{
	HRESULT result = CreateTransformViewWithCBV();
	assert(SUCCEEDED(result));
}

BasicObject::~BasicObject() {

}

HRESULT BasicObject::CreateVertexBufferView(Vertex* begin, Vertex* end, size_t sizeOfVertices) {
	//�o�b�t�@�쐬
	HRESULT result;
	_vertexBuffer = CreateResourceBuffer(_dx12, result, sizeOfVertices);
	assert(SUCCEEDED(result));
	if (FAILED(result)) return result;

	//�}�b�v
	Vertex* mapVertexBuffer = nullptr;
	result = _vertexBuffer->Map(0, nullptr, (void**)&mapVertexBuffer);
	assert(SUCCEEDED(result));
	if (FAILED(result)) return result;

	//�f�[�^�A�b�v���[�h
	std::copy(begin, end, mapVertexBuffer);
	_vertexBuffer->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̐ݒ�
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.SizeInBytes = sizeOfVertices;
	_vertexBufferView.StrideInBytes = sizeof(Vertex);

	return result;
}

HRESULT BasicObject::CreateIndexBufferView(unsigned short* begin, unsigned short* end, size_t sizeOfIndices) {
	//�o�b�t�@�̍쐬
	HRESULT result;
	_indexBuffer = CreateResourceBuffer(_dx12, result, sizeOfIndices);
	assert(SUCCEEDED(result));
	if (FAILED(result)) return result;

	//�}�b�v
	unsigned short* mapIndicesBuffer = nullptr;
	result = _indexBuffer->Map(0, nullptr, (void**)&mapIndicesBuffer);
	assert(SUCCEEDED(result));
	if (FAILED(result)) return result;

	//�f�[�^�A�b�v���[�h
	std::copy(begin, end, mapIndicesBuffer);
	_indexBuffer->Unmap(0, nullptr); //�}�b�v����

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
	_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.SizeInBytes = sizeOfIndices;

	return result;
}

void BasicObject::Update() {
	//���ɂȂ�
}

void BasicObject::Render() const {
	_dx12->GetCmdList()->IASetVertexBuffers(0, 1, &_vertexBufferView);
	_dx12->GetCmdList()->IASetIndexBuffer(&_indexBufferView);

	ID3D12DescriptorHeap* transformDescriptorHeaps[] = {
		_transformDescriptorHeap.Get()
	};
	_dx12->GetCmdList()->SetDescriptorHeaps(_countof(transformDescriptorHeaps), transformDescriptorHeaps);
	_dx12->GetCmdList()->SetGraphicsRootDescriptorTable(1, _transformDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	_dx12->GetCmdList()->DrawIndexedInstanced(_indexBufferView.SizeInBytes / sizeof(unsigned short), 1, 0, 0, 0);
}

HRESULT BasicObject::CreateTransformViewWithCBV() {
	//�o�b�t�@�̍쐬
	HRESULT result;
	_transformBuffer = CreateResourceBufferForCBV(_dx12, result, sizeof(Transform));
	assert(SUCCEEDED(result));
	if (FAILED(result)) return result;

	//�}�b�v
	result = _transformBuffer->Map(0, nullptr, (void**)&_mappedTransform);
	assert(SUCCEEDED(result));
	if (FAILED(result)) return result;

	//�f�B�X�N���v�^�q�[�v�̍쐬
	_transformDescriptorHeap = CreateDescriptorHeap(_dx12, result, 1);
	assert(SUCCEEDED(result));
	if (FAILED(result)) return result;

	//CBV�̍쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvForTransform = {};
	cbvForTransform.BufferLocation = _transformBuffer->GetGPUVirtualAddress();
	cbvForTransform.SizeInBytes = (UINT)_transformBuffer->GetDesc().Width;
	_dx12->GetDevice()->CreateConstantBufferView(
		&cbvForTransform, _transformDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

void* BasicObject::Transform::operator new(size_t size) {
	return _aligned_malloc(size, 16);
}
