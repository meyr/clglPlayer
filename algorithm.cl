__kernel void algorithm(__global char* mem_source, __global char* mem_dest)
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
	unsigned long pos = j * 1920 * 3 + i;
	
	if ((pos/2880)%2 == 0) {
		mem_dest[pos + 2880    ] = mem_source[pos    ];
		mem_dest[pos + 2880 + 1] = mem_source[pos + 1];
		mem_dest[pos + 2880 + 2] = mem_source[pos + 2];
	} else {
		mem_dest[pos - 2880    ] = mem_source[pos    ];
		mem_dest[pos - 2880 + 1] = mem_source[pos + 1];
		mem_dest[pos - 2880 + 2] = mem_source[pos + 2];
	}

}
