#include <stdio.h>
#include "opencl.h"

int gl_image_id;

void transferParam(void)
{
	cl_int err;
	
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &cl_pbos[0]);
	checkError("clSetKernelArg0", err);
	err  = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &cl_pbos[1]);
	checkError("clSetKernelArg1", err);

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
	checkError("acquireGLObjects", err);
	clFinish(command_queue);
	
	//execute the kernel
    	err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &num, NULL, 0, NULL, &event);
	checkError("clEnqueueNDRangeKernel", err);
	clFinish(command_queue);
	
	//Release the VBOs so OpenGL can play with them
	clEnqueueReleaseGLObjects(command_queue, 2, cl_pbos, 0, NULL, NULL);
	checkError("releaseGLObjects", err);
	clFinish(command_queue);
}


