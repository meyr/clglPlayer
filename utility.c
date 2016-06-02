#include <stdio.h>


#ifdef _WIN32
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

