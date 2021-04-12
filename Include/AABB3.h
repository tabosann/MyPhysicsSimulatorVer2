#pragma once

#include<MyVector3.h>

//�o�E���f�B���O�{�b�N�X(���^�̓����蔻��)
class AABB3
{
public:

	MyVec3 _min;
	MyVec3 _max;

	AABB3(FMyVec3& min, FMyVec3& max);
	//�f�t�H���g�R���X�g���N�^�ŁA�{�b�N�X����ɃZ�b�g����
	// NOTE: ��̒�`�� _min > _max �ƂȂ邱�Ƃł���B
	AABB3();

	MyVec3 GetCenterPos() const;
	MyVec3 GetSizeVec()   const; //�{�b�N�X�̑Ίp�����ԃx�N�g��
	MyVec3 GetRadiusVec() const; //�{�b�N�X�̑Ίp�����ԃx�N�g���̔���
	
	//AABB���`������8�̒��_�̂���1���擾
	MyVec3 GetCorner(int i) const;

	//�Փ˂��N���镽��(= ���̕��ʂ̖@���ƕ��ʏ�̓_)���擾
	void GetPlane(
		FMyVec3& rayOrg, FMyVec3& rayDir, 
		MyVec3& outNormal, MyVec3& outPosOnPlane
	) const;

	//���AABB�ł���΁Atrue��Ԃ�
	bool IsEmpty() const;

	//AABB�𒸓_'add'���܂ł��邾���̃T�C�Y�Ɋg�傷��
	void Add(MyVec3 add);

	//'pos'�Ɉ�ԋ߂�AABB3��̓_���擾
	//'pos'��AABB3���ɂ���΂��̂܂�'pos'��Ԃ�
	MyVec3 Closest(FMyVec3& pos) const;

	//������AABB3�Ƃ̌������v�Z���A���������݂����true��Ԃ�
	bool RayIntersect(
		FMyVec3& rayOrg, FMyVec3& rayDir,
		MyVec3* outNormal, MyVec3* outColPos, float* t
	) const;

	//AABB3��BS3�̏Փ˂��v�Z����
	bool IntersectsBS3(
		FMyVec3& spherePos0, FMyVec3& spherePos1, float sphereRadius,
		FMyVec3& posOnPlane, FMyVec3& normal,
		MyVec3& outSpherePos, float& outStep
	) const;

	bool IntersectsBS3(
		FMyVec3& acc, FMyVec3& vel, FMyVec3& pos, float radius, float step,
		FMyVec3& posOnPlane, FMyVec3& normal, MyVec3& outVel, MyVec3& outPos, float& outStep
	) const;
};
