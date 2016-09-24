
#if 0
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