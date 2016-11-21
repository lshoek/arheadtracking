#include "stdafx.h"
#include "oglcontroller.h"
#include "shader.h"

OGLShaderProgram* defaultShader;
OGLShaderProgram* vertexColShader;

GLuint vboIds[2];
GLuint vaoIds[2];

GLfloat fNear = 0.01f;
GLfloat fFar = 60.0f;
GLfloat fFovy = 80.0f;

GLuint iGridScale = 1.0f;
GLuint iGridSize = 8*8;
GLfloat fElapsedTime = 0;

void InitScene(LPVOID lpParam)
{
	glClearColor(NORMALIZERGBA(0, 0, 0, 255));

	/* FILL VBO 0 */
	glm::vec3* verts = new glm::vec3[iGridSize];
	GLint iRowCol = glm::sqrt(iGridSize);

	for (int i=0; i<=iRowCol; i++) {
		verts[4*i] =	glm::vec3(0,								0, (i/float(iRowCol))*iGridScale);
		verts[4*i+1] =	glm::vec3(iGridScale,						0, (i/float(iRowCol))*iGridScale);
		verts[4*i+2] =	glm::vec3((i/float(iRowCol))*iGridScale,	0, 0);
		verts[4*i+3] =	glm::vec3((i/float(iRowCol))*iGridScale,	0, iGridScale);
	}

	/* FILL VBO 1 */
	float fTriangle[9];
	fTriangle[0] = 0.1f; fTriangle[1] = 0.5f; fTriangle[2] = 0.2f;
	fTriangle[3] = 0.7f; fTriangle[4] = 0.4f; fTriangle[5] = 0.2f;
	fTriangle[6] = 0.2f; fTriangle[7] = 0.0f; fTriangle[8] = 0.2f;

	float fTriangleCol[9];
	fTriangleCol[0] = 1.0f; fTriangleCol[1] = 0.0f; fTriangleCol[2] = 0.0f;
	fTriangleCol[3] = 0.0f; fTriangleCol[4] = 1.0f; fTriangleCol[5] = 0.0f;
	fTriangleCol[6] = 0.0f; fTriangleCol[7] = 0.0f; fTriangleCol[8] = 1.0f;

	/* GENERATE + BIND BUFFERS */
	glGenBuffers(4, vboIds);		// generate new VBO, get associated IDs
	glGenVertexArrays(2, vaoIds);	// generate new VAO, get associated IDs

	/* WIREFRAME */
	glBindVertexArray(vaoIds[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, iGridSize*sizeof(glm::vec3), verts, GL_STATIC_DRAW);	// upload data to VBO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	/* TRIANGLE */
	glBindVertexArray(vaoIds[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
	glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), fTriangle, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), fTriangleCol, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] verts;		
	
	/* LOAD + LINK SHADER PROGRAMS */
	defaultShader = new OGLShaderProgram();
	defaultShader->LoadShaderProgram(SHADER_DEFAULT_VERT, SHADER_DEFAULT_FRAG, 4, 
		0, "vertex",
		1, "normal",
		2, "texcoord0",
		3, "color");
	defaultShader->LinkShaderProgram();
	defaultShader->CheckShaderProgramErrors();

	vertexColShader = new OGLShaderProgram();
	vertexColShader->LoadShaderProgram(SHADER_VERTEXCOL_VERT, SHADER_VERTEXCOL_FRAG, 4,
		0, "vertex",
		1, "normal",
		2, "texcoord0",
		3, "color");
	vertexColShader->LinkShaderProgram();
	vertexColShader->CheckShaderProgramErrors();
}

void RenderScene(LPVOID lpParam)
{
	/* Typecast lpParam to OGLControl pointer */
	OGLController* oc = (OGLController*)lpParam;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fElapsedTime = clock();

	/* OCV DATA */
	OCVController::OCVData ocvData = oc->m_pOCVRef->GetData();
	glm::vec3 vHeadNormalized = ocvData.vHeadPositionNormalized;
	GLfloat fAspect = WND_WIDTH / GLfloat(WND_HEIGHT);

	PRINTGLMVEC(vHeadNormalized);

	defaultShader->UseShaderProgram();
	glBindVertexArray(vaoIds[0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLint iModelViewLoc = glGetUniformLocation(defaultShader->getProgramId(), "mModelView");
	GLint iProjectionLoc = glGetUniformLocation(defaultShader->getProgramId(), "mProjection");

	/* FRUSTUM TRANSFORMATION */
	glm::vec3 pa = glm::vec3(-0.5f, -0.5f, 0);
	glm::vec3 pb = glm::vec3(0.5f, -0.5f, 0);
	glm::vec3 pc = glm::vec3(-0.5f, 0.5f, 0);

	glm::vec3 vRight = pb - pa;
	glm::vec3 vUp = pc - pa;
	vRight = glm::normalize(vRight);
	vUp = glm::normalize(vUp);

	glm::vec3 vNormal = glm::cross(vRight, vUp);	
	vNormal = glm::normalize(vNormal);

	// screen corner vectors
	glm::vec3 va = pa - vHeadNormalized;
	glm::vec3 vb = pb - vHeadNormalized;
	glm::vec3 vc = pc - vHeadNormalized;

	// distance from eye to screen plane
	GLfloat d = -glm::dot(va, vNormal);

	// extent the perpendicular projection.
	GLfloat fLeft = glm::dot(vRight, va) * fNear / d;
	GLfloat fRight = glm::dot(vRight, vb) * fNear / d;
	GLfloat fBottom = glm::dot(vUp, va) * fNear / d;
	GLfloat fTop = glm::dot(vUp, vc) * fNear / d;

	glm::mat4 mCurrent;
	glm::mat4 mView = glm::lookAt(vNormal, glm::vec3(0) , vUp);
	mView = glm::translate(mView, -vHeadNormalized);

	//glm::mat4 mProjection = glm::perspective(fFovy, fAspect, fNear, fFar);
	glm::mat4 mProjection = glm::frustum(fLeft, fRight, fBottom, fTop, fNear, fFar+d);

	glUniformMatrix4fv(iProjectionLoc, 1, GL_FALSE, glm::value_ptr(mProjection));
	PRINTOGLERROR();

	/* TOP */
	mCurrent = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0));
	mCurrent = mView * mCurrent;
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_LINES, 0, iGridSize);

	/* BOTTOM */
	mCurrent = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, 0));
	mCurrent = mView * mCurrent;
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_LINES, 0, iGridSize);

	/* RIGHT */
	mCurrent = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1.0f));
	mCurrent = glm::translate(mCurrent, glm::vec3(-0.5f, 0.5f, 0));
	mCurrent = mView * mCurrent;
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_LINES, 0, iGridSize);

	/* LEFT */
	mCurrent = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1.0f));
	mCurrent = glm::translate(mCurrent, glm::vec3(-0.5f, -0.5f, 0));
	mCurrent = mView * mCurrent;
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_LINES, 0, iGridSize);

	/* FRONT */
	mCurrent = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0, 0));
	mCurrent = glm::translate(mCurrent, glm::vec3(-0.5f, 0, -0.5f));
	mCurrent = mView * mCurrent;
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_LINES, 0, iGridSize);

	PRINTOGLERROR();

	/* DRAW TRIANGLE */
	vertexColShader->UseShaderProgram();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(vaoIds[1]);

	iModelViewLoc = glGetUniformLocation(vertexColShader->getProgramId(), "mModelView");
	iProjectionLoc = glGetUniformLocation(vertexColShader->getProgramId(), "mProjection");

	mCurrent = mView;
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glUniformMatrix4fv(iProjectionLoc, 1, GL_FALSE, glm::value_ptr(mProjection));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	/* UNBIND */
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* SWAP BUFFERS */
	PRINTOGLERROR();
	oc->SwapBuffersHDC();
}

void TerminateRendering()
{ 
	// delete vbo when program terminated
	glDeleteBuffers(2, vboIds);
	glDeleteBuffers(4, vaoIds);
}