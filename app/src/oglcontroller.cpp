#include "oglcontroller.h"

BOOL OGLController::m_bClassRegistered = false;
BOOL OGLController::m_bGlewInitialized = false;

LRESULT CALLBACK SimpleMsgHandler(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	switch (msg)
	{
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		std::cout << "\ndestroying window\n";
		PostQuitMessage(0);
		return 0L;
	case WM_KEYDOWN:
		std::cout << "\ndestroying window\n";
		PostQuitMessage(0);
		return 0L;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
}

BOOL OGLController::InitGLEW(HINSTANCE hInstance)
{
	if (m_bGlewInitialized) return true;

	RegisterSimpleOGLClass(hInstance);

	HWND hWndDummy = CreateWindow(_T(WNDCLASSNAME), _T(WNDTITLE), WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		NULL, hInstance, NULL);

	m_hDC = GetDC(hWndDummy);

	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
		32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};
	int iPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (iPixelFormat == 0) return false;
	if (!SetPixelFormat(m_hDC, iPixelFormat, &pfd)) return false;

	HGLRC hRCDummy = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, hRCDummy);

	bool bResult = true;

	if (!m_bGlewInitialized)
	{
		if (glewInit() != GLEW_OK)
		{
			MessageBox(*m_hWnd, _T("Couldn't initialize GLEW!"), _T("Fatal Error"), MB_ICONERROR);
			bResult = false;
		}
		m_bGlewInitialized = true;
	}
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCDummy);
	DestroyWindow(hWndDummy);

	return bResult;
}

BOOL OGLController::InitOpenGL(HINSTANCE hInstance, HWND* hWnd, int iMajorVersion, int iMinorVersion,
	void(*InitScene)(LPVOID), void(*RenderScene)(LPVOID), void(*ReleaseScene)(LPVOID), LPVOID lpParam)
{
	if (!InitGLEW(hInstance)) return false;

	m_hWnd = hWnd;
	m_hDC = GetDC(*m_hWnd);

	bool bError = false;
	PIXELFORMATDESCRIPTOR pfd;

	if (iMajorVersion <= 2)
	{
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int iPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
		if (iPixelFormat == 0)return false;

		if (!SetPixelFormat(m_hDC, iPixelFormat, &pfd))return false;

		// Create the old style context (OpenGL 2.1 and before)
		m_hRC = wglCreateContext(m_hDC);
		if (m_hRC)wglMakeCurrent(m_hDC, m_hRC);
		else bError = true;
	}
	else if (WGLEW_ARB_create_context && WGLEW_ARB_pixel_format)
	{
		const int iPixelFormatAttribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			0,
		};
		int iContextAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, iMajorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, iMinorVersion,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0,
		};

		int iPixelFormat, iNumFormats;
		wglChoosePixelFormatARB(m_hDC, iPixelFormatAttribList, NULL, 1, &iPixelFormat, (UINT*)&iNumFormats);

		if (!SetPixelFormat(m_hDC, iPixelFormat, &pfd)) return false;
		
		m_hRC = wglCreateContextAttribsARB(m_hDC, 0, iContextAttribs);
		if (m_hRC) wglMakeCurrent(m_hDC, m_hRC);
		else bError = true;
	}
	else bError = true;

	if (bError)
	{
		char sErrorMessage[255];
		char sErrorTitle[255];
		sprintf(sErrorMessage, "OpenGL %d.%d is not supported! Please download latest GPU drivers!", iMajorVersion, iMinorVersion);
		sprintf(sErrorTitle, "OpenGL %d.%d Not Supported", iMajorVersion, iMinorVersion);
		MessageBox(*m_hWnd, _T("DummyErrorMessage"), _T("DummyErrorTitle"), MB_ICONINFORMATION);
		return false;
	}

	m_pRenderScene = RenderScene;
	m_pInitScene = InitScene;
	m_pReleaseScene = ReleaseScene;

	if (m_pInitScene != NULL) m_pInitScene(lpParam);

	return true;
}

void OGLController::ResizeOGLViewportFull()
{
	if (m_hWnd == NULL) return;
	RECT rect;
	GetClientRect(*m_hWnd, &rect);
	glViewport(0, 0, rect.right, rect.bottom);
}

void OGLController::SwapBuffersHDC()
{
	SwapBuffers(m_hDC);
}

void OGLController::MakeCurrent()
{
	wglMakeCurrent(m_hDC, m_hRC);
}

void OGLController::Render(LPVOID lpParam)
{
	if (m_pRenderScene) m_pRenderScene(lpParam);
}

void OGLController::RegisterSimpleOGLClass(HINSTANCE hInstance)
{
	if (m_bClassRegistered)return;
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)SimpleMsgHandler;
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_MENUBAR + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T(WNDCLASSNAME);

	RegisterClassEx(&wc);
	m_bClassRegistered = true;
}

void OGLController::UnregisterSimpleOGLClass(HINSTANCE hInstance)
{
	if (m_bClassRegistered)
	{
		UnregisterClass(_T(WNDCLASSNAME), hInstance);
		m_bClassRegistered = false;
	}
}

void OGLController::ReleaseOGLController(LPVOID lpParam)
{
	if (m_pReleaseScene) m_pReleaseScene(lpParam);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);
	ReleaseDC(*m_hWnd, m_hDC);

	m_hWnd = NULL;
}