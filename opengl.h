#ifndef __OPENGL_H
#define __OPENGL_H
#include <GL/glut.h>

void init_gl(int argc, char** argv);
void setImageAttr(int width, int height, unsigned char *image);
GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage);
extern int gl_image_id;

#endif
