#pragma once

class DX12Wrapper;

namespace DX12Util
{
	//自作のDX12ヘルパー関数
	Microsoft::WRL::ComPtr<ID3D12Resource>
	CreateResourceBuffer(DX12Wrapper* dx12, HRESULT& result, UINT64 bufferSize);

	Microsoft::WRL::ComPtr<ID3D12Resource>
	CreateResourceBufferForCBV(DX12Wrapper* dx12, HRESULT& result, UINT64 bufferSize);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
	CreateDescriptorHeap(DX12Wrapper* dx12, HRESULT& result, UINT numDescriptors);

	//球体の頂点データを作成するヘルパー関数
	DirectX::XMFLOAT3 CalcVertexOfSphere(int u, int v, const int U_MAX, const int V_MAX);
	unsigned short* CalcIndicesOfSphere(unsigned short* indicesOfSphere, const int U_MAX, const int V_MAX);
}
