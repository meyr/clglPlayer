#ifndef __PNG_H
#define __PN_H
#include <stdbool.h>
bool loadPngImage(char *name, int *outWidth, int *outHeight, bool *outHasAlpha, unsigned char **outData);
char openPNG(char *filename, char *image, int *width, int *height);
#endif
