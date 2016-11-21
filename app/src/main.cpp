#define NOMINMAX
#include <windows.h>
#include <glew.h>
#include <wglew.h>
#include <glut.h>
#include "oglapp.h"
#include "oglcontroller.h"
#include "stdafx.h"

OGLApp APP;

int main(int argc, char** argv)
{
	HINSTANCE hInst = GetModuleHandle(0);
	HWND consoleWnd = GetConsoleWindow();
	SetWindowPos(consoleWnd, 0, WND_MARGIN, WND_HEIGHT + WND_MARGIN, WND_WIDTH, 0, SWP_NOSIZE | SWP_NOZORDER);

	if (!APP.InitializeApp(&APP, WNDCLASSNAME)) return 0;

	APP.RegisterAppClass(hInst);

	if (!APP.CreateAppWindow(WNDTITLE)) return 0;

	APP.ResetTimer();
	APP.AppBody();
	APP.Shutdown();
	return 0;
}
