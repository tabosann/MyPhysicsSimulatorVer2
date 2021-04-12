#pragma once

class DX12Wrapper;

class ObjectRenderer {
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//@param dx12 DX12Wrapper�N���X�I�u�W�F�N�g
	ObjectRenderer(DX12Wrapper* dx12);
	virtual ~ObjectRenderer();

	ID3D12PipelineState* GetGraphicPipeline() const;
	ID3D12RootSignature* GetRootSignature() const;

	void Update();

private:
	//�P�̒��_�ɍڂ�����
	static constexpr D3D12_INPUT_ELEMENT_DESC _inputLayout[3] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	DX12Wrapper*                 _dx12;            //�R�}���h���X�g�ƃf�o�C�X�̎擾�Ɏg�p
	ComPtr<ID3D12RootSignature>  _rootSignature;   //�o�b�t�@�ƃV�F�[�_�[���Ȃ����W
	ComPtr<ID3D12PipelineState>  _graphicPipeline; //�`��̕��@���e��ݒ肷��I�u�W�F�N�g

	HRESULT CreateGraphicPipeline(const D3D12_INPUT_ELEMENT_DESC* inputLayout, size_t numElements, UINT numRenderTargets = 1);
	HRESULT CreateRootSignature();
};