#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "opengl.h"
#include "opencl.h"
#include "utility.h"
#include "decode.h"

int main(int argc, char *argv[])
{
	int width, height;

	/* read image file */
	if (argc >= 2)
		decode_init(argv[1], &width, &height);

	/* openGL */
	init_gl(argc, argv);
	createGLBuffers(&pbo_source, width, height, GL_BUFFER_SORC);
	createGLBuffers(&pbo_dest, width, height, GL_BUFFER_DEST);
	/* create texture for blitting onto the screen */
	createGLTexture(&tex_screen);

	/* openCL */
	init_cl();
	clloadProgram("./algorithm.cl");
	createCLBufferFromGL();
	transferParam();

	glutMainLoop();
exit:
	decode_close();
	exit_cl();
	exit_gl();
err:
	return 0;
}
