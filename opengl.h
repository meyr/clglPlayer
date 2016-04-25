#ifndef __OPENGL_H
#define __OPENGL_H
#include <GL/glut.h>

void init_gl(int argc, char** argv);
void setImageAttr(int width, int height, unsigned char *image);
void createPBO(GLuint* pbo);
void createTexture( GLuint* tex_name);
void pushImage(void);

extern GLuint orign_texture;
extern GLuint modify_texture;
extern GLuint pbo_source;
extern GLuint pbo_dest;
extern GLuint tex_screen;
#endif
