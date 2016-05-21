#ifdef _DATA_FILE_READ
/*
C����������fscanf������ļ��е���ֵ���뵽��ά�����С�
*/
#include<stdio.h>
#include<stdlib.h>

#define N 4   //4��
#define L 100  //100��

const char file_name[50] = "d:\\a.txt";

int main(int argc, char *argv[])
{
    FILE *fp;
    double data[N][L] = {0.0};   //��ά����
    int index[N] = {0};   //��ά�������±�
    double temp;  
    int i, j;
    int count = 0;  //����������¼�Ѷ����ĸ�����
    if((fp=fopen(file_name, "rb")) == NULL) {
        printf("��ȷ���ļ�(%s)�Ƿ����!\n", file_name);
        exit(1);
    }
    while(1==fscanf(fp, "%lf", &temp)) {
        data[count%N][(index[count%N])++] = temp;
        count++;
    }
    for(i = 0; i < N; i++) {
        printf("��%d��Ԫ��Ϊ: \n", i+1);
        for(j = 0; j < index[i]; j++) {
            printf("%.16f ", data[i][j]);
        }
        printf("\n");
    }
    fclose(fp);
    return 0;
}

#endif