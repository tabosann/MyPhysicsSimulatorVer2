#pragma once

#include<MyVector3.h>

//Bounding Sphere(‹…Œ^‚Ì“–‚½‚è”»’è)
class BS3 
{
public:

	MyVec3 _c; //‹…‚Ì’†S
	float  _r; //‹…‚Ì”¼Œa

	BS3(FMyVec3& c, float r);
	BS3();

	float GetSurfaceAreas() const; //•\–ÊÏ
	float GetVolumes()      const; //‘ÌÏ

	//‹…‚Æ‹…‚ÌŒğ·‚ğŒvZ
	bool IntersectsBS3(
		FMyVec3& p1_first, FMyVec3& p1_last, float r1, 
		FMyVec3& p2_first, FMyVec3& p2_last, float r2, 
		MyVec3* outPos1, MyVec3* outPos2, float* t
	);

	bool IntersectsBS3(
		FMyVec3& p1_first, FMyVec3& p1_last, float r1,
		FMyVec3& p2_first, FMyVec3& p2_last, float r2,
		MyVec3& outV1, MyVec3& outV2, MyVec3& outP1, MyVec3& outP2, float& outStep
	) const;

	bool IntersectsBS3(
		FMyVec3& a1, FMyVec3& v1, FMyVec3& p1, float r1, float t1,
		FMyVec3& a2, FMyVec3& v2, FMyVec3& p2, float r2, float t2,
		MyVec3& out_v1, MyVec3& out_v2, MyVec3& out_p1, MyVec3& out_p2, float& out_step
	) const;
};