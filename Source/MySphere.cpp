#include<DX12Utility.h>
#include<BS3.h>
#include<MySphere.h>

#include"../ImGui/imgui.h"

using namespace std;
using namespace DirectX;
using namespace DX12Util;

namespace {
	const int U_MAX = 30; //‹…–Ê‚ð'c'‚É•ªŠ„‚µ‚½Û‚Ì•ªŠ„”
	const int V_MAX = 15; //‹…–Ê‚ð'‰¡'‚É•ªŠ„‚µ‚½Û‚Ì•ªŠ„”
}

int MySphere::_id = 0;

MySphere::MySphere(
	DX12Wrapper* dx12, const Char32& name,
	const MyVec3& acc, const MyVec3& vel, const MyVec3& pos,
	float r, float m, float e, int frames, const MyVec3& col
)
	:BasicObject(dx12, name, MyVec3(_r, _r, _r * 0.5f), m, e, col), MyPhysics(frames, acc, vel, pos),
	_r(r), _t(1.f), _tempPos(pos), _tempVel(vel)
{
	_bs3.reset(new BS3(pos, r));

	_init_acc   = acc;
	_init_vel   = vel;
	_init_pos   = pos;
	_init_r     = r;
	_init_m     = m;
	_init_e     = e;
	_init_dt    = 1.f / frames;
	_init_color = col;

	HRESULT result = CreateVertexBufferView();
	assert(SUCCEEDED(result));

	result = CreateIndexBufferView();
	assert(SUCCEEDED(result));
}

MySphere::MySphere(
	DX12Wrapper* dx12,
	const MyVec3& acc, const MyVec3& vel, const MyVec3& pos, 
	float r, float m, float e, int frames, const MyVec3& col
)
	:MySphere(dx12, "", acc, vel, pos, r, m, e, frames, col)
{
	Char32::Copy(_name, "Sphere(%d)", MySphere::_id++);
}

void MySphere::BeginCalc() {
	_t = 1.f;
	_tempPos = GetPositionIn(_dt);
	_tempVel = GetVelocityIn(_dt);
}

void MySphere::Update() {
	//•¨‘Ì‚Ì‘å‚«‚³AˆÊ’u‚ðÝ’è
	_mappedTransform->world  = XMMatrixIdentity();
	_mappedTransform->world *= XMMatrixScaling(_r, _r, _r * 0.5f);
	_mappedTransform->world *= XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
	//•¨‘Ì‚ÌF‚ðÝ’è
	_mappedTransform->color.x = _color.x;
	_mappedTransform->color.y = _color.y;
	_mappedTransform->color.z = _color.z;
}

void MySphere::EndCalc() {
	_pos = _tempPos;
	_vel = _tempVel;
}

void MySphere::Render() const { BasicObject::Render(); }

void MySphere::Adjuster() {
	Char32 name   = _name;
	MyVec3 acc    = _acc;
	MyVec3 vel    = _vel;
	MyVec3 pos    = _pos;
	MyVec3 col    = _color;
	float  rad    = _r;
	float  mass   = _m;
	float  cef    = _e;
	int    frames = 1.f / _dt + 0.5f;

	if (ImGui::InputText  ("Name"  , name.data(), Char32::_kSize)) _name  = name;
	if (ImGui::InputFloat3("Accel" , &acc.x))                _acc   = acc;
	if (ImGui::InputFloat3("Veloc" , &vel.x))                _vel   = vel;
	if (ImGui::InputFloat3("Posit" , &pos.x))                _pos   = pos;
	if (ImGui::InputFloat ("Radius", &rad))                  _r     = rad;
	if (ImGui::InputFloat ("Mass"  , &mass))                 _m     = mass;
	if (ImGui::DragFloat  ("Coeff" , &cef, 5e-3f, 0.f, 1.f)) _e     = cef;
	if (ImGui::InputInt   ("Frames", &frames))               _dt    = 1.f / frames;
	if (ImGui::ColorEdit3 ("Color" , &col.x))                _color = col;
}

void MySphere::Reset() {
	_acc   = _init_acc;
	_vel   = _init_vel;
	_pos   = _init_pos;
	_r     = _init_r;
	_m     = _init_m;
	_e     = _init_e;
	_dt    = _init_dt;
	_color = _init_color;
}

HRESULT MySphere::CreateVertexBufferView() {
	constexpr int vertex_num = U_MAX * (V_MAX + 1);
	Vertex verticesOfSphere[vertex_num];

	for (int v = 0; v <= V_MAX; ++v) {
		for (int u = 0; u < U_MAX; ++u) {
			XMFLOAT3 vertexOfSphere = CalcVertexOfSphere(u, v, U_MAX, V_MAX);
			verticesOfSphere[U_MAX * v + u].pos	   = vertexOfSphere;
			verticesOfSphere[U_MAX * v + u].normal = vertexOfSphere;
			verticesOfSphere[U_MAX * v + u].color  = XMFLOAT3(1, 1, 1);
		}
	}
	return BasicObject::CreateVertexBufferView(
		begin(verticesOfSphere), end(verticesOfSphere), sizeof(Vertex) * vertex_num);
}

HRESULT MySphere::CreateIndexBufferView() {
	constexpr int index_num = 2 * V_MAX * (U_MAX + 1);
	unsigned short indicesOfSphere[index_num];

	CalcIndicesOfSphere(indicesOfSphere, U_MAX, V_MAX);

	return BasicObject::CreateIndexBufferView(
		begin(indicesOfSphere), end(indicesOfSphere), sizeof(unsigned short) * index_num);
}
