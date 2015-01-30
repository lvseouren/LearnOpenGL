#define WIN32
#include<GLTools.h>
#include<GL/glut.h>
#include<GLShaderManager.h>
#include<GLFrame.h>
#include<GLMatrixStack.h>
#include<GLFrustum.h>
#include<GLGeometryTransform.h>
#include<StopWatch.h>


GLTriangleBatch sphereBatch;

GLFrame viewFrame;
GLFrustum viewFrustum;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager shaderManager;
GLint myIdentityShader;

GLuint textureID;
static bool flag = false;

void SpecialKey(int key,int x,int y)
{
	if(key == GLUT_KEY_UP)
	{
		flag = !flag;
	}
	if(key == GLUT_KEY_DOWN)
		flag = !flag;
	glutPostRedisplay();
}
void ChangeSize(int w,int h)
{
	if(h == 0)
		h = 1;
	glViewport(0,0,w,h);

	viewFrustum.SetPerspective(35.0,float(w)/float(h),1.0f,100.0f);

	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}
// Load a TGA as a 2D Texture. Completely initialize the state
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
	return true;
}
void SetupRC()
{
	glClearColor(0.3,0.3,0.3,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	shaderManager.InitializeStockShaders();
	viewFrame.MoveForward(4.0f);

	gltMakeSphere(sphereBatch,1.0,26,13);

	/*myIdentityShader = gltLoadShaderPairWithAttributes("ShadedIdentity.vp","ShadedIdentity.fp",2,GLT_ATTRIBUTE_VERTEX,"vVertex",GLT_ATTRIBUTE_COLOR,"vColor");*/
	myIdentityShader = shaderManager.LoadShaderPairWithAttributes("ShadedIdentity.vp","ShadedIdentity.fp",3,GLT_ATTRIBUTE_VERTEX,"vVertex",GLT_ATTRIBUTE_NORMAL,"vNormal",GLT_ATTRIBUTE_TEXTURE0,"vTexCoords");
	
	//load texture;
	glGenTextures(1,&textureID);
	glBindTexture(GL_TEXTURE_2D,textureID);
	LoadTGATexture("CoolTexture.tga",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE);
	

}

void ShutdownRC()
{
	glDeleteProgram(myIdentityShader);
	glDeleteTextures(1,&textureID);
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	static CStopWatch rotTimer;

	modelViewMatrix.PushMatrix(viewFrame);
	modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds()*10.0f,0,1,0);

	GLfloat vEyeLight[] = {-100.0f,100.0f,100.0f};
	GLfloat vDiffuseColor[] = {1,1,1,0};
	GLfloat vSpecularColor[] = {1,1,1,1};
	GLfloat vAmbientColor[] = {0.1,0.1,0.1,1};

	glUseProgram(myIdentityShader);
	glBindTexture(GL_TEXTURE_2D,textureID);
	//set the uniform
	GLint locMVPMat;
	GLint locMVmat;
	GLint locNormalMat;
	GLint locLightPos;
	GLint locDiffuseColor;
	GLint locColor;
	GLint locFlag;
	GLint locSpecularColor;
	GLint locAmbientColor;
	GLint locColorMap;

	locMVPMat = glGetUniformLocation(myIdentityShader,"mvpMatrix");
	locMVmat = glGetUniformLocation(myIdentityShader,"mvMatrix");
	locNormalMat = glGetUniformLocation(myIdentityShader,"normalMatrix");
	locLightPos = glGetUniformLocation(myIdentityShader,"vLightPosition");
	locDiffuseColor = glGetUniformLocation(myIdentityShader,"diffuseColor");
	locSpecularColor = glGetUniformLocation(myIdentityShader,"specularColor");
	locAmbientColor = glGetUniformLocation(myIdentityShader,"ambientColor");
	locColor = glGetUniformLocation(myIdentityShader,"myColor");
	locFlag = glGetUniformLocation(myIdentityShader,"myFlag");
	locColorMap = glGetUniformLocation(myIdentityShader,"colorMap");
	GLfloat vRed[] = {1,0,0,1};
	
	glUniform4fv(locColor,1,vRed);
	glUniform1i(locFlag,flag);

	//set the matrix uniform --TODO:get the matrix 
	//TODO:get lightPos
	glUniformMatrix4fv(locMVPMat,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMVmat,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNormalMat,1,GL_FALSE,transformPipeline.GetNormalMatrix());
	glUniform4fv(locDiffuseColor,1,vDiffuseColor);
	glUniform4fv(locAmbientColor,1,vAmbientColor);
	glUniform4fv(locSpecularColor,1,vSpecularColor);
	glUniform3fv(locLightPos,1,vEyeLight);
	glUniform1i(locColorMap,0);

	sphereBatch.Draw();

	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc,char* argv[])
{
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Shaded Triangle");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKey);

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