#ifndef __UTILITY_H
#define __UTILITY_H
#ifdef _WIN32
#define myfopen(pfile, path, mode) \
	fopen_s(&pfile, path, mode)
#else
#define myfopen(pfile, path, mode) \
	do { \
		pfile = fopen(path, mode);\
	} while(0)
#endif

#ifdef  __cplusplus
extern "C" {
#endif
double shrDeltaT(int iCounterID);
unsigned char loadBMP(const char * imagepath,unsigned int *width,unsigned int *height, unsigned char **data);
#ifdef  __cplusplus
}
#endif
#endif
