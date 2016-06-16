__kernel void algorithm(__global char* mem_source, __global char* mem_dest)
{
    unsigned int i = get_global_id(0);

	if ((i/2880)%2 == 0)
    		mem_dest[i+2880] = mem_source[i];
	else
    		mem_dest[i-2880] = mem_source[i];
}
