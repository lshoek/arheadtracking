#include "oglapp.h"

OGLApp* pMainApp;
char Keys::kp[256] = { 0 };

int Keys::Key(int iKey)
{
	return (GetAsyncKeyState(iKey) >> 15) & 1;
}

int Keys::Onekey(int iKey)
{
	if (Key(iKey) && !kp[iKey])
	{ 
		kp[iKey] = 1; 
		return 1; 
	}
	if (!Key(iKey)) kp[iKey] = 0;
	return 0;
}

void OGLApp::ResetTimer()
{
	m_dwLastFrame = GetTickCount();
	m_fFrameInterval = 0.0f;
}

void OGLApp::UpdateTimer()
{
	DWORD dwCur = GetTickCount();
	m_fFrameInterval = float(dwCur - m_dwLastFrame)*0.001f;
	m_dwLastFrame = dwCur;
}

float OGLApp::sof(float fVal)
{
	return fVal*m_fFrameInterval;
}

bool OGLApp::InitializeApp(OGLApp *appRef, std::string appName)
{
	pMainApp = appRef;
	m_sAppName = appName;
	m_hMutex = CreateMutex(NULL, 1, _T(WNDCLASSNAME));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, _T("This application already runs!"), _T("Multiple Instances Found."), MB_ICONINFORMATION | MB_OK);
		return 0;
	}

	m_ocvController.InitOCV();
	m_oglController.m_pOCVRef = &m_ocvController;

	return 1;
}

LRESULT CALLBACK GlobalMessageHandler(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	return pMainApp->MsgHandlerMain(hWnd, uiMsg, wParam, lParam);
}

void OGLApp::RegisterAppClass(HINSTANCE a_hInstance)
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.hIcon = LoadIcon(m_hInstance, IDI_WINLOGO);
	wcex.hIconSm = LoadIcon(m_hInstance, IDI_WINLOGO);
	wcex.hCursor = LoadCursor(m_hInstance, IDC_ARROW);
	wcex.hInstance = m_hInstance;
	wcex.lpfnWndProc = GlobalMessageHandler;
	wcex.lpszClassName = _T(WNDCLASSNAME);
	wcex.lpszMenuName = NULL;

	RegisterClassEx(&wcex);
}

bool OGLApp::CreateAppWindow(std::string sTitle)
{
	m_hWnd = CreateWindowEx(
		0, _T(WNDCLASSNAME), _T(WNDTITLE), 
		WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
		WND_MARGIN, 0, WND_WIDTH, WND_HEIGHT,
		NULL, NULL, m_hInstance, NULL
	);

	if (!m_oglController.InitOpenGL(m_hInstance, &m_hWnd, 3, 1, InitScene, RenderScene, NULL, &m_oglController)) return false;

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(m_hWnd);
	return true;
}

void OGLApp::AppBody()
{
	MSG msg;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;	// If the message was WM_QUIT then exit application
			else
			{
				TranslateMessage(&msg);	// Otherwise send message to appropriate window
				DispatchMessage(&msg);
			}
		}
		else if (m_bAppActive)
		{
			UpdateTimer();
			m_ocvController.Update();
			m_oglController.Render(&m_oglController);
		}
		else Sleep(200);	// Do not consume processor power if application isn't active
	}
}

void OGLApp::Shutdown()
{
	m_oglController.ReleaseOGLController(&m_oglController);

	DestroyWindow(m_hWnd);
	UnregisterClass(_T(WNDCLASSNAME), m_hInstance);
	OGLController::UnregisterSimpleOGLClass(m_hInstance);
	ReleaseMutex(m_hMutex);
}

LRESULT CALLBACK OGLApp::MsgHandlerMain(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (uiMsg)
	{
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_ACTIVATE:
	{
		switch (LOWORD(wParam))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			m_bAppActive = true;
			break;
		case WA_INACTIVE:
			m_bAppActive = false;
			break;
		}
		break;
	}
	case WM_SIZE:
		m_oglController.ResizeOGLViewportFull();
		break;
	default:
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}
	return 0;
}

HINSTANCE OGLApp::GetInstance()
{
	return m_hInstance;
}
