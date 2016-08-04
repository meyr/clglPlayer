#ifndef __OPENGL_H
#define __OPENGL_H
#ifdef _WIN32
#	include <Windows.h>
#	include <gl/glew.h>
#endif
#include <GL/freeglut.h>
#ifdef  __cplusplus
extern "C" {
#endif

void init_gl(int argc, char** argv);
void exit_gl(void);
void setImageAttr(int width, int height, unsigned char *image);
void createGLBuffers(unsigned int *pbo, int width, int height, char property);
void createGLTexture(unsigned int *tex_name, int width, int height);
void createGLRenderBuffers(unsigned int *_index);
void createGLBufTex(int width, int height, char property);
void pushImage(int width, int height, char *image);

extern unsigned int orign_texture;
extern unsigned int modify_texture;
extern unsigned int pbo_source;
extern unsigned int pbo_dest;
extern unsigned int tex_screen;
#define GL_BUFFER_SORC 1
#define GL_BUFFER_DEST 2
#ifdef  __cplusplus
}
#endif
#endif
