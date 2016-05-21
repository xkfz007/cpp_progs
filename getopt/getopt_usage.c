/*
 * this program is the usage example of function "getopt"
 */
#ifdef _GETOPT_USAGE
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
#include <getopt.h>
const char *program_name;
void print_usage(FILE*stream,int exit_code){
    fprintf(stream,"Usage:%s [options]\n",program_name);
    fprintf(stream,
            "-a argument Use 1\n"
            "-b argument Use 2\n"
            "-c Use 3\n"
            "-d Use 4\n"
            "-e Use 5\n");
    
    exit(exit_code);
}
int main(int argc,char*argv[]){
    char *servername=getenv("SERVERNAME");
    char *options="a:b::cde";
    int ch;
    program_name=argv[0];
    if(argc==1){
        print_usage(stdout,0);
    }
    if(servername==NULL)
    {
        while((ch=getopt(argc,argv,options))!=-1){
            printf("optind:%d\n",optind);
            printf("optarg:%s\n",optarg);
            printf("ch:%c\n",ch);
            switch(ch)
            {
                case 'a':
                    printf("option a:'%s'\n",optarg);
                    servername="a.com";
                    break;
                case 'b':
                    printf("option b:'%s'\n",optarg);
                    servername="b.com";
                    break;
                case 'c':
                    printf("option c\n");
                    servername="c.com";
                    break;
                case 'd':
                    printf("option d\n");
                    servername="d.com";
                    break;
                case 'e':
                    printf("option e\n");
                    servername="e.com";
                    break;
                default:
                    printf("unknown option:%c\n",ch);
                    servername="localhost.localmain";
                    break;
            }
            printf("optopt+%c\n",optopt);
        }

    }
    printf("servername:%s\n",servername);
    return 0;
}
#endif