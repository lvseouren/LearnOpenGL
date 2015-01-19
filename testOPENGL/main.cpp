#define WIN32

#include<GLTools.h>
#include<gl/glut.h>
#include<GLShaderManager.h>

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

color pumpkinOrange(0.98,0.625,0.12,1.0);
color barneyPurple(0.60,0.40,0.70,1);
color githubGreen(0.51,0.82,0.7,1);

GLBatch triangleBatch;
GLBatch githubBatch;
GLShaderManager shaderManager;

GLfloat vVerts[] = {-0.5f,0.0f,0.0f,
		0.5f,0.0f,0.0f,
		0.0f,0.5f,0.0f};

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
	//用当前清除颜色清除窗口
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
	shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
	triangleBatch.Draw();

	GLfloat vGithubGreen[] = {githubGreen.r,githubGreen.g,githubGreen.b,0.5};
	shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vGithubGreen);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	githubBatch.Draw();
	glDisable(GL_BLEND);

	//TODO:draw the WireFrame
	glPolygonOffset(-1.0f,-1.0f);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(0.5f);
	GLfloat vblack[] = {0,0,0,1};
	shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vblack);
	githubBatch.Draw();
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_BLEND);
	//perform the buffer swap to display the back buffer
	glutSwapBuffers();

	////刷新绘图命令
	//glFlush();
}

//////////////////////////////////////////
//设置渲染状态
void SetupRC(color bg,float alpha)
{
	glClearColor(bg.r,bg.g,bg.b,alpha);

	shaderManager.InitializeStockShaders();
	//load up a triangle


	triangleBatch.Begin(GL_TRIANGLES,3);
	triangleBatch.CopyVertexData3f(vVerts);
	triangleBatch.End();

	githubBatch.Begin(GL_TRIANGLES,36);//TODO:change 3 to the right value
	githubBatch.CopyVertexData3f(ghVerts);//TODO;fill ghVerts
	githubBatch.End();
}

/////////////////////////////////////////
//当窗口改变大小时由GLUT函数库调用
void ChangeSize(GLsizei w,GLsizei h)
{
	GLfloat aspectRatio;

	//防止被0所除
	if(h==0)
		h = 1;

	int viewportSize = w>h?h:w;
	glViewport(0,0,viewportSize,viewportSize);

	glMatrixMode(GL_PROJECTION);//specify which matrix is the current matrix
	glLoadIdentity();

	aspectRatio = (GLfloat)w/(GLfloat)h;
	if(w<=h)
		glOrtho(-100.0,100.0,-100.0/aspectRatio,100.0/aspectRatio,1.0,-1.0);
	else
		glOrtho(-100.0*aspectRatio,100.0*aspectRatio,-100.0,100.0,1.0,-1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
///////////////////////////////////////////
//主程序入口
int main(int argc,char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
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

	//test point size range
	GLfloat sizes[2];
	GLfloat step;

	glGetFloatv(GL_POINT_SIZE_RANGE,sizes);
	glGetFloatv(GL_POINT_SIZE_GRANULARITY,&step);

	glutMainLoop();
	return 0;
}

void SpecialKeys(int key,int x,int y)
{
	GLfloat stepSize = 0.025f;

	GLfloat blockX = vVerts[0];
	GLfloat blockY = vVerts[7];

	//TODO:find out blockSize's value
	GLfloat blockSize = 0.2f;

	if(key == GLUT_KEY_UP)
	{
		blockY += stepSize;
	}
	if(key == GLUT_KEY_DOWN)
	{
		blockY -= stepSize;
	}
	if(key == GLUT_KEY_LEFT)
	{
		blockX -= stepSize;
	}
	if(key == GLUT_KEY_RIGHT)
	{
		blockX += stepSize;
	}

	//Collision detection
	if(blockX<-1.0f)
		blockX = -1.0f;
	if(blockX>(1.0f-blockSize*2))
		blockX = 1.0f-blockSize*2;
	if(blockY<-1.0f+blockSize*2)
		blockY = -1.0f+blockSize*2;
	if(blockY>1.0f)
		blockY = 1.0f;

	//Recalculate vertex positions
	vVerts[0] = blockX;
	vVerts[1] = blockY - blockSize*2;

	vVerts[3] = blockX + blockSize*2;
	vVerts[4] = blockY - blockSize*2;

	vVerts[6] = blockX + blockSize*2;
	vVerts[7] = blockY;

	vVerts[9] = blockX;
	vVerts[10] = blockY;

	triangleBatch.CopyVertexData3f(vVerts);
	/*squareBatch.CopyVertexData3f(vVerts);*/

	glutPostRedisplay();

}
