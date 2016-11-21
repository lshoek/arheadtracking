#pragma once

#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

#include <glew.h>
#include <wglew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <cassert>
#include <tchar.h>

#define PRINTOGLERROR() printOpenGLError(__FILE__, __LINE__)
#define PRINTOGLQUERY(x) printOpenGLQuery(x)
#define PRINTGLMVEC(x) printGlmVec(x)

#define WND_WIDTH 640
#define WND_HEIGHT 640
#define WND_MARGIN 10

#define WNDCLASSNAME "HEADTRACKING"
#define WNDTITLE "headtracking"

#define WINDOWS_LEAN_AND_MEAN

#define STDMIN(a,b) (((a) < (b)) ? (a) : (b))
#define STDMAX(a,b) (((a) > (b)) ? (a) : (b))
#define INVF(x) (1.0f-x)
#define INVCHAR(x) (255-x)

#define NORMALIZERGB(r,g,b) r/255.0f, g/255.0f, b/255.0f
#define NORMALIZERGBA(r,g,b,a) r/255.0f, g/255.0f, b/255.0f, a/255.0f

#define FOR(q,n) for(int q=0;q<n;++q)
#define SFOR(q,s,e) for(int q=s;q<=e;++q)
#define RFOR(q,n) for(int q=n;q>=0;--q)
#define RSFOR(q,s,e) for(int q=s;q>=e;--q)

#define ESZ(elem) (int)elem.size()

inline int printOpenGLError(char *file, int line)
{
	GLenum glErr;
	int retCode = 0;

	glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
	}
	return retCode;
}

inline void printOpenGLQuery(GLenum e)
{
	GLint temp;
	glGetIntegerv(e, &temp);
	printf("QUERY: %d\n", temp);
}

inline void printGlmVec(glm::vec3 &v)
{
	printf("(%f, %f, %f);\n", v.x, v.y, v.z);
}

/*
wchar_t *StrToLPCWSTR(std::string &str)
{
	wchar_t* wString = new wchar_t[255];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wString, 255);
	return wString;
}
*/
