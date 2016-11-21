#pragma once
#include <windows.h>
#include "ocvcontroller.h"
#include "stdafx.h"

class OGLController
{
public:
	BOOL InitOpenGL(HINSTANCE hInstance, HWND* hWnd, int iMajorVersion, int iMinorVersion, 
		void(*InitScene)(LPVOID), void(*RenderScene)(LPVOID), void(*ReleaseScene)(LPVOID), LPVOID lpParam);

	void ResizeOGLViewportFull();

	void Render(LPVOID lpParam);
	void ReleaseOGLController(LPVOID lpParam);

	static void RegisterSimpleOGLClass(HINSTANCE);
	static void UnregisterSimpleOGLClass(HINSTANCE);

	void MakeCurrent();
	void SwapBuffersHDC();

	OCVController* m_pOCVRef;

private:
	BOOL InitGLEW(HINSTANCE hInstance);

	HDC m_hDC;
	HWND* m_hWnd;
	HGLRC m_hRC;

	static BOOL m_bClassRegistered;
	static BOOL m_bGlewInitialized;

	int m_iMajorVersion;
	int m_iMinorVersion;

	void(*m_pInitScene)(LPVOID);
	void(*m_pRenderScene)(LPVOID);
	void(*m_pReleaseScene)(LPVOID);
};