#pragma once

//DirectX::XMFLOAT3�N���X�̊e�퉉�Z���\�ɂ����N���X
class MyVector3 : public DirectX::XMFLOAT3 {
public:
	MyVector3(float x, float y, float z);
	MyVector3();
	MyVector3(const MyVector3& v);
	MyVector3(const DirectX::XMFLOAT3& v);

	//������Z
	MyVector3& operator=(const MyVector3& v);
	MyVector3& operator=(const DirectX::XMFLOAT3& v);

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
	bool operator!=(const MyVector3& v);

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
using MyVec3 = MyVector3;

inline std::ostream& operator << (std::ostream& os, const MyVector3& v) {
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}