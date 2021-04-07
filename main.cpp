#include<DX12Wrapper.h>
#include<ObjectRenderer.h>
#include<BS3.h>
#include<AABB3.h>
#include<MyRectangle.h>
#include<MySphere.h>
#include<Application.h>

using namespace std;

namespace //"MyPhysicsEngine For DX12"を扱うためのアプリ
{
	Application& app = Application::Instance();
}

namespace //前方宣言
{
	//void MainLoop();
	//void CalcCollision();
	void Display(); //メインループ内の処理
}

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
	if (!app.Init()) return -1;
	app.SetMainFunc(Display);
	app.Run();
	app.ShutDown();
}

namespace
{
	void Display()
	{
		//再生・停止
		static bool play = false;

		//ウィンドウの設定
		static ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_AlwaysUseWindowPadding;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

		//----------------------------------------------------------------------------------------------------------------------
		// SECTION: 編集
		//----------------------------------------------------------------------------------------------------------------------
		app.BeginEdit();

		//==========================================================================================================
		//以下の方法で衝突位置を計算する。詳細は → 「書籍：実例で学ぶゲーム3D数学」
		// - 求めるべき衝突の瞬間の位置や速度は、最小の衝突時刻を採用して求めればそれっぽく見えることを利用。
		// - (1) 球と球、球と壁など、様々な衝突を考え、それぞれの場合から衝突時刻'step_now'を算出。
		// - (2) その衝突時刻'step_now'が今までで求めた衝突時刻'step_min'より小さければ'step_min'を'step_now'に更新。
		// - (3) (1)(2)をforループすれば、最終的に最小の衝突時刻'step_min'で次の位置や速度を算出する形になる。
		//==========================================================================================================
		
		if (play)
		{
			for (auto rec : app._rectangles) rec->BeginCalc();
			for (auto sph : app._spheres)    sph->BeginCalc();

			//球と球
			const int kSpheresNum = app._spheres.size();
			for (int i = 0; i < kSpheresNum; ++i) 
			{
				MySphere* s1 = app._spheres[i];
				for (int j = i + 1; j < kSpheresNum; ++j) 
				{
					MySphere* s2 = app._spheres[j];

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
				}
			}

			//以下同様に衝突演算を行う

			// FIXME: なぜか必ず若い番号の球体が静止してしまう...
			//球と壁
			for (auto s : app._spheres) 
			{
				for (auto r : app._rectangles) 
				{
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
				}
			}

			//-------------------------------------------------------------------------------------------------
			// (3) (1)(2)をforループすれば、最終的に最小の衝突時刻'step_min'で次の位置や速度を算出する形になる。
			//-------------------------------------------------------------------------------------------------

			for (auto rec : app._rectangles) rec->EndCalc();
			for (auto sph : app._spheres)    sph->EndCalc();
		}
		else
		{
			//_play == false: 衝突演算をスキップ
		}

		//===============================================================
		//ImGuiの編集
		// - 物体の配置、カメラアングルなどを変更しやすいようにしている。
		// - ImGui::ShowDemoWindow()の実装を見ると、仕組みがわかりやすい。
		//===============================================================

		DX12Wrapper* dx12 = app.GetDX12();

		static bool  showDemo = false;
		static bool  showDebug = false;
		static bool  showSupervisor = false;
		static bool  showCreator = false;

		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu("Setting")) 
			{
				if (ImGui::MenuItem("Show Demo"))     showDemo = !showDemo;
				if (ImGui::MenuItem("Debug Console")) showDebug = !showDebug;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) 
			{
				if (ImGui::MenuItem("Supervisor")) showSupervisor = !showSupervisor;
				if (ImGui::MenuItem("Creator"))    showCreator = !showCreator;
				ImGui::EndMenu();
			}

			ImGui::Dummy(ImVec2(ImGui::GetWindowContentRegionWidth() * 0.7f, 0));

			Char32 text = play ? "Stop" : "Play";
			if (ImGui::Button(text, ImVec2(100, 0))) play = !play;

			text = "All Reset";
			if (ImGui::Button(text, ImVec2(100, 0))) 
			{
				for (auto r : app._rectangles) r->Reset();
				for (auto s : app._spheres)    s->Reset();
				dx12->Reset();
			}

			ImGui::EndMainMenuBar();
		}

		if (showDemo)
			ImGui::ShowDemoWindow();

		if (showDebug) { //背景、カメラアングル、FOVなどの変更
			ImGui::Begin("Debug Console", &showDebug, windowFlags);
			app.ShowDebugConsoleMenu();
			ImGui::End();
		}
		if (showSupervisor) { //フィールド上の物体を管理する
			ImGui::Begin("Supervisor", &showSupervisor);
			app.ShowSupervisorMenu();
			ImGui::End();
		}
		if (showCreator) { //フィールド上にアイテムを生成したり、削除したりする
			ImGui::Begin("Creator", &showCreator, windowFlags);
			app.ShowCreatorMenu();
			ImGui::End();
		}

		app.EndEdit();

		//----------------------------------------------------------------------------------------------------------------------
		// SECTION: 描画
		//----------------------------------------------------------------------------------------------------------------------
		ObjectRenderer* renderer = app.GetRenderer();

		dx12->BeginRender();

		//独自のパイプラインを設定
		dx12->GetCmdList()->SetGraphicsRootSignature(renderer->GetRootSignature());
		dx12->GetCmdList()->SetPipelineState(renderer->GetGraphicPipeline());
		dx12->SetSceneDataToShader();

		dx12->GetCmdList()->IASetPrimitiveTopology(MyRectangle::_kPrimitiveType);
		for (auto rec : app._rectangles) rec->Render();

		dx12->GetCmdList()->IASetPrimitiveTopology(MySphere::_kPrimitiveType);
		for (auto sph : app._spheres) sph->Render();

		//ImGuiの描画
		app.ImGui_Render();

		dx12->EndRender();

		dx12->GetSwapChain()->Present(1 /* = 垂直同期あり */, 0);
	} /* void display() */
}