#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

#ifdef _WIN32
#	include <Windows.h>
#	include <time.h>
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	
	GetLocalTime(&wtm);
	tm.tm_year     = wtm.wYear - 1900;
	tm.tm_mon      = wtm.wMonth - 1;
	tm.tm_mday     = wtm.wDay;
	tm.tm_hour     = wtm.wHour;
	tm.tm_min      = wtm.wMinute;
	tm.tm_sec      = wtm.wSecond;
	tm. tm_isdst   = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	
	return (0);
}
#else
#	include <sys/time.h>
#endif

double shrDeltaT(int iCounterID)
{
	// local var for computation of microseconds since last call
	double DeltaT;
	
	static struct timeval _NewTime;  // new wall clock time (struct representation in seconds and microseconds)
	static struct timeval _OldTime[3]; // old wall clock timers 0, 1, 2 (struct representation in seconds and microseconds)

        // Get new counter reading
        gettimeofday(&_NewTime, NULL);

	if (iCounterID >= 0 && iCounterID <= 2) {
	    // Calculate time difference for timer (iCounterID).  (zero when called the first time) 
	    DeltaT =  ((double)_NewTime.tv_sec + 1.0e-6 * (double)_NewTime.tv_usec) - ((double)_OldTime[iCounterID].tv_sec + 1.0e-6 * (double)_OldTime[iCounterID].tv_usec);
	    // Reset old timer (iCounterID) to new timer
	    _OldTime[iCounterID].tv_sec  = _NewTime.tv_sec;
	    _OldTime[iCounterID].tv_usec = _NewTime.tv_usec;
	}
	else 
	// Requested counterID is out of rangewith respect to available counters
		DeltaT = -9999.0;

	// Returns time difference in seconds sunce the last call
	return DeltaT;

} 

unsigned char loadBMP(const char * imagepath,unsigned int *width,unsigned int *height, unsigned char **data)
{

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	FILE *file;
	unsigned int i;
	unsigned char *image;
	// Actual RGB data

	// Open the file
	myfopen(file, imagepath,"rb");
	if (!file) {
		printf("%s could not be opened.\n", imagepath); 
		getchar(); 
		return 0;
	}

	// Read the header, i.e. the 54 first bytes
	// If less than 54 bytes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Not a correct BMP file\n");
		return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	*width     = *(int*)&(header[0x12]);
	*height    = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    
		imageSize = *width * *height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      
		dataPos=54; // The BMP header is done that way

	// Create a buffer
	image = (unsigned char *)malloc(imageSize);

	// Read the actual data from the file into the buffer
	fread(image,1,imageSize,file);

	// Everything is in memory now, the file wan be closed
	fclose (file);

	*data = image;
	return 1;
}

