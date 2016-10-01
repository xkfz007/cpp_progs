/*
 *    File Destroyer 
 *
 *    Copyright (C) 2015 Chaobs
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    E-mail: chaobs@outlook.com
 *    Blog: www.cnblogs.com/chaobs
 *
 *    �÷�: file-destroyer [filename1] <filename2>...
 *
 *     �㷨���ܣ�
 *     ��������������DOD5220.22M��׼�����ļ����٣�������������:
 *     ��1�����ļ�����0x00���ǣ�����0x01���ǣ�����ظ����Σ�
 *     ��2�����ļ���һ�����ֵ���ǣ�
 *     ��3�����ļ�����Ϊһ�����ַ��ļ��������ɾ��֮��
 *
 *     �㷨�ɿ�����֤��
 *     ���㷨��Ȼ�Ѿ����ٱ��������������ã���Ҳ�㹻Ӧ��һ��Ļ��������������ļ��ָ�����ָ��Ŀ����Ի��д���֤��
 *
 */
//#define _FILE_DESTROYER
#ifdef _FILE_DESTROYER
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
void notice(int i, char *s);    /* print short notice */
 
void wipe(FILE *f, char c);    /* core func */
 
long file_size(FILE *f);    /* get the size of a file */
 
int require(int c, char *s[]);
 
int main(int argc, char *argv[])
{
    int i, j;
    char ch;
 
    FILE *f;
 
    notice(1, argv[0]);
 
    if (argc < 2) {
        /* too few arguments */
        notice(2, argv[0]);
        exit(0);
    }
 
    if (!require(argc, argv)) {
        fprintf(stderr, "Cancel Operating.\n");
        exit(0);    /* cancel */
    }
 
    srand(time(NULL));    /* randomize */
 
    for (i = 1; i < argc; ++i) {
        /* process each file */
 
        if ((f = fopen(argv[i], "r+b")) == NULL) {/* fail to open file */
            fprintf(stderr, "Error when open %s:\n", argv[i]);
            exit(0);
        }
 
        for (j = 0; j < 3; ++j) {
            /* DOD5220.22M Step 1 */
            wipe(f, 0x00);
            wipe(f, 0x01);
        }
 
        wipe(f, rand() % 256);    /* Step 2 */
 
        if (rename(argv[i], "C")) {
            /* Step 3*/
            fprintf(stderr, "Error when rename %s\n", argv[i]);
            exit(0);
 
            /* XXX:�ļ�����ͻ�Ľ�������Կ���ʹ��tmpnam()��*/
        }
 
        remove("C"); /* XXX:�����һ����������������֤ɾ�������������ļ��� */
        fclose(f);
    }
 
    printf("Done! Destroy %d files\n", argc - 1);
 
    return 0;
}
 
/* implementation */
 
void notice(int i, char *s)
{
    if (i == 1) {
        printf("\nFile Destroyer Copyright (C) 2015 Chaobs\n");
        printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
        printf("This is free software, and you are welcome to redistribute under certain conditions.\n\n");
    } else {
        fprintf(stderr, "Usage: %s [filename1] <filename2> ...\n", s);
    }
}
 
void wipe(FILE *f, char c)
{
    long i;
    long len = file_size(f); /*����ļ�����*/
 
    for (i = 0; i < len; ++i)
        fwrite(&c, 1, 1, f);    /* overwrite */
}
 
long file_size(FILE *f)
{
    long len;
    fseek(f, 0, SEEK_END);    /* jump to the and of file */
    len = ftell(f);
    fseek(f, 0, SEEK_SET);    /* restore */
    return len; /*��л���ѱ�����*/
}
 
int require(int c, char *s[])
{
    int i;
    char ch;
    for (i = 1; i < c; ++i) {
        /* FIXME: the comfirm function can make mistakes and
         * it is not convenient even can't work in some cases.
         */
        printf("Do you want to destroy %s ?(y/n) ", s[i]);
        ch = getchar();
        getchar(); /* '\n' */
        if (ch == 'n')
            return 0;
    }
 
    return 1;
}
#endif