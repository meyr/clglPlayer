#ifndef __OPENCL_H
#define __OPENCL_H
#include <CL/cl.h>


void init_cl(void);
void exit_cl(void);
void clloadProgram(const char* relative_path);
const char* oclErrorString(cl_int error);
void loadData(char *inputimage, unsigned int size);
void transferParam(void);
void runKernel(void);

extern cl_kernel kernel;
extern cl_program program;
extern cl_context context;
extern cl_command_queue command_queue;
extern cl_mem clgl_mem, cl_buffer;


#endif
