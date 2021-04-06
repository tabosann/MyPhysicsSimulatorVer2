#include<AABB3.h>
#include<MyChar32.h>
#include<MyRectangle.h>

using namespace std;
using namespace DirectX;

int MyRectangle::_id = 0;

MyRectangle::MyRectangle(
	DX12Wrapper* dx12, const Char32& name,
	const MyVec3& acc, const MyVec3& vel, const MyVec3& pos, const MyVec3& size,
	float m, float e, int frames, const MyVec3& col
)
	:BasicObject(dx12, name, size, m, e, col), MyPhysics(frames, acc, vel, pos),
	 _t(1.f), _tempPos(pos), _tempVel(vel)
{
	_aabb3.reset(new AABB3(_pos - _size * 0.5f, _pos + _size * 0.5f));

	_init_acc   = acc;
	_init_vel   = vel;
	_init_pos   = pos;
	_init_size  = size;
	_init_m     = m;
	_init_e     = e;
	_init_dt    = 1.f / frames;
	_init_color = col;

	HRESULT result = CreateVertexBufferView();
	assert(SUCCEEDED(result));

	result = CreateIndexBufferView();
	assert(SUCCEEDED(result));
}

MyRectangle::MyRectangle(
	DX12Wrapper* dx12,
	const MyVec3& acc, const MyVec3& vel, const MyVec3& pos, const MyVec3& size,
	float m, float e, int frames, const MyVec3& col
)
	:MyRectangle(dx12, "", acc, vel, pos, size, m, e, frames, col)
{
	Char32::Copy(_name, "Rectangle(%d)", MyRectangle::_id++);
}

HRESULT MyRectangle::CreateVertexBufferView() {
	BasicObject::Vertex verticesOfRectangle[] = {
		//前面
		//[位置]				[法線]				  [色RGB]
		{ XMFLOAT3(-1, -1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1,  1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1, -1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1,  1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT3( 1,  1,  1) },
		//右側面
		{ XMFLOAT3( 1, -1, -1), XMFLOAT3( 1,  0,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1,  1, -1), XMFLOAT3( 1,  0,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1, -1,  1), XMFLOAT3( 1,  0,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1,  1,  1), XMFLOAT3( 1,  0,  0), XMFLOAT3( 1,  1,  1) },
		//後面
		{ XMFLOAT3( 1, -1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1,  1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1, -1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1,  1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT3( 1,  1,  1) },
		//左側面
		{ XMFLOAT3(-1, -1,  1), XMFLOAT3(-1,  0,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1,  1,  1), XMFLOAT3(-1,  0,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1, -1, -1), XMFLOAT3(-1,  0,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1,  1, -1), XMFLOAT3(-1,  0,  0), XMFLOAT3( 1,  1,  1) },
		//上面
		{ XMFLOAT3(-1,  1, -1), XMFLOAT3( 0,  1,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1,  1,  1), XMFLOAT3( 0,  1,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1,  1, -1), XMFLOAT3( 0,  1,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1,  1,  1), XMFLOAT3( 0,  1,  0), XMFLOAT3( 1,  1,  1) },
		//下面
		{ XMFLOAT3(-1, -1, -1), XMFLOAT3( 0, -1,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3(-1, -1,  1), XMFLOAT3( 0, -1,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1, -1, -1), XMFLOAT3( 0, -1,  0), XMFLOAT3( 1,  1,  1) },
		{ XMFLOAT3( 1, -1,  1), XMFLOAT3( 0, -1,  0), XMFLOAT3( 1,  1,  1) }
	};

	return BasicObject::CreateVertexBufferView(
		begin(verticesOfRectangle), end(verticesOfRectangle), sizeof(verticesOfRectangle));
}

HRESULT MyRectangle::CreateIndexBufferView() {
	unsigned short indicesOfRectangle[] = {
		//前面
		0,1,2,2,1,3,
		//右側面
		4,5,6,6,5,7,
		//後面
		8,9,10,10,9,11,
		//左側面
		12,13,14,14,13,15,
		//上面
		16,17,18,18,17,19,
		//下面
		20,21,22,22,21,23
	};

	return BasicObject::CreateIndexBufferView(
		begin(indicesOfRectangle), end(indicesOfRectangle), sizeof(indicesOfRectangle));
}

void MyRectangle::BeginCalc() {
	_t = 1.f;
	_tempPos = GetPositionIn(_dt);
	_tempVel = GetVelocityIn(_dt);
}

void MyRectangle::Update() {
	//物体の大きさ、位置を設定
	_mappedTransform->world  = XMMatrixIdentity();
	_mappedTransform->world *= XMMatrixScaling(_size.x * 0.5f, _size.y * 0.5f, _size.z * 0.5f);
	_mappedTransform->world *= XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
	//物体の色を設定
	_mappedTransform->color.x = _color.x;
	_mappedTransform->color.y = _color.y;
	_mappedTransform->color.z = _color.z;
}

void MyRectangle::EndCalc() {
	_pos = _tempPos;
	_vel = _tempVel;
	_aabb3->_min = _pos - _size * 0.5f;
	_aabb3->_max = _pos + _size * 0.5f;
}

void MyRectangle::Render() const { BasicObject::Render(); }

void MyRectangle::Adjuster() {
	Char32 name   = _name;
	MyVec3 acc    = _acc;
	MyVec3 vel    = _vel;
	MyVec3 pos    = _pos;
	MyVec3 size   = _size;
	MyVec3 col    = _color;
	float  mass   = _m;
	float  cef    = _e;
	int    frames = 1.f / _dt + 0.5f;

	if (ImGui::InputText  ("Name"  , name.data(), Char32::_kSize)) _name  = name;
	if (ImGui::InputFloat3("Accel" , &acc.x))                _acc   = acc;
	if (ImGui::InputFloat3("Veloc" , &vel.x))                _vel   = vel;
	if (ImGui::InputFloat3("Posit" , &pos.x))                _pos   = pos;
	if (ImGui::InputFloat3("Size"  , &size.x))               _size  = size;
	if (ImGui::InputFloat ("Mass"  , &mass))                 _m     = mass;
	if (ImGui::DragFloat  ("Coeff" , &cef, 5e-3f, 0.f, 1.f)) _e     = cef;
	if (ImGui::InputInt   ("Frames", &frames))               _dt    = 1.f / frames;
	if (ImGui::ColorEdit3 ("Color" , &col.x))                _color = col;
}

void MyRectangle::Reset() {
	_acc   = _init_acc;
	_vel   = _init_vel;
	_pos   = _init_pos;
	_size  = _init_size;
	_m     = _init_m;
	_e     = _init_e;
	_dt    = _init_dt;
	_color = _init_color;
}
