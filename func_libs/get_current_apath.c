//#define _GET_CURRENT_APATH
#ifdef _GET_CURRENT_APATH

int main(){
char current_absolute_path[MAX_SIZE];
//��ȡ��ǰ�������·��
int cnt = readlink("/proc/self/exe", current_absolute_path, MAX_SIZE);
if (cnt < 0 || cnt >= MAX_SIZE)
{
    printf("***Error***\n");
    exit(-1);
}
}

#endif