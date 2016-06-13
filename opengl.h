#ifndef __OPENGL_H
#define __OPENGL_H

#ifdef  __cplusplus
extern "C" {
#endif

void init_gl(int argc, char** argv);
void exit_gl(void);
void setImageAttr(int width, int height, unsigned char *image);
void createGLBuffers(unsigned int *pbo);
void createGLTexture(unsigned int *tex_name);
void createGLRenderBuffers(unsigned int *_index);
void pushImage(void);

extern unsigned int orign_texture;
extern unsigned int modify_texture;
extern unsigned int pbo_source;
extern unsigned int pbo_dest;
extern unsigned int tex_screen;
#ifdef  __cplusplus
}
#endif
#endif
