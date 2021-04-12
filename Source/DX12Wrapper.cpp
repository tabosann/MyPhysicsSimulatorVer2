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
	//NOTE: デバイス作成後にデバッグレイヤーを有効化してはならない。デバイスが取り除かれてしまう。
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
	//最初はDXGIの初期化が必須
#ifdef _DEBUG
	//デバッグ時、デバッグ情報を表示するようにする
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	//デバッグ情報は特に表示しない
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#endif
	assert(SUCCEEDED(result));

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1, //Macbook Air 2017 はこれ
		D3D_FEATURE_LEVEL_11_0
	};
	IDXGIAdapter* selectedAdapter = nullptr;

	//使用したいアダプタが指定されていれば、それを検索する段階に入る
	if (targetAdapterName != L"") {
		vector<IDXGIAdapter*> allAdapters;
		//すべてのアダプタを列挙するため、i番目のアダプタを'allAdapters'に順次追加していく
		//アダプタが見つからなくなるまで列挙を繰り返すことで実現
		for (int i = 0; _dxgiFactory->EnumAdapters(i, &selectedAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
			allAdapters.push_back(selectedAdapter);
		}

		//'allAdapters'に含まれるアダプタの中から、使用したいアダプタを検索する
		for (auto& adpt : allAdapters) {
			DXGI_ADAPTER_DESC adptDesc = {};
			result = adpt->GetDesc(&adptDesc); //アダプタの説明文を取得するために、説明オブジェクトを取得
			assert(SUCCEEDED(result));

			wstring description = adptDesc.Description;
			//アダプタの説明文の中に、'targetAdapterName'が含まれているアダプタが
			//使用したいアダプタなので、その使用したいアダプタを記憶する。
			if (description.find(targetAdapterName) != string::npos) {
				selectedAdapter = adpt;
				break;
			}
		}
	}

	//'selectedAdapter'を使って、デバイス作成に成功するフィーチャーレベルを検索
	for (auto level : featureLevels) {
		result = D3D12CreateDevice(selectedAdapter, level, IID_PPV_ARGS(_device.ReleaseAndGetAddressOf()));
		if (SUCCEEDED(result)) return result;
	}

	return result;
}

HRESULT DX12Wrapper::CreateCommandFlow()
{
	//コマンドアロケータの作成
	auto result = _device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_commandAllocator.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//コマンドリストの作成
	result = _device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(),
		nullptr, IID_PPV_ARGS(_commandList.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//コマンドキューの設定と作成
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
	swapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				//１ピクセルを0.0~1.0の256段階で表現
	swapChainDesc1.BufferUsage = DXGI_USAGE_BACK_BUFFER;			//
	swapChainDesc1.BufferCount = 2;									//表裏画面用のバッファ２つ
	swapChainDesc1.Width = _windowSize.cx = windowWidth;			//画面解像度(横)
	swapChainDesc1.Height = _windowSize.cy = windowHeight;			//画面解像度(縦)
	swapChainDesc1.Stereo = false;									//スワップエフェクトがシーケンシャルモードならtrue
	swapChainDesc1.SampleDesc.Count = 1;							//マルチサンプリング関連
	swapChainDesc1.SampleDesc.Quality = 0;							//
	swapChainDesc1.Scaling = DXGI_SCALING_STRETCH;					//裏画面のサイズに合わせてバックバッファをリサイズ
	swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		//フリップ前の表画面をフリップ後リセット(フリップ時初期化)
	swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			//特に指定しない
	//ウィンドウモード、フルスクリーンモードの切り替え時に
	//そのときのウィンドウサイズに合わせて表示モードも切り替える
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
	//ウィンドウサイズを取得するためにスワップチェーンの説明文を取得
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1;
	auto result = _swapChain->GetDesc1(&swapChainDesc1);
	assert(SUCCEEDED(result));
	
	//表裏画面用の２つのバッファを確保する
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
	//バックバッファ毎にRTVを作成
	auto rtvDescriptorHeapHandle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < swapChainDesc1.BufferCount; ++i) {
		//'_backBuffers[i]'に表裏画面用のバッファを代入
		result = _swapChain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
		assert(SUCCEEDED(result));

		rtvDesc.Format = _backBuffers[i]->GetDesc().Format;
		_device->CreateRenderTargetView(_backBuffers[i], &rtvDesc, rtvDescriptorHeapHandle);

		//次のRTVを参照
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
	sceneResDesc.Width = (sizeof(SceneData) + 0xff) & ~0xff; //256の倍数に区切ってリソースを確保(ムダも生じる)
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

	//ビューを設定
	_mapSceneData->view = XMMatrixLookAtLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&target), XMLoadFloat3(&up));

	//ウィンドウサイズを取得するためにスワップチェーンの設定を取得
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	_swapChain->GetDesc1(&desc);

	//プロジェクションを設定
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

//画面に描画していたものをクリアし、深度値を適応させて隠面消去を有効にする。
// NOTE: リソース状態がレンダーターゲット状態に移行する。
void DX12Wrapper::BeginRender()
{
	//現在のバックバッファのインデックスを取得(現在、0番目、1番目どちらの画面バッファが裏画面か)
	auto currentBackBuffIdx = _swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER resBarrierDesc = {};
	resBarrierDesc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resBarrierDesc.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resBarrierDesc.Transition.pResource   = _backBuffers[currentBackBuffIdx];
	resBarrierDesc.Transition.Subresource = 0;
	resBarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		//待機状態から
	resBarrierDesc.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;	//レンダーターゲット状態へ遷移
	_commandList->ResourceBarrier(1, &resBarrierDesc);

	//現在のバックバッファ用のRTVを参照する
	auto rtvDescHeapHandle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvDescHeapHandle.ptr += currentBackBuffIdx * _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//現在のレンダーターゲットを'clearColor'でクリア
	_commandList->ClearRenderTargetView(rtvDescHeapHandle, &_bgColor.x, 0, nullptr);

	auto dsvDescHeapHandle = _dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//'clearColor'でクリアしたレンダーターゲットと現在の深度値を適応
	_commandList->OMSetRenderTargets(
		1,
		&rtvDescHeapHandle, 
		false,
		&dsvDescHeapHandle
	);

	//現在の深度値をセットし終えたから、次の深度値計算のために深度値クリア
	_commandList->ClearDepthStencilView(dsvDescHeapHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, 0, nullptr);

	_commandList->RSSetViewports(1, &_viewPort);
	_commandList->RSSetScissorRects(1, &_scissorRect);
}

//溜め込んでいた命令を一気に、溜め込んだ順に実行していく。
// NOTE: リソース状態が待機状態に移行する。
void DX12Wrapper::EndRender()
{
	// NOTE: この関数内で新たにパイプラインを設定しているので、これ以降独自の命令を追加しないこと。
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());

	//現在のバックバッファのインデックスを取得(現在、0番目、1番目どちらの画面バッファが裏画面か)
	auto backBuffIdx = _swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER resBarrierDesc = {};
	resBarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resBarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resBarrierDesc.Transition.pResource = _backBuffers[backBuffIdx];
	resBarrierDesc.Transition.Subresource = 0;
	resBarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	//レンダーターゲット状態から
	resBarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;		//待機状態へ遷移
	_commandList->ResourceBarrier(1, &resBarrierDesc);

	//NOTE: 待機状態でクローズすること
	_commandList->Close();

	ID3D12CommandList* cmdLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	//GPU側のフェンス値(GPUにSignal()関数でパスしている)と、CPU側のフェンス値(_fenceValueをインクリメントしている)が
	//一致する、即ち実行終了までの間処理を待ち続ける。詳細は → https://bit.ly/3sxJSrr
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
