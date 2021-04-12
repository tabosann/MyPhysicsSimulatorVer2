#pragma once

class DX12Wrapper 
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:

	SIZE     _windowSize;
	XMFLOAT4 _bgColor;
	float    _fov;
	XMFLOAT3 _cameraPos;
	XMFLOAT4 _lightDir; //アライメント防止のXMFLOAT4

	DX12Wrapper(HWND hwnd, int width, int height);

	void SetSceneDataToShader();
	void BeginRender();
	void EndRender();
	void Update();
	void Reset();

	ID3D12Device*                GetDevice() const;
	ID3D12GraphicsCommandList*   GetCmdList() const;
	IDXGISwapChain4*             GetSwapChain() const;
	ComPtr<ID3D12DescriptorHeap> GetDescHeapForImGui() const;

	void SetViewPort(D3D12_VIEWPORT* view, D3D12_RECT* rect);

	virtual ~DX12Wrapper();

private:

	XMFLOAT4 _initBgColor;
	float    _initFov;
	XMFLOAT3 _initCamera;
	XMFLOAT4 _initLightDir;

	//DXGI関連
	ComPtr<IDXGIFactory6>   _dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> _swapChain = nullptr;
	
	//DirectX12関連
	ComPtr<ID3D12Device>              _device = nullptr;
	ComPtr<ID3D12CommandAllocator>    _commandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
	ComPtr<ID3D12CommandQueue>        _commandQueue = nullptr;

	//深度値に関するバッファ関連
	ComPtr<ID3D12Resource>       _depthBuffer = nullptr;
	ComPtr<ID3D12DescriptorHeap> _dsvDescriptorHeap = nullptr;
	//レンダーターゲットに関するバッファ関連
	std::vector<ID3D12Resource*> _backBuffers;
	ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap = nullptr;
	//描画範囲を設定するパラメータ
	D3D12_VIEWPORT _viewPort = {};    //指定した座標からどのくらいの範囲を描画領域にするか
	D3D12_RECT     _scissorRect = {}; //ビューポート範囲内でさらに描画範囲を設定する

	//ビュープロジェクション関連
	struct SceneData {
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT3 eyePos;
		XMFLOAT4 lightDir;
	};
	SceneData*                   _mapSceneData = nullptr;
	ComPtr<ID3D12Resource>       _sceneBuffer = nullptr;
	ComPtr<ID3D12DescriptorHeap> _sceneDescriptorHeap = nullptr;

	//ImGuiに使用するディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> _descHeapForImGui = nullptr;

	//フェンス関連
	ComPtr<ID3D12Fence> _fence = nullptr; //GPU側で'自動で'計算されるフェンス値
	UINT64              _fenceValue = 0;  //CPU側で'手動で'計算するフェンス値

	//アダプタの説明文の中に'targetAdapterName'の文字列を含むアダプタをデバイスにする。
	//仮引数を設定しなければ、自動でアダプタを選択してデバイスに設定する。
	// NOTE: デバイス作成後にデバッグレイヤーを有効化してはならない。デバイスが取り除かれてしまう。
	HRESULT CreateDXGIAndDevice(const std::wstring& targetAdapterName = L"");
	HRESULT CreateCommandFlow();
	//'hwnd'で指定したウィンドウを描画対象にする（スワップチェーンを作成）。
	//NOTE: スワップチェーンの作成を行うだけで、RTVの作成は行わない
	HRESULT CreateSwapChain(const HWND& hwnd, int windowWidth, int windowHeight);
	//スワップチェーンにRTVを適応させ、レンダーターゲットを完成させる。ビューポートとシザーレクトの設定を行う。
	// NOTE: スワップチェーンの作成がすでに済んでいることが想定されている
	HRESULT CreateFinalRenderTarget();
	HRESULT CreateDepthStencilView();
	HRESULT CreateSceneDataViewWithCBV();

	DX12Wrapper(const DX12Wrapper&) = delete;
	DX12Wrapper& operator=(const DX12Wrapper&) = delete;
};

extern const float kSkyColor4[4];