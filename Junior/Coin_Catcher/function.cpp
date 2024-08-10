#include<GL/glut.h>
#include"role.cpp"
#include<iostream>
#include<math.h>
#include<string.h>
#include<windows.h>
#include<wingdi.h>
using namespace std;
getter g;
Coin coins;
unsigned int clicks = 0;
int level = 1;
bool sign = false;
string s;
string login1 = "Welcome to this game";
string login2 = "Press any things and the game will start";
string login3 = "Press a to move left, press d to move right";
string level1 = "level1";
string level2 = "level2";
string level3 = "level3";
int moon_x = 500;
int moon_y = 500;
bool istate = false;
int a= 640,b = 640;
void add(string& s)
{
	for(int i = 0 ; i<s.length() ; i++)
	{
		if(s[i] == '9')
		{
			s[i] = '0';
			continue;
		}
		s[i]++;
		break;
	}
	return;
}
void bowel()
{
	glColor3f(1,1,1);
	glRectf(g.x, g.y, g.x+g.wid, g.y+g.hei);
	glBegin(GL_POLYGON);
	glColor3f(1,1,1);
	int n = 30;
	int x = g.x+50;
	int y = g.y+25;
	for(int j = 0 ; j<n ; j++) glVertex2f(x+58*cos(0.84*3.141+j*(0.4*3.14159/n)),y+58*sin(0.84*3.141+j*(0.4*3.14159/n)));
	
	for(int j = 0 ; j<n ; j++) glVertex2f(x+58*cos(1.70477*3.14159+j*(0.5*3.14159/n)),y+58*sin(1.70477*3.14159+j*(0.5*3.14159/n)));
	
	//for(int j = 0 ; j<n ; j++) glVertex2f(x+25*sqrt(5)*cos(1.70477*3.14159+j*(0.295*3.14159/n)/2),y+25*sqrt(5)*sin(1.70477*3.14159+j*(0.295*3.14159/n)/2));
	glEnd();
	glColor3f(1,0,0);
	glRectf(g.x+10, g.y+10, g.x+g.wid-10, g.y+g.hei-10);
	glBegin(GL_POLYGON);
	glColor3f(1,0,0);
	n = 30;
	x = g.x+50;
	y = g.y+25;
	for(int j = 0 ; j<n ; j++) glVertex2f(x+55*cos(0.84*3.14159+j*(0.4*3.14159/n)),y+55*sin(0.84*3.14159+j*(0.4*3.14159/n)));
	
	for(int j = 0 ; j<n ; j++) glVertex2f(x+55*cos(1.70477*3.14159+j*(0.5*3.14159/n)),y+55*sin(1.70477*3.14159+j*(0.5*3.14159/n)));
	
	//for(int j = 0 ; j<n ; j++) glVertex2f(x+25*sqrt(5)*cos(1.70477*3.14159+j*(0.295*3.14159/n)/2),y+25*sqrt(5)*sin(1.70477*3.14159+j*(0.295*3.14159/n)/2));
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(1,1,1);
	n = 30;
	x = g.x+50;
	y = g.y+53;
	for(int j = 0 ; j<n ; j++) glVertex2f(x+30*cos(3.142*j/n+0.1),y+30*sin(3.142*j/n+0.1));
	//for(int j = 0 ; j<n ; j++) glVertex2f(x+25*sqrt(5)*cos(1.70477*3.14159+j*(0.295*3.14159/n)/2),y+25*sqrt(5)*sin(1.70477*3.14159+j*(0.295*3.14159/n)/2));
	glEnd();
	glBegin(GL_POLYGON);
	glColor3f(0,0,0);
	n = 30;
	x = g.x+50;
	y = g.y+53;
	for(int j = 0 ; j<n ; j++) glVertex2f(x+25*cos(3.142*j/n+0.1),y+25*sin(3.142*j/n+0.1));	
	//for(int j = 0 ; j<n ; j++) glVertex2f(x+25*sqrt(5)*cos(1.70477*3.14159+j*(0.295*3.14159/n)/2),y+25*sqrt(5)*sin(1.70477*3.14159+j*(0.295*3.14159/n)/2));
	glEnd();
}
void background()
{
	glBegin(GL_POLYGON);
		glColor3f(0.8,0.6,0);
		int n = 30;
		int x = 500;
		int y = 500;
		for(int j = 0 ; j<n ; j++) glVertex2f(x+50*cos(2*3.14159*j/n),y+50*sin(2*3.14159*j/n));
	glEnd();
	glBegin(GL_POLYGON);
		glColor3f(0,0,0);
		n = 30;
		x = moon_x;
		y = moon_y;
		for(int j = 0 ; j<n ; j++) glVertex2f(x+50*cos(2*3.14159*j/n),y+50*sin(2*3.14159*j/n));
	glEnd();
	/*glBegin(GL_POLYGON);
		glColor3f(0.8,0.6,0);
		n = 30;
		x = 150;
		y = 250;
		for(int j = 0 ; j<n ; j++) glVertex2f(x+50*cos(2*3.14159*j/n),y+50*sin(2*3.14159*j/n));
	glEnd();*/
		n = 6;
		int xx[] = {150,480};
		int yy[] = {250,320};
		float rr1[] = {0,0.9};
		float gg1[] = {1,0.9};
		float bb1[] = {1,0};
		float rr2[] = {1,1};
		float gg2[] = {0,1};
		float bb2[] = {1,1};
		for(int i = 0; i<2 ; i++)
		{
			x = xx[i];
			y = yy[i];
			for(int j = 0 ; j<n ; j++)
			{
				glBegin(GL_LINE_STRIP);
					glColor3f(rr1[i],gg1[i],bb1[i]);
					int str = 5;
					for(int kk = 0; kk<5 ; kk++)
					{
						float k = kk+1;
						float a = 2*3.14159*j/n;
						if(a>=3.14159/8 && a<= 3.14159*3/8 && j!=1)	glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n),y+(str+k*20)*sin(2*3.14159*j/n));
						else if(2*3.14159*j/n>=3.14159/2 && 2*3.14159*j/n<=3.14159*3/2) glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.1*k),y+(str+k*20)*sin(2*3.14159*j/n+0.1*k));
						else  glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n-0.1*k),y+(str+k*20)*sin(2*3.14159*j/n-0.1*k));
					}
				glEnd();
			} 
			for(int j = 0 ; j<n ; j++)
			{
				glBegin(GL_LINE_STRIP);
					glColor3f(rr1[i],gg1[i],bb1[i]);
					int str = 5;
					for(int kk = 0; kk<5 ; kk++)
					{
						float k = kk+1;
						float a = 2*3.14159*j/n;
						if(a>=3.14159/8 && a<= 3.14159*3/8 && j!=1)	glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.05),y+(str+k*20)*sin(2*3.14159*j/n+0.05));
						else if(2*3.14159*j/n>=3.14159/2 && 2*3.14159*j/n<=3.14159*3/2) glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.1*k+0.05),y+(str+k*20)*sin(2*3.14159*j/n+0.1*k+0.05));
						else glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n-0.1*k+0.05),y+(str+k*20)*sin(2*3.14159*j/n-0.1*k+0.05));
					}
				glEnd();
			} 
			for(int j = 0 ; j<n ; j++)
			{
				if(j==4) continue;
				glBegin(GL_LINE_STRIP);
					glColor3f(rr2[i],gg2[i],bb2[i]);
					int str = 5;
					for(int kk = 0; kk<5 ; kk++)
					{
						float a = 2*3.14159*j/n;
						float k = kk+1;
						if(a>=3.14159/8 && a<= 3.14159*3/8) glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.5),y+(str+k*20)*sin(2*3.14159*j/n+0.5));
						else if(a>=3.14159/2 && a<=3.14159*3/2)glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.5+0.1*k),y+(str+k*20)*sin(2*3.14159*j/n+0.5+0.1*k));
						else 	glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.5-0.1*k),y+(str+k*20)*sin(2*3.14159*j/n+0.5-0.1*k));
					}
				glEnd();
			} 
			for(int j = 0 ; j<n ; j++)
			{
				if(j==4) continue;
				glBegin(GL_LINE_STRIP);
				glColor3f(rr2[i],gg2[i],bb2[i]);
				int str = 5;
					for(int kk = 0; kk<5 ; kk++)
					{
						float k = kk+1;
						float a = 2*3.14159*j/n;
						if(a>=3.14159/8 && a<= 3.14159*3/8)glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.55),y+(str+k*20)*sin(2*3.14159*j/n+0.55));
						else if(a>=3.14159/2 && a<=3.14159*3/2) glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.55+0.1*k),y+(str+k*20)*sin(2*3.14159*j/n+0.55+0.1*k));
						else glVertex2f(x+(str+k*20)*cos(2*3.14159*j/n+0.55-0.1*k),y+(str+k*20)*sin(2*3.14159*j/n+0.55-0.1*k));
					}
				glEnd();
			} 	
		}
		
		
}
void myinit()
{
	s = "0000000";
	g.hit = 0;
	level = 1;
	sign = false;
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClearDepth( 0.0 );
	//glEnable( GL_LIGHT0 );
	//glEnable( GL_LIGHTING );
	//glEnable( GL_DEPTH_TEST );

	glutPostRedisplay();
}
void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT );
	if(istate)
	{
		glRasterPos2f(20.0f,620.0f);
		//glColor3f(0,0,0);
		if(level == 1)
		{
			glColor3f(1,1,1);
			for(int i = 0; i<level1.length() ; i++)	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,level1[i]);
		}
		else if(level == 2)
		{
			glColor3f(1,1,1);
			for(int i = 0; i<level2.length() ; i++)	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,level2[i]);
		}
		else
		{
			glColor3f(1,1,1);
			for(int i = 0; i<level3.length() ; i++)	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,level3[i]);
		}
		glColor3f(0,1,0);
		for(int i = 0; i<g.health ; i++)	glRectf(550+27*i+3,612,550+27*(i+1),633);
		background();
		bowel();
		for(int i = 0 ; i<coins.num ; i++)
		{
			glBegin(GL_POLYGON);
			int n = 20;
			glColor3f(1,1,0);
			for(int j = 0 ; j<n ; j++) glVertex2f(coins.x[i]+coins.r[i]*cos(2*3.14159*j/n),coins.y[i]+coins.r[i]*sin(2*3.14159*j/n));
			glEnd();
		}
		// show the score of the game
		glColor3f(1.0f, 1.0f,1.0f);
		glRasterPos2f(270.0f,620.0f);
		//show the score of the game
		for(int i = s.length()-1; i>=0 ; i--)	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,s[i]);
		//draw the outline of the health
		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f,1.0f);
			glVertex2f(550,610);
			glVertex2f(634,610);
			glVertex2f(634,635);
			glVertex2f(550,635);
		glEnd();
		//draw the rectangle in the health
		//cout << g.health << endl;
	}
	else
	{
		//show the text on the first game
		//myinit();
		//show the outline of the intriduction
		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f,1.0f);
			glVertex2f(58,100);
			glVertex2f(570,100);
			glVertex2f(570,600);
			glVertex2f(58,600);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f,1.0f);
			glVertex2f(68,110);
			glVertex2f(560,110);
			glVertex2f(560,590);
			glVertex2f(68,590);
		glEnd();
		glColor3f(1,1,1);
		glRasterPos2f(195.0f,400.0f);
		for(int i = 0; i<login1.length() ; i++)	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,login1[i]);
		glColor3f(1.0f, 1.0f,1.0f);
		glRasterPos2f(115.0f,360.0f);
		for(int i = 0; i<login2.length() ; i++)	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,login2[i]);
		glColor3f(1.0f, 1.0f,1.0f);
		glRasterPos2f(105.0f,320.0f);
		for(int i = 0; i<login3.length() ; i++)	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,login3[i]);
	}
	glFlush();
	glutSwapBuffers();
}
void idle()
{
	//g.x += g.v;
	if(istate)
	{
		if(g.x >540 ) g.x = 540;
		if(g.x <0 ) g.x = 0;
		clicks++;
		//check if the coins is the out-of-the-bound and move the coins
		if(clicks%10 == 0)
		{
			if(clicks%500 ==0)
			{
				moon_x--;
				if(moon_x<400) moon_x = 500;
			}
			for(int i = 0 ; i<coins.num ; i++)
			{
				//coins.x[i] += coins.v_x[i];
				coins.y[i] += coins.v_y[i];
				if(coins.x[i] > 640-coins.r[i] || coins.y[i] < coins.r[i] || coins.x[i] < coins.r[i])
				{
					if(coins.y[i] < coins.r[i]) g.health--;
					coins.r[i] = rand()%30+10;
					coins.x[i] = rand()%(640-coins.r[i]);
					coins.y[i] = 640+rand()%coins.r[i];
					if(level == 3) coins.v_x[i] = rand()%10-5;
					else coins.v_x[i] = 0;//rand()%10-5;
					if(level == 1) coins.v_y[i] = -1;//(rand()%2+1)*(-1);
					else if(level == 2) coins.v_y[i] = -2;
					else if(level == 3) coins.v_y[i] = (rand()%2+1)*(-1);
				}
				if(g.health == 0)
				{
					g.health = 3;
					istate = false;
					myinit();
					cout << "You are died!!" << endl;
				}
			} 
		}
		//chek if the collision happens
		for(int i = 0 ; i<coins.num ; i++)
		{
			int r = coins.r[i];
			if( ((coins.x[i]-r>=g.x) && (coins.x[i]-r<=g.x+g.wid)) || ((coins.x[i]+r>=g.x) && (coins.x[i]+r<=g.x+g.x+g.wid)))
			{
				if( ((coins.y[i]-r>=0) && (coins.y[i]-r<=g.hei)) || ((coins.y[i]+r>=0) && (coins.y[i]+r<=g.hei)) )
				{
					add(s);
					g.hit++;
					coins.r[i] = rand()%30+10;
					coins.x[i] = rand()%(640-coins.r[i]);
					coins.y[i] = 640+rand()%coins.r[i];
					if(level == 3) coins.v_x[i] = rand()%3+2;
					else coins.v_x[i] = 0;//rand()%10-5;
					if(level == 1) coins.v_y[i] = -1;//(rand()%2+1)*(-1);
					else if(level == 2) coins.v_y[i] = -2;
					else if(level == 3) coins.v_y[i] = (rand()%2+1)*(-1);
					if(g.hit == 10)
					{
						cout << "You are in level " << level << endl;
						g.hit = 0;
						level++;
						if(level>3) level = 3;
					}	
					char bell = 7;
					cout << bell <<  endl;
				} 
			}
		}	
	}
	
	//show the health of the getter
	
	
	glutPostRedisplay();
}
void key(unsigned char w,int x,int y)
{
	switch (w)
	{
		case 'a':
			istate = true;
			g.x -= g.v;
			break;
		case 'd':
			istate = true;
			g.x += g.v;
			break;
		default:
			istate = true;
			break; 
	}
}
