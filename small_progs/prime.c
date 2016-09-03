/*
 *    Description: 利用一个比较好的方法判断是否是素数：根据5以后的素数之间的间隔都是2或4
 */
#ifdef _PRIME
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//方法一：
//这个是最原始的做法：从2开始，检测数n是否能被其之前的任意一个数整除，如果能，说明其不是素数。
int isPrime(int n)//函数返回1表示是质数，返回0表示不是质数
{
	int i;
	for (i = 2; i < n; i++)
	if (n % i == 0)
	return 0;
	return 1;
}

// 方法二：
//对上面算法的优化是缩小搜索范围，将范围有[2,n)缩小到[2, sqrt(n)). 因为对于一个小于n的整数x，如何n不能整除x，则n必然不能整除n/x，
//反之，相同，所以只需到sqrt(n)即可。
int isPrime2(int n)
{
	int i;
	for (i = 2; i*i<n; i++)
	if (n % i == 0)
	return 0;
	return 1;
}

// 方法三：
//这个方法是删除掉偶数。因为我们知道，一个数如果不能被2整除，那么也就不能被4、6、等所有的偶数整除。所以我们可以把循环规则改变成先判断2，
//如果不能被2整除就从3开始判断所有的奇数。
int isPrime3(int n)
{
	int i;
	if (n <= 3) return 1;
	if (n % 2 == 0) return 0;
	for (i = 3; i*i<n; i += 2)
	if (n % i == 0)
	return 0;
	return 1;
}

int is_prime(int n)
{
	int i,step=2;
	if(n<=3)
	return 1;
	if(n%2==0)
	return 0;
	if(n%3==0)
	return 0;
	for(i=5;i*i<=n;i+=step)
	{
		if(n%i==0)
		return 0;
		step^=6;
	}
	return 1;
}

void print_primes(int n1,int n2)
{
	int number;
	int divisor;
	int c=0;
	int step=2;
	number=n1;
	while(number<=0)
	number++;
	while(number<3) {
		fprintf(stdout, "%d ", number);
		number++;
		c++;
	}
	if(number%2==0)
	number++;
	// No other even numbers are prime; look at the remaining odd ones.
	while(number <= n2) {
		// See if any divisor from 3 up to the number evenly divides the number.
		for( divisor = 3; divisor < number; divisor = divisor + 2 ) {
			if( number % divisor == 0 )
			break;
		}
		// If the loop above stopped because the divisor got too big, we have got a prime.
		if( divisor >= number ) {
			fprintf(stdout, "%d ", number );
			c++;
			if((c+1)%10==0)
			fprintf(stdout,"\n");
		}
		number+=step;
	}
	fprintf(stdout,"\n");
}
void print_primes2(int n1,int n2) {
	int c=0;
	int i;
	for(i=n1;i<n2+1;i++) {
		if(i<=0)
		continue;
		if(is_prime(i)) {
			c++;
			fprintf(stdout,"%d ",i);
			if((c+1)%10==0)
			fprintf(stdout,"\n");
		}
	}
}

void usage() {
	fprintf(stdout,"Usage:\n");
	fprintf(stdout,"prime n1 n2 n3 ....\n");
	fprintf(stdout,"\tcheck if n1/n2/n3 is prime number\n");
	fprintf(stdout,"prime n1-n2 ....\n");
	fprintf(stdout,"\tprint prime numbers between n1 and n2\n");
}

int main(int argc,char*argv[]) {
	int i;
	char*p;
	if(argc<1) {
		usage();
		exit(-1);
	}
	if((p=strchr(argv[1],'-'))!=NULL) {
		int n1,n2;
		if(*argv[1]=='-'||*(p+1)=='\0') {
			usage();
			exit(-1);
		}
		*p='\0';
		n1=atoi(argv[1]);
		n2=atoi(p+1);
		print_primes2(n1,n2);

		return 0;
	}
	for(i=i;i<argc;i++) {
		int n=atoi(argv[i]);
		if(n<=1) {
			fprintf(stdout,"%d INVALID",n);
		}
		if(is_prime(n))
		fprintf(stdout,"%d YES\n",n);
		else
		fprintf(stdout,"%d NO\n",n);
	}
	return 0;
}
#endif
