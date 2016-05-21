#ifdef _DATA_FILE_READ
/*
C语言中利用fscanf逐个将文件中的数值读入到二维数组中。
*/
#include<stdio.h>
#include<stdlib.h>

#define N 4   //4列
#define L 100  //100行

const char file_name[50] = "d:\\a.txt";

int main(int argc, char *argv[])
{
    FILE *fp;
    double data[N][L] = {0.0};   //二维数组
    int index[N] = {0};   //二维数组列下标
    double temp;  
    int i, j;
    int count = 0;  //计数器，记录已读出的浮点数
    if((fp=fopen(file_name, "rb")) == NULL) {
        printf("请确认文件(%s)是否存在!\n", file_name);
        exit(1);
    }
    while(1==fscanf(fp, "%lf", &temp)) {
        data[count%N][(index[count%N])++] = temp;
        count++;
    }
    for(i = 0; i < N; i++) {
        printf("第%d列元素为: \n", i+1);
        for(j = 0; j < index[i]; j++) {
            printf("%.16f ", data[i][j]);
        }
        printf("\n");
    }
    fclose(fp);
    return 0;
}

#endif