#include <stdio.h>
#include "opencl.h"

int gl_image_id;

void transferParam(void)
{
	cl_int err;
	printf("%s\n",__func__);
	unsigned int w = 5760;
	unsigned int h = 1080;
	float sintheda = 1.0;
	float costheda = 0.0;

	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &cl_pbos[0]);
	checkError("clSetKernelArg0", err);
	err  = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &cl_pbos[1]);
	checkError("clSetKernelArg1", err);
	err  = clSetKernelArg(kernel, 2, sizeof(cl_int), &w);
	checkError("clSetKernelArg2", err);
	err  = clSetKernelArg(kernel, 3, sizeof(cl_int), &h);
	checkError("clSetKernelArg3", err);
	err  = clSetKernelArg(kernel, 4, sizeof(float), &sintheda);
	checkError("clSetKernelArg4", err);
	err  = clSetKernelArg(kernel, 5, sizeof(float), &costheda);
	checkError("clSetKernelArg5", err);

	clFinish(command_queue);
}

void runKernel(void)
{

	cl_int err;
	cl_event event;
	size_t num = 1920*1080*3;
	size_t global_ids[] = {1920 * 3, 1080};
	//this will update our system by calculating new velocity and updating the positions of our particles
	//Make sure OpenGL is done using our VBOs
	glFinish();
	// map OpenGL buffer object for writing from OpenCL
	//this passes in the vector of VBO buffer objects (position and color)
	err = clEnqueueAcquireGLObjects(command_queue, 2, cl_pbos, 0, NULL, NULL);
	checkError("clEnqueueAcquireGLObjects", err);
	clFinish(command_queue);
	
	//execute the kernel
    	err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_ids, NULL, 0, NULL, &event);
	checkError("clEnqueueNDRangeKernel", err);
	clFinish(command_queue);
	
	//Release the VBOs so OpenGL can play with them
	clEnqueueReleaseGLObjects(command_queue, 2, cl_pbos, 0, NULL, NULL);
	checkError("clEnqueueReleaseGLObjects", err);
	clFinish(command_queue);
}


