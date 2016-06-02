#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel  void algorithm(__global uchar *src_data, __global uchar *dest_data,
	                                            int W,    int H,							
	                                           float sinTheta, float cosTheta )					
{    
	const int ix = get_global_id(0);  //W
	const int iy = get_global_id(1);  //H    
	 
	int xc = (W/3)/2;
	int yc = H/2;
	
	int iix = (int)(ix / 3.0);
	int rgb = ix % 3;  //B:0, G:1, R:2
	
	int xpos =  (int)((iix - xc) * cosTheta - (iy-yc) * sinTheta + xc);    
	int ypos =  (int)((iix - xc) * sinTheta + (iy-yc) * cosTheta + yc);
	
	if ((xpos>=0) && (xpos< (H*3))	&& (ypos>=0) && (ypos< (W/3)))	
	{
	     dest_data[(ypos*H*3)+(xpos*3) + rgb]= src_data[iy*W+ix]; 
	}

}

