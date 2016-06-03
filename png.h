#ifndef __PNG_H
#define __PN_H
char loadPngImage(char *name, int *outWidth, int *outHeight, char *outHasAlpha, unsigned char **outData);
char openPNG(char *filename, char *image, int *width, int *height);
#endif
