#include<MyPhysics.h>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

FMyVec3 kGravity = MyVec3(0.f, -9.8f, 0.f);

MyPhysics::MyPhysics(int frames, FMyVec3& acc, FMyVec3& vel, FMyVec3& pos)
	:_dt(1.f / frames), _acc(acc), _vel(vel), _pos(pos)
{}

MyPhysics::MyPhysics()
	: MyPhysics(30, kZeroVec, kZeroVec, kZeroVec)
{}

void MyPhysics::Update()
{
	CheckOutputPhysicsParam();

	_pos = GetPositionIn(_dt);
	_vel = GetVelocityIn(_dt);
}

MyVec3 MyPhysics::GetVelocityIn(float time) const
{
	MyVec3 dv(_acc * _dt);
	return _vel + time / _dt * dv;
}

MyVec3 MyPhysics::GetVelocityIn(float time, FMyVec3& acc) const
{
	MyVec3 dv(acc * _dt);
	return _vel + time / _dt * dv;
}

MyVec3 MyPhysics::GetPositionIn(float time) const
{
	MyVec3 dp(_vel * _dt + 0.5f * _acc * _dt * _dt);
	return _pos + time / _dt * dp;
}

MyVec3 MyPhysics::GetPositionIn(float time, FMyVec3& acc, FMyVec3& vel) const
{
	MyVec3 dp(vel * _dt + 0.5f * acc * _dt * _dt);
	return _pos + time / _dt * dp;
}

//vp : ���̂̌��ݑ��x
//vw : �����̂̌��ݑ��x
//normal : �Փ˖ʂ̖@���x�N�g��
//e : �����W��
MyVec3 MyPhysics::CalcReflectionVector(FMyVec3& vp, FMyVec3& vw, FMyVec3& normal, float e)
{
	return vp - (1 + e) * Dot(vp - vw, normal) * normal;
}

//v1, v2 : �Q�̋��̂̌��ݑ��x
//p1, p2 : �Q�̋��̂̌��݈ʒu
//m1, m2 : �Q�̋��̂̎���
//e1, e2 : �Q�̋��̂̔����W��
//out_v1, out_v2 : �Q�̋��̂̏Փˌ�̑��x��Ԃ�
void MyPhysics::CalcReflectionVector(
	FMyVec3& v1, FMyVec3& v2,
	FMyVec3& p1, FMyVec3& p2,
	float m1, float m2, float e1, float e2,
	MyVec3& out_v1, MyVec3& out_v2
) 
{
	FMyVec3 kC = Normalize(p2 - p1); //���K��C�x�N�g��

	float dot = Dot(v1 - v2, kC);
	float e = 1 + e1 * e2;
	float div_m = 1.f / (m1 + m2);

	out_v1 = -m2 * div_m * e * dot * kC + v1;
	out_v2 =  m1 * div_m * e * dot * kC + v2;
}

//@param vel : ���x�x�N�g��
//@param normal : ���K�����ꂽ�Փ˕��ʂ̖@��
MyVec3 MyPhysics::CalcVectorAlongAABB3(FMyVec3& vel, FMyVec3& normal)
{
	return vel - Dot(vel, normal) * normal;
}

MyPhysics::~MyPhysics()
{
	//���ɂȂ�
}

void MyPhysics::CheckOutputPhysicsParam()
{
#if 0
	cout << "_acc:" << _acc << endl;
	cout << "_vel:" << _vel << endl;
	cout << "_pos:" << _pos << endl;
	cout << endl;
#endif
}

