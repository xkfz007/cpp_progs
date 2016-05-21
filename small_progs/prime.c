/*
 *    Description: 利用一个比较好的方法判断是否是素数：根据5以后的素数之间的间隔都是2或4
 */
#ifdef _PRIME
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	while(number<3){
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
		if( divisor >= number ){
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
		if(is_prime(i)){
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
