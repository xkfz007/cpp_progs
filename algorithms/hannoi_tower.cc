/*
 * hannoi_tower.cc
 *
 *  Created on: 2016-5月-23 上午1:00:25
 *      Author: Felix
 */

/*
 * 汉诺塔（又称河内塔）问题其实是印度的一个古老的传说。开 天辟地的神勃拉玛（和中国的盘古差不多的神吧）在一个庙里留下了三根金刚石的棒，第一根上面套着64个圆的金片，
 * 最大的一个在底下，其余一个比一 个小，依次叠上去，庙里的众僧不倦地把它们一个个地从这根棒搬到另一根棒上，规定可利用中间的一根棒作为帮助，但每次只能搬一个，而且大
 * 的不能放在小的上 面。计算结果非常恐怖(移动圆片的次数)18446744073709551615，众僧们即便是耗尽毕生精力也不可能完成金片的移动了。
 * 算法介绍：
 * 其 实算法非常简单，当盘子的个数为n时，移动的次数应等于2^n – 1（有兴趣的可以自己证明试试看）。后来一位美国学者发现一种出人意料的简单方法，只要轮流进行两步操作就可以了。首先把三根柱子按顺序排成品字型，把所 有的圆盘按从大到小的顺序放在柱子A上，根据圆盘的数量确定柱子的排放顺序：若n为偶数，按顺时针方向依次摆放 A B C；
 * 若n为奇数，按顺时针方向依次摆放 A C B。
 * （1）按顺时针方向把圆盘1从现在的柱子移动到下一根柱子，即当n为偶数时，若圆盘1在柱子A，则把它移动到B；若圆盘1在柱子B，则把它移动到C；若圆盘1在柱子C，则把它移动到A。
 * （2）接着，把另外两根柱子上可以移动的圆盘移动到新的柱子上。即把非空柱子上的圆盘移动到空柱子上，当两根柱子都非空时，移动较小的圆盘。这一步没有明确规定移动哪个圆盘，你可能以为会有多种可能性，其实不然，可实施的行动是唯一的。
 * （3）反复进行（1）（2）操作，最后就能按规定完成汉诺塔的移动。
 * 所以结果非常简单，就是按照移动规则向一个方向移动金片：
 * 如3阶汉诺塔的移动：A→C,A→B,C→B,A→C,B→A,B→C,A→C
 * 汉诺塔问题也是程序设计中的经典递归问题，下面我们将给出递归和非递归的不同实现源代码。
 */

#include <fstream>
#include <iostream>
using namespace std;
ofstream fout("out.txt");
void Move(int n, char x, char y) {
	fout << "把" << n << "号从" << x << "挪动到" << y << endl;
}
void Hannoi(int n, char a, char b, char c) {
	if (n == 1)
		Move(1, a, c);
	else {
		Hannoi(n - 1, a, c, b);
		Move(n, a, c);
		Hannoi(n - 1, b, a, c);
	}
}
int main() {
	fout << "以下是7层汉诺塔的解法:" << endl;
	Hannoi(7, 'a', 'b', 'c');
	fout.close();
	cout << "输出完毕！" << endl;
	return 0;
}
//#include<stdio.h>
//void hanoi(int n, char A, char B, char C) {
//	if (n == 1) {
//		printf("Move disk %d from %c to %c\n", n, A, C);
//	} else {
//		hanoi(n - 1, A, C, B);
//		printf("Move disk %d from %c to %c\n", n, A, C);
//		hanoi(n - 1, B, A, C);
//	}
//}
//int main() {
//	int n;
//	printf("请输入数字n以解决n阶汉诺塔问题：\n");
//	scanf("%d", &n);
//	hanoi(n, 'A', 'B', 'C');
//}
//
#if 0
#include <iostream>
using namespace std;

//圆盘的个数最多为64
const int MAX = 64;

//用来表示每根柱子的信息
struct st {
	int s[MAX]; //柱子上的圆盘存储情况
	int top; //栈顶，用来最上面的圆盘
	char name; //柱子的名字，可以是A，B，C中的一个
	int Top() //取栈顶元素
	{
		return s[top];
	}
	int Pop() //出栈
	{
		return s[top--];
	}
	void Push(int x) //入栈
			{
		s[++top] = x;
	}
};

long Pow(int x, int y); //计算x^y
void Creat(st ta[], int n); //给结构数组设置初值
void Hannuota(st ta[], long max); //移动汉诺塔的主要函数

int main(void) {
	int n;
	cin >> n; //输入圆盘的个数
	st ta[3]; //三根柱子的信息用结构数组存储
	Creat(ta, n); //给结构数组设置初值

	long max = Pow(2, n) - 1; //动的次数应等于2^n - 1
	Hannuota(ta, max); //移动汉诺塔的主要函数

	system("pause");
	return 0;
}
void Creat(st ta[], int n) {
	ta[0].name = 'A';
	ta[0].top = n - 1;
	//把所有的圆盘按从大到小的顺序放在柱子A上
	for (int i = 0; i < n; i++)
		ta[0].s[i] = n - i;
	//柱子B，C上开始没有没有圆盘
	ta[1].top = ta[2].top = 0;
	for (int i = 0; i < n; i++)
		ta[1].s[i] = ta[2].s[i] = 0;
	//若n为偶数，按顺时针方向依次摆放 A B C
	if (n % 2 == 0) {
		ta[1].name = 'B';
		ta[2].name = 'C';
	} else  //若n为奇数，按顺时针方向依次摆放 A C B
	{
		ta[1].name = 'C';
		ta[2].name = 'B';
	}
}
long Pow(int x, int y) {
	long sum = 1;
	for (int i = 0; i < y; i++)
		sum *= x;

	return sum;
}
void Hannuota(st ta[], long max) {
	int k = 0; //累计移动的次数
	int i = 0;
	int ch;
	while (k < max) {
		//按顺时针方向把圆盘1从现在的柱子移动到下一根柱子
		ch = ta[i % 3].Pop();
		ta[(i + 1) % 3].Push(ch);
		cout << ++k << ": " << "Move disk " << ch << " from " << ta[i % 3].name << " to "
				<< ta[(i + 1) % 3].name << endl;
		i++;
		//把另外两根柱子上可以移动的圆盘移动到新的柱子上
		if (k < max) {  //把非空柱子上的圆盘移动到空柱子上，当两根柱子都为空时，移动较小的圆盘
			if (ta[(i + 1) % 3].Top() == 0
					|| ta[(i - 1) % 3].Top() > 0 && ta[(i + 1) % 3].Top() > ta[(i - 1) % 3].Top()) {
				ch = ta[(i - 1) % 3].Pop();
				ta[(i + 1) % 3].Push(ch);
				cout << ++k << ": " << "Move disk " << ch << " from " << ta[(i - 1) % 3].name
						<< " to " << ta[(i + 1) % 3].name << endl;
			} else {
				ch = ta[(i + 1) % 3].Pop();
				ta[(i - 1) % 3].Push(ch);
				cout << ++k << ": " << "Move disk " << ch << " from " << ta[(i + 1) % 3].name
						<< " to " << ta[(i - 1) % 3].name << endl;
			}
		}
	}
}

#endif
