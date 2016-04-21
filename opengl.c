#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>
#include "opengl.h"
#include "opencl.h"

static GLuint Mytexture;
static int glutWindowHandle = 0;
static int img_width;
static int img_height;
static unsigned char *the_image;
static bool showInfo;

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
		return;
	}


	diffTime = currentTime - lastTime;
	//printf("%f %f %d\n",currentTime,diffTime,nbFrames);
	if (diffTime >= 1000.0) {
		fps = nbFrames * (1000.0 / diffTime);
		nbFrames = 0;
		lastTime = currentTime;
		printf("%f\n", fps);
	}

	return fps;
}

void appRender(void)
{
	char strfps[16];
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//this updates the particle system by calling the kernel
	//runKernel();

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glBindTexture(GL_TEXTURE_2D, Mytexture);
        glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);

        glEnd();
    	glutSwapBuffers();
    	glDisable(GL_TEXTURE_2D);

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

void switchShowInfo(void)
{
	showInfo ^= true;	
}

void appKeyboard(unsigned char key, int x, int y)
{
    //this way we can exit the program cleanly
    switch(key)
    {
    	case 'f':
	case 'F':
		glutFullScreen();
		break;
    	case 'i':
	case 'I':
		switchShowInfo();
		break;
        case '\033': // escape quits
        case '\015': // Enter quits    
        case 'Q':    // Q quits
        case 'q':    // q (or escape) quits
            	appDestroy();
            	break;
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

	/* create texture buffer */
    	glGenTextures( 1, &Mytexture);    // Load 2 texture names into array, this function will assign value into array MytextureName
        glBindTexture(GL_TEXTURE_2D, Mytexture);    // Texture buffer #i is active now
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_FLAT);
        glEnable(GL_DEPTH_TEST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, the_image);
	glGenerateMipmap(GL_TEXTURE_2D);

	//glutFullScreen();

	glutDisplayFunc(appRender); //main rendering function
	//glutTimerFunc(30, timerCB, 30); //determin a minimum time between frames
	glutIdleFunc(idle);
	glutKeyboardFunc(appKeyboard);
	//glutMouseFunc(appMouse);
	//glutMotionFunc(appMotion);

}

