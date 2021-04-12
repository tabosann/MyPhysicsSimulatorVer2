#include <BS3.h>

using namespace std;

namespace {
	const float kPi = 3.14159265f;
}

namespace {
	MyVec3 GetSpherePosAt(float time);
}

BS3::BS3(const MyVec3& c, float r) 
	:_c(c), _r(r)
{}

BS3::BS3()
	:BS3(kZeroVec, 0.5f)
{}

float BS3::GetSurfaceAreas() const {
	return 4.0f * kPi * _r * _r;
}

float BS3::GetVolumes() const {
	return 4.0f / 3.0f * kPi * _r * _r * _r;
}

//@param p1_first, p1_last : ���P�̎n�_�ƏI�_
//@param p2_first, p2_last : ���Q�̎n�_�ƏI�_
//@param r1, r2 : ���̔��a
//@param outPos1, outPos2 : �Փˎ��̋��̈ʒu
//@param t : �Փˎ���
bool BS3::IntersectsBS3(
	const MyVec3& p1_first, const MyVec3& p1_last, float r1, 
	const MyVec3& p2_first, const MyVec3& p2_last, float r2, 
	MyVec3* outPos1, MyVec3* outPos2, float* t
) {
	const MyVec3 kE = p2_first - p1_first; //E�x�N�g��
	const MyVec3 kD = p1_last - p1_first;  //D�x�N�g��

	float dotED = Dot(kD.Normalize(), kE);
	float dotEE = Dot(kE, kE);
	float r12   = r1 + r2;
	float val = dotED * dotED + r12 * r12 - dotEE;

	//�������̒��g('val')�����ł���Ό����͑��݂��Ȃ��̂�
	//�����̎��̒n�_��false��Ԃ��B
	if (val < 0) {
		*t = 1.0f;
		*outPos1 = p1_last;
		*outPos2 = p2_last;
		return false;
	}

	//t <  0      :���łɌ������Ă��āA�ߋ��̎��Ԃɖ߂��B
	//0 <= t <= 1 :�ړ��̊ԂɌ������N����B
	//t >  1      :�ړ����Ă��������N����Ȃ��B
	*t = dotED - sqrt(val);

	//�����̏u�Ԃ̈ʒu���Z�o
	*outPos1 = p1_first + *t * (p1_last - p1_first);
	*outPos2 = p2_first + *t * (p2_last - p2_first);

	//���łɌ������Ă���̂�true��Ԃ�(t < 0)
	if (dotEE < r12 * r12) return true;

	if (0 <= *t && *t <= 1) return true;
	return false;
}

bool BS3::IntersectsBS3(
	const MyVec3& p1_first, const MyVec3& p1_last, float r1,
	const MyVec3& p2_first, const MyVec3& p2_last, float r2,
	MyVec3& outV1, MyVec3& outV2, MyVec3& outP1, MyVec3& outP2, float& outStep
) const {
	#define SPHERE_POS1_AT(t) (p1_first + t * (p1_last - p1_first))
	#define SPHERE_POS2_AT(t) (p2_first + t * (p2_last - p2_first))
	#define DELTA_SPHERE_POS_AT(t) (SPHERE_POS2_AT(t) - SPHERE_POS1_AT(t))

	const MyVec3 kD = DELTA_SPHERE_POS_AT(1) - DELTA_SPHERE_POS_AT(0);
	//P = 0: ���x�x�N�g�����݂��ɓ��������œ����傫���Ȃ̂�
	//     : ���܂ł��Փ˂��N����Ȃ�
	float P = Dot(kD, kD);
	float r12 = r1 + r2;

	//t = 0�ŏՓ˂��Ă��Ȃ��̂�false��Ԃ��B
	if (P == 0 && Magnitude(p2_first - p1_first) > r12)
		return false;

	//���S�ɏd�Ȃ荇���Ă���Ƃ��́A�E�����ɒe���o���B
	if (P == 0 && p1_first == p2_first) {
		outStep = 0.f;
		outP1 = p1_first;
		outP2 = p1_first + r12 * MyVec3(1.f, 0.f, 0.f);
		return true;
	}

	//�ꕔ�d�Ȃ荇���Ă���Ƃ��́A����̋���ڂ���`�ɂȂ�܂Œe���o��
	if (P == 0) {
		outStep = 0.f;
		outP1 = p1_first;
		outP2 = p1_first + r12 * Normalize(p2_first - p1_first);
		return true;
	}

	float Q = Dot(DELTA_SPHERE_POS_AT(0), kD);
	float R = Dot(DELTA_SPHERE_POS_AT(0), DELTA_SPHERE_POS_AT(0));
	float val = Q * Q - P * (R - r12 * r12);

	//���������݂��Ȃ��̂ŁA���̂܂ܐi���false��Ԃ��B
	if (val < 0) {
		outStep = 1.f;
		outP1 = p1_last;
		outP2 = p2_last;
		return false;
	}

	outStep = (-Q - sqrt(val)) / P;
	outV1 = outV1 + MyVec3(0.f, -9.8f, 0.f) * 1.f / 30 * outStep;
	outV2 = outV2 + MyVec3(0.f, -9.8f, 0.f) * 1.f / 30 * outStep;
	outP1 = SPHERE_POS1_AT(outStep);
	outP2 = SPHERE_POS2_AT(outStep);

	//TODO: �ŏ�����(t = 0)�Փ˂��Ă���̂ŁAtrue��Ԃ��B
	if (Magnitude(p2_first - p1_first) <= r12) {
		//return true;
	}

	//�l��������ԓ��ɏՓ˂��N����̂�true
	if (0 <= outStep && outStep <= 1) return true;
	return false;
}

bool BS3::IntersectsBS3(
	const MyVec3& a1, const MyVec3& v1, const MyVec3& p1, float r1, float t1, 
	const MyVec3& a2, const MyVec3& v2, const MyVec3& p2, float r2, float t2, 
	MyVec3& outV1, MyVec3& outV2, MyVec3& outP1, MyVec3& outP2, float& outStep
) const {
	const MyVec3 lastV1 = v1 + a1 * t1;
	const MyVec3 lastV2 = v2 + a2 * t2;
	const MyVec3 lastP1 = p1 + v1 * t1 + 0.5f * a1 * t1 * t1;
	const MyVec3 lastP2 = p2 + v2 * t2 + 0.5f * a2 * t2 * t2;

	const MyVec3 kD = (lastP2 - lastP1) - (p2 - p1);
	float r = r1 + r2;

	//P == 0: �i�ޕ������݂��ɕ��s�ŁA���x���ꏏ�Ȃ̂Ō���邱�Ƃ͂Ȃ��B
	float P = Dot(kD, kD);
	const float kEPS = 1e-5f;

	if (fabs(P) < kEPS && Magnitude(p2 - p1) > r)
		return false;

	if (fabs(P) < kEPS && p1 == p2) {
		outStep = 0.f;
		outV1 = v1; outV2 = v2;
		outP1 = p1; 
		outP2 = p1 + r * MyVec3(1.f, 0.f, 0.f); //�����I�ɉE�ɉ����o��
		return true;
	}
	if (fabs(P) < kEPS) {
		outStep = 0.f;
		outV1 = v1; outV2 = v2;
		outP1 = p1; outP2 = p1 + r * Normalize(p2 - p1);
		printf("%.3f\n", outStep);
		return true;
	}

	float Q = Dot(p2 - p1, kD);
	float R = Dot(p2 - p1, p2 - p1);
	float vv = Q * Q - P * (R - r * r);

	if (vv < 0) {
		outStep = 1.f;
		outV1 = lastV1; outV1 = lastV2;
		outP1 = lastP1; outP2 = lastP2;
		return false;
	}

	float t_min = (-Q - sqrt(vv)) / P;
	float t_max = (-Q + sqrt(vv)) / P;
	outStep = t_min < t_max ? t_min : t_max;
	outV1 = v1 + outStep * (lastV1 - v1); outV2 = v2 + outStep * (lastV2 - v2);
	outP1 = p1 + outStep * (lastP1 - p1); outP2 = p2 + outStep * (lastP2 - p2);

	//FIXME: outStep�����̂Ƃ��A��Βl���傫�����Đ������ł��܂��B
	if (Magnitude(p2 - p1) <= r) {
		//return true;
	}

	if (0.f <= outStep && outStep <= 1.f) return true;
	return false;
}

namespace {
	MyVec3 GetSpherePosAt(const MyVec3& a, const MyVec3& v, const MyVec3& p, const MyVec3& step) {
		return kZeroVec;
	}
}