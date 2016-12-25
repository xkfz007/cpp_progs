//#define _GET_CURRENT_APATH
#ifdef _GET_CURRENT_APATH

int main(){
char current_absolute_path[MAX_SIZE];
//获取当前程序绝对路径
int cnt = readlink("/proc/self/exe", current_absolute_path, MAX_SIZE);
if (cnt < 0 || cnt >= MAX_SIZE)
{
    printf("***Error***\n");
    exit(-1);
}
}

#endif