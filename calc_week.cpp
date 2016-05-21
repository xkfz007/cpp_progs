#ifdef _CALC_WEEK
/*
 * 根据日期计算星期
*/
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
using namespace std ;

const int MONTH [12]={31,28,31,30,31,30,31,31,30,31,30,31,};
const string WEEK[]={"SUN" ,"MON" ,"TUE" ,"WEN" ,"THU" ,"FRI" ,"SAT" };
int get_day_count (int year ,int month ,int day )
{
    bool isleap=((year %4==0&&year%100!=0)||( year%400==0));
    int count;
    count=0;
    if( isleap&&month >2)
        count++;
    for( int i=1; i< month;i ++)
        count+=MONTH [i -1];
    count+= day;

    return count;
}
int str2int (string str )
{
    istringstream is( str);
    int n;
    is>> n;
    return n;

}
void parse (string str ,int &year, int & month,int &day)
{
    size_t p1, p2;
    p1= str. find( '-');
    if( p1== string::npos )
    {
        cout<< "error"<<endl ;
    }
    string subs= str. substr(0,p1 );
    year= str2int(subs );

    p2= str. find_last_of('-' );
    if( p1== string::npos )
    {
        cout<< "error"<<endl ;
    }
    subs= str. substr(p1 +1,p2 -p1 );
    month= str2int(subs );

    subs= str. substr(p2 +1);
    day= str2int(subs );
}

int main (int argc ,char **argv )
{
    if( argc<2)
    {
        cout<< "usage:"<<argv [0]<<" <year>-<month>-<day>"<< endl;
        return 1;
    }
    string str( argv[1]);
    int year, month,day ;
    parse( str, year, month,day );

    cout<< "year="<<year <<endl;
    cout<< "month="<<month <<endl;
    cout<< "day="<<day <<endl;
    int count;
    count= get_day_count(year ,month ,day );
    cout<< "count="<<count <<endl;

    int week=((year -1)+(year-1)/4-( year-1)/100+( year-1)/400+ count)%7;
   
#if 0
    struct tm tm1,* tm2;
    time_t timep;
    int week;
    tm1. tm_year= year -1900; 
    tm1. tm_mon=month -1; 
    tm1. tm_mday=day ; 
    tm1. tm_hour=12; 
    tm1. tm_min=0; 
    tm1. tm_sec=0;
    timep= mktime(&tm1 );
    tm2= localtime(&timep );
    cout<< asctime(tm2 )<<endl;

    week=( int) ( tm2-> tm_wday);
#endif
    cout<< WEEK[ week]<< endl;
}

static int daytab[][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31}
,{0,31,29,31,30,31,30,31,31,30,31,30,31}};
int day_of_year(int year,int month,int day){
    int i,leap;
    leap=year%4==0&&year%100!=0||year%400==0;
    if(month<1||month>12)
        return -1;
    if(day<1||day>daytab[leap][month])
        return -1;
    for(i=1;i<month;i++)
        day+=daytab[leap][i];
    return day;
}
void month_day(int year,int yearday,int *pmonth,int *pday){
    int i,leap;
    if(year<1){
        *pmonth=-1;
        *pday=-1;
        return ;
    }
    leap=year%4==0&&year%100!=0||year%400==0;
    for(i=1;i<=12&&yearday>daytab[leap][i];i++)
        yearday-=daytab[leap][i];
    if(i>12&&yearday>daytab[leap][12]){                         
        *pmonth=-1;
        *pday=-1;
    }
    else{
        *pmonth=i;
        *pday=yearday;
    }
}   
#endif