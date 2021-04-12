#pragma once

class DX12Wrapper;

class ObjectRenderer
{
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	ObjectRenderer(DX12Wrapper* dx12);

	ID3D12PipelineState* GetGraphicPipeline() const;
	ID3D12RootSignature* GetRootSignature() const;

	void Update();

	virtual ~ObjectRenderer();

private:

	//１つの頂点に載せる情報
	static constexpr D3D12_INPUT_ELEMENT_DESC _inputLayout[3] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	DX12Wrapper*                 _dx12;            //コマンドリストとデバイスの取得に使用
	ComPtr<ID3D12RootSignature>  _rootSignature;   //バッファとシェーダーをつなぐ道標
	ComPtr<ID3D12PipelineState>  _graphicPipeline; //描画の方法を各種設定するオブジェクト

	HRESULT CreateGraphicPipeline(const D3D12_INPUT_ELEMENT_DESC* inputLayout, size_t numElements, UINT numRenderTargets = 1);
	HRESULT CreateRootSignature();
};