#pragma once

//DirectX::XMFLOAT3�N���X�̊e�퉉�Z���\�ɂ����N���X
// - update ver0.222 : DirectX::XMVECTOR�N���X�ɑΉ����܂����B
class MyVector3 : public DirectX::XMFLOAT3 
{
public:

	MyVector3(float x, float y, float z);
	MyVector3();
	MyVector3(const MyVector3& v);
	MyVector3(const DirectX::XMFLOAT3& v);
	MyVector3(const DirectX::XMVECTOR& v);

	//������Z
	MyVector3& operator=(const MyVector3& v);
	MyVector3& operator=(const DirectX::XMFLOAT3& v);
	MyVector3& operator=(const DirectX::XMVECTOR& v);

	//���]���Z
	MyVector3 operator-() const;

	//�l�����Z
	friend MyVector3 operator+(const MyVector3& a, const MyVector3& b);
	friend MyVector3 operator-(const MyVector3& a, const MyVector3& b);
	friend MyVector3 operator*(float val, const MyVector3& v); //��F2 * (x, y, z) = (2x, 2y, 2z)
	friend MyVector3 operator*(const MyVector3& v, float val); //��F(x, y, z) * 2 = (2x, 2y, 2z)
	friend MyVector3 operator/(const MyVector3& v, float val);

	//�g�ݍ��킹������Z
	MyVector3& operator+=(const MyVector3& v);
	MyVector3& operator-=(const MyVector3& v);
	MyVector3& operator*=(float val);
	MyVector3& operator/=(float val);

	//�]�����Z�q
	friend bool operator==(const MyVector3& a, const MyVector3& b);
	friend bool operator!=(const MyVector3& a, const MyVector3& b);

	//�L���X�g���Z�q
	operator DirectX::XMVECTOR();
	operator DirectX::XMVECTOR() const;

	//�[���x�N�g���ɃZ�b�g
	MyVector3 Zero();

	//�x�N�g���̑傫�������߂�
	       float Magnitude() const;
	friend float Magnitude(const MyVector3 & v);

	//���K��
	       MyVector3 Normalize() const;
	friend MyVector3 Normalize(const MyVector3& v);

	friend float     Distance(const MyVector3& a, const MyVector3& b); //2�̃x�N�g���̋���
	friend float	 Dot(const MyVector3& a, const MyVector3& b);      //����
	friend MyVector3 Cross(const MyVector3& a, const MyVector3& b);    //�O��
};

// NOTE: const�I�u�W�F�N�g�̓f�t�H���g�����������Ȃ̂�
//     : extern�錾��p���āA�O�������i���̃t�@�C�������������悤�j�ɂ���B
extern const MyVector3 kZeroVec;
using MyVec3  = MyVector3;
using FMyVec3 = const MyVec3; // F = Fixed(�Œ�I��)

inline std::ostream& operator << (std::ostream& os, const MyVector3& v)
{
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}