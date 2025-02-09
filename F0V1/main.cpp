#include "Windows.h"
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	OutputDebugString(L"Hello, World!\n");
	std::getchar();
	return 0;
}

