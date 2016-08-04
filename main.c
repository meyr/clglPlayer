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
	width = height = 0;
	/* read image file */
	if (argc >= 2)
		decode_init(argv[1], &width, &height);

	/* openGL */
	init_gl(argc, argv);
	createGLBufTex(width, height, GL_BUFFER_SORC);
	createGLBufTex(width/2, height, GL_BUFFER_DEST);

	/* openCL */
	init_cl();
	clloadProgram("./algorithm.cl");
	createCLBufferFromGL();
	setImageWidth(width);
	setKernelRange(width/2, height);
	transferParam();

	glutMainLoop();
exit:
	decode_close();
	exit_cl();
	exit_gl();
err:
	return 0;
}
