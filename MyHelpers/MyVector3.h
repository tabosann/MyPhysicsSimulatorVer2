#pragma once

//DirectX::XMFLOAT3クラスの各種演算を可能にしたクラス
// - update ver0.222 : DirectX::XMVECTORクラスに対応しました。
class MyVector3 : public DirectX::XMFLOAT3 
{
public:

	MyVector3(float x, float y, float z);
	MyVector3();
	MyVector3(const MyVector3& v);
	MyVector3(const DirectX::XMFLOAT3& v);
	MyVector3(const DirectX::XMVECTOR& v);

	//代入演算
	MyVector3& operator=(const MyVector3& v);
	MyVector3& operator=(const DirectX::XMFLOAT3& v);
	MyVector3& operator=(const DirectX::XMVECTOR& v);

	//反転演算
	MyVector3 operator-() const;

	//四則演算
	friend MyVector3 operator+(const MyVector3& a, const MyVector3& b);
	friend MyVector3 operator-(const MyVector3& a, const MyVector3& b);
	friend MyVector3 operator*(float val, const MyVector3& v); //例：2 * (x, y, z) = (2x, 2y, 2z)
	friend MyVector3 operator*(const MyVector3& v, float val); //例：(x, y, z) * 2 = (2x, 2y, 2z)
	friend MyVector3 operator/(const MyVector3& v, float val);

	//組み合わせ代入演算
	MyVector3& operator+=(const MyVector3& v);
	MyVector3& operator-=(const MyVector3& v);
	MyVector3& operator*=(float val);
	MyVector3& operator/=(float val);

	//評価演算子
	friend bool operator==(const MyVector3& a, const MyVector3& b);
	friend bool operator!=(const MyVector3& a, const MyVector3& b);

	//キャスト演算子
	operator DirectX::XMVECTOR();
	operator DirectX::XMVECTOR() const;

	//ゼロベクトルにセット
	MyVector3 Zero();

	//ベクトルの大きさを求める
	       float Magnitude() const;
	friend float Magnitude(const MyVector3 & v);

	//正規化
	       MyVector3 Normalize() const;
	friend MyVector3 Normalize(const MyVector3& v);

	friend float     Distance(const MyVector3& a, const MyVector3& b); //2つのベクトルの距離
	friend float	 Dot(const MyVector3& a, const MyVector3& b);      //内積
	friend MyVector3 Cross(const MyVector3& a, const MyVector3& b);    //外積
};

// NOTE: constオブジェクトはデフォルトが内部結合なので
//     : extern宣言を用いて、外部結合（他のファイルからも見えるよう）にする。
extern const MyVector3 kZeroVec;
using MyVec3  = MyVector3;
using FMyVec3 = const MyVec3; // F = Fixed(固定的な)

inline std::ostream& operator << (std::ostream& os, const MyVector3& v)
{
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}