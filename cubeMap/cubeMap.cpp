#define WIN32
#include <GLTools.h>	// OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <Stopwatch.h>
#include <GL\glut.h>

GLFrame viewFrame;
GLFrustum viewFrustum;
GLTriangleBatch sphereBatch;
GLBatch cubeBatch;

GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;

GLint cubeShader;
GLint reflecShader;
GLuint cubeTexture;

GLint locMVPReflect,locMVReflect,locNormalReflect,locInvertedCamera;
GLint locMVPSkyBox;
GLint locTexReflect;
GLint locTexCube;

//six sides of a cube map
const char *szCubeFaces[6] = {"pos_x.tga","neg_x.tga","pos_y.tga","neg_y.tga","pos_z.tga","neg_z.tga"};

GLenum cube[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};


void ChangeSize(int w,int h)
{
	if(h == 0)
		h = 1;
	glViewport(0,0,w,h);
	viewFrustum.SetPerspective(35.0f,float(w)/float(h),1,1000);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
}
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	M3DMatrix44f mCamera;
	M3DMatrix44f mCameraRotOnly;
	M3DMatrix44f mInvertedCamera;

	viewFrame.GetCameraMatrix(mCamera);
	viewFrame.GetCameraMatrix(mCameraRotOnly,true);
	m3dInvertMatrix44(mInvertedCamera,mCameraRotOnly);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.MultMatrix(mCamera);

	glUseProgram(reflecShader);
	glUniformMatrix4fv(locMVPReflect,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMVReflect,1,GL_FALSE,transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNormalReflect,1,GL_FALSE,transformPipeline.GetNormalMatrix());
	glUniformMatrix4fv(locInvertedCamera,1,GL_FALSE,mInvertedCamera);
	glBindTexture(GL_TEXTURE_CUBE_MAP,cubeTexture);
	/*glUniform1i(locTexReflect,1);*/
	glEnable(GL_CULL_FACE);
	sphereBatch.Draw();
	glDisable(GL_CULL_FACE);
	modelViewMatrix.PopMatrix();

	modelViewMatrix.PushMatrix();
	modelViewMatrix.MultMatrix(mCameraRotOnly);
	glUseProgram(cubeShader);
	glUniformMatrix4fv(locMVPSkyBox,1,GL_FALSE,transformPipeline.GetModelViewProjectionMatrix());
	/*glUniform1i(locTexCube,0);*/
	cubeBatch.Draw();
	modelViewMatrix.PopMatrix();

	glutSwapBuffers();

}
void SpecialKeys(int key,int x,int y)
{
    if(key == GLUT_KEY_UP)
        viewFrame.MoveForward(0.1f);

    if(key == GLUT_KEY_DOWN)
        viewFrame.MoveForward(-0.1f);

    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateLocalY(0.1);
      
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateLocalY(-0.1);
                        
    // Refresh the Window
    glutPostRedisplay();
}
void ShutdownRC()
{
	glDeleteTextures(1,&cubeTexture);
}
void SetupRC()
{
	// Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
	//set camera
	viewFrame.MoveForward(-4.0f);

	gltMakeSphere(sphereBatch,0.8,30,40);
	gltMakeCube(cubeBatch,20);

	//shader
	cubeShader = gltLoadShaderPairWithAttributes("cubeMapShader.vp","cubeMapShader.fp",2,GLT_ATTRIBUTE_VERTEX,"vVertex",GLT_ATTRIBUTE_NORMAL,"vNormal");
	reflecShader = gltLoadShaderPairWithAttributes("reflection.vp","reflection.fp",2,GLT_ATTRIBUTE_VERTEX,"vVertex",GLT_ATTRIBUTE_NORMAL,"vNormal");

	//uniform location
	locMVPReflect = glGetUniformLocation(reflecShader,"mvpMatrix");
	locMVReflect = glGetUniformLocation(reflecShader,"mvMatrix");
	locNormalReflect = glGetUniformLocation(reflecShader,"normalMatrix");
	locInvertedCamera = glGetUniformLocation(reflecShader,"invertedCamera");
	locMVPSkyBox = glGetUniformLocation(cubeShader,"mvpMatrix");
	locTexReflect = glGetUniformLocation(reflecShader,"colorMap");
	locTexCube = glGetUniformLocation(cubeShader,"colorMap");


	//build up texture
	glGenTextures(1,&cubeTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP,cubeTexture);

	// Set up texture maps        
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);       
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLbyte* pBytes;
	GLint iComponents;
	GLenum eFormat;
	GLint width,height;
	//load cube map images
	for(int i = 0;i<6;++i)
	{
		pBytes = gltReadTGABits(szCubeFaces[i],&width,&height,&iComponents,&eFormat);
		glTexImage2D(cube[i],0,iComponents,width,height,0,eFormat,GL_UNSIGNED_BYTE,pBytes);
		free(pBytes);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

}
int main(int argc,char* argv[])
{
glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutCreateWindow("OpenGL Cube Maps");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    
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