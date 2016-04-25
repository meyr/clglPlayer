#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <CL/cl_gl.h>
#include "opengl.h"
#include "opencl.h"

GLuint pbo_source;
GLuint pbo_dest;
GLuint tex_screen;                          // (offscreen) render target
GLuint orign_texture;
GLuint modify_texture;
static int glutWindowHandle = 0;
static int img_width;
static int img_height;
static unsigned char *the_image;

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
	char strfps[16];

	nbFrames++;
	if (lastTime == 0) {
		lastTime = currentTime;
		return;
	}


	diffTime = currentTime - lastTime;
	//printf("%f %f %d\n",currentTime,diffTime,nbFrames);
	if (diffTime >= 1000.0) {
		fps = nbFrames * (1000.0 / diffTime);
		nbFrames = 0;
		lastTime = currentTime;
		sprintf(strfps, "fps : %2.2f\n", fps);
		glutSetWindowTitle(strfps);
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
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_dest);
	//glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_source);
	glBindTexture(GL_TEXTURE_2D, tex_screen);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
	                img_width, img_height, 
	                GL_RGB, GL_UNSIGNED_BYTE, NULL);

}

void appRender(void)
{
	runKernel();

	processImage();

    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//this updates the particle system by calling the kernel
	//runKernel();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        //glBindTexture(GL_TEXTURE_2D, orign_texture);
        glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);

        glEnd();
    	glutSwapBuffers();
    	glDisable(GL_TEXTURE_2D);
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	caculateFPS();
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

void appDestroy()
{
	//this makes sure we properly cleanup our OpenCL context
	if(glutWindowHandle)
    		glutDestroyWindow(glutWindowHandle);

    	exit(0);
}

void appKeyboard(unsigned char key, int x, int y)
{
	static bool fullScreen = false;
	//this way we can exit the program cleanly
	switch(key) {
		case 'f':
		case 'F':
			fullScreen ^= true;
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
		case '\033': 
		case '\015':
		case 'Q':   
		case 'q': appDestroy(); break;
		default : break;
	}
}


void init_gl(int argc, char** argv)
{
	int i;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH)/2, glutGet(GLUT_SCREEN_HEIGHT)/2);
	glutInitWindowPosition (glutGet(GLUT_SCREEN_WIDTH)/4, 
				glutGet(GLUT_SCREEN_HEIGHT)/4);

	glutWindowHandle = glutCreateWindow("auo CLGL image show");

	//glutFullScreen();

	glutDisplayFunc(appRender); //main rendering function
	//glutTimerFunc(30, timerCB, 30); //determin a minimum time between frames
	glutIdleFunc(idle);
	glutKeyboardFunc(appKeyboard);
	//glutMouseFunc(appMouse);
	//glutMotionFunc(appMotion);

}


void createPBO(GLuint* pbo)
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

void createTexture( GLuint* tex_name)
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
}

