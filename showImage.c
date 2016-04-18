#include <png.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "opengl.h"

bool loadPngImage(char *name, int *outWidth, int *outHeight, bool *outHasAlpha, unsigned char **outData) {
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type, i;
    FILE *fp;
    
    if ((fp = fopen(name, "rb")) == NULL)
        return false;
    
    /* Create and initialize the png_struct
     * with the desired error handler
     * functions.  If you want to use the
     * default stderr and longjump method,
     * you can supply NULL for the last
     * three parameters.  We also supply the
     * the compiler header file version, so
     * that we know if the application
     * was compiled with a compatible version
     * of the library.  REQUIRED
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL);
    
    if (png_ptr == NULL) {
        fclose(fp);
        return false;
    }
    
    /* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }
    
    /* Set error handling if you are
     * using the setjmp/longjmp method
     * (this is the normal method of
     * doing things with libpng).
     * REQUIRED unless you  set up
     * your own error handlers in
     * the png_create_read_struct()
     * earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        /* If we get here, we had a
         * problem reading the file */
        return false;
    }
    
    /* Set up the output control if
     * you are using standard C streams */
    png_init_io(png_ptr, fp);
    
    /* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);
    
    /*
     * If you have enough memory to read
     * in the entire image at once, and
     * you need to specify only
     * transforms that can be controlled
     * with one of the PNG_TRANSFORM_*
     * bits (this presently excludes
     * dithering, filling, setting
     * background, and doing gamma
     * adjustment), then you can read the
     * entire image (including pixels)
     * into the info structure with this
     * call
     *
     * PNG_TRANSFORM_STRIP_16 |
     * PNG_TRANSFORM_PACKING  forces 8 bit
     * PNG_TRANSFORM_EXPAND forces to
     *  expand a palette into RGB
     */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
    
    png_uint_32 width, height;
    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);
    *outWidth = width;
    *outHeight = height;
    
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    printf("malloc size : %d\n", row_bytes * (*outHeight));
    *outData = (unsigned char*) malloc(row_bytes * (*outHeight));
    
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    
    for (i = 0; i < *outHeight; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(*outData+(row_bytes * (*outHeight-1-i)), row_pointers[i], row_bytes);
    }
    
    /* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    
    /* Close the file */
    fclose(fp);
    
    /* That's it */
    return true;
}


int main(int argc, char *argv[])
{
	char filename[32] = "logo.png";
	bool brtn, hasAlpha;
	int width, height; 
	unsigned char *image;

	if (argc >= 2)
		strcpy(filename,argv[1]);
	
	printf("open file %s\n",filename);
	brtn = loadPngImage(filename, &width, &height, &hasAlpha, &image);
	if (brtn) {
		printf("width : %d , height : %d\n", width, height);
		printf("alpha : %s\n", hasAlpha ? "Yes" : "No");
	}


	/* openGL */
	init_gl(argc, argv);
	setImageAttr(width, height, image);
	glutMainLoop();

	free(image);	
	return 0;
}
