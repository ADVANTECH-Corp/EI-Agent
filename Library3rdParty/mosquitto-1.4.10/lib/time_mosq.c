/*
Copyright (c) 2013,2014 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
Contributors:
   Roger Light - initial implementation and documentation.
*/

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#ifdef WIN32
//#  define _WIN32_WINNT _WIN32_WINNT_VISTA
#  include <windows.h>
#else
#  include <unistd.h>
#endif
#include <time.h>

#include "mosquitto.h"
#include "time_mosq.h"

#ifdef WIN32
unsigned long long _GetTickCount()
{
	LARGE_INTEGER qpc;
	LARGE_INTEGER qpf;
	unsigned long long tickMs = 0;
	QueryPerformanceCounter(&qpc);
	QueryPerformanceFrequency(&qpf);
	tickMs = qpc.QuadPart*1000/qpf.QuadPart;
	return tickMs;
}
#endif



time_t mosquitto_time(void)
{
#ifdef WIN32
		return _GetTickCount()/1000; /* FIXME - need to deal with overflow. */
#elif _POSIX_TIMERS>0 && defined(_POSIX_MONOTONIC_CLOCK)
	struct timespec tp;

	clock_gettime(CLOCK_MONOTONIC, &tp);
	return tp.tv_sec;
#elif defined(__APPLE__)
	static mach_timebase_info_data_t tb;
    uint64_t ticks;
	uint64_t sec;

	ticks = mach_absolute_time();

	if(tb.denom == 0){
		mach_timebase_info(&tb);
	}
	sec = ticks*tb.numer/tb.denom/1000000000;

	return (time_t)sec;
#else
	return time(NULL);
#endif
}

