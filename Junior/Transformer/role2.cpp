#include<cmath>
#include<iostream>
#include"glut.h"
#include"arm.cpp"
#include"leg.cpp"
using namespace std;

class Head
{
	public:
		double cen_x,cen_y,cen_z;
		double len;
		double y_axis;
		double z_axis;
		Vertex vertex[8];
		double state[4][4];
		void get(double x,double y,double z)
		{
			cen_x = x;
			cen_y = y;
			cen_z = z;
			y_axis = 0;
			z_axis = 0;
			len = 20;
			vertex[0].x = x-len; vertex[0].y = y-len; vertex[0].z = z-len; vertex[0].w = 1; 
			vertex[1].x = x+len; vertex[1].y = y-len; vertex[1].z = z-len; vertex[1].w = 1; 
			vertex[2].x = x+len; vertex[2].y = y+len; vertex[2].z = z-len; vertex[2].w = 1; 
			vertex[3].x = x-len; vertex[3].y = y+len; vertex[3].z = z-len; vertex[3].w = 1; 
			vertex[4].x = x-len; vertex[4].y = y-len; vertex[4].z = z+len; vertex[4].w = 1; 
			vertex[5].x = x+len; vertex[5].y = y-len; vertex[5].z = z+len; vertex[5].w = 1; 
			vertex[6].x = x+len; vertex[6].y = y+len; vertex[6].z = z+len; vertex[6].w = 1; 
			vertex[7].x = x-len; vertex[7].y = y+len; vertex[7].z = z+len; vertex[7].w = 1;
			for(int i = 0 ; i<4 ; i++)
			{
				for(int j = 0; j<4 ; j++)
				{
					if(i==j) state[i][j] = 1;
					else state[i][j] = 0;
				}
			} 
		}
		void display()
		{
			glBegin(GL_POLYGON);  //front
				glColor3f(0,0,1);
				for(int i = 0 ; i<4 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //back
				glColor3f(0,0,1);
				for(int i = 4 ; i<8 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //left
				glColor3f(1,1,0);
				glVertex3f(vertex[4].x,vertex[4].y,vertex[4].z);
				glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //right
				glColor3f(1,1,0);
				glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
				glVertex3f(vertex[5].x,vertex[5].y,vertex[5].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
			glEnd();
			glBegin(GL_POLYGON);  //top
				glColor3f(1,1,0);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //bottom
				glColor3f(1,1,0);
				glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
				glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
				glVertex3f(vertex[5].x,vertex[5].y,vertex[5].z);
				glVertex3f(vertex[4].x,vertex[4].y,vertex[4].z);
			glEnd();
		}
		void rotate(char cmd)
		{	
			cen_x = 0;cen_y = 0;cen_z = 0;
			double temp[4][4],I[4][4];
			for(int i =0 ; i<4 ;i++)
			{
				for(int j = 0 ; j<4 ; j++)
				{
					if(i==j) temp[i][j] = 0;
					else temp[i][j] = 1;
				}
			}
			for(int i = 0 ; i<8 ; i++)
			{
				cen_x += vertex[i].x/8;
				cen_y += vertex[i].y/8;
				cen_z += vertex[i].z/8;
			}  
			double R[4][4] = {
				{1,0,0, -cen_x},
				{0,1,0, -cen_y},
				{0,0,1,-cen_z},
				{0,0,0,1}
			};
			for(int i = 0; i<8; i++) mul(R,vertex[i]);
			inverse(state,I);
			for(int i = 0; i<8; i++) mul(I,vertex[i]);
			if(cmd == 'q')
			{
				double x = 5*3.14159/180;
				double R1 [4][4] = {{cos(x),0,sin(x),0},{0,1,0,0},{-sin(x),0,cos(x),0},{0,0,0,1}};
				for(int i = 0; i<8; i++) mul(R1,vertex[i]);
			}
			else if(cmd == 'e')
			{
				double x = 355*3.14159/180;
				double R1 [4][4] = {{cos(x),0,sin(x),0},{0,1,0,0},{-sin(x),0,cos(x),0},{0,0,0,1}};
				for(int i = 0; i<8; i++) mul(R1,vertex[i]);
			}
			for(int i = 0 ; i<8 ; i++) mul(state,vertex[i]);
			double R4[4][4] = {
				{1,0,0, cen_x},
				{0,1,0, cen_y},
				{0,0,1,cen_z},
				{0,0,0,1}
			};
			for(int i = 0; i<8; i++) mul(R4,vertex[i]);
		}
};


class Upper
{
	public:
		double cen_x,cen_y,cen_z;
		double wid_l,wid_h,height;
		double y_axis;
		double z_axis;
		double l;
		double state[4][4];
		arm left;
		arm right;
		Head head;
		Vertex vertex[8];
		void get(double x,double y,double z)
		{
			head.get(x,y+100,z);
			left.get(x-100,y+60,z,x,y,z,true);
			right.get(x+100,y+60,z,x,y,z,false);
			y_axis = 0;
			z_axis = 0;
			cen_x = x;
			cen_y = y;
			cen_z = z;
			wid_l = 40;
			wid_h = 60;
			height = 80;
			l = 50;
			vertex[0].x=cen_x-wid_l; vertex[0].y=cen_y-height; vertex[0].z=cen_z-l; vertex[0].w=1;
			vertex[1].x=cen_x+wid_l; vertex[1].y=cen_y-height; vertex[1].z=cen_z-l; vertex[1].w=1;
			vertex[2].x=cen_x+wid_h; vertex[2].y=cen_y+height; vertex[2].z=cen_z-l; vertex[2].w=1;
			vertex[3].x=cen_x-wid_h; vertex[3].y=cen_y+height; vertex[3].z=cen_z-l; vertex[3].w=1;
			vertex[4].x=cen_x-wid_l; vertex[4].y=cen_y-height; vertex[4].z=cen_z+l; vertex[4].w=1;
			vertex[5].x=cen_x+wid_l; vertex[5].y=cen_y-height; vertex[5].z=cen_z+l; vertex[5].w=1;
			vertex[6].x=cen_x+wid_h; vertex[6].y=cen_y+height; vertex[6].z=cen_z+l; vertex[6].w=1;
			vertex[7].x=cen_x-wid_h; vertex[7].y=cen_y+height; vertex[7].z=cen_z+l; vertex[7].w=1;
			for(int i = 0; i<4 ; i++)
			{
				for(int j = 0; j<4 ; j++)
				{
					if(i==j) state[i][j] = 1;
					else state[i][j] = 0;
				}
			}
		}
		void display()
		{
			head.display();
			left.display();
			right.display();
			glBegin(GL_POLYGON);  //front
				glColor3f(1,0,0);
				for(int i = 0 ; i<4 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //back
				glColor3f(1,0,0);
				for(int i = 4 ; i<8 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //left
				glColor3f(1,1,1);
				glVertex3f(vertex[4].x,vertex[4].y,vertex[4].z);
				glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //right
				glColor3f(1,1,1);
				glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
				glVertex3f(vertex[5].x,vertex[5].y,vertex[5].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
			glEnd();
			glBegin(GL_POLYGON);  //top
				glColor3f(1,1,1);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //bottom
				glColor3f(1,1,1);
				glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
				glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
				glVertex3f(vertex[5].x,vertex[5].y,vertex[5].z);
				glVertex3f(vertex[4].x,vertex[4].y,vertex[4].z);
			glEnd();
		}
		void rotate_all(double R[][4])
		{
			for(int i = 0 ;i<8; i++)
			{
				mul(R,vertex[i]);
				mul(R,head.vertex[i]);
				mul(R,left.vertex[i]);
				mul(R,right.vertex[i]);
				mul(R,left.hand.vertex[i]);
				mul(R,right.hand.vertex[i]);
			}
		}
		void load_state(double R[][4])
		{
			for(int i = 0; i<4 ; i++)
			{
				for(int j = 0 ; j<4 ; j++)
				{
					state[i][j] = R[i][j];
					head.state[i][j] = R[i][j];
					left.mstate[i][j] = R[i][j];
					right.mstate[i][j] = R[i][j];
				} 
			}
		}
		void rotate(char cmd)
		{
			//translate to the right original frame
			if(cmd == 'w' && z_axis>15) return;
			if(cmd == 's' && z_axis<-15) return;
			if(cmd == 'q' || cmd == 'e')
			{
				head.rotate(cmd);
				return;
			}
			if(cmd == 'f' || cmd == 'g' || cmd=='r' || cmd=='t' || cmd == 'z' || cmd == 'x' || cmd == '4' ||  cmd =='5')
			{
				left.rotate(cmd);
				return;
			}
			if(cmd == 'y' || cmd == 'u' || cmd=='h' || cmd=='j' || cmd == 'c' || cmd == 'v')
			{
				if(cmd == 'y') cmd = 'r';
				if(cmd == 'u') cmd = 't';
				if(cmd == 'h') cmd = 'f';
				if(cmd == 'j') cmd = 'g';
				right.rotate(cmd);
				return;
			}
			double temp[4][4];
			double temp2[4][4];
			for(int i = 0 ; i<4 ; i++)
			{
				for(int j = 0 ; j<4; j++)
				{
					if(i==j)
					{
						temp[i][j] = 1;
						temp2[i][j] = 1;
					}
					else
					{
						temp[i][j] = 0;
						temp2[i][j] = 0;
					} 
				}
			}
			double i[4][4];
			double R[4][4] = {
				{1,0,0, -cen_x},
				{0,1,0, -cen_y},
				{0,0,1,0},
				{0,0,0,1}
			};
			rotate_all(R);
			inverse(state,i);
			rotate_all(i);
			if(cmd == 'w')
			{
				z_axis += 5;
				double x = 5*3.14159/180;
				double R1 [4][4] = {{1,0,0,0},{0,cos(x),-sin(x),0},{0,sin(x),cos(x),0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);		
			}
			else if(cmd == 'd')
			{
				y_axis -= 5;
				double x = 355*3.14159/180;
				double R1 [4][4] = {{cos(x),0,sin(x),0},{0,1,0,0},{-sin(x),0,cos(x),0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);
			}
			else if(cmd == 's')
			{
				z_axis -= 5;
				double x = 355*3.14159/180;
				double R1 [4][4] = {{1,0,0,0},{0,cos(x),-sin(x),0},{0,sin(x),cos(x),0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);
			}
			else if(cmd == 'a')
			{
				y_axis += 5;
				double x = 5*3.14159/180;
				double R1 [4][4] = {{cos(x),0,sin(x),0},{0,1,0,0},{-sin(x),0,cos(x),0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);
			}
			rotate_all(state);
			double R2[4][4] = {
				{1,0,0, cen_x},
				{0,1,0, cen_y},
				{0,0,1,0},
				{0,0,0,1}
			};
			rotate_all(R2);
			load_state(temp);
		}
};
class Robot
{
	public:
		double cen_x,cen_y,cen_z;
		Upper upper;
		Leg left;
		Leg right;
		void get(double x,double y,double z)
		{
			cen_x = x;
			cen_y = y;
			cen_z = z;
			upper.get(cen_x,cen_y+100,cen_z);
			left.get(cen_x-20,cen_y-20,cen_z,cen_x,cen_y,cen_z,true);
			right.get(cen_x+20,cen_y-20,cen_z,cen_x,cen_y,cen_z,false);
		}
		void display()
		{
			upper.display();
			left.display();
			right.display();
		}
};
