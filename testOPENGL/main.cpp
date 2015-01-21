#define WIN32

#include<GLTools.h>
#include<gl/glut.h>
#include<GLShaderManager.h>
#include<GLFrame.h>
#include<GLMatrixStack.h>
#include<GLGeometryTransform.h>
#include<StopWatch.h>
#include<GLFrustum.h>
//start class

struct color
{
	color(float r,float g,float b,float a):r(r),g(g),b(b),a(a){}
	float r;
	float g;
	float b;
	float a;
};
//end class

//start function declare
void SpecialKeys(int key,int x,int y);
//end function declare

//start global var,ingle-
GLFrame cameraFrame;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLFrustum viewFrustum;

GLfloat xPos;
GLfloat yPos;

color pumpkinOrange(0.98,0.625,0.12,1.0);
color barneyPurple(0.60,0.40,0.70,1);
color githubGreen(0.51,0.82,0.7,1);

GLBatch triangleBatch;
GLBatch githubBatch;
GLBatch floorBatch;

GLTriangleBatch myCoolObject;

GLShaderManager shaderManager;

GLfloat vVerts[] = {-0.4f,0.0f,0.0f,
		0.4f,0.0f,0.0f,
		0.0f,0.4f,0.0f};

GLfloat ghVerts[36][3] = {-0.1,0,0,
		0,-0,0,
		-0.1,-0.1,0,
		0,-0,0,
		-0.1,-0.1,0,
		0,-0.1,0,
		-0.1,0.3,0,//
		0,0.3,0,
		-0.1,0.2,0,
		0,0.3,0,
		-0.1,0.2,0,
		0,0.2,0,
		0,0,0,//right from here
		0.2,0,0,
		0.2,0.1,0,
		0,0,0,
		0,0.1,0,
		0.2,0.1,0,
		0.1,0.3,0,
		0.1,0.1,0,
		0.2,0.3,0,
		0.1,0.1,0,
		0.2,0.3,0,
		0.2,0.1,0,
		-0.1,0,0,//left from here
		-0.3,0,0,
		-0.3,0.1,0,
		-0.1,0,0,
		-0.1,0.1,0,
		-0.3,0.1,0,
		-0.2,0.3,0,
		-0.2,0.1,0,
		-0.3,0.3,0,
		-0.2,0.1,0,
		-0.3,0.3,0,
		-0.3,0.1,0
};
//end global var,-ingle
/////////////////////////////////////////
//绘制场景
void RenderScene(void)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
	static GLfloat vFloorColor[] = {0.0f,1.0f,0.0f,1.0f};
	static GLfloat vTorusColor[] = {1.0f,0.0f,0.0f,1.0f};

	//Time based animation
	static CStopWatch rotTimer;
	float yRot = rotTimer.GetElapsedSeconds()*60.0f;

	//save the current modelview matrix(the identity matrix)
	modelViewMatrix.PushMatrix();

	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);
	modelViewMatrix.PushMatrix(mCamera);

	//add light 
	M3DVector4f vLightPos = {0,10,5,1};
	M3DVector4f vLightEyePos;
	m3dTransformVector4(vLightEyePos,vLightPos,mCamera);

	//Draw the ground
	shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vFloorColor);
	floorBatch.Draw();

	//Draw the Torus
	modelViewMatrix.Translate(0,0,-2.5f);
	modelViewMatrix.PushMatrix();

	modelViewMatrix.Rotate(yRot,0.0f,1.0f,0.0f);
	shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightEyePos,vTorusColor);
	myCoolObject.Draw();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();

	////add transform
	//M3DMatrix44f mFinalTransform;
	//M3DMatrix44f mTranslationMatrix;
	//M3DMatrix44f mRotationMatrix;

	////just translate
	//m3dTranslationMatrix44(mTranslationMatrix,xPos,yPos,0);

	////rotate 5 degree every time we redraw
	//static float yRot = 0.0f;
	//yRot += 0.1f;
	//if(yRot > 360)
	//{
	//	yRot -=360;
	//}
	//m3dRotationMatrix44(mRotationMatrix,m3dDegToRad(yRot),0,1,0);

	//m3dMatrixMultiply44(mFinalTransform,mTranslationMatrix,mRotationMatrix);

	///*shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);*/
	//shaderManager.UseStockShader(GLT_SHADER_FLAT,mFinalTransform,vRed);
	//triangleBatch.Draw();

	//GLfloat vGithubGreen[] = {githubGreen.r,githubGreen.g,githubGreen.b,0.5};
	//shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vGithubGreen);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//githubBatch.Draw();
	//glDisable(GL_BLEND);

	////TODO:draw the WireFrame
	//glPolygonOffset(-1.0f,-1.0f);
	//glEnable(GL_POLYGON_OFFSET_LINE);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//glLineWidth(0.5f);
	//GLfloat vblack[] = {0,0,0,1};
	//shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vblack);
	//githubBatch.Draw();
	//glDisable(GL_LINE_SMOOTH);
	//glDisable(GL_POLYGON_OFFSET_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDisable(GL_BLEND);
	////TODO:draw the torus
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//shaderManager.UseStockShader(GLT_SHADER_FLAT,mRotationMatrix,vblack);
	//myCoolObject.Draw();
	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	////perform the buffer swap to display the back buffer
	glutSwapBuffers();

	glutPostRedisplay();
	////刷新绘图命令
	//glFlush();
}

//////////////////////////////////////////
//设置渲染状态
void SetupRC(color bg,float alpha)
{
	//glClearColor(bg.r,bg.g,bg.b,alpha);

	shaderManager.InitializeStockShaders();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	//load up a triangle


	triangleBatch.Begin(GL_TRIANGLES,3);
	triangleBatch.CopyVertexData3f(vVerts);
	triangleBatch.End();

	githubBatch.Begin(GL_TRIANGLES,36);//TODO:change 3 to the right value
	githubBatch.CopyVertexData3f(ghVerts);//TODO;fill ghVerts
	githubBatch.End();


	gltMakeTorus(myCoolObject,0.5,0.3,50,40);

	//floorBatch start
	floorBatch.Begin(GL_LINES,324);
	for(GLfloat x = -20.0;x<=20.0f;x+=0.5)
	{
		floorBatch.Vertex3f(x,-0.55f,20.0f);
		floorBatch.Vertex3f(x,-0.55f,-20.0f);

		floorBatch.Vertex3f(20.0f,-0.55f,x);
		floorBatch.Vertex3f(-20.0f,-0.55f,x);
	}
	floorBatch.End();
	//floorBatch end

}

/////////////////////////////////////////
//当窗口改变大小时由GLUT函数库调用
void ChangeSize(GLsizei w,GLsizei h)
{
	glViewport(0,0,w,h);

	//create the projection matrix,and load it on the projection matrix stack
	viewFrustum.SetPerspective(35.0,float(w)/float(h),1.0f,100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	//set the transformation pipeline to use the two matrix stacks
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}
///////////////////////////////////////////
//主程序入口
int main(int argc,char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("Triangle!");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);

	GLenum err = glewInit();
	if(GLEW_OK!=err)
	{
		fprintf(stderr,"GLEW Error:%s\n",glewGetErrorString(err));
		return 1;
	}

	SetupRC(barneyPurple,1);

	glutMainLoop();
	return 0;
}

void SpecialKeys(int key,int x,int y)
{
	float linear = 0.1f;
	float angular = float(m3dDegToRad(5.0f));

	if(key == GLUT_KEY_UP)
	{
		cameraFrame.MoveForward(linear);
	}
	if(key == GLUT_KEY_DOWN)
	{
		cameraFrame.MoveForward(-linear);
	}
	if(key == GLUT_KEY_LEFT)
	{
		cameraFrame.RotateWorld(angular,0,1,0);
	}
	if(key == GLUT_KEY_RIGHT)
	{
		cameraFrame.RotateWorld(-angular,0,1,0);
	}
}
