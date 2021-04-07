#include<DX12Wrapper.h>
#include"../MyHelpers/MyVector3.h"
#include<ObjectRenderer.h>
#include<DX12Utility.h>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

ComPtr<ID3D12Resource>
	DX12Util::CreateResourceBuffer(DX12Wrapper* dx12, HRESULT& result, UINT64 bufferSize) {
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Width = bufferSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Alignment = 0;

	ComPtr<ID3D12Resource> buf = nullptr;
	result = dx12->GetDevice()->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buf.ReleaseAndGetAddressOf()));

	return SUCCEEDED(result) ? buf : nullptr;
}

ComPtr<ID3D12Resource>
DX12Util::CreateResourceBufferForCBV(DX12Wrapper* dx12, HRESULT& result, UINT64 bufferSize) {
	return DX12Util::CreateResourceBuffer(dx12, result, (bufferSize + 0xff) & ~0xff);
}

ComPtr<ID3D12DescriptorHeap>
DX12Util::CreateDescriptorHeap(DX12Wrapper* dx12, HRESULT& result, UINT numDescriptors) {
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NumDescriptors = numDescriptors;
	desc.NodeMask = 0;

	ComPtr<ID3D12DescriptorHeap> descHeap;
	result = dx12->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	return SUCCEEDED(result) ? descHeap : nullptr;
}

XMFLOAT3 DX12Util::CalcVertexOfSphere(int u, int v, const int U_MAX, const int V_MAX) {
	float phi = XMConvertToRadians(180.0f * v / V_MAX);
	float theta = XMConvertToRadians(360.0f * u / U_MAX);
	float x = cos(theta) * cos(phi);
	float y = sin(theta);
	float z = cos(theta) * sin(phi);
	return XMFLOAT3(x, y, z);
}

unsigned short* DX12Util::CalcIndicesOfSphere(unsigned short* indicesOfSphere, const int U_MAX, const int V_MAX) {
	int i = 0;
	for (int v = 0; v < V_MAX; ++v) {
		for (int u = 0; u <= U_MAX; ++u) {
			if (u == U_MAX) {
				indicesOfSphere[i++] = v * U_MAX;
				indicesOfSphere[i++] = indicesOfSphere[i - 1] + U_MAX;
				continue;
			}
			indicesOfSphere[i++] = (v * U_MAX) + u;
			indicesOfSphere[i++] = indicesOfSphere[i - 1] + U_MAX;
		}
	}
	return indicesOfSphere;
}
