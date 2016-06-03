#ifndef __UTILITY_H
#define __UTILITY_H

#ifdef  __cplusplus
extern "C" {
#endif
double shrDeltaT(int iCounterID);
unsigned char loadBMP(const char * imagepath,unsigned int *width,unsigned int *height, unsigned char **data);
#ifdef  __cplusplus
}
#endif
#endif
