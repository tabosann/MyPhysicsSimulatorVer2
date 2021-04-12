#include <BS3.h>

using namespace std;
using namespace DirectX;

namespace 
{
	const float kPi = 3.14159265f;
}

BS3::BS3(FMyVec3& c, float r) 
	:_c(c), _r(r)
{}

BS3::BS3()
	:BS3(kZeroVec, 0.5f)
{}

float BS3::GetSurfaceAreas() const 
{
	return 4.0f * XM_PI * _r * _r;
}

float BS3::GetVolumes() const 
{
	return 4.0f / 3.0f * XM_PI * _r * _r * _r;
}

//@param p1_first, p1_last : 球１の始点と終点
//@param p2_first, p2_last : 球２の始点と終点
//@param r1, r2 : 球の半径
//@param outPos1, outPos2 : 衝突時の球の位置
//@param t : 衝突時刻
bool BS3::IntersectsBS3(
	FMyVec3& p1_first, FMyVec3& p1_last, float r1, 
	FMyVec3& p2_first, FMyVec3& p2_last, float r2, 
	MyVec3* outPos1, MyVec3* outPos2, float* outStep
) {
	FMyVec3 kE = p2_first - p1_first; //Eベクトル
	FMyVec3 kD = p1_last - p1_first;  //Dベクトル

	float dotED = Dot(kD.Normalize(), kE);
	float dotEE = Dot(kE, kE);
	float r12   = r1 + r2;
	float val = dotED * dotED + r12 * r12 - dotEE;

	//平方根の中身('val')が負であれば交差は存在しないので
	//自分の次の地点とfalseを返す。
	if (val < 0) {
		*outStep = 1.0f;
		*outPos1 = p1_last;
		*outPos2 = p2_last;
		return false;
	}

	//t <  0      :すでに交差していて、過去の時間に戻す。
	//0 <= t <= 1 :移動の間に交差が起こる。
	//t >  1      :移動しても交差が起こらない。
	*outStep = dotED - sqrt(val);

	//交差の瞬間の位置を算出
	*outPos1 = p1_first + *outStep * (p1_last - p1_first);
	*outPos2 = p2_first + *outStep * (p2_last - p2_first);

	//すでに交差しているのでtrueを返す(t < 0)
	if (dotEE < r12 * r12) return true;

	if (0 <= *outStep && *outStep <= 1) return true;
	return false;
}

bool BS3::IntersectsBS3(
	FMyVec3& p1_first, FMyVec3& p1_last, float r1,
	FMyVec3& p2_first, FMyVec3& p2_last, float r2,
	MyVec3& outV1, MyVec3& outV2, MyVec3& outP1, MyVec3& outP2, float& outStep
) const {
	#define SPHERE_POS1_AT(t) (p1_first + t * (p1_last - p1_first))
	#define SPHERE_POS2_AT(t) (p2_first + t * (p2_last - p2_first))
	#define DELTA_SPHERE_POS_AT(t) (SPHERE_POS2_AT(t) - SPHERE_POS1_AT(t))

	FMyVec3 kD = DELTA_SPHERE_POS_AT(1) - DELTA_SPHERE_POS_AT(0);
	//P = 0: 速度ベクトルが互いに同じ方向で同じ大きさなので
	//     : いつまでも衝突が起こらない
	float P = Dot(kD, kD);
	float r12 = r1 + r2;

	//t = 0で衝突していないのでfalseを返す。
	if (P == 0 && Magnitude(p2_first - p1_first) > r12)
		return false;

	//完全に重なり合っているときは、右方向に弾き出す。
	if (P == 0 && p1_first == p2_first) {
		outStep = 0.f;
		outP1 = p1_first;
		outP2 = p1_first + r12 * MyVec3(1.f, 0.f, 0.f);
		return true;
	}

	//一部重なり合っているときは、一方の球を接する形になるまで弾き出す
	if (P == 0) {
		outStep = 0.f;
		outP1 = p1_first;
		outP2 = p1_first + r12 * Normalize(p2_first - p1_first);
		return true;
	}

	float Q = Dot(DELTA_SPHERE_POS_AT(0), kD);
	float R = Dot(DELTA_SPHERE_POS_AT(0), DELTA_SPHERE_POS_AT(0));
	float val = Q * Q - P * (R - r12 * r12);

	//交差が存在しないので、そのまま進んでfalseを返す。
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

	//TODO: 最初から(t = 0)衝突しているので、trueを返す。
	if (Magnitude(p2_first - p1_first) <= r12) {
		//return true;
	}

	//考慮する期間内に衝突が起こるのでtrue
	if (0 <= outStep && outStep <= 1) return true;
	return false;
}

bool BS3::IntersectsBS3(
	FMyVec3& a1, FMyVec3& v1, FMyVec3& p1, float r1, float t1, 
	FMyVec3& a2, FMyVec3& v2, FMyVec3& p2, float r2, float t2, 
	MyVec3& out_v1, MyVec3& out_v2, MyVec3& out_p1, MyVec3& out_p2, float& out_step
) const 
{
	FMyVec3 kLast_v1 = v1 + a1 * t1;
	FMyVec3 kLast_v2 = v2 + a2 * t2;
	FMyVec3 kLast_p1 = p1 + v1 * t1 + 0.5f * a1 * t1 * t1;
	FMyVec3 kLast_p2 = p2 + v2 * t2 + 0.5f * a2 * t2 * t2;

	FMyVec3 kD = (kLast_p2 - kLast_p1) - (p2 - p1);
	const float kTotalRadius = r1 + r2;

	//P == 0: 進む方向が互いに平行で、速度も一緒なので交わることはない。
	const float kP   = Dot(kD, kD);
	const float kEPS = 1e-5f;
	const float kNominal = 1.01f;
	
	if (fabs(kP) < kEPS && Distance(p1, p2) > kTotalRadius)
		return false;

	if (fabs(kP) < kEPS && p1 == p2) 
	{
		out_step = 0.f;
		out_v1 = v1; out_v2 = v2;
		out_p1 = p1;
		out_p2 = p1 + kTotalRadius * MyVec3(1.01f, 0.f, 0.f); //強制的に右に押し出し
		return true;
	}
	if (fabs(kP) < kEPS) 
	{
		out_step = 0.f;
		out_v1 = v1; out_v2 = v2;
		out_p1 = p1; out_p2 = p1 + kTotalRadius * Normalize(p2 - p1) * kNominal;
		return true;
	}

	// TODO: 最初から衝突している場合の扱いをうまいこと行う
	if (Distance(p1, p2) <= kTotalRadius)
	{
		out_step = 0.f;
		out_v1 = v1; out_v2 = v2;
		out_p1 = p1; out_p2 = p1 + kTotalRadius * Normalize(p2 - p1);
		//return true;
	}

	const float kQ   = Dot(p2 - p1, kD);
	const float kR   = Dot(p2 - p1, p2 - p1);
	const float kVal = kQ * kQ - kP * (kR - kTotalRadius * kTotalRadius);

	if (kVal < 0) 
	{
		out_step = 1.f;
		out_v1 = kLast_v1; out_v1 = kLast_v2;
		out_p1 = kLast_p1; out_p2 = kLast_p2;
		return false;
	}

	float t_min = (-kQ - sqrt(kVal)) / kP;
	float t_max = (-kQ + sqrt(kVal)) / kP;
	out_step = t_min < t_max ? t_min : t_max; //小さい方の時刻を代入

	if (out_step < 0.f || 1.f < out_step) return false;

	out_v1 = v1 + out_step * (kLast_v1 - v1); out_v2 = v2 + out_step * (kLast_v2 - v2);
	out_p1 = p1 + out_step * (kLast_p1 - p1); out_p2 = p2 + out_step * (kLast_p2 - p2);

	return true;
}
