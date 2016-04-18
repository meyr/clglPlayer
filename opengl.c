#include <stdio.h>
#include <GL/glut.h>

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
	}

	return fps;
}


void appRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //this updates the particle system by calling the kernel
    //example->runKernel();

    //render the particles from VBOs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(5.);
    
    //printf("color buffer\n");
    //glBindBuffer(GL_ARRAY_BUFFER, example->c_vbo);
    glColorPointer(4, GL_FLOAT, 0, 0);

    //printf("vertex buffer\n");
    //glBindBuffer(GL_ARRAY_BUFFER, example->p_vbo);
    glVertexPointer(4, GL_FLOAT, 0, 0);

    //printf("enable client state\n");
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    //Need to disable these for blender
    glDisableClientState(GL_NORMAL_ARRAY);

    //printf("draw arrays\n");
    //glDrawArrays(GL_POINTS, 0, example->num);

    //printf("disable stuff\n");
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glutSwapBuffers();
    caculateFPS();
}

void timerCB(int ms)
{
    //this makes sure the appRender function is called every ms miliseconds
    glutTimerFunc(ms, timerCB, ms);
    glutPostRedisplay();
}

void printbitmap(const char *msg, double x, double y)
{
   glRasterPos2d(x, y);
   while(*msg)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *msg++);
}

void display(void)
{
   char strfps[16];
   glClear(GL_COLOR_BUFFER_BIT);

   // Draw image
   glRasterPos2d(-1.00, -1.00);
   glDrawPixels(img_width, img_height,
                GL_RGB, GL_UNSIGNED_BYTE,
                the_image);

   // Draw instructions
   //glColor3d(0.0, 0.0, 0.0);
   sprintf(strfps, "fps : %2.2f", caculateFPS());
   printbitmap(strfps, -0.9, 0.9); 

   glutSwapBuffers();
}



void init_gl(int argc, char** argv)
{

	int window_width = 800;
	int window_height = 600;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH)/2, glutGet(GLUT_SCREEN_HEIGHT)/2);
	glutInitWindowPosition (glutGet(GLUT_SCREEN_WIDTH)/4, 
				glutGet(GLUT_SCREEN_HEIGHT)/4);

    
	glutWindowHandle = glutCreateWindow("auo 3d image show");

	glutFullScreen();

	// Tell OpenGL to treat image as normal array
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glutDisplayFunc(display); //main rendering function
	//glutDisplayFunc(appRender); //main rendering function
	glutTimerFunc(30, timerCB, 30); //determin a minimum time between frames
	//glutKeyboardFunc(appKeyboard);
	//glutMouseFunc(appMouse);
	//glutMotionFunc(appMotion);

}

