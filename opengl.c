#include <stdio.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <CL/cl_gl.h>
#include "opengl.h"
#include "opencl.h"
#include "shader.h"
#include "utility.h"
#ifdef _WIN32
#include <GL/glext.h>
#endif

GLuint pbo_source;
GLuint pbo_dest;
GLuint tex_screen;                          // (offscreen) render target
GLuint programID;
GLuint TextureID;
GLuint vertexbuffer, uvbuffer;
static int glutWindowHandle = 0;
static int img_width;
static int img_height;
static unsigned char *the_image;
static const char titleName[] = "auo clgl player";
static char selectSource;

const GLfloat g_vertex_data[] = { 
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
};

const GLfloat g_uv_data[] = { 
	0.00f, 0.00f, 
	1.00f, 0.00f, 
	0.00f, 1.00f, 
	1.00f, 0.00f, 
	1.00f, 1.00f, 
	0.00f, 1.00f, 
};

void setImageAttr(int width, int height, unsigned char *image)
{
	img_width = width;
	img_height = height;
	the_image = image;
}

float caculateFPS(void)
{
	static int nbFrames;
	static double lastTime;
	double currentTime = glutGet(GLUT_ELAPSED_TIME);
	double diffTime;
	static float fps;


	nbFrames++;
	if (lastTime == 0) {
		lastTime = currentTime;
		return 0.0;
	}


	diffTime = currentTime - lastTime;
	//printf("%f %f %d\n",currentTime,diffTime,nbFrames);
	if (diffTime >= 1000.0) {
		fps = nbFrames * (1000.0 / diffTime);
		nbFrames = 0;
		lastTime = currentTime;
	}

	return fps;
}

void pushImage(void)
{
	// activate destination buffer
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo_source);
	glBufferSubData(GL_PIXEL_PACK_BUFFER_ARB, 0, img_width * img_height * 3, the_image);

}

void processImage(void)
{
	// download texture from PBO
	if (selectSource)
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_source);
	else
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_dest);
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_source);
	glBindTexture(GL_TEXTURE_2D, tex_screen);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
	                img_width, img_height, 
	                GL_RGB, GL_UNSIGNED_BYTE, NULL);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	//glGenerateMipmap(GL_TEXTURE_2D);

	/* bind our texture in texture unit 0 */
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex_screen);
	/* set our texture sampler to user texture unit 0 */
	glUniform1i(TextureID, 0);


}

void showTitle(float fps)
{
	char strfps[48];

	sprintf(strfps, "%s | %s | %2.2f fps\n", titleName,
			selectSource == 0 ? "source" : "result", fps);
	glutSetWindowTitle(strfps);

}

void appRender(void)
{
	double pTime0, pTime1, pTime2;
	//shrDeltaT(0);
	runKernel();
	//pTime0 = shrDeltaT(0);
	//shrDeltaT(1);
	processImage();
	//pTime1 = shrDeltaT(1);
	//pTime2 = shrDeltaT(2);
	//printf("runkernel : %.4f, processImage : %.6f, appRender : %.4f\n", pTime0, pTime1, pTime2);
    	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

	glutSwapBuffers();
	glFlush();
	//glFinish();
	showTitle(caculateFPS());
}

void timerCB(int ms)
{
    //this makes sure the appRender function is called every ms miliseconds
    glutTimerFunc(ms, timerCB, ms);
    glutPostRedisplay();
}

void idle(void)
{
	glutPostRedisplay();
}

void exit_gl(void)
{
	glUseProgram(0);
	glDeleteProgram(programID);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &pbo_source);
	glDeleteBuffers(1, &pbo_dest);
	glDeleteTextures(1, &tex_screen);

}

void appKeyboard(unsigned char key, int x, int y)
{
	static char fullScreen = 0;
	//this way we can exit the program cleanly
	switch(key) {
		case 'f':
		case 'F':
			fullScreen = (fullScreen == 0 ? 1 : 0);
			if (fullScreen)
				glutFullScreen();
			else {
				glutReshapeWindow(glutGet(GLUT_SCREEN_WIDTH)/2,
						  glutGet(GLUT_SCREEN_HEIGHT)/2);
				glutPositionWindow(glutGet(GLUT_SCREEN_WIDTH)/4,
						   glutGet(GLUT_SCREEN_HEIGHT)/4);
			}
			glutPostRedisplay();
		 	break;
		case 's':
		case 'S':
			selectSource = (selectSource == 0 ? 1 : 0);	
			break;
		case 'Q':   
		case 'q': 
			glutLeaveMainLoop();
			break;
		default : break;
	}
}

void appReshape(GLsizei w, GLsizei h)
{
	//GLsizei vsize;
	//
	//vsize = (w < h) ? w : h;
        glViewport(0,0,w,h);
}

void init_gl(int argc, char** argv)
{
	int i;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH)/2, glutGet(GLUT_SCREEN_HEIGHT)/2);
	glutInitWindowPosition (glutGet(GLUT_SCREEN_WIDTH)/4, 
				glutGet(GLUT_SCREEN_HEIGHT)/4);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutWindowHandle = glutCreateWindow(titleName);

	//glutFullScreen();

	glutDisplayFunc(appRender); //main rendering function
	//glutTimerFunc(33, timerCB, 33); //determin a minimum time between frames
	glutIdleFunc(idle);
	glutKeyboardFunc(appKeyboard);
	glutReshapeFunc(appReshape);
	//glutMouseFunc(appMouse);
	//glutMotionFunc(appMotion);

	/* load shader */
	/* Create and compile our GLSL program from the shaders */
	programID = LoadShaders("opengl.vert", "opengl.frag");

	/* get a handle for our texture uniform */
	TextureID = glGetUniformLocation(programID, "myTextureSampler");

	/* transfer vertex/uv data to opengl */
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_data), g_vertex_data, GL_STATIC_DRAW);


	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_data), g_uv_data, GL_STATIC_DRAW);


	/* uset our shader */
	glUseProgram(programID);

	/* 1rst attribute buffer : vertices */
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

}


void createGLBuffers(GLuint* pbo)
{
	unsigned int size_tex_data;
	unsigned int num_texels;
	unsigned int num_values;

	// set up data parameter
	num_texels = img_width * img_height;
	num_values = num_texels * 3;
	size_tex_data = sizeof(GLubyte) * num_values;
	
	// create buffer object
	glGenBuffers(1, pbo);
	glBindBuffer(GL_ARRAY_BUFFER, *pbo);
	
	// buffer data
	glBufferData(GL_ARRAY_BUFFER, size_tex_data, NULL, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void createGLRenderBuffers(GLuint *_index)
{
	// create buffer object
	glGenRenderbuffers(1, _index);
	glBindRenderbuffer(GL_RENDERBUFFER, *_index);
	
	// buffer data
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, img_width, img_height);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void createGLTexture( GLuint* tex_name)
{
	// create a texture
	glGenTextures(1, tex_name);
	glBindTexture(GL_TEXTURE_2D, *tex_name);

	// set basic parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// buffer data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}


