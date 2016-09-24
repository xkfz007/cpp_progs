#ifndef _FX_CPU_H
#define _FX_CPU_H
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
//#include "winxp.h"  // XP workarounds for CONDITION_VARIABLE and ATOMIC_OR
#else
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#endif

#if HAVE_LIBNUMA
#include <numa.h>
#endif
int getNumaNodeCount()
{
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN7 
    ULONG num = 1;
    if (GetNumaHighestNodeNumber(&num))
        num++;
    return (int)num;
#elif HAVE_LIBNUMA
    if (numa_available() >= 0)
        return numa_max_node() + 1;
    else
        return 1;
#else
    return 1;
#endif
}

/* static */
int getCpuCount()
{
#if _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif __unix__
    return sysconf(_SC_NPROCESSORS_ONLN);
#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW;
    nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if (count < 1)
    {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if (count < 1)
            count = 1;
    }

    return count;
#else
    return 2; // default to 2 threads, everywhere else
#endif
}

/*
 * check system whether little endian or big endian 
 * ret=1: littleEndian
 * ret=0: bigEndian
 */
int checkSystem() {
    union check
    {
        int i;
        char ch;
    }c;
    c.i=1;
    return (c.ch==1);
}
int checkSystem2() {
    int i=1;
    return ((i>>24&&15)==0);
}



#endif