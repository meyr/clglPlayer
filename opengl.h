#ifndef __OPENGL_H
#define __OPENGL_H
#include <GL/glut.h>

#ifdef  __cplusplus
extern "C" {
#endif

void init_gl(int argc, char** argv);
void exit_gl(void);
void setImageAttr(int width, int height, unsigned char *image);
void createGLBuffers(GLuint *pbo);
void createGLTexture(GLuint *tex_name);
void createGLRenderBuffers(GLuint *_index);
void pushImage(void);

extern GLuint orign_texture;
extern GLuint modify_texture;
extern GLuint pbo_source;
extern GLuint pbo_dest;
extern GLuint tex_screen;
#ifdef  __cplusplus
}
#endif
#endif
