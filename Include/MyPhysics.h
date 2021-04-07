#pragma once

#include<MyVector3.h>

class MyPhysics {
public:
	float  _dt;	 //1�t���[�����̌o�ߎ���[sec]
	MyVec3 _acc; //[m/s^2]
	MyVec3 _vel; //[m/s]
	MyVec3 _pos; //[m]

	//@param frames : 1�t���[�����̌o�ߎ���[sec]
	//@param acc : �����x[m/s^2]
	//@param vel : ���x[m/s]
	//@param pos : �ʒu���W[m]
	MyPhysics(int frames, const MyVec3& acc, const MyVec3& vel, const MyVec3& pos);
	MyPhysics();
	virtual ~MyPhysics();

	virtual void Update() = 0;

	//���݂̑��x����'time'�b��̑��x���Z�o
	//NOTE: �����x'_acc'�Ɉˑ����邽�߁A��ɉ����x���X�V���Ă͂����Ȃ��B
	MyVec3 GetVelocityIn(float time) const;
	MyVec3 GetVelocityIn(float time, const MyVec3& acc) const;
	//���݂̈ʒu����'time'�b��̈ʒu���Z�o
	//NOTE: �����x'_acc'�Ƒ��x'_vel'�Ɉˑ����邽�߁A��ɉ����x�Ƒ��x���X�V���Ă͂����Ȃ��B
	MyVec3 GetPositionIn(float time) const;
	MyVec3 GetPositionIn(float time, const MyVec3& acc, const MyVec3& vel) const;

	//AABB3��BS3�̏Փ˂ɂ�锽�˃x�N�g�����Z�o
	static MyVec3 CalcReflectionVector(const MyVec3& vp, const MyVec3& vw, const MyVec3& normal, float e);
	//BS3��BS3�̏Փ˂ɂ�锽�˃x�N�g�����Z�o
	static void CalcReflectionVector(
		const MyVec3& v1, const MyVec3& v2,
		const MyVec3& p1, const MyVec3& p2,
		float m1, float m2, float e1, float e2,
		MyVec3& outV1, MyVec3& outV2
	);
	//BS3��AABB3�̕ǂ���x�N�g�����Z�o
	static MyVec3 CalcVectorAlongAABB3(const MyVec3& vel, const MyVec3& normal);

private:
	//----------------------------------------
	//	�f�o�b�O�֐��Q
	//----------------------------------------
	//�����p�����[�^�̐��l���m�F
	void CheckOutputPhysicsParam();
};

extern const MyVec3 kGravity;