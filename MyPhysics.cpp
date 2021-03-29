#include<MyPhysics.h>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

const MyVec3 kGravity = MyVec3(0.f, -9.8f, 0.f);

MyPhysics::MyPhysics(int frames, const MyVec3& acc, const MyVec3& vel, const MyVec3& pos)
	:_dt(1.f / frames), _acc(acc), _vel(vel), _pos(pos)
{}

MyPhysics::MyPhysics()
	: MyPhysics(30, kZeroVec, kZeroVec, kZeroVec)
{}

MyPhysics::~MyPhysics() {

}

void MyPhysics::Update() {
	CheckOutputPhysicsParam();

	_pos = GetPositionIn(_dt);
	_vel = GetVelocityIn(_dt);
}

MyVec3 MyPhysics::GetVelocityIn(float time) const {
	MyVec3 dv(_acc * _dt);
	return _vel + time / _dt * dv;
}

MyVec3 MyPhysics::GetVelocityIn(float time, const MyVec3& acc) const {
	MyVec3 dv(acc * _dt);
	return _vel + time / _dt * dv;
}

MyVec3 MyPhysics::GetPositionIn(float time) const {
	MyVec3 dp(_vel * _dt + 0.5f * _acc * _dt * _dt);
	return _pos + time / _dt * dp;
}

MyVec3 MyPhysics::GetPositionIn(float time, const MyVec3& acc, const MyVec3& vel) const {
	MyVec3 dp(vel * _dt + 0.5f * acc * _dt * _dt);
	return _pos + time / _dt * dp;
}

//@param vp 球体の現在速度
//@param vw 直方体の現在速度
//@param normal 衝突面の法線ベクトル
//@param e 反発係数
MyVec3 MyPhysics::CalcReflectionVector(const MyVec3& vp, const MyVec3& vw, const MyVec3& normal, float e) {
	return vp - (1 + e) * Dot(vp - vw, normal) * normal;
}

//@param v1, v2 ２つの球体の現在速度
//@param p1, p2 ２つの球体の現在位置
//@param m1, m2 ２つの球体の質量
//@param e1, e2 ２つの球体の反発係数
//@param outV1, outV2 ２つの球体の衝突後の速度を返す
void MyPhysics::CalcReflectionVector(
	const MyVec3& v1, const MyVec3& v2,
	const MyVec3& p1, const MyVec3& p2,
	float m1, float m2, float e1, float e2,
	MyVec3& outV1, MyVec3& outV2
) {
	const MyVec3 kC = Normalize(p2 - p1); //正規化Cベクトル

	float dot_dv_c = Dot(v1 - v2, kC);
	float e = 1 + e1 * e2;
	
	outV1 = -m2 / (m1 + m2) * e * dot_dv_c * kC + v1;
	outV2 =  m1 / (m1 + m2) * e * dot_dv_c * kC + v2;
}

//@param vel : 速度ベクトル
//@param normal : 正規化された衝突平面の法線
MyVec3 MyPhysics::CalcVectorAlongAABB3(const MyVec3& vel, const MyVec3& normal) {
	return vel - Dot(vel, normal) * normal;
}

void MyPhysics::CheckOutputPhysicsParam() {
#if 0
	cout << "_acc:" << _acc << endl;
	cout << "_vel:" << _vel << endl;
	cout << "_pos:" << _pos << endl;
	cout << endl;
#endif
}

