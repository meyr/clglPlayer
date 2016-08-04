__kernel void getLeft(__global char *mem_source, __global char *mem_dest, int width)
{
	unsigned int i = 3 * get_global_id(0);
	unsigned int j = get_global_id(1);
	unsigned long src = j * width * 3 + i;
	unsigned long des = j * width / 2 * 3 + i;
	
	mem_dest[des    ] = mem_source[src    ];
	mem_dest[des + 1] = mem_source[src + 1];
	mem_dest[des + 2] = mem_source[src + 2];

}

__kernel void algorithm(__global char* mem_source, __global char* mem_dest, int width)
{
	//unsigned int i = 3 * get_global_id(0);
	//
	//if ((i/2880)%2 == 0) {
	//	mem_dest[i + 2880    ] = mem_source[i    ];
	//	mem_dest[i + 2880 + 1] = mem_source[i + 1];
	//	mem_dest[i + 2880 + 2] = mem_source[i + 2];
	//} else {
	//	mem_dest[i - 2880    ] = mem_source[i    ];
	//	mem_dest[i - 2880 + 1] = mem_source[i + 1];
	//	mem_dest[i - 2880 + 2] = mem_source[i + 2];
	//}

	unsigned int i = 3 * get_global_id(0);
	unsigned int j = get_global_id(1);
	unsigned long pos = j * width * 3 + i;
	unsigned int center = width * 3 / 2;
	
	if ((pos/2880)%2 == 0) {
		mem_dest[pos + center    ] = mem_source[pos    ];
		mem_dest[pos + center + 1] = mem_source[pos + 1];
		mem_dest[pos + center + 2] = mem_source[pos + 2];
	} else {
		mem_dest[pos - center    ] = mem_source[pos    ];
		mem_dest[pos - center + 1] = mem_source[pos + 1];
		mem_dest[pos - center + 2] = mem_source[pos + 2];
	}

}

__kernel void findMaxValue(__global char* mem_source, __global char *local_max)
{
	unsigned int i = get_global_id(0);
	if (i < 16)
		local_max[i] = i;
}
