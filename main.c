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

	/* openCL */
	//init_cl();
	//clloadProgram("../algorithm.cl");
	//transferParam();

	glutMainLoop();

exit:
	//exit_cl();
	free(image);	
err:
	return 0;
}