#include<Application.h>

using namespace std;

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
	Application& app = Application::Instance();
	if (!app.Init()) return -1;
	//app.Run();

	app.Show();
	app.Update();
	while (WndBase::GetCount() != 0)
	{
		if (WndBase::HelperForMsgProcess(app)) continue;
		app.Close(VK_SPACE);

		cout << "*";
	}
	app.ShutDown();
}
