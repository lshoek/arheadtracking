#pragma once
#include "oglcontroller.h"
#include "ocvcontroller.h"

class OGLApp
{
public:
	HWND m_hWnd;
	OGLController m_oglController;
	OCVController m_ocvController;

	void ResetTimer();
	void UpdateTimer();
	float sof(float fVal);

	bool InitializeApp(OGLApp*, std::string);
	void RegisterAppClass(HINSTANCE);
	bool CreateAppWindow(std::string);

	void AppBody();
	void Shutdown();

	HINSTANCE GetInstance();

	LRESULT CALLBACK MsgHandlerMain(HWND, UINT, WPARAM, LPARAM);

private:
	HINSTANCE m_hInstance;
	std::string m_sAppName;
	HANDLE m_hMutex;

	bool m_bAppActive;
	DWORD m_dwLastFrame;
	float m_fFrameInterval;
};

namespace Keys
{
	int Key(int iKey);
	int Onekey(int iKey);
	extern char kp[256];
}

extern OGLApp* pMainApp;

void InitScene(LPVOID);
void RenderScene(LPVOID);