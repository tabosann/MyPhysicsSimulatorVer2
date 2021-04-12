#include<DX12Wrapper.h>
#include<ObjectRenderer.h>
#include<BS3.h>
#include<AABB3.h>
#include<MyRectangle.h>
#include<MySphere.h>
#include<Application.h>

using namespace std;

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
	Application& app = Application::Instance();
	if (!app.Init()) return -1;
	app.Run();
	app.ShutDown();
}

