
#include<DX12Wrapper.h>
#include<BS3.h>
#include<AABB3.h>
#include<MyRectangle.h>
#include<MySphere.h>
#include<Application.h>

#include<fstream>
#include<MyImGui.h>

using namespace std;

namespace
{
	Application& app = Application::Instance();
}

namespace
{
	MyVec3 acc(kGravity);
	MyVec3 vel(kZeroVec);
	MyVec3 pos(-8.f, 0.f, 0.f);
	MyVec3 siz(1.f, 15.f, 5.f);
	bool ImportObjectParamsFromExel();
}

namespace
{
	void Edit();
	void Display();
}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	if (!ImportObjectParamsFromExel()) return -1;
	if (!app.Init()) return -1;

	app.SetEditFunc(::Edit);
	app.SetRenderFunc(::Display);
	
	app.Run();
	app.ShutDown();
}

namespace
{
	void Edit()
	{
		//�Փˌ��m���ɃA�j���[�V�������~����
		void (*StopWhenCollided)(bool& dst, bool src) = [](bool& dst, bool src)->void
		{
			dst = !src && dst;
		};

		//�E�B���h�E�̐ݒ�
		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_AlwaysUseWindowPadding;
		windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

		static bool play = false;
		static bool stopWhenCollided = false;
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

					MyVector3 out_v1, out_v2; //�Փˎ��̑��x
					MyVector3 out_p1, out_p2; //�Փˎ��̈ʒu
					float     step = 0;       //�Փˎ���

					//���Ƌ��̏Փ˂��N������true
					bool collided = s1->_bs3->IntersectsBS3(
						s1->_acc, s1->_vel, s1->_pos, s1->_r, s1->_dt,
						s2->_acc, s2->_vel, s2->_pos, s2->_r, s2->_dt,
						out_v1, out_v2, out_p1, out_p2, step
					);

					//�Փ˂��N����Ȃ���΁A�Փˏ������X�L�b�v
					if (!collided) continue;
					//�Z�o�����Փˎ����̂ق����傫����΁A�Փˏ������X�L�b�v
					if (step >= s1->_t || step >= s2->_t) continue;

					//--------------------------------------------------------------------------------------------------------
					// (2) ���̏Փˎ���'step_now'�����܂łŋ��߂��Փˎ���'step_min'��菬�������'step_min'��'step_now'�ɍX�V�B
					//--------------------------------------------------------------------------------------------------------

					StopWhenCollided(play, stopWhenCollided);

					//�Փˎ������X�V
					s1->_t = s2->_t = step;
					//�Փ˒���̑��x���Z�o���A�X�V����
					MyPhysics::CalcReflectionVector(
						out_v1, out_v2, out_p1, out_p2, s1->_m, s2->_m, s1->_e, s2->_e,
						s1->_tempVel, s2->_tempVel
					);
					//�Փˈʒu���X�V
					//s1->_tempPos = s1->GetPositionIn((1 - step) * s1->_dt, s1->_acc, s1->_tempVel);
					//s2->_tempPos = s2->GetPositionIn((1 - step) * s2->_dt, s2->_acc, s2->_tempVel);
					s1->_tempPos = out_p1; s2->_tempPos = out_p2;
				}
			}

			//�ȉ����l�ɏՓˉ��Z���s��

			// FIXME: �Ȃ����K���Ⴂ�ԍ��̋��̂��Î~���Ă��܂�...
			//���ƕ�
			for (auto s : app._spheres) 
			{
				for (auto r : app._rectangles) 
				{
					MyVec3 out_planeNormal; //���ʂ̖@��
					MyVec3 out_posOnPlane;  //���ʏ�̓_
					MyVec3 out_pos;         //�Փˎ��̋��̂̈ʒu
					MyVec3 out_vel;         //�Փˎ��̋��̂̑��x
					float  step = 0;        //�Փˎ���

					MyVec3 rayOrg = s->GetPositionIn(s->_dt, s->_acc, s->_vel - r->_vel);
					MyVec3 rayDir = rayOrg - s->_pos;
					r->_aabb3->GetPlane(s->_pos, rayDir, out_planeNormal, out_posOnPlane);

					bool collided = r->_aabb3->IntersectsBS3(
						s->_acc, s->_vel - r->_vel, s->_pos, s->_r, s->_dt,
						out_posOnPlane, out_planeNormal.Normalize(), out_vel, out_pos, step
					);

					if (!collided) continue;
					if (step >= s->_t || step >= r->_t) continue;

					StopWhenCollided(play, stopWhenCollided);

					r->_t = s->_t = step;
					s->_tempPos = out_pos;
					s->_tempVel = MyPhysics::CalcReflectionVector(
						out_vel, r->GetVelocityIn(r->_dt), out_planeNormal.Normalize(), s->_e
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
			// play == false: �Փˉ��Z���X�L�b�v
		}

		//---------------------------------------------------------------
		//ImGui�̕ҏW
		// - ���̂̔z�u�A�J�����A���O���Ȃǂ�ύX���₷���悤�ɂ��Ă���B
		// - ImGui::ShowDemoWindow()�̎���������ƁA�d�g�݂��킩��₷���B
		//---------------------------------------------------------------

		static bool showDemo       = false;
		static bool showDebug      = false;
		static bool showSupervisor = false;
		static bool showCreator    = false;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Setting"))
			{
				if (ImGui::MenuItem("Show Demo"))     showDemo = !showDemo;
				if (ImGui::MenuItem("Debug Console")) showDebug = !showDebug;
				ImGui::Checkbox("Stop When Collided", &stopWhenCollided);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Supervisor")) showSupervisor = !showSupervisor;
				if (ImGui::MenuItem("Creator"))    showCreator = !showCreator;
				ImGui::EndMenu();
			}

			ImGui::Dummy(ImVec2(ImGui::GetWindowContentRegionWidth() * 0.7f, 0));

			ImGui::PlayButton(play);
			ImGui::ResetButton(&app);

			ImGui::EndMainMenuBar();
		}

		if (showDemo)
			ImGui::ShowDemoWindow();

		if (showDebug) //�w�i�A�J�����A���O���AFOV�Ȃǂ̕ύX
		{ 
			ImGui::Begin("Debug Console", &showDebug, windowFlags);
			app.ShowDebugConsoleMenu();
			ImGui::End();
		}
		if (showSupervisor) //�t�B�[���h��̕��̂��Ǘ�����
		{ 
			ImGui::Begin("Supervisor", &showSupervisor);
			app.ShowSupervisorMenu();
			ImGui::End();
		}
		if (showCreator) //�t�B�[���h��ɃA�C�e���𐶐�������A�폜�����肷��
		{ 
			ImGui::Begin("Creator", &showCreator, windowFlags);
			app.ShowCreatorMenu();
			ImGui::End();
		}
	}

	void Display()
	{
		app.GetDX12()->GetCmdList()->IASetPrimitiveTopology(MyRectangle::_kPrimitiveType);
		for (auto rec : app._rectangles) rec->Render();

		app.GetDX12()->GetCmdList()->IASetPrimitiveTopology(MySphere::_kPrimitiveType);
		for (auto sph : app._spheres) sph->Render();
	}

	bool ImportObjectParamsFromExel()
	{
		ifstream file("ObjectParams.csv");
		if (!file) return false;

		while (true)
		{
			string str;
			file >> str;
			if (!file) break;
			cout << str << endl;
		}
		return true;
	}
}
