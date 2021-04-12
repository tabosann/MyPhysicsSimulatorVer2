#include<DX12Utility.h>
#include<DX12Wrapper.h>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

#ifdef _DEBUG
#define ENABLE_DXGI_DEBUGLAYER_FOR_DX12
#endif

const float kSkyColor4[4] = { 0.290196f, 0.372549f, 0.501961f, 1.f };

namespace 
{
	//NOTE: �f�o�C�X�쐬��Ƀf�o�b�O���C���[��L�������Ă͂Ȃ�Ȃ��B�f�o�C�X����菜����Ă��܂��B
	void EnableDebugLayer()
	{
		ComPtr<ID3D12Debug> debugLayer;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf())))) {
			debugLayer->EnableDebugLayer();
		}
	}
}

DX12Wrapper::DX12Wrapper(HWND hwnd, int windowWidth, int windowHeight) 
{
#ifdef ENABLE_DXGI_DEBUGLAYER_FOR_DX12
	::EnableDebugLayer();
#endif

	_initBgColor  = _bgColor   = XMFLOAT4(kSkyColor4);
	_initFov      = _fov       = XM_PIDIV2;
	_initCamera   = _cameraPos = XMFLOAT3(0.f, 0.f, -10.f);
	_initLightDir = _lightDir  = XMFLOAT4(-5.f, -1.f, -4.f, 1.f);

	auto result = CreateDXGIAndDevice(L"NVIDIA");
	assert(SUCCEEDED(result));

	result = CreateCommandFlow();
	assert(SUCCEEDED(result));

	result = CreateSwapChain(hwnd, windowWidth, windowHeight);
	assert(SUCCEEDED(result));

	result = CreateFinalRenderTarget();
	assert(SUCCEEDED(result));

	result = CreateDepthStencilView();
	assert(SUCCEEDED(result));

	result = CreateSceneDataViewWithCBV();
	assert(SUCCEEDED(result));

	result = _device->CreateFence(_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	_descHeapForImGui = DX12Util::CreateDescriptorHeap(this, result, 1);
	assert(SUCCEEDED(result));
}

DX12Wrapper::~DX12Wrapper()
{
	for (int i = 0; i < _backBuffers.size(); ++i) _backBuffers[i]->Release();
	vector<ID3D12Resource*>().swap(_backBuffers);
}

HRESULT DX12Wrapper::CreateDXGIAndDevice(const wstring& targetAdapterName)
{
	//�ŏ���DXGI�̏��������K�{
#ifdef _DEBUG
	//�f�o�b�O���A�f�o�b�O����\������悤�ɂ���
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	//�f�o�b�O���͓��ɕ\�����Ȃ�
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#endif
	assert(SUCCEEDED(result));

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1, //Macbook Air 2017 �͂���
		D3D_FEATURE_LEVEL_11_0
	};
	IDXGIAdapter* selectedAdapter = nullptr;

	//�g�p�������A�_�v�^���w�肳��Ă���΁A�������������i�K�ɓ���
	if (targetAdapterName != L"") {
		vector<IDXGIAdapter*> allAdapters;
		//���ׂẴA�_�v�^��񋓂��邽�߁Ai�Ԗڂ̃A�_�v�^��'allAdapters'�ɏ����ǉ����Ă���
		//�A�_�v�^��������Ȃ��Ȃ�܂ŗ񋓂��J��Ԃ����ƂŎ���
		for (int i = 0; _dxgiFactory->EnumAdapters(i, &selectedAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
			allAdapters.push_back(selectedAdapter);
		}

		//'allAdapters'�Ɋ܂܂��A�_�v�^�̒�����A�g�p�������A�_�v�^����������
		for (auto& adpt : allAdapters) {
			DXGI_ADAPTER_DESC adptDesc = {};
			result = adpt->GetDesc(&adptDesc); //�A�_�v�^�̐��������擾���邽�߂ɁA�����I�u�W�F�N�g���擾
			assert(SUCCEEDED(result));

			wstring description = adptDesc.Description;
			//�A�_�v�^�̐������̒��ɁA'targetAdapterName'���܂܂�Ă���A�_�v�^��
			//�g�p�������A�_�v�^�Ȃ̂ŁA���̎g�p�������A�_�v�^���L������B
			if (description.find(targetAdapterName) != string::npos) {
				selectedAdapter = adpt;
				break;
			}
		}
	}

	//'selectedAdapter'���g���āA�f�o�C�X�쐬�ɐ�������t�B�[�`���[���x��������
	for (auto level : featureLevels) {
		result = D3D12CreateDevice(selectedAdapter, level, IID_PPV_ARGS(_device.ReleaseAndGetAddressOf()));
		if (SUCCEEDED(result)) return result;
	}

	return result;
}

HRESULT DX12Wrapper::CreateCommandFlow()
{
	//�R�}���h�A���P�[�^�̍쐬
	auto result = _device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_commandAllocator.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//�R�}���h���X�g�̍쐬
	result = _device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(),
		nullptr, IID_PPV_ARGS(_commandList.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//�R�}���h�L���[�̐ݒ�ƍ쐬
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	result = _device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_commandQueue.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	return result;
}

HRESULT
DX12Wrapper::CreateSwapChain(const HWND& hwnd, int windowWidth, int windowHeight)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = {};
	swapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//�P�s�N�Z����0.0~1.0��256�i�K�ŕ\��
	swapChainDesc1.BufferUsage = DXGI_USAGE_BACK_BUFFER;			//
	swapChainDesc1.BufferCount = 2;									//�\����ʗp�̃o�b�t�@�Q��
	swapChainDesc1.Width = _windowSize.cx = windowWidth;			//��ʉ𑜓x(��)
	swapChainDesc1.Height = _windowSize.cy = windowHeight;			//��ʉ𑜓x(�c)
	swapChainDesc1.Stereo = false;									//�X���b�v�G�t�F�N�g���V�[�P���V�������[�h�Ȃ�true
	swapChainDesc1.SampleDesc.Count = 1;							//�}���`�T���v�����O�֘A
	swapChainDesc1.SampleDesc.Quality = 0;							//
	swapChainDesc1.Scaling = DXGI_SCALING_STRETCH;					//����ʂ̃T�C�Y�ɍ��킹�ăo�b�N�o�b�t�@�����T�C�Y
	swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		//�t���b�v�O�̕\��ʂ��t���b�v�ナ�Z�b�g(�t���b�v��������)
	swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			//���Ɏw�肵�Ȃ�
	//�E�B���h�E���[�h�A�t���X�N���[�����[�h�̐؂�ւ�����
	//���̂Ƃ��̃E�B���h�E�T�C�Y�ɍ��킹�ĕ\�����[�h���؂�ւ���
	swapChainDesc1.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto result = _dxgiFactory->CreateSwapChainForHwnd(
		_commandQueue.Get(),
		hwnd,
		&swapChainDesc1,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapChain.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	return result;
}

HRESULT
DX12Wrapper::CreateFinalRenderTarget()
{
	//�E�B���h�E�T�C�Y���擾���邽�߂ɃX���b�v�`�F�[���̐��������擾
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1;
	auto result = _swapChain->GetDesc1(&swapChainDesc1);
	assert(SUCCEEDED(result));
	
	//�\����ʗp�̂Q�̃o�b�t�@���m�ۂ���
	_backBuffers.resize(swapChainDesc1.BufferCount);
	
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeapDesc.NumDescriptors = swapChainDesc1.BufferCount;
	rtvDescriptorHeapDesc.NodeMask = 0;
	result = _device->CreateDescriptorHeap(
		&rtvDescriptorHeapDesc,
		IID_PPV_ARGS(_rtvDescriptorHeap.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//�o�b�N�o�b�t�@����RTV���쐬
	auto rtvDescriptorHeapHandle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < swapChainDesc1.BufferCount; ++i) {
		//'_backBuffers[i]'�ɕ\����ʗp�̃o�b�t�@����
		result = _swapChain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
		assert(SUCCEEDED(result));

		rtvDesc.Format = _backBuffers[i]->GetDesc().Format;
		_device->CreateRenderTargetView(_backBuffers[i], &rtvDesc, rtvDescriptorHeapHandle);

		//����RTV���Q��
		rtvDescriptorHeapHandle.ptr += _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	D3D12_VIEWPORT viewPortDesc = {};
	viewPortDesc.Width    = 1.f * swapChainDesc1.Width;
	viewPortDesc.Height   = 1.f * swapChainDesc1.Height;
	viewPortDesc.TopLeftX = 0.f;
	viewPortDesc.TopLeftY = 0.f;
	viewPortDesc.MaxDepth = 1.f;
	viewPortDesc.MinDepth = 0.f;

	D3D12_RECT scissorRectDesc = {};
	scissorRectDesc.left = 0;
	scissorRectDesc.top = 0;
	scissorRectDesc.right = scissorRectDesc.left + swapChainDesc1.Width;
	scissorRectDesc.bottom = scissorRectDesc.top + swapChainDesc1.Height;

	_viewPort = viewPortDesc;
	_scissorRect = scissorRectDesc;

	assert(SUCCEEDED(result));
	return S_OK;
}

HRESULT DX12Wrapper::CreateDepthStencilView()
{
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	depthHeapProp.CreationNodeMask = 0;
	depthHeapProp.VisibleNodeMask = 0;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = {};
	_swapChain->GetDesc1(&swapChainDesc1);
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthResDesc.Width = swapChainDesc1.Width;
	depthResDesc.Height = swapChainDesc1.Height;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.MipLevels = 1;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.SampleDesc.Quality = 0;
	depthResDesc.Alignment = 0;

	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthClearValue.DepthStencil.Depth = 1.0;

	auto result = _device->CreateCommittedResource(
		&depthHeapProp, D3D12_HEAP_FLAG_NONE,
		&depthResDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.NodeMask = 0;
	result = _device->CreateDescriptorHeap(
		&dsvDescriptorHeapDesc, IID_PPV_ARGS(_dsvDescriptorHeap.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice = 0;
	_device->CreateDepthStencilView(
		_depthBuffer.Get(), &dsvDesc,
		_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
	);

	assert(SUCCEEDED(result));
	return S_OK;
}

HRESULT DX12Wrapper::CreateSceneDataViewWithCBV()
{
	D3D12_HEAP_PROPERTIES sceneHeapProp = {};
	sceneHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	sceneHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	sceneHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	sceneHeapProp.CreationNodeMask = 0;
	sceneHeapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC sceneResDesc = {};
	sceneResDesc.Format = DXGI_FORMAT_UNKNOWN;
	sceneResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	sceneResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	sceneResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	sceneResDesc.Width = (sizeof(SceneData) + 0xff) & ~0xff; //256�̔{���ɋ�؂��ă��\�[�X���m��(���_��������)
	sceneResDesc.Height = 1;
	sceneResDesc.DepthOrArraySize = 1;
	sceneResDesc.MipLevels = 1;
	sceneResDesc.SampleDesc.Count = 1;
	sceneResDesc.SampleDesc.Quality = 0;
	sceneResDesc.Alignment = 0;

	auto result = _device->CreateCommittedResource(
		&sceneHeapProp, D3D12_HEAP_FLAG_NONE,
		&sceneResDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_sceneBuffer.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	result = _sceneBuffer->Map(0, nullptr, (void**)&_mapSceneData);
	assert(SUCCEEDED(result));

	XMFLOAT3 eyePos(0.f, 0.f, 0.f);
	XMFLOAT3 target(0.f, 0.f, 1.f);
	XMFLOAT3 up(0.f, 1.f, 0.f);

	//�r���[��ݒ�
	_mapSceneData->view = XMMatrixLookAtLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&target), XMLoadFloat3(&up));

	//�E�B���h�E�T�C�Y���擾���邽�߂ɃX���b�v�`�F�[���̐ݒ���擾
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	_swapChain->GetDesc1(&desc);

	//�v���W�F�N�V������ݒ�
	_mapSceneData->proj = XMMatrixPerspectiveFovLH(
		XM_PIDIV2,
		static_cast<float>(desc.Width) / static_cast<float>(desc.Height),
		0.1f, 100.0f
	);

	_mapSceneData->eyePos = _cameraPos;
	_mapSceneData->lightDir = _lightDir;

	D3D12_DESCRIPTOR_HEAP_DESC sceneDescriptorHeapDesc = {};
	sceneDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	sceneDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	sceneDescriptorHeapDesc.NumDescriptors = 1;
	sceneDescriptorHeapDesc.NodeMask = 0;
	result = _device->CreateDescriptorHeap(
		&sceneDescriptorHeapDesc, IID_PPV_ARGS(_sceneDescriptorHeap.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_CONSTANT_BUFFER_VIEW_DESC sceneCBVDesc = {};
	sceneCBVDesc.BufferLocation = _sceneBuffer->GetGPUVirtualAddress();
	sceneCBVDesc.SizeInBytes = (UINT)_sceneBuffer->GetDesc().Width;
	_device->CreateConstantBufferView(&sceneCBVDesc, _sceneDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	assert(SUCCEEDED(result));
	return S_OK;
}

void DX12Wrapper::SetSceneDataToShader()
{
	ID3D12DescriptorHeap* sceneDescriptorHeaps[] = { _sceneDescriptorHeap.Get() };
	_commandList->SetDescriptorHeaps(_countof(sceneDescriptorHeaps), sceneDescriptorHeaps);
	_commandList->SetGraphicsRootDescriptorTable(0, _sceneDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

//��ʂɕ`�悵�Ă������̂��N���A���A�[�x�l��K�������ĉB�ʏ�����L���ɂ���B
// NOTE: ���\�[�X��Ԃ������_�[�^�[�Q�b�g��ԂɈڍs����B
void DX12Wrapper::BeginRender()
{
	//���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾(���݁A0�ԖځA1�Ԗڂǂ���̉�ʃo�b�t�@������ʂ�)
	auto currentBackBuffIdx = _swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER resBarrierDesc = {};
	resBarrierDesc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resBarrierDesc.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resBarrierDesc.Transition.pResource   = _backBuffers[currentBackBuffIdx];
	resBarrierDesc.Transition.Subresource = 0;
	resBarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		//�ҋ@��Ԃ���
	resBarrierDesc.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;	//�����_�[�^�[�Q�b�g��Ԃ֑J��
	_commandList->ResourceBarrier(1, &resBarrierDesc);

	//���݂̃o�b�N�o�b�t�@�p��RTV���Q�Ƃ���
	auto rtvDescHeapHandle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvDescHeapHandle.ptr += currentBackBuffIdx * _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//���݂̃����_�[�^�[�Q�b�g��'clearColor'�ŃN���A
	_commandList->ClearRenderTargetView(rtvDescHeapHandle, &_bgColor.x, 0, nullptr);

	auto dsvDescHeapHandle = _dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//'clearColor'�ŃN���A���������_�[�^�[�Q�b�g�ƌ��݂̐[�x�l��K��
	_commandList->OMSetRenderTargets(
		1,
		&rtvDescHeapHandle, 
		false,
		&dsvDescHeapHandle
	);

	//���݂̐[�x�l���Z�b�g���I��������A���̐[�x�l�v�Z�̂��߂ɐ[�x�l�N���A
	_commandList->ClearDepthStencilView(dsvDescHeapHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);

	_commandList->RSSetViewports(1, &_viewPort);
	_commandList->RSSetScissorRects(1, &_scissorRect);
}

//���ߍ���ł������߂���C�ɁA���ߍ��񂾏��Ɏ��s���Ă����B
// NOTE: ���\�[�X��Ԃ��ҋ@��ԂɈڍs����B
void DX12Wrapper::EndRender()
{
	// NOTE: ���̊֐����ŐV���Ƀp�C�v���C����ݒ肵�Ă���̂ŁA����ȍ~�Ǝ��̖��߂�ǉ����Ȃ����ƁB
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());

	//���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾(���݁A0�ԖځA1�Ԗڂǂ���̉�ʃo�b�t�@������ʂ�)
	auto backBuffIdx = _swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER resBarrierDesc = {};
	resBarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resBarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resBarrierDesc.Transition.pResource = _backBuffers[backBuffIdx];
	resBarrierDesc.Transition.Subresource = 0;
	resBarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//�����_�[�^�[�Q�b�g��Ԃ���
	resBarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;		//�ҋ@��Ԃ֑J��
	_commandList->ResourceBarrier(1, &resBarrierDesc);

	//NOTE: �ҋ@��ԂŃN���[�Y���邱��
	_commandList->Close();

	ID3D12CommandList* cmdLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	//GPU���̃t�F���X�l(GPU��Signal()�֐��Ńp�X���Ă���)�ƁACPU���̃t�F���X�l(_fenceValue���C���N�������g���Ă���)��
	//��v����A�������s�I���܂ł̊ԏ�����҂�������B�ڍׂ� �� https://bit.ly/3sxJSrr
	_commandQueue->Signal(_fence.Get(), ++_fenceValue);
	if (_fence->GetCompletedValue() < _fenceValue) {
		auto event = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceValue, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator.Get(), nullptr);
}

void DX12Wrapper::Update()
{
	_mapSceneData->proj     = XMMatrixPerspectiveFovLH(_fov, 1.f * _windowSize.cx / _windowSize.cy, 0.1f, 100.f);
	_mapSceneData->eyePos   = _cameraPos;
	_mapSceneData->lightDir = _lightDir;
}

void DX12Wrapper::Reset()
{
	_bgColor   = _initBgColor;
	_fov       = _initFov;
	_cameraPos = _initCamera;
	_lightDir  = _initLightDir;
}

ID3D12Device*                DX12Wrapper::GetDevice() const           { return _device.Get(); }
ID3D12GraphicsCommandList*   DX12Wrapper::GetCmdList() const          { return _commandList.Get(); }
IDXGISwapChain4*             DX12Wrapper::GetSwapChain() const        { return _swapChain.Get(); }
ComPtr<ID3D12DescriptorHeap> DX12Wrapper::GetDescHeapForImGui() const { return _descHeapForImGui; }

void DX12Wrapper::SetViewPort(D3D12_VIEWPORT* view, D3D12_RECT* rect) { _viewPort = *view; _scissorRect = *rect; }
