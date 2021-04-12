#pragma once

#include<MyVector3.h>

//バウンディングボックス(箱型の当たり判定)
class AABB3
{
public:

	MyVec3 _min;
	MyVec3 _max;

	AABB3(FMyVec3& min, FMyVec3& max);
	//デフォルトコンストラクタで、ボックスを空にセットする
	// NOTE: 空の定義は _min > _max となることである。
	AABB3();

	MyVec3 GetCenterPos() const;
	MyVec3 GetSizeVec()   const; //ボックスの対角を結ぶベクトル
	MyVec3 GetRadiusVec() const; //ボックスの対角を結ぶベクトルの半分
	
	//AABBを形成する8つの頂点のうち1つを取得
	MyVec3 GetCorner(int i) const;

	//衝突が起こる平面(= その平面の法線と平面上の点)を取得
	void GetPlane(
		FMyVec3& rayOrg, FMyVec3& rayDir, 
		MyVec3& outNormal, MyVec3& outPosOnPlane
	) const;

	//空のAABBであれば、trueを返す
	bool IsEmpty() const;

	//AABBを頂点'add'を包含できるだけのサイズに拡大する
	void Add(MyVec3 add);

	//'pos'に一番近いAABB3上の点を取得
	//'pos'がAABB3内にあればそのまま'pos'を返す
	MyVec3 Closest(FMyVec3& pos) const;

	//光線とAABB3との交差を計算し、交差が存在すればtrueを返す
	bool RayIntersect(
		FMyVec3& rayOrg, FMyVec3& rayDir,
		MyVec3* outNormal, MyVec3* outColPos, float* t
	) const;

	//AABB3とBS3の衝突を計算する
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
