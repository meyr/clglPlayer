#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "opengl.h"
#include "opencl.h"
#include "utility.h"

int main(int argc, char *argv[])
{
	char filename[32] = "logo.bmp";
	unsigned char *image, crtn;
	int width, height;
	char brtn, hasAlpha;
	unsigned int i;

	/* read image file */
	if (argc >= 2)
		strcpy(filename,argv[1]);

	printf("open file %s\n",filename);
	brtn = loadBMP(filename, &width, &height, &image); 
	if (brtn)
		printf("width : %d , height : %d\n", width, height);

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
	clloadProgram("./algorithm.cl");
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
