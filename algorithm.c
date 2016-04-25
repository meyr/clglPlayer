#include <stdio.h>
#include "opencl.h"
#include "opengl.h"

int gl_image_id;

void transferParam(void)
{
	cl_int err;
	printf("%s\n",__func__);
	
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &cl_pbos[0]);
	printf("clSetKernelArg0: %s\n", oclErrorString(err));
	err  = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &cl_pbos[1]);
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
	err = clEnqueueAcquireGLObjects(command_queue, 2, cl_pbos, 0, NULL, NULL);
	if (err != CL_SUCCESS)
		printf("acquireGLObjects: %s\n", oclErrorString(err));
	clFinish(command_queue);
	
	//execute the kernel
    	err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &num, NULL, 0, NULL, &event);
	if (err != CL_SUCCESS)
		printf("clEnqueueNDRangeKernel: %s\n", oclErrorString(err));
	clFinish(command_queue);
	
	//Release the VBOs so OpenGL can play with them
	clEnqueueReleaseGLObjects(command_queue, 2, cl_pbos, 0, NULL, NULL);
	if (err != CL_SUCCESS)
		printf("releaseGLObjects: %s\n", oclErrorString(err));
	clFinish(command_queue);
}


