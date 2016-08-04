#include <stdio.h>
#include "opencl.h"
#include "opengl.h"

cl_mem mobj = NULL;
static int cl_width;
static int cl_height;
static int image_width;

void setImageWidth(int width)
{
	image_width = width;
}

void setKernelRange(int width, int height)
{
	cl_width = width;
	cl_height = height;
}

void transferParam(void)
{
	cl_int err;

	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &cl_pbos[0]);
	checkError("clSetKernelArg0", err);
	err  = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &cl_pbos[1]);
	checkError("clSetKernelArg1", err);
	err  = clSetKernelArg(kernel, 2, sizeof(cl_width), (void *) &image_width);
	checkError("clSetKernelArg1", err);


	mobj = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * 4 * sizeof(char), NULL, &err);
	checkError("clCreateBuffer", err);
	err  = clSetKernelArg(kernel_max, 0, sizeof(cl_mem), (void *)&cl_pbos[0]);
	checkError("clSetKernelArg max 0", err);
	err  = clSetKernelArg(kernel_max, 1, sizeof(cl_mem), (void *)&mobj);
	checkError("clSetKernelArg max 1", err);

	clFinish(command_queue);
}

void runKernel(void)
{

	cl_int err;
	cl_event event;
	size_t global_item_size_max = 16;
	size_t global_item_size = cl_width * cl_height;
	size_t global_item_size2[] = {cl_width, cl_height};
	char *output;
	int i;
	/* local item size :
	 * the number of work item
	 * in a work group in each diamension
	 *
	 * the only constraint for the global_work_size is
	 * that it must be a multiple of the local_work_size (for each dimension).
	 */
	size_t local_item_size = 64;
	size_t local_item_size2[] = {64, 8};
	//this will update our system by calculating new velocity and updating the positions of our particles
	//Make sure OpenGL is done using our VBOs
	glFinish();
	// map OpenGL buffer object for writing from OpenCL
	//this passes in the vector of VBO buffer objects (position and color)
	err = clEnqueueAcquireGLObjects(command_queue, 2, cl_pbos, 0, NULL, NULL);
	checkError("acquireGLObjects", err);
	clFinish(command_queue);

	//execute the kernel
	//err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
	//		&global_item_size,
	//		//&local_item_size,
	//		NULL,
	//		0, NULL, &event);

	err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL,
	                             global_item_size2,
	                             local_item_size2,
	                             //NULL,
	                             0, NULL, &event);

	checkError("clEnqueueNDRangeKernel", err);
	err = clEnqueueNDRangeKernel(command_queue, kernel_max, 1, NULL,
	                             &global_item_size_max,
	                             //&local_item_size,
	                             NULL,
	                             0, NULL, &event);
	checkError("clEnqueueNDRangeKernel max", err);


	///* Transfer result to host */
	output = malloc(4 * 4);
	err = clEnqueueReadBuffer(command_queue, mobj, CL_TRUE, 0, 4 * 4 * sizeof(char), output, 0, NULL, NULL);
	checkError("clEnqueueReadBuffer", err);

	free(output);
	//clFinish(command_queue);


	//Release the VBOs so OpenGL can play with them
	clEnqueueReleaseGLObjects(command_queue, 2, cl_pbos, 0, NULL, NULL);
	checkError("releaseGLObjects", err);
	clFlush(command_queue);
	clFinish(command_queue);
}


