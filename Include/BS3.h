#pragma once

#include"../MyHelpers/MyVector3.h"

//Bounding Sphere(���^�̓����蔻��)
class BS3 {
public:
	MyVec3 _c; //���̒��S
	float     _r; //���̔��a

	BS3(const MyVec3& c, float r);
	BS3();

	float GetSurfaceAreas() const; //�\�ʐ�
	float GetVolumes()      const; //�̐�

	//���Ƌ��̌������v�Z
	bool IntersectsBS3(
		const MyVec3& p1_first, const MyVec3& p1_last, float r1, 
		const MyVec3& p2_first, const MyVec3& p2_last, float r2, 
		MyVec3* outPos1, MyVec3* outPos2, float* t
	);

	bool IntersectsBS3(
		const MyVec3& p1_first, const MyVec3& p1_last, float r1,
		const MyVec3& p2_first, const MyVec3& p2_last, float r2,
		MyVec3& outV1, MyVec3& outV2, MyVec3& outP1, MyVec3& outP2, float& outStep
	) const;

	bool IntersectsBS3(
		const MyVec3& a1, const MyVec3& v1, const MyVec3& p1, float r1, float t1,
		const MyVec3& a2, const MyVec3& v2, const MyVec3& p2, float r2, float t2,
		MyVec3& outV1, MyVec3& outV2, MyVec3& outP1, MyVec3& outP2, float& outStep
	) const;
};