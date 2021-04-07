#pragma once

#include"../MyHelpers/MyVector3.h"

//バウンディングボックス(箱型の当たり判定)
class AABB3 {
public:
	MyVector3 _min;
	MyVector3 _max;

	AABB3(const MyVector3& min, const MyVector3& max);
	//デフォルトコンストラクタで、ボックスを空にセットする
	// NOTE: 空の定義は _min > _max となることである。
	AABB3();

	MyVector3 GetCenterPos() const;
	MyVector3 GetSizeVec()   const; //ボックスの対角を結ぶベクトル
	MyVector3 GetRadiusVec() const; //ボックスの対角を結ぶベクトルの半分
	
	//AABBを形成する8つの頂点のうち1つを取得
	MyVector3 GetCorner(int i) const;

	//衝突が起こる平面(= その平面の法線と平面上の点)を取得
	void GetPlane(
		const MyVector3& rayOrg, const MyVector3& rayDir, 
		MyVector3& outNormal, MyVector3& outPosOnPlane
	) const;

	//空のAABBであれば、trueを返す
	bool IsEmpty() const;

	//AABBを頂点'add'を包含できるだけのサイズに拡大する
	void Add(MyVector3 add);

	//'pos'に一番近いAABB3上の点を取得
	//'pos'がAABB3内にあればそのまま'pos'を返す
	MyVector3 Closest(const MyVector3& pos) const;

	//光線とAABB3との交差を計算し、交差が存在すればtrueを返す
	bool RayIntersect(
		const MyVector3& rayOrg, const MyVector3& rayDir,
		MyVector3* outNormal, MyVector3* outColPos, float* t
	) const;

	//AABB3とBS3の衝突を計算する
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
