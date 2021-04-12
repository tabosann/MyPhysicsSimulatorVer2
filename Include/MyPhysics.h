#pragma once

#include<MyVector3.h>

class MyPhysics
{
public:

	float  _dt;	 //1�t���[�����̌o�ߎ���[sec]
	MyVec3 _acc; //[m/s^2]
	MyVec3 _vel; //[m/s]
	MyVec3 _pos; //[m]

	//@param frames : 1�t���[�����̌o�ߎ���[sec]
	//@param acc : �����x[m/s^2]
	//@param vel : ���x[m/s]
	//@param pos : �ʒu���W[m]
	MyPhysics(int frames, FMyVec3& acc, FMyVec3& vel, FMyVec3& pos);
	MyPhysics();

	virtual void Update() = 0;

	//���݂̑��x����'time'�b��̑��x���Z�o
	// NOTE: �����x'_acc'�Ɉˑ����邽�߁A��ɉ����x���X�V���Ă͂����Ȃ��B
	MyVec3 GetVelocityIn(float time) const;
	MyVec3 GetVelocityIn(float time, FMyVec3& acc) const;
	//���݂̈ʒu����'time'�b��̈ʒu���Z�o
	// NOTE: �����x'_acc'�Ƒ��x'_vel'�Ɉˑ����邽�߁A��ɉ����x�Ƒ��x���X�V���Ă͂����Ȃ��B
	MyVec3 GetPositionIn(float time) const;
	MyVec3 GetPositionIn(float time, FMyVec3& acc, FMyVec3& vel) const;

	//AABB3��BS3�̏Փ˂ɂ�锽�˃x�N�g�����Z�o
	static MyVec3 CalcReflectionVector(FMyVec3& vp, FMyVec3& vw, FMyVec3& normal, float e);
	//BS3��BS3�̏Փ˂ɂ�锽�˃x�N�g�����Z�o
	static void CalcReflectionVector(
		FMyVec3& v1, FMyVec3& v2,
		FMyVec3& p1, FMyVec3& p2,
		float m1, float m2, float e1, float e2,
		MyVec3& out_v1, MyVec3& out_v2
	);
	//BS3��AABB3�̕ǂ���x�N�g�����Z�o
	static MyVec3 CalcVectorAlongAABB3(FMyVec3& vel, FMyVec3& normal);

	virtual ~MyPhysics();

private:

	void CheckOutputPhysicsParam();
};

extern FMyVec3 kGravity;