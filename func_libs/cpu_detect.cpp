#define _CPU_DETECT
#ifdef _CPU_DETECT
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
int main(){
    int numNumaNodes = getNumaNodeCount();
    int cpuCount = getCpuCount();
    bool bNumaSupport = false;
    int cpusPerNode[31+ 1]={0};

#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN7 
    bNumaSupport = true;
#elif HAVE_LIBNUMA
    bNumaSupport = numa_available() >= 0;
#endif


    for (int i = 0; i < cpuCount; i++)
    {
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN7 
        UCHAR node;
        if (GetNumaProcessorNode((UCHAR)i, &node))
            cpusPerNode[node]++;
        else
#elif HAVE_LIBNUMA
        if (bNumaSupport )
            cpusPerNode[numa_node_of_cpu(i)]++;
        else
#endif
            cpusPerNode[0]++;
    }

    fprintf(stdout,"NUMA node count:%d\n",numNumaNodes);
    fprintf(stdout,"cpu count:%d\n",cpuCount);
    for(int i=0;i<numNumaNodes;i++){
        fprintf(stdout,"Node%d has %d cpu(s)\n",i,cpusPerNode[i]);
    }

}
#endif