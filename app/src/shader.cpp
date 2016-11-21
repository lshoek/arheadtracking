#include "shader.h"

OGLShaderProgram::OGLShaderProgram()
{
	m_bLoaded = false;
}

OGLShaderProgram::~OGLShaderProgram()
{
	glDeleteShader(m_uiProgramId);
}

GLuint OGLShaderProgram::LoadShaderProgram(std::string sVertShader, std::string sFragShader, ...)
{
	/* CREATE SHADERS */
	m_uiVertShaderId = glCreateShader(GL_VERTEX_SHADER);
	m_uiFragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	/* COMPILE SHADERS */
	CompileShader(m_uiVertShaderId, sVertShader);
	CompileShader(m_uiFragShaderId, sFragShader);

	/* CREATE SHADER PROGRAM */
	m_uiProgramId = glCreateProgram();
	glAttachShader(m_uiProgramId, m_uiVertShaderId);
	glAttachShader(m_uiProgramId, m_uiFragShaderId);
	//glDeleteShader(m_uiVertShaderId);
	//glDeleteShader(m_uiFragShaderId);

	/* BIND ATTRIBUTES */
	va_list attribs;
	va_start(attribs, sFragShader);

	char* pNextArg;
	int iArgCount = va_arg(attribs, int);
	FOR(i, iArgCount) {
		int index = va_arg(attribs, int);
		pNextArg = va_arg(attribs, char*);
		glBindAttribLocation(m_uiProgramId, index, pNextArg);
	}
	va_end(attribs);

	m_bLoaded = true;
	return 1;
}

GLuint OGLShaderProgram::CompileShader(GLuint uiShader, std::string sRawShader)
{
	std::vector<std::string> sLines;
	char sLine[255];

	FILE* fp = fopen(sRawShader.c_str(), "rt");
	if (!fp) return GLuint(NULL);
	while (fgets(sLine, 255, fp)) sLines.push_back(sLine);
	fclose(fp);

	const char** vsProgram = new const char*[ESZ(sLines)];
	FOR(i, ESZ(sLines)) vsProgram[i] = sLines[i].c_str();

	glShaderSource(uiShader, ESZ(sLines), vsProgram, NULL);
	glCompileShader(uiShader);
	delete[] vsProgram;

	GLint iCompilationStatus;
	char log[1024];
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iCompilationStatus);
	if (iCompilationStatus == GL_FALSE) {
		glGetShaderInfoLog(uiShader, 1024, NULL, log);
		printf("%s/n", log);
		return GLuint(NULL);
	}
	return 1;
}

GLuint OGLShaderProgram::LinkShaderProgram()
{
	glLinkProgram(m_uiProgramId);
	return 1;
}

GLuint OGLShaderProgram::CheckShaderProgramErrors()
{
	GLint iStatus0, iStatus1;
	glGetProgramiv(m_uiProgramId, GL_LINK_STATUS, &iStatus0);
	if (iStatus0 == GL_FALSE) {
		GetInfoLog("GL_LINK_STATUS");
		return GLuint(NULL);
	}
	glValidateProgram(m_uiProgramId);
	glGetProgramiv(m_uiProgramId, GL_VALIDATE_STATUS, &iStatus1);
	if (iStatus1 == GL_FALSE) {
		GetInfoLog("GL_VALIDATE_STATUS");
		return GLuint(NULL);
	}
	printf("SUCCESSFULLY LINKED PROGRAM %d\n", m_uiProgramId);
}

void OGLShaderProgram::GetInfoLog(std::string sNote)
{
	char log[1024];
	glGetProgramInfoLog(m_uiProgramId, 1024, NULL, log);
	std::cout << "SHADER COMPILE ERROR: " << log << '(' << sNote << ')' << std::endl;
}

void OGLShaderProgram::UseShaderProgram()
{
	glUseProgram(m_uiProgramId);
}

bool OGLShaderProgram::isLoaded() { return m_bLoaded; }
GLuint OGLShaderProgram::getProgramId() { return m_uiProgramId; }