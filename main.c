#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "opengl.h"
#include "opencl.h"
#include "png.h"

int main(int argc, char *argv[])
{
	char filename[32] = "logo.png";
	unsigned char *image, crtn;
	int width, height;
	bool brtn, hasAlpha;

	/* read image file */
	if (argc >= 2)
		strcpy(filename,argv[1]);

	printf("open file %s\n",filename);
	brtn = loadPngImage(filename, &width, &height, &hasAlpha, &image);
	if (brtn) {
		printf("width : %d , height : %d\n", width, height);
		printf("alpha : %s\n", hasAlpha ? "Yes" : "No");
	}

	/* openGL */
	setImageAttr(width, height, image);
	init_gl(argc, argv);
	createGLBuffers(&pbo_source);
	createGLBuffers(&pbo_dest);
        // create texture for blitting onto the screen
        createGLTexture(&tex_screen);        
	pushImage();


	/* openCL */
	init_cl();
	clloadProgram("../algorithm.cl");
	createCLBufferFromGL();
	transferParam();
	
	glutMainLoop();
exit:
	exit_cl();
	exit_gl();
	free(image);	
err:
	return 0;
}
