//----------------------------------------------------------------------------------------------------------------------
//目次: 以下の「SECTION: ***」を検索すれば、その場所にジャンプできます。以下ジャンプの仕方です。
//    : 「Ctrl + F」もしくは「編集→検索と置換→クイック検索」で検索ウィンドウが出現します。
//    : そのウィンドウの検索欄で検索し、Enterを押すとジャンプできます。
//    : 目次に戻るには「Ctrl + G」で「１」を行指定すると簡単に戻れます。
//----------------------------------------------------------------------------------------------------------------------

//初期化
// SECTION: 初期化
// SECTION: 物体配置

//メインループ
// SECTION: 編集
// SECTION: 描画

//後始末
// SECTION: リリース

//ImGuiによるコントロールセンター
// SECTION: ImGui関数群
// - 導入の詳細は → https://bit.ly/3rwZKJE / https://bit.ly/3u2RP8G


//独自のヘッダ群
#include<DX12Wrapper.h>
#include<ObjectRenderer.h>
#include<MyRectangle.h>
#include<MySphere.h>
#include<AABB3.h>
#include<BS3.h>
#include<Application.h>

//ヘルパー
#include"../MyHelpers/MyImGui.h"
#include"../MyHelpers/MyChar32.h"

//標準のヘッダ群
#include<tchar.h>
#include<memory> //std::unique_ptr
#include<functional>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

namespace //ウィンドウサイズ
{
	constexpr int kWindowWidth = 1280;
	constexpr int kWindowHeight = 720;
}

Application::Application() 
{
	//特になし
}

Application::~Application() 
{
	for (int i = 0; i < _rectangles.size(); ++i) delete _rectangles[i];
	for (int i = 0; i < _spheres.size(); ++i)    delete _spheres[i];
	vector<MyRectangle*>().swap(_rectangles);
	vector<MySphere*>().swap(_spheres);
}

Application& Application::Instance()
{
	static Application app;
	return app;
}

bool Application::Init() 
{
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(result)) return false;

	//----------------------------------------------------------------------------------------------------------------------
	// SECTION: 初期化
	//----------------------------------------------------------------------------------------------------------------------
	//メンバ変数の初期化
	WndBase::Init(_T("MyPhysicsEngine ver0.2"), 0, 0, kWindowWidth, kWindowHeight);

	_dx12.reset(new DX12Wrapper(_hwnd, kWindowWidth, kWindowHeight));
	if (!_dx12) return false;

	_renderer.reset(new ObjectRenderer(_dx12.get()));
	if (!_renderer) return false;

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();

	if (!ImGui_ImplWin32_Init(_hwnd))
		return false;

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	_dx12->GetSwapChain()->GetDesc1(&desc);
	if (!ImGui_ImplDX12_Init(
		_dx12->GetDevice(), 3, desc.Format,
		_dx12->GetDescHeapForImGui().Get(), _dx12->GetDescHeapForImGui()->GetCPUDescriptorHandleForHeapStart(),
		_dx12->GetDescHeapForImGui()->GetGPUDescriptorHandleForHeapStart())
		)
		return false;

	//----------------------------------------------------------------------------------------------------------------------
	// SECTION: 物体配置
	//----------------------------------------------------------------------------------------------------------------------
	//直方体の配置
	MyRectangle* rectangles[] = {
		new MyRectangle(_dx12.get(), "Left",
						kZeroVec, kZeroVec, MyVec3(-8.0f,  0.0f,  0.0f), MyVec3( 1.0f, 15.0f,  5.0f), 1.0f, 0.75f
		),
		new MyRectangle(_dx12.get(), "Right",
						kZeroVec, kZeroVec, MyVec3( 8.0f,  0.0f,  0.0f), MyVec3( 1.0f, 15.0f,  5.0f), 1.0f, 0.75f
		),
		new MyRectangle(_dx12.get(), "Far",
						kZeroVec, kZeroVec, MyVec3( 0.0f,  0.0f,  3.0f), MyVec3(15.0f, 15.0f,  1.0f), 1.0f, 0.75f
		),
		new MyRectangle(_dx12.get(), "Top",
						kZeroVec, kZeroVec, MyVec3( 0.0f,  7.0f,  0.0f), MyVec3(15.0f,  1.0f,  5.0f), 1.0f, 0.75f
		),
		new MyRectangle(_dx12.get(), "Bottom",
						kZeroVec, kZeroVec, MyVec3( 0.0f, -7.0f,  0.0f), MyVec3(15.0f,  1.0f,  5.0f), 1.0f, 0.75f
		)
	};
	_rectangles.assign(rectangles, end(rectangles));

	//球体の配置
	//e.g. v0(30.f, 20.f, 0.f), v1(-20.f, 20.f, 0.f), p0(-2.5f, 0.f, 0.f), p1(2.5f, 0.f, 0.f)だといい感じに検証できる。
	const MyVec3 v0(30.f, 20.f, 0.f); const MyVec3 v1(-20.f, 20.f, 0.f);
	const MyVec3 p0(-2.5f, 0.f, 0.f); const MyVec3 p1(2.5f, 0.f, 0.f);
	MySphere* spheres[] = {
		new MySphere(_dx12.get(), kGravity, v0, p0, 0.5f, 1.0f, 0.75f),
		new MySphere(_dx12.get(), kGravity, v1, p1, 0.5f, 1.0f, 0.75f)
	};
	_spheres.assign(spheres, end(spheres));

	return true;
}

void Application::BeginEdit() 
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Application::EndEdit() 
{
	for (auto rec : _rectangles) rec->Update();
	for (auto sph : _spheres)    sph->Update();

	//ここに更新処理を書いていく

	_dx12->Update();
}

void Application::ImGui_Render()
{
	ImGui::Render();
	_dx12->GetCmdList()->SetDescriptorHeaps(1, _dx12->GetDescHeapForImGui().GetAddressOf());
}

void Application::Run()
{
	//ウィンドウの設定
	static ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_AlwaysUseWindowPadding;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	WndBase::Show();
	WndBase::Update();

	while (WndBase::GetCount() != 0) {
		//メッセージ受け取ると0以外の値(= true)を返す。詳細は→ https://bit.ly/3vStQe0
		//e.g. ウィンドウを閉じるとWM_CLOSEメッセージを受け取るので0以外の値(= true)を返す。
		if (WndBase::CatchMsg())
		{
			if (WndBase::GetMsg() == WM_QUIT)
				WndBase::Quit();
			continue;
		}
		WndBase::Close(VK_SPACE);

		//----------------------------------------------------------------------------------------------------------------------
		// SECTION: 編集
		//----------------------------------------------------------------------------------------------------------------------
		BeginEdit();

		//==========================================================================================================
		//以下の方法で衝突位置を計算する。詳細は → 「書籍：実例で学ぶゲーム3D数学」
		// - 求めるべき衝突の瞬間の位置や速度は、最小の衝突時刻を採用して求めればそれっぽく見えることを利用。
		// - (1) 球と球、球と壁など、様々な衝突を考え、それぞれの場合から衝突時刻'step_now'を算出。
		// - (2) その衝突時刻'step_now'が今までで求めた衝突時刻'step_min'より小さければ'step_min'を'step_now'に更新。
		// - (3) (1)(2)をforループすれば、最終的に最小の衝突時刻'step_min'で次の位置や速度を算出する形になる。
		//==========================================================================================================

		if (_play) {

			for (auto rec : _rectangles) rec->BeginCalc();
			for (auto sph : _spheres)    sph->BeginCalc();

			//球と球
			const int kSpheresNum = _spheres.size();
			for (int i = 0; i < kSpheresNum; ++i) { MySphere* s1 = _spheres[i];
			for (int j = i + 1; j < kSpheresNum; ++j) { MySphere* s2 = _spheres[j];

				//--------------------------------------------------------------------------------------
				// (1) 球と球、球と壁など、様々な衝突を考え、それぞれの場合から衝突時刻'step_now'を算出。
				//--------------------------------------------------------------------------------------

				MyVector3 outV1, outV2; //衝突時の速度
				MyVector3 outP1, outP2; //衝突時の位置
				float     step = 0;     //衝突時刻

				//球と球の衝突が起きたらtrue
				bool isCollision = s1->_bs3->IntersectsBS3(
					s1->_acc, s1->_vel, s1->_pos, s1->_r, s1->_dt,
					s2->_acc, s2->_vel, s2->_pos, s2->_r, s2->_dt,
					outV1, outV2, outP1, outP2, step
				);

				//衝突が起こらなければ、衝突処理をスキップ
				if (!isCollision) continue;
				//算出した衝突時刻のほうが大きければ、衝突処理をスキップ
				if (step >= s1->_t || step >= s2->_t) continue;

				//--------------------------------------------------------------------------------------------------------
				// (2) その衝突時刻'step_now'が今までで求めた衝突時刻'step_min'より小さければ'step_min'を'step_now'に更新。
				//--------------------------------------------------------------------------------------------------------

				//衝突時刻を更新
				s1->_t = s2->_t = step;
				//衝突位置を更新
				s1->_tempPos = outP1; s2->_tempPos = outP2;
				//衝突直後の速度を算出し、更新する
				MyPhysics::CalcReflectionVector(
					outV1, outV2, outP1, outP2, s1->_m, s2->_m, s1->_e, s2->_e,
					s1->_tempVel, s2->_tempVel
				);
			}}

			//以下同様に衝突演算を行う

			// FIXME: なぜか必ず若い番号の球体が静止してしまう...
			//球と壁
			for (auto s : _spheres) {
			for (auto r : _rectangles) {

				MyVec3 outPlaneNormal; //平面の法線
				MyVec3 outPosOnPlane;  //平面上の点
				MyVec3 outPos;         //衝突時の球体の位置
				MyVec3 outVel;         //衝突時の球体の速度
				float  step = 0;       //衝突時刻

				MyVec3 rayOrg = s->GetPositionIn(s->_dt, s->_acc, s->_vel - r->_vel);
				MyVec3 rayDir = rayOrg - s->_pos;
				r->_aabb3->GetPlane(s->_pos, rayDir, outPlaneNormal, outPosOnPlane);

				bool isCollision = r->_aabb3->IntersectsBS3(
					s->_acc, s->_vel - r->_vel, s->_pos, s->_r, s->_dt,
					outPosOnPlane, outPlaneNormal.Normalize(), outVel, outPos, step
				);

				if (!isCollision) continue;
				if (step >= s->_t || step >= r->_t) continue;

				r->_t = s->_t = step;
				s->_tempPos = outPos;
				s->_tempVel = MyPhysics::CalcReflectionVector(
					outVel, r->GetVelocityIn(r->_dt), outPlaneNormal.Normalize(), s->_e
				);
			}}

			//-------------------------------------------------------------------------------------------------
			// (3) (1)(2)をforループすれば、最終的に最小の衝突時刻'step_min'で次の位置や速度を算出する形になる。
			//-------------------------------------------------------------------------------------------------

			for (auto rec : _rectangles) rec->EndCalc();
			for (auto sph : _spheres)    sph->EndCalc();
		}
		else {
			//_play == false: 衝突演算をスキップ
		}

		//===============================================================
		//ImGuiの編集
		// - 物体の配置、カメラアングルなどを変更しやすいようにしている。
		// - ImGui::ShowDemoWindow()の実装を見ると、仕組みがわかりやすい。
		//===============================================================

		static bool showDemo = false;
		static bool showDebug = false;
		static bool showSupervisor = false;
		static bool showCreator = false;

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Setting")) {
				if (ImGui::MenuItem("Show Demo"))     showDemo = !showDemo;
				if (ImGui::MenuItem("Debug Console")) showDebug = !showDebug;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Supervisor")) showSupervisor = !showSupervisor;
				if (ImGui::MenuItem("Creator"))    showCreator = !showCreator;
				ImGui::EndMenu();
			}

			ImGui::Dummy(ImVec2(ImGui::GetWindowContentRegionWidth() * 0.7f, 0));

			Char32 text = _play ? "Stop" : "Play";
			if (ImGui::Button(text, ImVec2(100, 0))) _play = !_play;

			text = "All Reset";
			if (ImGui::Button(text, ImVec2(100, 0))) {
				for (auto r : _rectangles) r->Reset();
				for (auto s : _spheres)    s->Reset();
				_dx12->Reset();
			}

			ImGui::EndMainMenuBar();
		}

		if (showDemo)
			ImGui::ShowDemoWindow();

		if (showDebug) { //背景、カメラアングル、FOVなどの変更
			ImGui::Begin("Debug Console", &showDebug, windowFlags);
			ShowDebugConsoleMenu();
			ImGui::End();
		}
		if (showSupervisor) { //フィールド上の物体を管理する
			ImGui::Begin("Supervisor", &showSupervisor);
			ShowSupervisorMenu();
			ImGui::End();
		}
		if (showCreator) { //フィールド上にアイテムを生成したり、削除したりする
			ImGui::Begin("Creator", &showCreator, windowFlags);
			ShowCreatorMenu();
			ImGui::End();
		}

		EndEdit();

		//----------------------------------------------------------------------------------------------------------------------
		// SECTION: 描画
		//----------------------------------------------------------------------------------------------------------------------
		_dx12->BeginRender();
		
		//独自のパイプラインを設定
		_dx12->GetCmdList()->SetGraphicsRootSignature(_renderer->GetRootSignature());
		_dx12->GetCmdList()->SetPipelineState(_renderer->GetGraphicPipeline());
		_dx12->SetSceneDataToShader();

		_dx12->GetCmdList()->IASetPrimitiveTopology(MyRectangle::_kPrimitiveType);
		for (auto rec : _rectangles) rec->Render();

		_dx12->GetCmdList()->IASetPrimitiveTopology(MySphere::_kPrimitiveType);
		for (auto sph : _spheres) sph->Render();

		//ImGuiの描画
		ImGui_Render();

		_dx12->EndRender();

		_dx12->GetSwapChain()->Present(1 /* = 垂直同期あり */, 0);
	}
}

void Application::ShutDown() {
	//----------------------------------------------------------------------------------------------------------------------
	// SECTION: リリース
	//----------------------------------------------------------------------------------------------------------------------
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//----------------------------------------------------------------------------------------------------------------------
// SECTION: ImGui関数群
//----------------------------------------------------------------------------------------------------------------------
void Application::ShowDebugConsoleMenu() {
	XMFLOAT4 clearColor4 = _dx12->_bgColor;
	float fov            = _dx12->_fov;
	XMFLOAT3 camera3     = _dx12->_cameraPos;
	XMFLOAT4 lightDir4   = _dx12->_lightDir;

	ImGui::ColorEdit4 ("Background", &clearColor4.x);
	ImGui::SliderAngle("FOV"       , &fov, 30.f, 150.f);
	ImGui::InputFloat3("Camera"    , &camera3.x);
	ImGui::InputFloat3("Light Dir" , &lightDir4.x);

	_dx12->_bgColor = clearColor4;
	_dx12->_fov = fov;
	_dx12->_cameraPos = camera3;
	_dx12->_lightDir = lightDir4;
}

void Application::ShowSupervisorMenu() {
	//フォーカスしたアイテムの名前と番号
	static Char32 selectedLabel;
	BasicObject*  selectedItem = nullptr;

	//左：アイテムリスト表示メニュー
	const float kWidth = ImGui::GetWindowContentRegionWidth() * 0.3f;
	const float kHeight = 250;
	ImGui::BeginChild("Left", ImVec2(kWidth, 0));

	ImGui::HelpMarker("Delete Menu is shown by right-click on selected item.");

	if (ImGui::BeginListBox("", ImVec2(kWidth, kHeight))) {
		//MyRectangleアイテムの選択
		for (int i = 0; i < _rectangles.size(); ++i) {

			BasicObject* item = _rectangles[i];
			Char32 label = item->_name;

			bool selected = (label == selectedLabel);
			if (ImGui::Selectable(label, selected) || selected) {
				selectedLabel = label;
				selectedItem = item;

				if (ImGui::AskToDelete(_rectangles, i)) {
					selectedLabel = "";
					selectedItem = nullptr;
				}
			}
		}
		//MySphereアイテムの選択
		for (int i = 0; i < _spheres.size(); ++i) {

			BasicObject* item = _spheres[i];
			Char32 label = item->_name;

			bool selected = (label == selectedLabel);
			if (ImGui::Selectable(label, selected) || selected) {
				selectedLabel = label;
				selectedItem = item;

				if (ImGui::AskToDelete(_spheres, i)) {
					selectedLabel = "";
					selectedItem = nullptr;
				}
			}
		}
		ImGui::EndListBox();
	}
	ImGui::EndChild(); //str_id = "Left"

	//アイテム名の被りをなくすための措置として、アイテムを一つのコンテナにまとめてアイテム名を走査できるようにする。
	vector<BasicObject*> itemList;
	itemList.insert(itemList.begin(), _rectangles.cbegin(), _rectangles.cend());
	itemList.insert(itemList.begin() + itemList.size(), _spheres.cbegin(), _spheres.cend());

	//右：アイテム情報表示メニュー
	ImGui::SameLine();
	ImGui::BeginChild("Right");

	ImGui::Text(selectedLabel);
	ImGui::Separator();

	if (selectedItem != nullptr) {
		Char32 prev = selectedItem->_name;
		selectedItem->Adjuster();
		Char32 aftr = selectedItem->_name;

		// FIXME: アイテム名被りをなくすための措置だが、アイテム名の点滅が起こる...
		if (aftr != prev) {
			selectedItem->_name = prev;
			selectedLabel = selectedItem->_name = ImGui::CreateUniqueItemName(itemList, aftr);
		}
	}
	ImGui::EndChild(); //str_id = "Right"
}

void Application::ShowCreatorMenu() {

	vector<BasicObject*> itemList;
	itemList.insert(itemList.begin(), _rectangles.cbegin(), _rectangles.cend());
	itemList.insert(itemList.begin() + itemList.size(), _spheres.cbegin(), _spheres.cend());

	//BeginMenu()  : すぐUIが上書きされて使いづらい
	//BeginTabBar(): ラベル付けのようで使いやすい
	if (ImGui::BeginTabBar("GameObjectTabs")) {
		if (ImGui::BeginTabItem("Rectangle")) {
			static Char32 label("Rectangle");   ImGui::InputText  ("Name"   , label.data(), Char32::_kSize);
			static MyVec3 acl(0.f, -9.8f, 0.f); ImGui::InputFloat3("Accel"  , &acl.x);
			static MyVec3 vel;                  ImGui::InputFloat3("Veloc"  , &vel.x);
			static MyVec3 pos;                  ImGui::InputFloat3("Posit"  , &pos.x);
			static MyVec3 size(1.f, 1.f, 1.f);  ImGui::InputFloat3("Size"   , &size.x);
			static float  mass = 1.f;           ImGui::InputFloat ("Mass"   , &mass);
			static float  cef = 0.5f;           ImGui::DragFloat  ("Coeff"  , &cef, 5e-3f, 0.f, 1.f);
			static int    frames = 30;          ImGui::InputInt   ("Frames" , &frames);
			static MyVec3 col(1.f, 1.f, 1.f);   ImGui::ColorEdit3 ("Color"  , &col.x);

			Char32 name = ImGui::CreateUniqueItemName(itemList, label);

			if (ImGui::Button("Generate"))
				_rectangles.push_back(new MyRectangle(_dx12.get(), name, acl, vel, pos, size, mass, cef, frames, col));
			ImGui::SafeDeleteButton(_rectangles);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Sphere")) {
			static Char32 label("Sphere");      ImGui::InputText  ("Name"   , label.data(), Char32::_kSize);
			static MyVec3 acl(0.f, -9.8f, 0.f); ImGui::InputFloat3("Accel"  , &acl.x);
			static MyVec3 vel;                  ImGui::InputFloat3("Veloc"  , &vel.x);
			static MyVec3 pos;                  ImGui::InputFloat3("Posit"  , &pos.x);
			static float  rad = 0.5f;           ImGui::InputFloat ("Radius" , &rad);
			static float  mass = 1.f;           ImGui::InputFloat ("Mass"   , &mass);
			static float  cef = 0.5f;           ImGui::DragFloat  ("Coeff"  , &cef, 5e-3f, 0.f, 1.f);
			static int    frames = 30;          ImGui::InputInt   ("Frames" , &frames);
			static MyVec3 col(1.f, 1.f, 1.f);   ImGui::ColorEdit3 ("Color"  , &col.x);

			Char32 name = ImGui::CreateUniqueItemName(itemList, label);
			
			if (ImGui::Button("Generate"))
				_spheres.push_back(new MySphere(_dx12.get(), name, acl, vel, pos, rad, mass, cef, frames, col));
			ImGui::SafeDeleteButton(_spheres);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

LRESULT Application::LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_SIZE: //ウィンドウリサイズ

		DXGI_SWAP_CHAIN_DESC1 desc;
		_dx12->GetSwapChain()->GetDesc1(&desc);

		RECT wrc;
		if (!GetWindowRect(_hwnd, &wrc)) break;

		_dx12->_windowSize.cx = wrc.right - wrc.left;
		_dx12->_windowSize.cy = wrc.bottom - wrc.top;
		_dx12->GetSwapChain()->ResizeBuffers(desc.BufferCount, wrc.right - wrc.left, wrc.bottom - wrc.top, desc.Format, desc.Flags);
		
		D3D12_VIEWPORT view;
		view.Width    = (float)desc.Width;
		view.Height   = (float)desc.Height;
		view.TopLeftX = 0.f;
		view.TopLeftY = 0.f;
		view.MaxDepth = 1.f;
		view.MinDepth = 0.f;

		D3D12_RECT rect;
		rect.left   = 0;
		rect.top    = 0;
		rect.right  = rect.left + desc.Width;
		rect.bottom = rect.top + desc.Height;

		_dx12->SetViewPort(&view, &rect);
		break;

	case WM_DESTROY: //ウィンドウ破棄
		::PostQuitMessage(0);
		return 0;
	}

	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	return WndBase::LocalWndProc(hWnd, msg, wParam, lParam);
}

////lpParam = this にすることで、ウィンドウプロシージャをメンバ関数化できる！
////静的なウィンドウプロシージャでthisポインタを得ることで実現。詳細は → https://bit.ly/3qNE8Ze
//LRESULT Application::ApplicationWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
//
//	Application* app = (Application*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
//
//	//取得できた場合
//	if (app) return app->WndProc(hwnd, msg, wparam, lparam);
//
//	//取得できなかった場合
//	if (msg == WM_CREATE) {
//		app = (Application*)((LPCREATESTRUCT)lparam)->lpCreateParams;
//		if (app) {
//			::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
//			return app->WndProc(hwnd, msg, wparam, lparam);
//		}
//	}
//	return ::DefWindowProc(hwnd, msg, wparam, lparam);
//}
