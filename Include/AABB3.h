#pragma once

#include"../MyHelpers/MyVector3.h"

//�o�E���f�B���O�{�b�N�X(���^�̓����蔻��)
class AABB3 {
public:
	MyVector3 _min;
	MyVector3 _max;

	AABB3(const MyVector3& min, const MyVector3& max);
	//�f�t�H���g�R���X�g���N�^�ŁA�{�b�N�X����ɃZ�b�g����
	// NOTE: ��̒�`�� _min > _max �ƂȂ邱�Ƃł���B
	AABB3();

	MyVector3 GetCenterPos() const;
	MyVector3 GetSizeVec()   const; //�{�b�N�X�̑Ίp�����ԃx�N�g��
	MyVector3 GetRadiusVec() const; //�{�b�N�X�̑Ίp�����ԃx�N�g���̔���
	
	//AABB���`������8�̒��_�̂���1���擾
	MyVector3 GetCorner(int i) const;

	//�Փ˂��N���镽��(= ���̕��ʂ̖@���ƕ��ʏ�̓_)���擾
	void GetPlane(
		const MyVector3& rayOrg, const MyVector3& rayDir, 
		MyVector3& outNormal, MyVector3& outPosOnPlane
	) const;

	//���AABB�ł���΁Atrue��Ԃ�
	bool IsEmpty() const;

	//AABB�𒸓_'add'���܂ł��邾���̃T�C�Y�Ɋg�傷��
	void Add(MyVector3 add);

	//'pos'�Ɉ�ԋ߂�AABB3��̓_���擾
	//'pos'��AABB3���ɂ���΂��̂܂�'pos'��Ԃ�
	MyVector3 Closest(const MyVector3& pos) const;

	//������AABB3�Ƃ̌������v�Z���A���������݂����true��Ԃ�
	bool RayIntersect(
		const MyVector3& rayOrg, const MyVector3& rayDir,
		MyVector3* outNormal, MyVector3* outColPos, float* t
	) const;

	//AABB3��BS3�̏Փ˂��v�Z����
	bool IntersectsBS3(
		const MyVector3& spherePos0, const MyVector3& spherePos1, float sphereRadius,
		const MyVector3& posOnPlane, const MyVector3& normal,
		MyVector3& outSpherePos, float& outStep
	) const;

	bool IntersectsBS3(
		const MyVector3& acc, const MyVector3& vel, const MyVector3& pos, float radius, float step,
		const MyVector3& posOnPlane, const MyVector3& normal, MyVector3& outVel, MyVector3& outPos, float& outStep
	) const;
};
