#include<AABB3.h>
#include<functional>
#include<algorithm>

using namespace std;

namespace {
	float EnforceValueIntoRange(float min, float max, float target);
	int   Range(unsigned int seed, int min, int max);
}

AABB3::AABB3(const MyVector3& min, const MyVector3& max) 
	:_min(min), _max(max)
{}

AABB3::AABB3()
	:AABB3(MyVector3(FLT_MAX, FLT_MAX, FLT_MAX), MyVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
{}

MyVector3 AABB3::GetCenterPos() const {
	return (_max + _min) / 2;
}

MyVector3 AABB3::GetSizeVec() const {
	return _max - _min;
}

MyVector3 AABB3::GetRadiusVec() const {
	return (_max - _min) / 2;
}

MyVector3 AABB3::GetCorner(int i) const {
	//�C���f�b�N�X���͈͓��ł��邩�`�F�b�N
	assert(0 <= i && i < 8);

	return MyVector3(
		(i & 0x2) ? _max.x : _min.y, //i == 2,3,6,7
		(i & 0x1) ? _max.y : _min.y, //i == 1,3,5,7
		(i & 0x4) ? _max.z : _min.z  //i == 4,5,6,7
	);
}

//@param rayOrg : ���_
//@param rayDir : ����
//@param outPlaneNormal : ���ʂ̖@��
//@param outPosOnPlane : ���ʏ�̓_
void AABB3::GetPlane(
	const MyVector3& rayOrg, const MyVector3& rayDir,
	MyVector3& outPlaneNormal, MyVector3& outPosOnPlane
) const {
	function<void(float, float, float&, float&)> Dist;
	Dist = [=](float min, float max, float& d, float& n)->void {
		if (d < min) {
			d = min - d;
			n = -1.0f;
		}
		else if (d > max) {
			d = d - max;
			n = 1.0f;
		} 
		else {
			d = -1.0f;
		}
	};

	//�e������AABB3�̕��ʂ܂ł̍ŒZ�����Ɩ@�����Z�o
	float nx, ny, nz;
	float dx = rayOrg.x, dy = rayOrg.y, dz = rayOrg.z;
	Dist(_min.x, _max.x, dx, nx);
	Dist(_min.y, _max.y, dy, ny);
	Dist(_min.z, _max.z, dz, nz);

	//���_���{�b�N�X���ɑ��݂���ꍇ�́A�����Ɛ^�t�̖@����Ԃ�
	if (dx == -1 && dy == -1 && dz == -1) {
		outPlaneNormal = -rayDir.Normalize();
		outPosOnPlane = rayOrg;
		return;
	}

	//'rayOrg'����AABB3�܂ň�ԋ����������Ȃ镽�ʂ�I��
	int whichPlane = 0;
	float max = dx;
	if (dy > max) {
		whichPlane = 1;
		max = dy;
	}
	if (dz > max) {
		whichPlane = 2;
		max = dz;
	}

	switch (whichPlane) {
	case 0: //yz����
		outPlaneNormal = MyVector3(nx, 0, 0);
		outPosOnPlane = MyVector3(nx < 0 ? _min.x : _max.x, 0, 0);
		break;
	case 1: //xz����
		outPlaneNormal = MyVector3(0, ny, 0);
		outPosOnPlane = MyVector3(0, ny < 0 ? _min.y : _max.y, 0);
		break;
	case 2: //xy����
		outPlaneNormal = MyVector3(0, 0, nz);
		outPosOnPlane = MyVector3(0, 0, nz < 0 ? _min.z : _max.z);
		break;
	}
}

bool AABB3::IsEmpty() const {
	//�����ꂩ�̐����� _min > _max �ł���΋�Ƃ݂Ȃ��B
	return _min.x > _max.x || _min.y > _max.y || _min.z > _max.z;
}

void AABB3::Add(MyVector3 add) {
	//�ŏ��l�̍X�V
	if (add.x < _min.x) _min.x = add.x;
	if (add.y < _min.y) _min.y = add.y;
	if (add.z < _min.z) _min.z = add.z;

	//�ő�l�̍X�V
	if (add.x > _max.x) _max.x = add.x;
	if (add.y > _max.y) _max.y = add.y;
	if (add.z > _max.z) _max.z = add.z;
}

MyVector3 AABB3::Closest(const MyVector3& pos) const {
	return MyVector3(
		EnforceValueIntoRange(_min.x, _max.x, pos.x),
		EnforceValueIntoRange(_min.y, _max.y, pos.y),
		EnforceValueIntoRange(_min.z, _max.z, pos.z)
	);
}

//@param rayOrg : ���_(�������̎n�_)
//@param rayDir : �����̒����ƕ���
//@param outNormal : �@�����󂯎��
//@param t : �����_�������p�����[�^���󂯎��
bool AABB3::RayIntersect(
	const MyVector3& rayOrg, const MyVector3& rayDir,
	MyVector3* outNormal, MyVector3* outColPos, float* t
) const {
	//���������݂��Ȃ���΋���Ȑ���Ԃ�
	const float kNoIntersect = 1e30f;
	*t = kNoIntersect;

	//�_���{�b�N�X�̒��ɂ��邩�ǂ������`�F�b�N���e���ʂ���̃p�����g���b�N�ȋ��������肷��B
	bool rayOrgIsInside = true;

	//�g���r�A�����W�F�N�g
	// - 'rayOrg.x'��'�{�b�N�X��yz����'�Ƃ̋������A�����̒����ȉ��ł����
	// - x�����ɂ��Ă͌������邱�Ƃ��킩��B
	// - �ȍ~y,z�����ɂ��Ă��`�F�b�N���A�e�������ׂĂɂ����Č����̒����ȉ��ł����
	// - ���������݂��邱�ƂɂȂ�B

	float tx; //�Փˈʒu(= rayOrg + tx * rayDir)��\���p�����[�^x����
	float nx = 0; //�@��(= {nx, ny, nz})��\��x����
	if (rayOrg.x < _min.x) {
		rayOrgIsInside = false;
		tx = _min.x - rayOrg.x;
		if (tx > rayDir.x) return false;
		tx /= rayDir.x;
		nx = -1.0f;
	}
	else if (rayOrg.x > _max.x) {
		rayOrgIsInside = false;
		tx = _max.x - rayOrg.x;
		if (tx < rayDir.x) return false;
		tx /= rayDir.x;
		nx = 1.0f;
	}
	else {
		//�{�b�N�X���͈̔͂�'rayOrg.x'������̂ŁA�ő�l�̌��ɂ�����Ȃ�����B
		tx = -1.0f;
	}

	//�ȍ~�Ay,z�����ɂ��Ă����l�ɏ�L�̃`�F�b�N���s��

	float ty, ny = 0;
	if (rayOrg.y < _min.y) {
		rayOrgIsInside = false;
		ty = _min.y - rayOrg.y;
		if (ty > rayDir.y) return false;
		ty /= rayDir.y;
		ny = -1.0f;
	}
	else if (rayOrg.y > _max.y) {
		rayOrgIsInside = false;
		ty = _max.y - rayOrg.y;
		if (ty < rayDir.y) return false;
		ty /= rayDir.y;
		ny = 1.0f;
	}
	else {
		ty = -1.0f;
	}

	float tz, nz = 0;
	if (rayOrg.z < _min.z) {
		rayOrgIsInside = false;
		tz = _min.z - rayOrg.z;
		if (tz > rayDir.z) return false;
		tz /= rayDir.z;
		nz = -1.0f;
	}
	else if (rayOrg.z > _max.z) {
		rayOrgIsInside = false;
		tz = _max.z - rayOrg.z;
		if (tz < rayDir.z) return false;
		tz /= rayDir.z;
		nz = 1.0f;
	}
	else {
		tz = -1.0f;
	}

	//���_���{�b�N�X���ɂ���Ƃ��́A���_ = �����_�A���]�������� = �@���ɐݒ肷��B
	if (rayOrgIsInside) {
		*outNormal = -rayDir;
		outNormal->Normalize();
		*outColPos = rayOrg;
		*t = 0.0f;
		return true;
	}

	//�ł����_���牓�����ʂ�I������(���ꂪ�������镽��)
	int which = 0;
	float max = tx;
	if (ty > max) {
		which = 1;
		max = ty;
	}
	else if (tz > max) {
		which = 2;
		max = tz;
	}

	float x, y, z;
	switch (which) {
	case 0: //yz���ʂƂ̌���
		y = rayOrg.y + max * rayDir.y;
		if (y < _min.y || _max.y < y) return false;
		z = rayOrg.z + max * rayDir.z;
		if (z < _min.z || _max.z < z) return false;

		//if (outNormal != nullptr) break;
		*outNormal = MyVector3(nx, 0, 0);

		break;
	case 1: //xz����
		x = rayOrg.x + max * rayDir.x;
		if (x < _min.x || _max.x < x) return false;
		z = rayOrg.z + max * rayDir.z;
		if (z < _min.z || _max.z < z) return false;
		
		//if (outNormal != nullptr) break;
		*outNormal = MyVector3(0, ny, 0);

		break;
	case 2: //xy����
		x = rayOrg.x + max * rayDir.x;
		if (x < _min.x || _max.x < x) return false;
		y = rayOrg.y + max * rayDir.y;
		if (y < _min.y || _max.y < y) return false;

		//if (outNormal != nullptr) break;
		*outNormal = MyVector3(0, 0, nz);

		break;
	}

	*t = max; //���ʂƂ̌����_�������p�����[�^
	*outColPos = rayOrg + *t * rayDir; //�����_
	return true;
}

//@param spherePos0 : ���̂̎n�_
//@param spherePos1 : ���̂̏I�_
//@param sphereRadius : ���̂̔��a
//@param posOnPlaneExceptColPos : �Փ˂��l���镽�ʏ�̓_(���� : �����_�͎g�p���Ă͂����Ȃ�)
//@param normal : �@��
//@param outSpherePos : �Փ˂̏u�Ԃ̋��̂̈ʒu
//@param outStep : �Փˎ���
bool AABB3::IntersectsBS3(
	const MyVector3& spherePos0, const MyVector3& spherePos1, float sphereRadius,
	const MyVector3& posOnPlane, const MyVector3& normal,
	MyVector3& outSpherePos, float& outStep
) const {
	//dot_dir_normal < 0: ���̂̐i�s���������ʂɌ������Ă���̂ŏՓ˂��N����
	//dot_dir_normal = 0: ���̂̐i�s���������ʂƕ��s�Ȃ̂ŏՓ˂͋N����Ȃ�
	//dot_dir_normal > 0: ���̂̐i�s���������ʂ��牓�������Ă���̂ŏՓ˂͋N����Ȃ�
	float dot_dir_normal = Dot(spherePos1 - spherePos0, normal);
	//���ʂƋ��̂܂ł̍ŒZ����
	float distPlaneToSphere = fabs(Dot(spherePos0 - posOnPlane, normal));

	//���s�ɐi��ł��Ȃ���Փ˂��Ă���X�y�V�����P�[�X
	const float kEPS = 1e-1f; //���̂��炢�̒l���Ó�
	if (fabs(dot_dir_normal) < kEPS && distPlaneToSphere < sphereRadius) {
		outStep = 1e30f; //������҂��Ă��Փ˂��Ȃ��̂ŁA�Փˎ����͋���Ȑ���Ԃ�
		outSpherePos = spherePos0;
		return true;
	}

	float dot_plane_normal = Dot(posOnPlane, normal);
	float dot_sphere_normal = Dot(spherePos0, normal);

	//�Փˎ����ƈʒu�̎Z�o
	outStep = (dot_plane_normal + sphereRadius - dot_sphere_normal) / dot_dir_normal;
	outSpherePos = spherePos0 + outStep * (spherePos1 - spherePos0);

	//�Z�o�����Փˈʒu����AABB3�܂ł̍ŒZ���������̔��a���傫�����false
	MyVector3 closest = AABB3::Closest(outSpherePos);
	if (Magnitude(outSpherePos - closest) > sphereRadius) return false;

	//�߂荞��ł���Ȃ�ߋ��̎��Ԃɖ߂�
	if (distPlaneToSphere < sphereRadius) return true;

	//���ʂ��牓����������Ȃ̂�false��Ԃ�
	if (dot_dir_normal >= 0) return false;

	//�l��������ԓ��ɏՓ˂��N����̂�true��Ԃ�
	if (0 <= outStep && outStep <= 1) return true;
	return false;
}

bool AABB3::IntersectsBS3(
	const MyVector3& acc, const MyVector3& vel, const MyVector3& pos, float radius, float step, 
	const MyVector3& posOnPlane, const MyVector3& normal, MyVector3& outVel, MyVector3& outPos, float& outStep
) const {
	const MyVector3 kNext = pos + vel * step + 0.5f * acc * step * step;
	float dot_dir_normal = Dot(kNext - pos, normal);
	//���ʂƋ��̂܂ł̍ŒZ����
	float distPlaneToSphere = fabs(Dot(pos - posOnPlane, normal));

	//���s�ɐi��ł��Ȃ���Փ˂��Ă���X�y�V�����P�[�X
	const float kEPS = 1e-10f; //���̂��炢�̒l���Ó�
	if (fabs(dot_dir_normal) < kEPS && distPlaneToSphere < radius) {
		outStep = 1e30f; //������҂��Ă��Փ˂��Ȃ��̂ŁA�Փˎ����͋���Ȑ���Ԃ�
		outVel  = MyVector3(vel.x, 0.f, vel.z);
		outPos  = pos + outVel * step; //�����x�𖳎����Ă�����ۂ�������
		return true;
	}

	float dot_plane_normal = Dot(posOnPlane, normal);
	float dot_sphere_normal = Dot(pos, normal);

	//�Փˎ����ƈʒu�̎Z�o
	outStep = (dot_plane_normal + radius - dot_sphere_normal) / dot_dir_normal;
	outVel = vel + acc * outStep * step;
	outPos = pos + outStep * (kNext - pos);

	//�Z�o�����Փˈʒu����AABB3�܂ł̍ŒZ���������̔��a���傫�����false
	if (Magnitude(outPos - Closest(outPos)) > radius) return false;

	//�߂荞��ł���Ȃ�ߋ��̎��Ԃɖ߂�
	if (distPlaneToSphere < radius) return true;

	//���ʂ��牓����������Ȃ̂�false��Ԃ�
	if (dot_dir_normal >= 0) return false;

	//�l��������ԓ��ɏՓ˂��N����̂�true��Ԃ�
	if (0 <= outStep && outStep <= 1) return true;
	return false;
}

namespace {
	//'target'�̒l��[min, max]�͈̔͂Ɏ��߂�
	float EnforceValueIntoRange(float min, float max, float target) {
		if (target < min) return min;
		if (target > max) return max;
		return target;
	}

	//[min, max]�͈̔͂Ń����_���Ȓl��Ԃ��Bseed�l�Ń����_���K����ύX
	int Range(unsigned int seed, int min, int max) {
		srand(seed * (unsigned int)time(NULL));
		return min + rand() % (max - min + 1);
	}
}