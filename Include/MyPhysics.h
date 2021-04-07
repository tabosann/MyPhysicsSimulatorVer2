#pragma once

#include<MyVector3.h>

class MyPhysics {
public:
	float  _dt;	 //1フレーム毎の経過時間[sec]
	MyVec3 _acc; //[m/s^2]
	MyVec3 _vel; //[m/s]
	MyVec3 _pos; //[m]

	//@param frames : 1フレーム毎の経過時間[sec]
	//@param acc : 加速度[m/s^2]
	//@param vel : 速度[m/s]
	//@param pos : 位置座標[m]
	MyPhysics(int frames, const MyVec3& acc, const MyVec3& vel, const MyVec3& pos);
	MyPhysics();
	virtual ~MyPhysics();

	virtual void Update() = 0;

	//現在の速度から'time'秒後の速度を算出
	//NOTE: 加速度'_acc'に依存するため、先に加速度を更新してはいけない。
	MyVec3 GetVelocityIn(float time) const;
	MyVec3 GetVelocityIn(float time, const MyVec3& acc) const;
	//現在の位置から'time'秒後の位置を算出
	//NOTE: 加速度'_acc'と速度'_vel'に依存するため、先に加速度と速度を更新してはいけない。
	MyVec3 GetPositionIn(float time) const;
	MyVec3 GetPositionIn(float time, const MyVec3& acc, const MyVec3& vel) const;

	//AABB3とBS3の衝突による反射ベクトルを算出
	static MyVec3 CalcReflectionVector(const MyVec3& vp, const MyVec3& vw, const MyVec3& normal, float e);
	//BS3とBS3の衝突による反射ベクトルを算出
	static void CalcReflectionVector(
		const MyVec3& v1, const MyVec3& v2,
		const MyVec3& p1, const MyVec3& p2,
		float m1, float m2, float e1, float e2,
		MyVec3& outV1, MyVec3& outV2
	);
	//BS3とAABB3の壁ずりベクトルを算出
	static MyVec3 CalcVectorAlongAABB3(const MyVec3& vel, const MyVec3& normal);

private:
	//----------------------------------------
	//	デバッグ関数群
	//----------------------------------------
	//物理パラメータの数値を確認
	void CheckOutputPhysicsParam();
};

extern const MyVec3 kGravity;