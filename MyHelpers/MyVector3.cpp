#include"MyVector3.h"

using namespace std;
using namespace DirectX;

const MyVector3 kZeroVec(0.f, 0.f, 0.f);

MyVector3::MyVector3(float x, float y, float z)
	:XMFLOAT3(x, y, z)
{}

MyVector3::MyVector3()
	:MyVector3(0.f, 0.f, 0.f)
{}

MyVector3::MyVector3(const MyVector3& v) 
	:XMFLOAT3(v.x, v.y, v.z)
{}

MyVector3::MyVector3(const DirectX::XMFLOAT3& v)
	:XMFLOAT3(v.x, v.y, v.z)
{}

MyVector3& MyVector3::operator=(const MyVector3& v) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	return *this;
}

MyVector3& MyVector3::operator=(const DirectX::XMFLOAT3& v) {
	*this = MyVector3(v.x, v.y, v.z);
	return *this;
}

MyVector3 MyVector3::operator-() const {
	return MyVector3(-x, -y, -z);
}

MyVector3& MyVector3::operator+=(const MyVector3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

MyVector3& MyVector3::operator-=(const MyVector3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

MyVector3& MyVector3::operator*=(float val) {
	x *= val;
	y *= val;
	z *= val;
	return *this;
}

MyVector3& MyVector3::operator/=(float val) {
	x /= val;
	y /= val;
	z /= val;
	return *this;
}

bool operator==(const MyVector3& a, const MyVector3& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool MyVector3::operator!=(const MyVector3& v) {
	return x != v.x || y != v.y || z != v.z;
}

MyVector3 MyVector3::Zero() {
	x = y = z = 0.0f;
	return *this;
}

MyVector3 
operator+(const MyVector3& a, const MyVector3& b) {
	return MyVector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

MyVector3
operator-(const MyVector3& a, const MyVector3& b) {
	return MyVector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

MyVector3 
operator*(float val, const MyVector3& v) {
	return MyVector3(val * v.x, val * v.y, val * v.z);
}

MyVector3 
operator*(const MyVector3& v, float val) {
	return val * v;
}

MyVector3 operator/(const MyVector3& v, float val) {
	return MyVector3(v.x / val, v.y / val, v.z / val);
}

float MyVector3::Magnitude() const {
	return sqrtf(x * x + y * y + z * z);
}

float Magnitude(const MyVector3 & v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

MyVector3 MyVector3::Normalize() const {
	return *this / this->Magnitude();
}

MyVector3 Normalize(const MyVector3& v) {
	return v / v.Magnitude();
}

float Distance(const MyVector3& a, const MyVector3& b) {
	return Magnitude(b - a);
}

float Dot(const MyVector3 & a, const MyVector3 & b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

MyVector3 Cross(const MyVector3& a, const MyVector3& b) {
	return MyVector3(a.y * b.z - b.y * a.z,
					 a.z * b.x - b.z * a.x,
					 a.x * b.y - b.x * a.y);
}
