#pragma once

#include<BasicObject.h>
#include<MyPhysics.h>

class DX12Wrapper;
class AABB3;

class MyRectangle : public BasicObject, public MyPhysics {
public:
	//プリミティブの描画の仕方
	static constexpr D3D_PRIMITIVE_TOPOLOGY _kPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//衝突計算用に使用
	std::unique_ptr<AABB3> _aabb3;

	//衝突時刻
	float  _t;

	//速度や位置の中間結果を保持する目的で利用する
	MyVec3 _tempPos;
	MyVec3 _tempVel;

	MyVec3 _init_acc;
	MyVec3 _init_vel;
	MyVec3 _init_pos;
	MyVec3 _init_size;
	float  _init_m;
	float  _init_e;
	float  _init_dt;
	MyVec3 _init_color;

	MyRectangle(
		DX12Wrapper* dx12, const Char32& name,
		const MyVec3& acc, const MyVec3& vel, const MyVec3& pos, const MyVec3& size,
		float m = 1.0f, float e = 0.5f, int frames = 30, const MyVec3& col = MyVec3(1.f, 1.f, 1.f)
	);
	MyRectangle(
		DX12Wrapper* dx12,
		const MyVec3& acc, const MyVec3& vel, const MyVec3& pos, const MyVec3& size,
		float m = 1.0f, float e = 0.5f, int frames = 30, const MyVec3& col = MyVec3(1.f, 1.f, 1.f)
	);

	void BeginCalc();
	void Update();
	void EndCalc();
	void Render() const;
	void Adjuster();
	void Reset();

private:
	static int _id;

	HRESULT CreateVertexBufferView();
	HRESULT CreateIndexBufferView();
};
