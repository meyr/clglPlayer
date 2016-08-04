#include <stdio.h>
#include <CL/cl_gl.h>
#include "opengl.h"
#include "opencl.h"
#include "shader.h"
#include "utility.h"
#include "decode.h"

GLuint pbo_source;
GLuint pbo_dest;
GLuint tex_source;                          // (offscreen) render target
GLuint tex_dest;                          // (offscreen) render target
GLuint programID;
GLuint TextureID;
GLuint vertexbuffer, uvbuffer;
static int glutWindowHandle = 0;
static int source_width;
static int source_height;
static int dest_width;
static int dest_height;
static unsigned char *the_image;
static const char titleName[] = "auo clgl player";
static char selectSource;
static char selectSourced;

const GLfloat g_vertex_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
};

const GLfloat g_uv_data[] = {
	0.00f, 1.00f,
	1.00f, 1.00f,
	0.00f, 0.00f,
	1.00f, 1.00f,
	1.00f, 0.00f,
	0.00f, 0.00f,
};

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

void pushImage(int width, int height, char *image)
{
	// activate destination buffer
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo_source);
	glBufferSubData(GL_PIXEL_PACK_BUFFER_ARB, 0, width * height * 3, image);

}

void processImage(void)
{
	int texture_width, texture_height;
	/* download texture from CL buffer */
	if (selectSourced){
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_source);
		glBindTexture(GL_TEXTURE_2D, tex_source);
		texture_width = source_width;
		texture_height = source_height;
	}else{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_dest);
		glBindTexture(GL_TEXTURE_2D, tex_dest);
		texture_width = dest_width;
		texture_height = dest_height;
	}

	/* bmp pixel format is BGR */
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
	                texture_width, texture_height,
	                //GL_BGR, GL_UNSIGNED_BYTE, NULL);
	                GL_RGB, GL_UNSIGNED_BYTE, NULL);

	/* bind our texture in texture unit 0 */
	if (selectSourced)
		glActiveTexture(GL_TEXTURE0);
	else
		glActiveTexture(GL_TEXTURE1);

	/* set our texture sampler to user texture unit 0 */
	glUniform1i(TextureID, 0);

	/* update source selected value */
	selectSourced = selectSource;
}

void showTitle(float fps)
{
	char strfps[48];

	mysprintf(strfps, "%s | %s | %2.2f fps\n", titleName,
	          selectSource ? "source" : "result", fps);
	glutSetWindowTitle(strfps);

}

void appRender(void)
{
	double pTime0, pTime1, pTime2;
	// grab frame
	decode_grab(&the_image);
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo_source);
	glBufferSubData(GL_PIXEL_PACK_BUFFER_ARB, 0, source_width * source_height * 3, the_image);

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
	glDeleteTextures(1, &tex_source);
	glDeleteTextures(1, &tex_dest);

}

void appKeyboard(unsigned char key, int x, int y)
{
	static char fullScreen = 0;
	//this way we can exit the program cleanly
	switch (key) {
	case 27:
		if(fullScreen){
			glutReshapeWindow(glutGet(GLUT_SCREEN_WIDTH) / 2,
			                  glutGet(GLUT_SCREEN_HEIGHT) / 2);
			glutPositionWindow(glutGet(GLUT_SCREEN_WIDTH) / 4,
			                   glutGet(GLUT_SCREEN_HEIGHT) / 4);
			fullScreen = 0;
		}
		break;
	case 'f':
	case 'F':
		fullScreen = (fullScreen == 0 ? 1 : 0);
		if (fullScreen)
			glutFullScreen();
		else {
			glutReshapeWindow(glutGet(GLUT_SCREEN_WIDTH) / 2,
			                  glutGet(GLUT_SCREEN_HEIGHT) / 2);
			glutPositionWindow(glutGet(GLUT_SCREEN_WIDTH) / 4,
			                   glutGet(GLUT_SCREEN_HEIGHT) / 4);
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
	default :
		break;
	}
}

void appReshape(GLsizei w, GLsizei h)
{
	//GLsizei vsize;
	//
	//vsize = (w < h) ? w : h;
	glViewport(0, 0, w, h);
}

void init_gl(int argc, char** argv)
{
	int i;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH) / 2, glutGet(GLUT_SCREEN_HEIGHT) / 2);
	glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH) / 4,
	                       glutGet(GLUT_SCREEN_HEIGHT) / 4);

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

	/* initial glew lib */
#ifdef _WIN32
	glewInit();
#endif
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

void createGLBufTex(int width, int height, char property)
{
	if(property == GL_BUFFER_SORC){
		createGLBuffers(&pbo_source, width, height, property);
		createGLTexture(&tex_source, width, height);
	}

	if(property == GL_BUFFER_DEST){
		createGLBuffers(&pbo_dest, width, height, property);
		createGLTexture(&tex_dest, width, height);
	}

}

void createGLBuffers(GLuint* pbo, int width, int height,char property)
{
	unsigned int size_tex_data;
	unsigned int num_texels;
	unsigned int num_values;

	/* set up data parameter */
	num_texels = width * height;
	num_values = num_texels * 3;
	size_tex_data = sizeof(GLubyte) * num_values;

	/* create buffer object */
	glGenBuffers(1, pbo);
	glBindBuffer(GL_ARRAY_BUFFER, *pbo);

	/* buffer data */
	glBufferData(GL_ARRAY_BUFFER, size_tex_data, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* save width/height */
	if(property == GL_BUFFER_SORC){
		source_width = width;
		source_height = height;
	}

	if(property == GL_BUFFER_DEST){
		dest_width = width;
		dest_height = height;
	}
}

void createGLTexture(GLuint* tex_name, int width, int height)
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
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

}


