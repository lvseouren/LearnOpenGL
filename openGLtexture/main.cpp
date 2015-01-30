#define WIN32
#include<GLTools.h>
#include<GL/glut.h>
#include<GLFrame.h>
#include<GLFrustum.h>
#include<GLMatrixStack.h>
#include<GLGeometryTransform.h>
#include<StopWatch.h>
#include<GLShaderManager.h>
//TODO:complete include
//TODO:complete opengl framework
//TODO:draw a cube
//TODO:add the texture to cube

///////////////////////////////////global var
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

GLBatch logoBatch;
GLBatch floorBatch;
GLTriangleBatch torusBatch;
GLTriangleBatch sphereBatch;

GLShaderManager shaderManager;
GLuint textureID[4];
//camera
GLFrame cameraFrame;
GLFrustum viewFrustum;
//matrix
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;

//locUniform
GLint locMVP; 
GLint locTexRect;

//shader
GLuint rectShader;

//function declare
bool LoadTGATexture(const char* szFileName,GLenum minFilter,GLenum magFilter,GLenum wrapMode);
bool LoadTGATextureRect(const char* szFileName,GLenum minFilter,GLenum magFilter,GLenum wrapMode);
void MakePyramid(GLBatch& pyramidBatch);
void ShutdownRC(void);
void DrawSongAndDance(GLfloat yRot);
//
void SetupRC()
{
	//Make sure OpenGl entry points are set
	glewInit();

	//black background
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	shaderManager.InitializeStockShaders();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//make torus and sphere
	gltMakeTorus(torusBatch,0.4f,0.15f,40,20);
	gltMakeSphere(sphereBatch,0.1f,26,13);

	//make the solid ground
	GLfloat texSize = 10.0f;
	floorBatch.Begin(GL_TRIANGLE_FAN,4,1);

	floorBatch.MultiTexCoord2f(0,0.0f,0.0f);
	floorBatch.Vertex3f(-20.0f,-0.41f,20.0f);

	floorBatch.MultiTexCoord2f(0,texSize,0.0f);
	floorBatch.Vertex3f(20.0f,-0.41f,20.0f);

	floorBatch.MultiTexCoord2f(0,texSize,texSize);
	floorBatch.Vertex3f(20.0f,-0.41f,-20.0f);

	floorBatch.MultiTexCoord2f(0,0.0f,texSize);
	floorBatch.Vertex3f(-20.0f,-0.41f,-20.0f);
	floorBatch.End();

	//make a logo rectangle
	int x = 500;
	int y = 155;
	int width = 300;
	int height = 155;
	logoBatch.Begin(GL_TRIANGLE_FAN,4,1);
	//------------------------------------------------注意顶点顺序，会影响正反面，如果开启了背面剔除，顶点顺序不对就会部分不可见了。
		//logoBatch.MultiTexCoord2f(0,0,height);
		//logoBatch.Vertex3f(x,y,0);
		//logoBatch.MultiTexCoord2f(0,0,0);
		//logoBatch.Vertex3f(x,y-height,0);
		//logoBatch.MultiTexCoord2f(0,width,height);
		//logoBatch.Vertex3f(x+width,y,0);
		//logoBatch.MultiTexCoord2f(0,width,0);
		//logoBatch.Vertex3f(x+width,y-height,0);
	 // Upper left hand corner
        logoBatch.MultiTexCoord2f(0, 0.0f, height);
        logoBatch.Vertex3f(x, y, 0.0);
        
        // Lower left hand corner
        logoBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        logoBatch.Vertex3f(x, y - height, 0.0f);

        // Lower right hand corner
        logoBatch.MultiTexCoord2f(0, width, 0.0f);
        logoBatch.Vertex3f(x + width, y - height, 0.0f);

        // Upper righ hand corner
        logoBatch.MultiTexCoord2f(0, width, height);
        logoBatch.Vertex3f(x + width, y, 0.0f);
	logoBatch.End();

	//make 3 texture objects
	glGenTextures(4,textureID);

	//load the marble texture
	glBindTexture(GL_TEXTURE_2D,textureID[0]);
	LoadTGATexture("Marble.tga",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_REPEAT);

	//load Mars
	glBindTexture(GL_TEXTURE_2D,textureID[1]);
	LoadTGATexture("marslike.tga",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE);

	//load Moon
	glBindTexture(GL_TEXTURE_2D,textureID[2]);
	LoadTGATexture("moonlike.tga",GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE);

	//load logo
	glBindTexture(GL_TEXTURE_RECTANGLE,textureID[3]);
	LoadTGATextureRect("OpenGL-Logo.tga",GL_NEAREST,GL_NEAREST,GL_CLAMP_TO_EDGE);

	//load shader
	rectShader = gltLoadShaderPairWithAttributes("shader.vp","shader.fp",2,GLT_ATTRIBUTE_VERTEX,"vVertex",GLT_ATTRIBUTE_TEXTURE0,"vTexCoord");
	//get location of uniform
	locMVP = glGetUniformLocation(rectShader,"mvpMatrix");
	locTexRect = glGetUniformLocation(rectShader,"colorMap");

	//randomly place the spheres
	for(int i = 0;i<NUM_SPHERES;++i)
	{
		GLfloat x = (GLfloat)((rand()%400-200)*0.1f);
		GLfloat z = (GLfloat)((rand()%400-200)*0.1f);
		spheres[i].SetOrigin(x,0.0f,z);
	}

}
void SpecialKeys(int key,int x,int y)
{
	float linear = 0.1f;
	float angular = float(m3dDegToRad(5.0f));

	if(key == GLUT_KEY_UP)
		cameraFrame.MoveForward(linear);
	if(key == GLUT_KEY_DOWN)
		cameraFrame.MoveForward(-linear);
	if(key == GLUT_KEY_LEFT)
		cameraFrame.RotateWorld(angular,0,1,0);
	if(key == GLUT_KEY_RIGHT)
		cameraFrame.RotateWorld(-angular,0,1,0);

}
void ChangeSize(int width,int height)
{
	glViewport(0, 0, width, height);
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);//??

	viewFrustum.SetPerspective(35.0f,float(width)/float(height),1.0f,500.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());//??
	modelViewMatrix.LoadIdentity();
	
}
void RenderScene()
{
	static CStopWatch rotTimer;
	float yRot = rotTimer.GetElapsedSeconds()*60.0f;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	//auxiliary
	//transform
	modelViewMatrix.PushMatrix();
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);
	modelViewMatrix.MultMatrix(mCamera);

	//draw the world upside down
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(1,-1,1);
	modelViewMatrix.Translate(0,0.8,0);
	glFrontFace(GL_CW);
	DrawSongAndDance(yRot);
	glFrontFace(GL_CCW);
	modelViewMatrix.PopMatrix();
	//draw the solid floor
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,textureID[0]);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	static GLfloat vFloorColor[] = {1,1,1,0.75};
	shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,
		transformPipeline.GetModelViewProjectionMatrix(),
		vFloorColor,0);
	floorBatch.Draw();
	glDisable(GL_BLEND);

	//draw the world 
	DrawSongAndDance(yRot);

	modelViewMatrix.PopMatrix();

	//draw the logo
	M3DMatrix44f mScreenSpace;
	m3dMakeOrthographicMatrix(mScreenSpace,0,800,0,600,-1.0,1.0f);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(rectShader);
	glUniformMatrix4fv(locMVP,1,GL_FALSE,mScreenSpace);
	glUniform1i(locTexRect,0);
	glBindTexture(GL_TEXTURE_RECTANGLE,textureID[3]);
	logoBatch.Draw();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glutSwapBuffers();
	glutPostRedisplay();
}
int main(int argc,char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	//glut init 
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(800,600);

	//create window
	glutCreateWindow("Cube And Texture");

	//add function
	glutSpecialFunc(SpecialKeys);
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);

	//GLenum err = glewInit();
	//if(GLEW_OK != err)
	//{
	//	fprintf_s(stderr,"GLEW Error:%s\n",glewGetErrorString(err));
	//	return 1;
	//}

	SetupRC();
	glutMainLoop();

	ShutdownRC();
	return 0;
}

//load a TGA as a 2D Texture.Completely initialize the state
bool LoadTGATexture(const char* szFileName,GLenum minFilter,GLenum magFilter,GLenum wrapMode)
{
	//read texture bits
	GLbyte *texBuffer;
	int tWidth;
	int tHeight;
	int tComp;
	GLenum tFormat;

	texBuffer = gltReadTGABits(szFileName,&tWidth,&tHeight,&tComp,&tFormat);
	if(texBuffer == NULL)
		return false;

	//set texture parameter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrapMode);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilter);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage2D(GL_TEXTURE_2D,0,GL_COMPRESSED_RGB,tWidth,tHeight,0,tFormat,GL_UNSIGNED_BYTE,texBuffer);

	free(texBuffer);

	if(minFilter == GL_LINEAR_MIPMAP_LINEAR||
		minFilter == GL_LINEAR_MIPMAP_NEAREST||
		minFilter == GL_NEAREST_MIPMAP_LINEAR||
		minFilter == GL_NEAREST_MIPMAP_NEAREST)
		glGenerateMipmap(GL_TEXTURE_2D);
	
	return true;
}

void MakePyramid(GLBatch& pyramidBatch)
{
	pyramidBatch.Begin(GL_TRIANGLES,18,1);

	//bottom of pyramid
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3f(1.0f, -1.0f, -1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	pyramidBatch.Vertex3f(-1.0f, -1.0f, 1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
	
	M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
	M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
	M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
	M3DVector3f vBackLeft = { -1.0f, -1.0f, -1.0f };
	M3DVector3f vBackRight = { 1.0f, -1.0f, -1.0f };
	M3DVector3f n;
	
	// Front of Pyramid
	m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);		// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontLeft);		// Front left corner
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontRight);		// Front right corner
    
    
	m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);		// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackLeft);		// Back left corner
	
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontLeft);		// Front left corner
    
	m3dFindNormal(n, vApex, vFrontRight, vBackRight);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);				// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontRight);		// Front right corner
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackRight);			// Back right cornder
    
    
	m3dFindNormal(n, vApex, vBackRight, vBackLeft);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);		// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackRight);		// Back right cornder

	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackLeft);		// Back left corner

	pyramidBatch.End();

}
///////////////////////////////////////////////////////////////////////////////
// Cleanup... such as deleting texture objects
void ShutdownRC(void)
{
	glDeleteTextures(4, textureID);
}

void DrawSongAndDance(GLfloat yRot)
{
	//some value
	static GLfloat vWhite[] = {1,1,1,1};
	static GLfloat vLightPos[] = {0,3,0,1};

	//draw the static object
	//set matrix
	M3DVector4f vLightTransformed;
	M3DMatrix44f mCamera;
	modelViewMatrix.GetMatrix(mCamera);
	m3dTransformVector4(vLightTransformed,vLightPos,mCamera);

	//draw the light source
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translatev(vLightPos);
	shaderManager.UseStockShader(GLT_SHADER_FLAT,
		transformPipeline.GetModelViewProjectionMatrix(),
		vWhite);
	sphereBatch.Draw();
	modelViewMatrix.PopMatrix();

	//set texture
	glBindTexture(GL_TEXTURE_2D,textureID[2]);
	for(int i = 0;i<NUM_SPHERES;++i)
	{
		modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(spheres[i]);
		shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
			modelViewMatrix.GetMatrix(),
			transformPipeline.GetProjectionMatrix(),
			vLightTransformed,
			vWhite,0);
		sphereBatch.Draw();
		modelViewMatrix.PopMatrix();
	}
	
	//draw the moving object
	modelViewMatrix.Translate(0,0.2f,-0.25f);
	modelViewMatrix.PushMatrix();	// Saves the translated origin
	modelViewMatrix.Rotate(yRot,0,1,0);

	glBindTexture(GL_TEXTURE_2D,textureID[1]);
	shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
		modelViewMatrix.GetMatrix(),
		transformPipeline.GetProjectionMatrix(),
		vLightTransformed,
		vWhite,
		0);
	torusBatch.Draw();
	modelViewMatrix.PopMatrix();

	modelViewMatrix.Rotate(yRot*-2.0f,0,1,0);
	modelViewMatrix.Translate(0.8,0,0);

	glBindTexture(GL_TEXTURE_2D,textureID[2]);
	shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
		modelViewMatrix.GetMatrix(),
		transformPipeline.GetProjectionMatrix(),
		vLightTransformed,
		vWhite,
		0);
	sphereBatch.Draw();
	//set matrix
	//set texture
}
bool LoadTGATextureRect(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
	{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, magFilter);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, nComponents, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);

	return true;
	}