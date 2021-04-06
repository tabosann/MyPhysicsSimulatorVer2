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
	//インデックスが範囲内であるかチェック
	assert(0 <= i && i < 8);

	return MyVector3(
		(i & 0x2) ? _max.x : _min.y, //i == 2,3,6,7
		(i & 0x1) ? _max.y : _min.y, //i == 1,3,5,7
		(i & 0x4) ? _max.z : _min.z  //i == 4,5,6,7
	);
}

//@param rayOrg : 光点
//@param rayDir : 光線
//@param outPlaneNormal : 平面の法線
//@param outPosOnPlane : 平面上の点
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

	//各成分でAABB3の平面までの最短距離と法線を算出
	float nx, ny, nz;
	float dx = rayOrg.x, dy = rayOrg.y, dz = rayOrg.z;
	Dist(_min.x, _max.x, dx, nx);
	Dist(_min.y, _max.y, dy, ny);
	Dist(_min.z, _max.z, dz, nz);

	//光点がボックス内に存在する場合は、光線と真逆の法線を返す
	if (dx == -1 && dy == -1 && dz == -1) {
		outPlaneNormal = -rayDir.Normalize();
		outPosOnPlane = rayOrg;
		return;
	}

	//'rayOrg'からAABB3まで一番距離が遠くなる平面を選択
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
	case 0: //yz平面
		outPlaneNormal = MyVector3(nx, 0, 0);
		outPosOnPlane = MyVector3(nx < 0 ? _min.x : _max.x, 0, 0);
		break;
	case 1: //xz平面
		outPlaneNormal = MyVector3(0, ny, 0);
		outPosOnPlane = MyVector3(0, ny < 0 ? _min.y : _max.y, 0);
		break;
	case 2: //xy平面
		outPlaneNormal = MyVector3(0, 0, nz);
		outPosOnPlane = MyVector3(0, 0, nz < 0 ? _min.z : _max.z);
		break;
	}
}

bool AABB3::IsEmpty() const {
	//いずれかの成分で _min > _max であれば空とみなす。
	return _min.x > _max.x || _min.y > _max.y || _min.z > _max.z;
}

void AABB3::Add(MyVector3 add) {
	//最小値の更新
	if (add.x < _min.x) _min.x = add.x;
	if (add.y < _min.y) _min.y = add.y;
	if (add.z < _min.z) _min.z = add.z;

	//最大値の更新
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

//@param rayOrg : 光点(半直線の始点)
//@param rayDir : 光線の長さと方向
//@param outNormal : 法線を受け取る
//@param t : 交差点を示すパラメータを受け取る
bool AABB3::RayIntersect(
	const MyVector3& rayOrg, const MyVector3& rayDir,
	MyVector3* outNormal, MyVector3* outColPos, float* t
) const {
	//交差が存在しなければ巨大な数を返す
	const float kNoIntersect = 1e30f;
	*t = kNoIntersect;

	//点がボックスの中にあるかどうかをチェックし各平面からのパラメトリックな距離を決定する。
	bool rayOrgIsInside = true;

	//トリビアルリジェクト
	// - 'rayOrg.x'と'ボックスのyz平面'との距離が、光線の長さ以下であれば
	// - x成分については交差することがわかる。
	// - 以降y,z成分についてもチェックし、各成分すべてにおいて光線の長さ以下であれば
	// - 交差が存在することになる。

	float tx; //衝突位置(= rayOrg + tx * rayDir)を表すパラメータx成分
	float nx = 0; //法線(= {nx, ny, nz})を表すx成分
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
		//ボックス内の範囲に'rayOrg.x'があるので、最大値の候補にあがらなくする。
		tx = -1.0f;
	}

	//以降、y,z成分についても同様に上記のチェックを行う

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

	//光点がボックス内にあるときは、光点 = 交差点、反転した光線 = 法線に設定する。
	if (rayOrgIsInside) {
		*outNormal = -rayDir;
		outNormal->Normalize();
		*outColPos = rayOrg;
		*t = 0.0f;
		return true;
	}

	//最も光点から遠い平面を選択する(それが交差する平面)
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
	case 0: //yz平面との交差
		y = rayOrg.y + max * rayDir.y;
		if (y < _min.y || _max.y < y) return false;
		z = rayOrg.z + max * rayDir.z;
		if (z < _min.z || _max.z < z) return false;

		//if (outNormal != nullptr) break;
		*outNormal = MyVector3(nx, 0, 0);

		break;
	case 1: //xz平面
		x = rayOrg.x + max * rayDir.x;
		if (x < _min.x || _max.x < x) return false;
		z = rayOrg.z + max * rayDir.z;
		if (z < _min.z || _max.z < z) return false;
		
		//if (outNormal != nullptr) break;
		*outNormal = MyVector3(0, ny, 0);

		break;
	case 2: //xy平面
		x = rayOrg.x + max * rayDir.x;
		if (x < _min.x || _max.x < x) return false;
		y = rayOrg.y + max * rayDir.y;
		if (y < _min.y || _max.y < y) return false;

		//if (outNormal != nullptr) break;
		*outNormal = MyVector3(0, 0, nz);

		break;
	}

	*t = max; //平面との交差点を示すパラメータ
	*outColPos = rayOrg + *t * rayDir; //交差点
	return true;
}

//@param spherePos0 : 球体の始点
//@param spherePos1 : 球体の終点
//@param sphereRadius : 球体の半径
//@param posOnPlaneExceptColPos : 衝突を考える平面上の点(注意 : 交差点は使用してはいけない)
//@param normal : 法線
//@param outSpherePos : 衝突の瞬間の球体の位置
//@param outStep : 衝突時刻
bool AABB3::IntersectsBS3(
	const MyVector3& spherePos0, const MyVector3& spherePos1, float sphereRadius,
	const MyVector3& posOnPlane, const MyVector3& normal,
	MyVector3& outSpherePos, float& outStep
) const {
	//dot_dir_normal < 0: 球体の進行方向が平面に向かっているので衝突が起こる
	//dot_dir_normal = 0: 球体の進行方向が平面と平行なので衝突は起こらない
	//dot_dir_normal > 0: 球体の進行方向が平面から遠ざかっているので衝突は起こらない
	float dot_dir_normal = Dot(spherePos1 - spherePos0, normal);
	//平面と球体までの最短距離
	float distPlaneToSphere = fabs(Dot(spherePos0 - posOnPlane, normal));

	//平行に進んでいながら衝突しているスペシャルケース
	const float kEPS = 1e-1f; //このくらいの値が妥当
	if (fabs(dot_dir_normal) < kEPS && distPlaneToSphere < sphereRadius) {
		outStep = 1e30f; //いくら待っても衝突しないので、衝突時刻は巨大な数を返す
		outSpherePos = spherePos0;
		return true;
	}

	float dot_plane_normal = Dot(posOnPlane, normal);
	float dot_sphere_normal = Dot(spherePos0, normal);

	//衝突時刻と位置の算出
	outStep = (dot_plane_normal + sphereRadius - dot_sphere_normal) / dot_dir_normal;
	outSpherePos = spherePos0 + outStep * (spherePos1 - spherePos0);

	//算出した衝突位置からAABB3までの最短距離が球の半径より大きければfalse
	MyVector3 closest = AABB3::Closest(outSpherePos);
	if (Magnitude(outSpherePos - closest) > sphereRadius) return false;

	//めり込んでいるなら過去の時間に戻す
	if (distPlaneToSphere < sphereRadius) return true;

	//平面から遠ざかる方向なのでfalseを返す
	if (dot_dir_normal >= 0) return false;

	//考慮する期間内に衝突が起こるのでtrueを返す
	if (0 <= outStep && outStep <= 1) return true;
	return false;
}

bool AABB3::IntersectsBS3(
	const MyVector3& acc, const MyVector3& vel, const MyVector3& pos, float radius, float step, 
	const MyVector3& posOnPlane, const MyVector3& normal, MyVector3& outVel, MyVector3& outPos, float& outStep
) const {
	const MyVector3 kNext = pos + vel * step + 0.5f * acc * step * step;
	float dot_dir_normal = Dot(kNext - pos, normal);
	//平面と球体までの最短距離
	float distPlaneToSphere = fabs(Dot(pos - posOnPlane, normal));

	//平行に進んでいながら衝突しているスペシャルケース
	const float kEPS = 1e-10f; //このくらいの値が妥当
	if (fabs(dot_dir_normal) < kEPS && distPlaneToSphere < radius) {
		outStep = 1e30f; //いくら待っても衝突しないので、衝突時刻は巨大な数を返す
		outVel  = MyVector3(vel.x, 0.f, vel.z);
		outPos  = pos + outVel * step; //加速度を無視してそれっぽく見せる
		return true;
	}

	float dot_plane_normal = Dot(posOnPlane, normal);
	float dot_sphere_normal = Dot(pos, normal);

	//衝突時刻と位置の算出
	outStep = (dot_plane_normal + radius - dot_sphere_normal) / dot_dir_normal;
	outVel = vel + acc * outStep * step;
	outPos = pos + outStep * (kNext - pos);

	//算出した衝突位置からAABB3までの最短距離が球の半径より大きければfalse
	if (Magnitude(outPos - Closest(outPos)) > radius) return false;

	//めり込んでいるなら過去の時間に戻す
	if (distPlaneToSphere < radius) return true;

	//平面から遠ざかる方向なのでfalseを返す
	if (dot_dir_normal >= 0) return false;

	//考慮する期間内に衝突が起こるのでtrueを返す
	if (0 <= outStep && outStep <= 1) return true;
	return false;
}

namespace {
	//'target'の値を[min, max]の範囲に収める
	float EnforceValueIntoRange(float min, float max, float target) {
		if (target < min) return min;
		if (target > max) return max;
		return target;
	}

	//[min, max]の範囲でランダムな値を返す。seed値でランダム規則を変更
	int Range(unsigned int seed, int min, int max) {
		srand(seed * (unsigned int)time(NULL));
		return min + rand() % (max - min + 1);
	}
}