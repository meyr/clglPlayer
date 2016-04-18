#include <stdio.h>
#include <CL/cl.h>

cl_context_properties props[] =
{
    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
    CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
    0
};
 
void init_cl(void)
{
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;


    printf("Initialize OpenCL object and context\n");

    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);


    deviceUsed = 0;
    err = platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    printf("getDevices: %s\n", oclErrorString(err));
    printf("devices.size(): %d\n", devices.size());
    int t = devices.front().getInfo<CL_DEVICE_TYPE>();
    printf("type: device: %d CL_DEVICE_TYPE_GPU: %d \n", t, CL_DEVICE_TYPE_GPU);

               context = cl::Context(CL_DEVICE_TYPE_GPU, props);
       queue = cl::CommandQueue(context, devices[deviceUsed], 0, &err);
    

}
