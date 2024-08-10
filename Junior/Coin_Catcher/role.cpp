#include<iostream>
#include<stdlib.h>
#include <time.h>
using namespace std;
class getter
{
	public:
		float x,y; // the leftdown coordinate
		float wid,hei; // the width & length of the getter
		float v;
		int hit;
		int health;
		getter()
		{
			health = 3;
			x = 0; y = 0;
			wid = 100; hei = 50;
			v = 16;
		}
};
class Coin
{
	public:
		int num; // number of the coins
		int* x;
		int* y;
		int* r;
		int* v_x;
		int* v_y;
		Coin()
		{
			num = 1;
			srand((unsigned)time(NULL)); 
			x = new int [num];  
			y = new int [num];  
			v_x = new int [num];
			r = new int [num];
			v_y = new int [num];
			for(int i = 0 ; i<num ; i++)
			{
				r[i] = rand()%50+10;
				x[i] = rand()%(640-r[i]);
				y[i] = 640+rand()%r[i];
				v_x[i] = 0;//rand()%10-5;
				v_y[i] = -1;//rand()%5*(-1);
			}
		}
};
