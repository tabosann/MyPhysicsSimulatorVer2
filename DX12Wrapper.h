#pragma once

class DX12Wrapper {
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	SIZE              _windowSize;
	DirectX::XMFLOAT4 _bgColor, _initBgColor;
	float             _fov       , _initFov;
	DirectX::XMFLOAT3 _cameraPos , _initCamera;
	DirectX::XMFLOAT4 _lightDir  , _initLightDir; //�A���C�����g�h�~��XMFLOAT4

	DX12Wrapper(HWND hwnd, int width, int height);
	virtual ~DX12Wrapper();

	ID3D12Device*                GetDevice() const;
	ID3D12GraphicsCommandList*   GetCmdList() const;
	IDXGISwapChain4*             GetSwapChain() const;
	ComPtr<ID3D12DescriptorHeap> GetDescHeapForImGui() const;

	void SetBackGroundColor(const DirectX::XMFLOAT4& bgColor);
	void SetFieldOfView(float fov);
	void SetCameraPos(const DirectX::XMFLOAT3& camera);
	void SetLightDirection(const DirectX::XMFLOAT4& dir);
	void SetViewPort(D3D12_VIEWPORT* view, D3D12_RECT* rect);
	void SetSceneDataToShader();

	void BeginRender();
	void EndRender();
	void Update();
	void Reset();

private:
	//DXGI�֘A
	ComPtr<IDXGIFactory6>   _dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> _swapChain = nullptr;
	
	//DirectX12�֘A
	ComPtr<ID3D12Device>              _device = nullptr;
	ComPtr<ID3D12CommandAllocator>    _commandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
	ComPtr<ID3D12CommandQueue>        _commandQueue = nullptr;

	//�[�x�l�Ɋւ���o�b�t�@�֘A
	ComPtr<ID3D12Resource>       _depthBuffer = nullptr;
	ComPtr<ID3D12DescriptorHeap> _dsvDescriptorHeap = nullptr;
	//�����_�[�^�[�Q�b�g�Ɋւ���o�b�t�@�֘A
	std::vector<ID3D12Resource*> _backBuffers;
	ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap = nullptr;
	//�`��͈͂�ݒ肷��p�����[�^
	D3D12_VIEWPORT _viewPort = {};    //�w�肵�����W����ǂ̂��炢�͈̔͂�`��̈�ɂ��邩
	D3D12_RECT     _scissorRect = {}; //�r���[�|�[�g�͈͓��ł���ɕ`��͈͂�ݒ肷��

	//�r���[�v���W�F�N�V�����֘A
	struct SceneData {
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		DirectX::XMFLOAT3 eyePos;
		DirectX::XMFLOAT4 lightDir;
	};
	SceneData*                   _mapSceneData = nullptr;
	ComPtr<ID3D12Resource>       _sceneBuffer = nullptr;
	ComPtr<ID3D12DescriptorHeap> _sceneDescriptorHeap = nullptr;

	//ImGui�Ɏg�p����f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> _descHeapForImGui = nullptr;

	//�t�F���X�֘A
	ComPtr<ID3D12Fence> _fence = nullptr; //GPU����'������'�v�Z�����t�F���X�l
	UINT64              _fenceValue = 0;  //CPU����'�蓮��'�v�Z����t�F���X�l

	//�A�_�v�^�̐������̒���'targetAdapterName'�̕�������܂ރA�_�v�^���f�o�C�X�ɂ���B
	//��������ݒ肵�Ȃ���΁A�����ŃA�_�v�^��I�����ăf�o�C�X�ɐݒ肷��B
	// NOTE: �f�o�C�X�쐬��Ƀf�o�b�O���C���[��L�������Ă͂Ȃ�Ȃ��B�f�o�C�X����菜����Ă��܂��B
	HRESULT CreateDXGIAndDevice(const std::wstring& targetAdapterName = L"");
	HRESULT CreateCommandFlow();
	//'hwnd'�Ŏw�肵���E�B���h�E��`��Ώۂɂ���i�X���b�v�`�F�[�����쐬�j�B
	//NOTE: �X���b�v�`�F�[���̍쐬���s�������ŁARTV�̍쐬�͍s��Ȃ�
	HRESULT CreateSwapChain(const HWND& hwnd, int windowWidth, int windowHeight);
	//�X���b�v�`�F�[����RTV��K�������A�����_�[�^�[�Q�b�g������������B�r���[�|�[�g�ƃV�U�[���N�g�̐ݒ���s���B
	// NOTE: �X���b�v�`�F�[���̍쐬�����łɍς�ł��邱�Ƃ��z�肳��Ă���
	HRESULT CreateFinalRenderTarget();
	HRESULT CreateDepthStencilView();
	HRESULT CreateSceneDataViewWithCBV();
};

extern const float kSkyColor4[4];