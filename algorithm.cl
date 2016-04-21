__kernel void algorithm(__global char* clgl_mem, __global char* cl_buffer)
{
    unsigned int i = get_global_id(0);

    cl_buffer[i] = clgl_mem[i];
}
