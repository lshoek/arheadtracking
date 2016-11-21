#include "stdafx.h"

#define SHADER_PATH "../data/shaders"
#define SHADER_DEFAULT_VERT "../data/shaders/simple.vert"
#define SHADER_DEFAULT_FRAG "../data/shaders/simple.frag"
#define SHADER_VERTEXCOL_VERT "../data/shaders/vertexcolor.vert"
#define SHADER_VERTEXCOL_FRAG "../data/shaders/vertexcolor.frag"

class OGLShaderProgram 
{
public:
	OGLShaderProgram();
	~OGLShaderProgram();

	GLuint LoadShaderProgram(std::string sVertShader, std::string sFragShader, ...);
	GLuint LinkShaderProgram();
	void UseShaderProgram();

	GLuint CheckShaderProgramErrors();
	void GetInfoLog(std::string sNote = "");

	GLuint DeleteShaderProgram();

	GLuint getProgramId();
	bool isLoaded();

private:
	GLuint m_uiVertShaderId;
	GLuint m_uiFragShaderId;
	GLuint m_uiProgramId;
	bool m_bLoaded;

	GLuint CompileShader(GLuint uiShader, std::string sVertShader);
};
