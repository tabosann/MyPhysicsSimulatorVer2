#pragma once

#include"../MyHelpers/MyVector3.h"
#include"../MyHelpers/MyChar32.h"

class DX12Wrapper;
class MySphere;

//すべての物体に共通する要素を詰め込んだクラス
//・頂点情報
//・頂点バッファビュー
//・インデックスバッファビュー
//・アフィン変換行列
class BasicObject {
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//フレーム毎にアップデートされるデータを扱う構造体
	struct Transform {
		//NOTE: DirectX::XMMATRIXが16の倍数バイトのみを扱うので
		//    : 16の倍数にアライメントしなければいけない。
		void* operator new(size_t size);
		DirectX::XMMATRIX world;
		DirectX::XMFLOAT4 color;
	};

public:
	//１つの頂点に載せる情報
	//NOTE: Vertex構造体を変更したら'_inputLayout'配列も変更すること
	struct Vertex {
		DirectX::XMFLOAT3 pos;	  //位置[m]
		DirectX::XMFLOAT3 normal; //法線ベクトル
		DirectX::XMFLOAT3 color;  //色(R, G, B)
	};

	//この構造体から'world'行列にアクセスし、アフィン変換を行う。
	//e.g. _mappedTransform->world = XMMatrixTranslation();
	Transform* _mappedTransform;

	Char32 _name;
	MyVec3 _size;
	float  _m; //質量[kg]
	float  _e; //反発係数
	MyVec3 _color;

	//@param dx12 : DX12Wrapperクラスオブジェクト
	BasicObject(DX12Wrapper* dx12, const Char32& name, const MyVec3& size, float m, float e, const MyVec3& col);
	virtual ~BasicObject();

	//用意された頂点データをシェーダに渡す準備をする
	HRESULT CreateVertexBufferView(Vertex* begin, Vertex* end, size_t sizeOfVertices);
	//用意された頂点データのインデックスでプリミティブ描画の順番を決定する。
	HRESULT CreateIndexBufferView(unsigned short* begin, unsigned short* end, size_t sizeOfIndices);

	virtual void Update();
	virtual void Render() const;
	virtual void Adjuster() = 0;

private:
	//コマンドリストとデバイスの取得に使用
	DX12Wrapper*                 _dx12;
	//物体を構成する頂点とインデックスの準備
	ComPtr<ID3D12Resource>       _vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW     _vertexBufferView = {};
	ComPtr<ID3D12Resource>       _indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW      _indexBufferView = {};
	//アフィン変換行列の準備
	ComPtr<ID3D12Resource>       _transformBuffer;
	ComPtr<ID3D12DescriptorHeap> _transformDescriptorHeap;

	//アフィン変換行列をシェーダに渡すための準備
	HRESULT CreateTransformViewWithCBV();
};
