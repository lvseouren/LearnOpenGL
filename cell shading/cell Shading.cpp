#define WIN32
#include<gltools.h>
#include<GL/glut.h>
#include<GLFrame.h>
#include<GLFrustum.h>
#include<GLMatrixStack.h>
#include<GLGeometryTransform.h>
#include<StopWatch.h>

GLTriangleBatch torusBatch;
GLuint texture;
GLuint toonShader;



GLFrustum viewFrustum;
GLFrame viewFrame;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;

GLShaderManager shaderManager;
GLint locLightPos;
GLint locMVP;
GLint locMV;
GLint locNM;
GLint locTex;
// Cleanup
void ShutdownRC(void)
{
	glDeleteTextures(1, &texture);
}
void ChangeSize(int w, int h)
{
// Prevent a divide by zero
if(h == 0)
	h = 1;

// Set Viewport to window dimensions
glViewport(0, 0, w, h);

viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	static CStopWatch rotTimer;
	glUseProgram(toonShader);

	GLfloat vEyeLight[] = {-100,100,100};

	modelViewMatrix.PushMatrix(viewFrame);
	modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds()*10.0f,0,1,0);

	glUniformMatrix4fv(locMVP,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMV,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNM,1,GL_FALSE,transformPipeline.GetNormalMatrix());
	glUniform3fv(locLightPos,1,vEyeLight);
	glUniform1i(locTex,0);
	torusBatch.Draw();

	modelViewMatrix.PopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
}
void SetupRC()
{
	//set bg
	glClearColor(0.025f,0.25f,0.25f,1.0f);

	glEnable(GL_DEPTH_TEST);

	shaderManager.InitializeStockShaders();
	viewFrame.MoveForward(4.0f);

	gltMakeTorus(torusBatch,0.8,0.3,50,50);

	
	//set up texture
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_1D,texture);
	GLubyte textureData[4][3] = {32,0,0,
								64,0,0,
								128,0,0,
								255,0,0};
	glTexImage1D(GL_TEXTURE_1D,0,GL_RGB,4,0,GL_RGB,GL_UNSIGNED_BYTE,textureData);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);

	//load shader
	toonShader = gltLoadShaderPairWithAttributes("toonShader.vp","toonShader.fp",2,GLT_ATTRIBUTE_VERTEX,"vVertex",GLT_ATTRIBUTE_NORMAL,"vNormal");
	//get uniform location
	locMVP = glGetUniformLocation(toonShader,"mvpMatrix");
	locMV = glGetUniformLocation(toonShader,"mvMatrix");
	locNM = glGetUniformLocation(toonShader,"normalMatrix");
	locLightPos = glGetUniformLocation(toonShader,"vLightPosition");
	locTex = glGetUniformLocation(toonShader,"colorMap");
}

///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
    {
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Cell (toon) shading");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
    }
	
	SetupRC();    
	glutMainLoop();
	ShutdownRC();
	return 0;
    }