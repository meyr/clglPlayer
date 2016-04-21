#include <stdio.h>
#include "opencl.h"
#include "opengl.h"

cl_mem clgl_mem, cl_buffer;
int gl_image_id;

void transferParam(void)
{
	cl_int err;
	printf("%s\n",__func__);
	
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &clgl_mem);
	printf("clSetKernelArg0: %s\n", oclErrorString(err));
	err  = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &cl_buffer);
	printf("clSetKernelArg1: %s\n", oclErrorString(err));

	clFinish(command_queue);
}

void runKernel(void)
{

	cl_int err;
	cl_event event;
	size_t num = 1920*1080*3;
	//this will update our system by calculating new velocity and updating the positions of our particles
	//Make sure OpenGL is done using our VBOs
	glFinish();
	// map OpenGL buffer object for writing from OpenCL
	//this passes in the vector of VBO buffer objects (position and color)
	err = clEnqueueAcquireGLObjects(&clgl_mem, NULL, &event);
	printf("acquire: %s\n", oclErrorString(err));
	clFinish(command_queue);
	
	//execute the kernel
    	err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &num, NULL, 0, NULL, &event);
	printf("clEnqueueNDRangeKernel: %s\n", oclErrorString(err));
	clFinish(command_queue);
	
	//Release the VBOs so OpenGL can play with them
	err = clEnqueueReleaseGLObjects(&clgl_mem, NULL, &event);
	printf("release gl: %s\n", oclErrorString(err));
	clFinish(command_queue);
}


