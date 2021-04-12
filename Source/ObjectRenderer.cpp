#include<DX12Wrapper.h>
#include<DX12Utility.h>
#include<ObjectRenderer.h>

using namespace std;
using namespace Microsoft::WRL;

#define SET_MY_ORIGINAL_DESCRIPTOR_TABLE

//�f�o�b�O�֐�
//NOTE: �֐����̐ړ�����Check...�ɂ��邱�ƁB
namespace {
	bool CheckShaderCompileResult(HRESULT result, ID3DBlob* error);
}

ObjectRenderer::ObjectRenderer(DX12Wrapper* dx12) 
	:_dx12(dx12)
{
	HRESULT result = CreateRootSignature();
	assert(SUCCEEDED(result));

	result = CreateGraphicPipeline(_inputLayout, _countof(_inputLayout));
	assert(SUCCEEDED(result));
}

ObjectRenderer::~ObjectRenderer() {

}

ID3D12PipelineState*
ObjectRenderer::GetGraphicPipeline() const {
	return _graphicPipeline.Get();
}

ID3D12RootSignature*
ObjectRenderer::GetRootSignature() const {
	return _rootSignature.Get();
}

HRESULT ObjectRenderer::CreateRootSignature() {
#ifndef SET_MY_ORIGINAL_DESCRIPTOR_TABLE
	//FIXME: ���̂�����̃f�B�X�N���v�^�e�[�u�����ƃG���[���N����...
	D3D12_DESCRIPTOR_RANGE descriptorRanges[2];
	//'SceneData'�\���̂̃f�[�^
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorRanges[0].NumDescriptors = 1;
	descriptorRanges[0].BaseShaderRegister = 0;
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//'Transform'�\���̂̃f�[�^
	descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorRanges[1].NumDescriptors = 1;
	descriptorRanges[1].BaseShaderRegister = 1;
	descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[2];
	//'SceneData'�\���̂̃f�[�^
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRanges;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//'Transform'�\���̂̃f�[�^
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRanges + 1;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

#else
	CD3DX12_DESCRIPTOR_RANGE descriptorRanges[2];
	descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	descriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	CD3DX12_ROOT_PARAMETER rootParameters[2];
	rootParameters[0].InitAsDescriptorTable(1, descriptorRanges);
	rootParameters[1].InitAsDescriptorTable(1, descriptorRanges + 1);

#endif

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = 0;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rootSignatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT result = D3D12SerializeRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSignatureBlob, &errorBlob);
	if (FAILED(result)) return result;

	result = _dx12->GetDevice()->CreateRootSignature(
		0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) return result;

	return result;
}

HRESULT ObjectRenderer::CreateGraphicPipeline(const D3D12_INPUT_ELEMENT_DESC* inputLayout, size_t numElements, UINT numRenderTargets) {
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	//���_�V�F�[�_�t�@�C���ǂݍ���
	HRESULT result = D3DCompileFromFile(
		L"VertexShader.hlsl", // NOTE: ���C�h������
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (!CheckShaderCompileResult(result, errorBlob)) {
		assert(SUCCEEDED(result));
	}

	//�s�N�Z���V�F�[�_�t�@�C���ǂݍ���
	result = D3DCompileFromFile(
		L"PixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (!CheckShaderCompileResult(result, errorBlob)) {
		assert(SUCCEEDED(result));
	}

	//�O���t�B�b�N�X�p�C�v���C���̐ݒ�ƍ쐬
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  graphicsPipelineDesc = {};
	//���[�g�V�O�l�`��
	graphicsPipelineDesc.pRootSignature = _rootSignature.Get();
	//�V�F�[�_�֘A
	graphicsPipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	graphicsPipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	graphicsPipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	graphicsPipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();
	//�T���v���}�X�N�֘A
	graphicsPipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//�u�����h�֘A
	graphicsPipelineDesc.BlendState.AlphaToCoverageEnable = false;
	graphicsPipelineDesc.BlendState.IndependentBlendEnable = false;
	D3D12_RENDER_TARGET_BLEND_DESC renderDesc = {};
	{
		renderDesc.BlendEnable = false;
		renderDesc.LogicOpEnable = false;
		renderDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	graphicsPipelineDesc.BlendState.RenderTarget[0] = renderDesc;
	//���X�^���C�U(�`��̃��[��)�֘A
	graphicsPipelineDesc.RasterizerState.MultisampleEnable = false;
	graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	graphicsPipelineDesc.RasterizerState.DepthClipEnable = true;
	//�[�x�l�֘A
	graphicsPipelineDesc.DepthStencilState.DepthEnable = true;
	graphicsPipelineDesc.DepthStencilState.StencilEnable = false;
	graphicsPipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	graphicsPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//���C�A�E�g�֘A
	graphicsPipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	graphicsPipelineDesc.InputLayout.NumElements = numElements;
	//�`��(�̎�@)�֘A
	graphicsPipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	graphicsPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//�����_�[�^�[�Q�b�g�֘A
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		_dx12->GetSwapChain()->GetDesc1(&desc);
		graphicsPipelineDesc.NumRenderTargets = numRenderTargets;
		graphicsPipelineDesc.RTVFormats[0] = desc.Format;
	}
	//���̑�
	graphicsPipelineDesc.SampleDesc.Count = 1;
	graphicsPipelineDesc.SampleDesc.Quality = 0;
	graphicsPipelineDesc.NodeMask = 0;

	result = _dx12->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineDesc, IID_PPV_ARGS(_graphicPipeline.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) return result;

	return result;
}

void ObjectRenderer::Update() {
	//���ɂȂ�
}

namespace {
	bool CheckShaderCompileResult(HRESULT result, ID3DBlob* error) {
		if (SUCCEEDED(result)) return true;

		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			OutputDebugStringA("�t�@�C������������܂���");
			return false;
		}

		string errorStr;
		errorStr.resize(error->GetBufferSize());
		copy_n((char*)error->GetBufferPointer(), error->GetBufferSize(), errorStr.begin());
		errorStr += "\n";
		OutputDebugStringA(errorStr.c_str());

		return false;
	}
}