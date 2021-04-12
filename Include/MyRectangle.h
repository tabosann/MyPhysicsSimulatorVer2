#pragma once

#include<BasicObject.h>
#include<MyPhysics.h>

class DX12Wrapper;
class AABB3;

class MyRectangle : public BasicObject, public MyPhysics
{
	template<class T>
	using unique_ptr = std::unique_ptr<T>;

public:

	//�v���~�e�B�u�̕`��̎d��
	static constexpr D3D_PRIMITIVE_TOPOLOGY _kPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	unique_ptr<AABB3> _aabb3; //�q�b�g�{�b�N�X
	float             _t;     //�Փˎ���

	//���x��ʒu�̒��Ԍ��ʂ�ێ�����ړI�ŗ��p����
	MyVec3 _tempPos;
	MyVec3 _tempVel;

	MyRectangle(
		DX12Wrapper*  dx12, 
		const Char32& name,
		const MyVec3& acc, 
		const MyVec3& vel, 
		const MyVec3& pos, 
		const MyVec3& size,
		float         m = 1.0f, 
		float         e = 0.5f, 
		int           frames = 30, 
		const MyVec3& col = MyVec3(1.f, 1.f, 1.f)
	);
	MyRectangle(
		DX12Wrapper*  dx12,
		const MyVec3& acc, 
		const MyVec3& vel, 
		const MyVec3& pos, 
		const MyVec3& size,
		float         m = 1.0f,
		float         e = 0.5f, 
		int           frames = 30, 
		const MyVec3& col = MyVec3(1.f, 1.f, 1.f)
	);

	void BeginCalc();
	void Update();
	void EndCalc();
	void Render() const;
	void Adjuster();
	void Reset();

private:

	static int _id;

	MyVec3 _init_acc;
	MyVec3 _init_vel;
	MyVec3 _init_pos;
	MyVec3 _init_size;
	float  _init_m;
	float  _init_e;
	float  _init_dt;
	MyVec3 _init_color;

	HRESULT CreateVertexBufferView();
	HRESULT CreateIndexBufferView();
};