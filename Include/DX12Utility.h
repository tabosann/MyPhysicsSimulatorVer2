#pragma once

class DX12Wrapper;

namespace DX12Util
{
	//�����DX12�w���p�[�֐�
	Microsoft::WRL::ComPtr<ID3D12Resource>
	CreateResourceBuffer(DX12Wrapper* dx12, HRESULT& result, UINT64 bufferSize);

	Microsoft::WRL::ComPtr<ID3D12Resource>
	CreateResourceBufferForCBV(DX12Wrapper* dx12, HRESULT& result, UINT64 bufferSize);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
	CreateDescriptorHeap(DX12Wrapper* dx12, HRESULT& result, UINT numDescriptors);

	//���̂̒��_�f�[�^���쐬����w���p�[�֐�
	DirectX::XMFLOAT3 CalcVertexOfSphere(int u, int v, const int U_MAX, const int V_MAX);
	unsigned short* CalcIndicesOfSphere(unsigned short* indicesOfSphere, const int U_MAX, const int V_MAX);
}
