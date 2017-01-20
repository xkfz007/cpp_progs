#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _DEBUG 1

//valid formats: 2016-1-1,2016/1/1
int check_date(const char* date,int *year,int *month,int *day){
    sscanf(date,"%4d%*[-/]%2d%*[-/]%2d",year,month,day);

#if _DEBUG
    {
        fprintf(stdout,"year=%4d\n",*year);
        fprintf(stdout,"month=%02d\n",*month);
        fprintf(stdout,"day=%02d\n",*day);
    }
#endif
    if(*year < 1900 || 2099 < *year)
        return 0;
    switch(*month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            if(*day<1||*day>31)
                return 0;
            break;
        case 4: case 6: case 9: case 11:
            if(*day<1||*day>30)
                return 0;
            break;
        case 2:
            {
                int leap = (0 == *year % 4 && 0 != *year % 100) || (0 == *year % 400);
                if(*day<1||leap&&*day>29||!leap&&*day>28)
                    return 0;
            }
            break;
        default: 
            return 0;
    }
    return 1;
}
//if ret<0, date1 is earlier than date2
//if ret>0, date1 is later than date2
int time_cmp(int y1,int m1,int d1,
        int y2,int m2,int d2){
    if(y1==y2){
        if(m1==m2){
            return d1-d2;
        }
        else
            return m1-m2;
    }
    else
        return y1-y2;
}
int main1(){
    char date[100];
    int year,month,day;
    double diff;
    time_t now;
    struct tm newyear;
    struct tm *now_info;
    time(&now);
    now_info = localtime (&now);
    while(1){
        fprintf(stdout,"Set date[xxxx-xx-xx]:");
        fflush(stdout);
        fscanf(stdin,"%s",date);
        fprintf(stdout,"date=%s\n",date);
        if(check_date(date,&year,&month,&day)){
            fprintf(stdout,"now=%s\n",asctime(now_info));
            if(time_cmp(year,month,day,
                        now_info->tm_year+1900,now_info->tm_mon+1,now_info->tm_mday)<=0) {
                fprintf(stderr,"Date %s has passed\n",date);
            }
            else
                break;

        }
        else{
            fprintf(stderr,"Invalid date\n");
        }
    }
    sprintf(date,"%4d%02d%02d",year,month,day);
    fprintf(stdout,"final date=%s\n",date);

    
    //diff=difftime(mktime(&newyear),now);
    //fprintf(stdout,"diff=%f\n",diff);
    //fprintf(stdout,"diff=%f\n",diff);
}

int is_valid_str(char*s){
    if(strlen(s)!=2)
        return 0;
    return isxdigit(s[0])
        &&isxdigit(s[1]);
}
//check if network card address is valid
int check_hwaddr(char* s){
    char tmp[6][4];
    sscanf(s,"%2s%*[-:]%2s%*[-:]%2s%*[-:]%2s%*[-:]%2s%*[-:]%2s",
            tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
    //sscanf(s,"%d:%d:%d:%d:%d:%d",&tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
#if _DEBUG
    {
        int i;
        for(i=0;i<6;i++){
            fprintf(stdout,"value=%s\n",tmp[i]);
        }
    }
#endif
    if(is_valid_str(tmp[0])
       &&is_valid_str(tmp[1])
       &&is_valid_str(tmp[2])
       &&is_valid_str(tmp[3])
       &&is_valid_str(tmp[4])
       &&is_valid_str(tmp[5])
      )
    {
        return 1;
    }
    return 0;
}

int main2(){
    char hwaddr[100];
    while(1){
        fprintf(stdout,"Set physical address[xx:xx:xx:xx]: ");
        fflush(stdout);
        fscanf(stdin,"%s",hwaddr);
        if(!check_hwaddr(hwaddr)){
            fprintf(stdout,"Invalid physical address\n");
            continue;
        }
        break;
    }

}
int stricmp(const char *a, const char *b)
{
    int c1, c2;
    do {
        c1 = tolower(*a++);
        c2 = tolower(*b++);
    } while (c1 && c1 == c2);
    return c1 - c2;
}

void main3(){
    char f[10];
    fprintf(stdout,"Enable or not[Yy][Nn]:");
    fflush(stdout);
    fscanf(stdin,"%s[YESyesNOno]",f);
    fprintf(stdout,"ans=%s\n",f);
    if(stricmp(f,"yes")||stricmp(f,"y")){
        fprintf(stdout,"flag=1");
    }
    else if(stricmp(f,"no")||stricmp(f,"n")){
        fprintf(stdout,"flag=0");
    }
    else{
        fprintf(stdout,"Invalid answer");
    }
}
