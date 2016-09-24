/*
 * least_common_seq.cc
 *
 *  Created on: 2016-5月-23 下午10:49:33
 *      Author: Felix
 */
//最长公共子序列
int lcseq_length(char* A,char *B,int **c,int **b){
	int lenA=strlen(A), lenB=strlen(B);
	for(int i=1;i<=lenA;i++)
		c[i][0]=0;
	for(int j=1;j<=lenB;j++)
		c[0][j]=0;
	for(int i=1;i<=lenA;i++){
		for(int j=1;j<=lenB;j++){
			if(A[i-1]==B[j-1]){
				c[i][j]=c[i-1][j-1]+1;
				b[i][j]=1;
			}
			else if(c[i-1][j]>=c[i][j-1]) {
				c[i][j]=c[i-1][j]; b[i][j]=2;
			}
			else{ c[i][j]=c[i][j-1]; b[i][j]=3;
			}
		}
	}
	return c[lenA][lenB];
}
void print_lcseq(char *A,int **c,int**b,int i,int j){
	if(i==0||j==0)
		return ;
	if(b[i][j]==1){
		print_lcseq(A,c,b,i-1,j-1);
		printf("%c",A[i-1]);
	}
	else if(b[i][j]==2){
		print_lcseq(A,c,b,i-1,j);
	} else
		print_lcseq(A,c,b,i,j-1);
}
//寻找最长公共子序列
void find_lcseq(char *A,char *B){
	int lenA=strlen(A), lenB=strlen(B);
	int **c=new int*[lenA+1];
	int **b=new int*[lenA+1];
	for(int i=0;i<=lenA;i++){
		c[i]=new int[lenB+1]();//申请空间，同时初始化为0
		b[i]=new int[lenB+1]();
	}
	//计算LCS的长度，保存在数组c中，b中信息是为了便于输出LCS，其实不需要b也可以输出LCS
	lcseq_length(A,B,c,b); //利用b输出LCS
	print_lcseq(A,c,b,lenA,lenB);
	printf("\n");
	for(int i=0;i<=lenA;i++){
		delete[] c[i];
		delete[] b[i];
	}
	delete[] c;
	delete[] b;
}

//寻找最长公共子串
void find_lcstr(char* A,char *B){
	int lenA=strlen(A), lenB=strlen(B);
	int len=0,end=-1;
	//同样利用二维数组c
	int **c=new int*[lenA+1];
	for(int i=0;i<=lenA;i++){
		c[i]=new int[lenB+1]();
	}
	//计算最长公共子串的长度，保存在c数组中
	for(int i=1;i<=lenA;i++){
		for(int j=1;j<=lenB;j++){
			if(A[i-1]==B[j-1]){
				c[i][j]=c[i-1][j-1]+1;
			}
			if(c[i][j]>len){
				len=c[i][j]; end=j;
			}
		}
	}
	int start=end-len+1;
	for(int i=start;i<=end;i++)
		printf("%c",B[i-1]); printf("\n");
	for(int i=0;i<=lenA;i++){
		delete[] c[i];
	}
	delete[] c;
}
//改进的寻找最长公共子串的方法：降低空间复杂度
void find_lcstr2(char* A,char *B){
	int lenA=strlen(A), lenB=strlen(B);
	int len=0,end=-1; //利用一维数组计算公共子串的长度
	int *c=new int[lenB]();
	for(int i=0;i<lenA;i++){
		for(int j=lenB-1;j>=0;j--){
			if(A[i]==B[j]) {
				if(j==0||i==0)
					c[j]=1;
				else c[j]=c[j-1]+1;
			} else
				c[j]=0;
			if(c[j]>len){
				len=c[j];
				end=j;
			}
		}
	}
	int start=end-len+1;
	for(int i=start;i<=end;i++)
		printf("%c",B[i]);
	delete[] c;
}

//最长递增子序列
int lis_length(int *A,int len){
	// int len=strlen(A);
	int *mem=new int[len]();
	int *indx=new int[len]();
	int count,last;
	mem[0]=1;
	indx[0]=0;
	count=1;
	last=-1;
	for(int i=1;i<len;i++){
		mem[i]=1;
		for(int j=0;j<i;j++){
			if(A[j]<A[i]&&mem[j]+1>mem[i]) {
				mem[i]=mem[j]+1;
				indx[mem[j]-1]=j;
				count=mem[j];
				last=i;
			}
		}
	}
	indx[count++]=last;
	for(int i=0;i<count;i++)
		printf("%d ",indx[i]);
	printf("\n");
	for(int i=0;i<count;i++)
		printf("%d ",A[indx[i]]);
	return 0;
}
int main(){
	int arr[]={100,-1,2,-3,4,-5,6,-7};
	int n=8; lis_length(arr,n);
}
}


