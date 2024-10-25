#include "MyWhatTheGolf.h"

// 디버깅 위해서 main 함수 사용. 위 함수 사용하려면 sln -> properties -> Linker -> System -> Subsystem -> Windows로 설정
// int main()
// int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE, char*, int)

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
	MyWhatTheGolf app(1280, 960);

	if (!app.Initialize())
		return 1;

	while (1)
		;

	return 0;
}

//
// WhatTheGolfApp
//
// Initializer::initialize()
//{
//	windowsInitializer.initialize();
//	directXInitializer.initialize();
//}
//
// wWinMain()
//{
//	wtgApp()
//	wtgApp.initialize();
//
//	wtgApp.run();
//}