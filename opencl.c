#include <stdio.h>
#include <string.h>
#include "opengl.h"
#include "opencl.h"
#include "utility.h"

cl_uint numDevices;
cl_device_id *devices;
cl_platform_id platform;
cl_context context;
cl_command_queue command_queue;
cl_program program;
cl_kernel kernel;
unsigned int deviceUsed;
cl_mem cl_pbos[2] = {0,0};

static const char* CL_GL_SHARING_EXT = "cl_khr_gl_sharing";

static char *file_contents(const char *filename, int *length)
{
	FILE *f;
	void *buffer;
	
	myfopen(f, filename, "r");
	if (!f) {
	    fprintf(stderr, "Unable to open %s for reading\n", filename);
	    return NULL;
	}
	
	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	buffer = malloc(*length+1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	((char*)buffer)[*length] = '\0';
	
	return (char*)buffer;
}



//NVIDIA's code follows
//license issues probably prevent you from using this, but shouldn't take long
//to reimplement
//////////////////////////////////////////////////////////////////////////////
//! Gets the platform ID for NVIDIA if available, otherwise default to platform 0
//!
//! @return the id 
//! @param clSelectedPlatformID         OpenCL platform ID
//////////////////////////////////////////////////////////////////////////////
static cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID)
{
    char chBuffer[1024];
    cl_uint num_platforms;
    cl_platform_id* clPlatformIDs;
    cl_int ciErrNum;
    cl_uint i = 0;

    *clSelectedPlatformID = NULL;

    // Get OpenCL platform count
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS)
    {
        //shrLog(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
        printf(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
        return -1000;
    }
    else
    {
        if(num_platforms == 0)
        {
            //shrLog("No OpenCL platform found!\n\n");
            printf("No OpenCL platform found!\n\n");
            return -2000;
        }
        else
        {
            // if there's a platform or more, make space for ID's
            if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
            {
                //shrLog("Failed to allocate memory for cl_platform ID's!\n\n");
                printf("Failed to allocate memory for cl_platform ID's!\n\n");
                return -3000;
            }

            // get platform info for each platform and trap the NVIDIA platform if found
            ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
            printf("Available platforms:\n");
            for(i = 0; i < num_platforms; ++i)
            {
                ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                if(ciErrNum == CL_SUCCESS)
                {
                    printf("platform %d: %s\n", i, chBuffer);
                    if(strstr(chBuffer, "NVIDIA") != NULL)
                    {
                        printf("selected platform %d\n", i);
                        *clSelectedPlatformID = clPlatformIDs[i];
                        break;
                    }
                }
            }

            // default to zeroeth platform if NVIDIA not found
            if(*clSelectedPlatformID == NULL)
            {
                //shrLog("WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
                //printf("WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
                printf("selected platform: %d\n", 0);
                *clSelectedPlatformID = clPlatformIDs[0];
            }

            free(clPlatformIDs);
        }
    }

    return CL_SUCCESS;
}


// Helper function to get error string
// *********************************************************************
const char* oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "";

}

void init_cl(void)
{
	cl_int err;
	int supported;
	cl_context_properties properties[7];
	int ext_size = 1024;
	char* ext_string;

		     
	printf("Initialize OpenCL object and context\n");
	//setup devices and context
	
	//this function is defined in util.cpp
	//it comes from the NVIDIA SDK example code
	err = oclGetPlatformID(&platform);
	//oclErrorString is also defined in util.cpp and comes from the NVIDIA SDK
	checkError("oclGetPlatformID", err);
	
	// Get the number of GPU devices available to the platform
	// we should probably expose the device type to the user
	// the other common option is CL_DEVICE_TYPE_CPU
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	checkError("clGetDeviceIDs", err);
	
	
	// Create the device list
	devices = (cl_device_id *)malloc(sizeof(cl_device_id) * numDevices);
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	checkError("clGetDeviceIDs", err);
	
	//create the context
	properties[0] = CL_GL_CONTEXT_KHR;
#ifdef _WIN32
	properties[1] = (cl_context_properties)wglGetCurrentContext();
	properties[2] = CL_WGL_HDC_KHR;
	properties[3] = (cl_context_properties)wglGetCurrentDC();
#else
	properties[1] = (cl_context_properties)glXGetCurrentContext();
	properties[2] = CL_GLX_DISPLAY_KHR;
	properties[3] = (cl_context_properties)glXGetCurrentDisplay();
#endif
	
	properties[4] = CL_CONTEXT_PLATFORM;
	properties[5] = (cl_context_properties)platform;
	properties[6] = 0;

	context = clCreateContext(properties, 1, devices, NULL, NULL, &err);
	checkError("clCreateContext", err);
	
	// Get string containing supported device extensions
	ext_string = (char*)malloc(ext_size);
	err = clGetDeviceInfo(devices[0], CL_DEVICE_EXTENSIONS, ext_size, ext_string, NULL);
	checkError("clGetDeviceInfo", err);
	//printf("%s\n", ext_string);
	// Search for GL support in extension string (space delimited)
	//supported = IsExtensionSupported(CL_GL_SHARING_EXT, ext_string, ext_size);
	//if(supported)
	// 	printf("Found GL Sharing Support!\n");



	//for right now we just use the first available device
	//later you may have criteria (such as support for different extensions)
	//that you want to use to select the device
	deviceUsed = 0;
	
	//create the command queue we will use to execute OpenCL commands
	command_queue = clCreateCommandQueue(context, devices[deviceUsed], 0, &err);
	checkError("clCreateConnabdQueue", err);

}

void exit_cl(void)
{
	printf("Releasing OpenCL memory\n");
	if(kernel)
		clReleaseKernel(kernel); 
	if(program)
		clReleaseProgram(program);
	if(command_queue)
		clReleaseCommandQueue(command_queue);
	
	//need to release any other OpenCL memory objects here
	//if(cl_a)
	//	clReleaseMemObject(cl_a);
	//if(cl_b)
	//	clReleaseMemObject(cl_b);
	//if(cl_c)
	//	clReleaseMemObject(cl_c);
	
	if(context)
		clReleaseContext(context);
	
	if(devices)
		free(devices);
	printf("OpenCL memory released\n");
}


//----------------------------------------------------------------------
static void clbuildExecutable()
{
    	char *build_log;
    	size_t ret_val_size;	// Build the program executable
	cl_build_status build_status;
	cl_int err;
	
	printf("building the program\n");
	// build the program
	//err = clBuildProgram(program, 0, NULL, "-cl-nv-verbose", NULL, NULL);
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	checkError("clBuildProgram", err);
	err = clGetProgramBuildInfo(program, devices[deviceUsed], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);
	err = clGetProgramBuildInfo(program, devices[deviceUsed], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
	build_log = (char *)malloc(sizeof(char) * (ret_val_size + 1));
	err = clGetProgramBuildInfo(program, devices[deviceUsed], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
	build_log[ret_val_size] = '\0';
	printf("BUILD LOG: \n %s", build_log);
	free(build_log);

	kernel = clCreateKernel(program, "algorithm", &err);
	checkError("clCreateKernel", err);
}

void clloadProgram(const char* relative_path)
{
	// Program Setup
	int pl;
	size_t program_length;
	char *cSourceCL;
	cl_int err;

	printf("load the program : %s\n", relative_path);
	
	//file_contents is defined in util.cpp
	//it loads the contents of the file at the given path
	cSourceCL = file_contents(relative_path, &pl);
	//printf("file: %s\n", cSourceCL);
	program_length = (size_t)pl;
	
	// create the program
	program = clCreateProgramWithSource(context, 1,
	                  (const char **) &cSourceCL, &program_length, &err);
	checkError("clCreateProgramWithSource", err);
	
	clbuildExecutable();
	
	//Free buffer returned by file_contents
	free(cSourceCL);
}

void createCLBufferFromGL(void)
{
	cl_int err;

        cl_pbos[0] = clCreateFromGLBuffer(context, CL_MEM_READ_ONLY , pbo_source, &err);
	checkError("clCreateFromGLBuffer(source)", err);
	cl_pbos[1] = clCreateFromGLBuffer(context, CL_MEM_WRITE_ONLY, pbo_dest,   &err);
	checkError("clCreateFromGLBuffer(dest)", err);

	//cl_pbos[1] = clCreateFromGLRenderbuffer(context, CL_MEM_READ_ONLY, pbo_dest,   &err);
	//printf("clCreateFromGLRenderBuffer(dest): %s\n", oclErrorString(err));
		
	//cl_pbos[1] = clCreateFromGLTexture2D(context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
	//				pbo_dest,   &err);
	//printf("clCreateFromGLTexture2D(dest): %s\n", oclErrorString(err));

}

