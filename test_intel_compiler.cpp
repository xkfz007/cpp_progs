#if 0
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
const int i_max_cuwidth=64;
const int LOWRES_MB_WIDTH=16;
const int i_mb_stride=120;
const int i_mb_height=68;
const int i_mb_width=120;
int test_for_loop1(int ctu_x,int ctu_y){
	const int noOfBlocks = i_max_cuwidth / LOWRES_MB_WIDTH; 
	const int i_mb_x = ctu_x * noOfBlocks;
	const int i_mb_y = ctu_y * noOfBlocks;
	srand((unsigned)time(NULL));

	double qp_offset = 0;
	int cnt = 0, idx = i_mb_x + i_mb_y * i_mb_stride;
	for(int j = 0,jj=i_mb_y; j < noOfBlocks && jj < i_mb_height; j++) {
		for(int i = 0,ii=i_mb_x; i < noOfBlocks && ii < i_mb_width; i++) {
			qp_offset += rand()%100;//qpoffs[idx + i];
			cnt++;
			ii++;
		}
		idx += i_mb_stride;
		jj++;
	}
	qp_offset /= cnt;
	return cnt;
}
int test_for_loop2(int ctu_x,int ctu_y){
	const int noOfBlocks = i_max_cuwidth / LOWRES_MB_WIDTH; 
	const int i_mb_x = ctu_x * noOfBlocks;
	const int i_mb_y = ctu_y * noOfBlocks;
	srand((unsigned)time(NULL));

	double qp_offset = 0;
	int cnt = 0, idx = i_mb_x + i_mb_y * i_mb_stride;
	int i, j, ii, jj;
	jj = i_mb_y;
	for(j = 0; j < noOfBlocks && jj < i_mb_height; j++) {
		ii = i_mb_x;
		for(i = 0; i < noOfBlocks && ii < i_mb_width; i++) {
			qp_offset += rand()%100;//qpoffs[idx + i];
			cnt++;
			ii++;
		}
		idx += i_mb_stride;
		jj++;
	}
	qp_offset /= cnt;
	//tmp_qp += qp_offset;
	return cnt;
}
int test_while_loop(int ctu_x,int ctu_y){
	const int noOfBlocks = i_max_cuwidth / LOWRES_MB_WIDTH; 
	const int i_mb_x = ctu_x * noOfBlocks;
	const int i_mb_y = ctu_y * noOfBlocks;
	srand((unsigned)time(NULL));

	double qp_offset = 0;
	int cnt = 0, idx = i_mb_x + i_mb_y * i_mb_stride;
	int j=0,jj=i_mb_y;
	while(j < noOfBlocks && jj < i_mb_height){
		int i = 0,ii=i_mb_x;
		while(i < noOfBlocks && ii < i_mb_width){
			qp_offset += rand()%100;//qpoffs[idx + i];
			cnt++;
			i++;
			ii++;
		}
		idx += i_mb_stride;
		j++;
		jj++;
	}
	qp_offset /= cnt;
	return cnt;
}
int main(){
	volatile int ctu_x=2;
	volatile int ctu_y=2;
	for(int i=0;i<10000000;i++){
	int cnt1=test_for_loop1(ctu_x,ctu_y);
	int cnt2=test_while_loop(ctu_x,ctu_y);
	if(cnt1!=cnt2)
		printf("i=%4d cnt1=%5d cnt2=%5d\n",i,cnt1,cnt2);
	}

}
#endif