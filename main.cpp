#include "MyWhatTheGolf.h"

// TODO SOMEDAY :: 1. SimpleMath -> XM

/*
 * TODO
 * 1. Seperate ProcessMesh Func. 1. ProcessMesh, 2. ProcessSkinnedMesh
 */

// ����� ���ؼ� main �Լ� ���. �� �Լ� ����Ϸ��� sln -> properties -> Linker -> System -> Subsystem -> Windows�� ����
// int main()
// int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE, char*, int)

// int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
int main()
{
	const int	  width = 1280;
	const int	  height = 960;
	MyWhatTheGolf app(width, height);

	if (!app.Initialize())
		return 1;

	app.Run();

	return 0;
}
