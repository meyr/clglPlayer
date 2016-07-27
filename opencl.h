#ifndef __OPENCL_H
#define __OPENCL_H
#ifdef _WIN32
#	include <Windows.h>
#endif
#include <CL/cl.h>
#include <CL/cl_gl.h>
#ifdef  __cplusplus
extern "C" {
#endif

void init_cl(void);
void exit_cl(void);
void clloadProgram(const char* relative_path);
const char* oclErrorString(cl_int error);
void transferParam(void);
void runKernel(void);
void createCLbufferFromGL(void);

extern cl_kernel kernel;
extern cl_kernel kernel_max;
extern cl_program program;
extern cl_context context;
extern cl_command_queue command_queue;
extern cl_mem cl_pbos[2];

#define checkError(func, err) \
	do { \
		if (err != CL_SUCCESS) \
			printf(func ": %s\n", oclErrorString(err));\
	} while(0)

#ifdef  __cplusplus
}
#endif

#endif
