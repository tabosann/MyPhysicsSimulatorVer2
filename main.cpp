#include<DX12Wrapper.h>
#include<ObjectRenderer.h>
#include<BS3.h>
#include<AABB3.h>
#include<MyRectangle.h>
#include<MySphere.h>
#include<Application.h>

using namespace std;

namespace //"MyPhysicsEngine For DX12"���������߂̃A�v��
{
	Application& app = Application::Instance();
}

namespace //�O���錾
{
	//void MainLoop();
	//void CalcCollision();
	void Display(); //���C�����[�v���̏���
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
		//�Đ��E��~
		static bool play = false;

		//�E�B���h�E�̐ݒ�
		static ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_AlwaysUseWindowPadding;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

		//----------------------------------------------------------------------------------------------------------------------
		// SECTION: �ҏW
		//----------------------------------------------------------------------------------------------------------------------
		app.BeginEdit();

		//==========================================================================================================
		//�ȉ��̕��@�ŏՓˈʒu���v�Z����B�ڍׂ� �� �u���ЁF����Ŋw�ԃQ�[��3D���w�v
		// - ���߂�ׂ��Փ˂̏u�Ԃ̈ʒu�⑬�x�́A�ŏ��̏Փˎ������̗p���ċ��߂�΂�����ۂ������邱�Ƃ𗘗p�B
		// - (1) ���Ƌ��A���ƕǂȂǁA�l�X�ȏՓ˂��l���A���ꂼ��̏ꍇ����Փˎ���'step_now'���Z�o�B
		// - (2) ���̏Փˎ���'step_now'�����܂łŋ��߂��Փˎ���'step_min'��菬�������'step_min'��'step_now'�ɍX�V�B
		// - (3) (1)(2)��for���[�v����΁A�ŏI�I�ɍŏ��̏Փˎ���'step_min'�Ŏ��̈ʒu�⑬�x���Z�o����`�ɂȂ�B
		//==========================================================================================================
		
		if (play)
		{
			for (auto rec : app._rectangles) rec->BeginCalc();
			for (auto sph : app._spheres)    sph->BeginCalc();

			//���Ƌ�
			const int kSpheresNum = app._spheres.size();
			for (int i = 0; i < kSpheresNum; ++i) 
			{
				MySphere* s1 = app._spheres[i];
				for (int j = i + 1; j < kSpheresNum; ++j) 
				{
					MySphere* s2 = app._spheres[j];

					//--------------------------------------------------------------------------------------
					// (1) ���Ƌ��A���ƕǂȂǁA�l�X�ȏՓ˂��l���A���ꂼ��̏ꍇ����Փˎ���'step_now'���Z�o�B
					//--------------------------------------------------------------------------------------

					MyVector3 outV1, outV2; //�Փˎ��̑��x
					MyVector3 outP1, outP2; //�Փˎ��̈ʒu
					float     step = 0;     //�Փˎ���

					//���Ƌ��̏Փ˂��N������true
					bool isCollision = s1->_bs3->IntersectsBS3(
						s1->_acc, s1->_vel, s1->_pos, s1->_r, s1->_dt,
						s2->_acc, s2->_vel, s2->_pos, s2->_r, s2->_dt,
						outV1, outV2, outP1, outP2, step
					);

					//�Փ˂��N����Ȃ���΁A�Փˏ������X�L�b�v
					if (!isCollision) continue;
					//�Z�o�����Փˎ����̂ق����傫����΁A�Փˏ������X�L�b�v
					if (step >= s1->_t || step >= s2->_t) continue;

					//--------------------------------------------------------------------------------------------------------
					// (2) ���̏Փˎ���'step_now'�����܂łŋ��߂��Փˎ���'step_min'��菬�������'step_min'��'step_now'�ɍX�V�B
					//--------------------------------------------------------------------------------------------------------

					//�Փˎ������X�V
					s1->_t = s2->_t = step;
					//�Փˈʒu���X�V
					s1->_tempPos = outP1; s2->_tempPos = outP2;
					//�Փ˒���̑��x���Z�o���A�X�V����
					MyPhysics::CalcReflectionVector(
						outV1, outV2, outP1, outP2, s1->_m, s2->_m, s1->_e, s2->_e,
						s1->_tempVel, s2->_tempVel
					);
				}
			}

			//�ȉ����l�ɏՓˉ��Z���s��

			// FIXME: �Ȃ����K���Ⴂ�ԍ��̋��̂��Î~���Ă��܂�...
			//���ƕ�
			for (auto s : app._spheres) 
			{
				for (auto r : app._rectangles) 
				{
					MyVec3 outPlaneNormal; //���ʂ̖@��
					MyVec3 outPosOnPlane;  //���ʏ�̓_
					MyVec3 outPos;         //�Փˎ��̋��̂̈ʒu
					MyVec3 outVel;         //�Փˎ��̋��̂̑��x
					float  step = 0;       //�Փˎ���

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
			// (3) (1)(2)��for���[�v����΁A�ŏI�I�ɍŏ��̏Փˎ���'step_min'�Ŏ��̈ʒu�⑬�x���Z�o����`�ɂȂ�B
			//-------------------------------------------------------------------------------------------------

			for (auto rec : app._rectangles) rec->EndCalc();
			for (auto sph : app._spheres)    sph->EndCalc();
		}
		else
		{
			//_play == false: �Փˉ��Z���X�L�b�v
		}

		//===============================================================
		//ImGui�̕ҏW
		// - ���̂̔z�u�A�J�����A���O���Ȃǂ�ύX���₷���悤�ɂ��Ă���B
		// - ImGui::ShowDemoWindow()�̎���������ƁA�d�g�݂��킩��₷���B
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

		if (showDebug) { //�w�i�A�J�����A���O���AFOV�Ȃǂ̕ύX
			ImGui::Begin("Debug Console", &showDebug, windowFlags);
			app.ShowDebugConsoleMenu();
			ImGui::End();
		}
		if (showSupervisor) { //�t�B�[���h��̕��̂��Ǘ�����
			ImGui::Begin("Supervisor", &showSupervisor);
			app.ShowSupervisorMenu();
			ImGui::End();
		}
		if (showCreator) { //�t�B�[���h��ɃA�C�e���𐶐�������A�폜�����肷��
			ImGui::Begin("Creator", &showCreator, windowFlags);
			app.ShowCreatorMenu();
			ImGui::End();
		}

		app.EndEdit();

		//----------------------------------------------------------------------------------------------------------------------
		// SECTION: �`��
		//----------------------------------------------------------------------------------------------------------------------
		ObjectRenderer* renderer = app.GetRenderer();

		dx12->BeginRender();

		//�Ǝ��̃p�C�v���C����ݒ�
		dx12->GetCmdList()->SetGraphicsRootSignature(renderer->GetRootSignature());
		dx12->GetCmdList()->SetPipelineState(renderer->GetGraphicPipeline());
		dx12->SetSceneDataToShader();

		dx12->GetCmdList()->IASetPrimitiveTopology(MyRectangle::_kPrimitiveType);
		for (auto rec : app._rectangles) rec->Render();

		dx12->GetCmdList()->IASetPrimitiveTopology(MySphere::_kPrimitiveType);
		for (auto sph : app._spheres) sph->Render();

		//ImGui�̕`��
		app.ImGui_Render();

		dx12->EndRender();

		dx12->GetSwapChain()->Present(1 /* = ������������ */, 0);
	} /* void display() */
}