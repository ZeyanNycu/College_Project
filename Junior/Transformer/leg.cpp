#include<cmath>
#include<iostream>
#include"glut.h"
using namespace std;

class Feet
{
	public:
		public:
		double cen_x,cen_y,cen_z;
		double mx,my,mz;
		double wid,height;
		double axis;
		bool isleft;
		double state[4][4];
		double mstate[4][4];
		Vertex vertex[8];
		//Hand hand;
		void get(double x,double y,double z,double x1,double y1,double z1,bool is)
		{
			cen_x = x;
			cen_y = y;
			cen_z = z;
			axis = 0;
			mx = x1;
			my = y1;
			mz = z1;
			wid = 8;
			height = 30;
			isleft = is;
			vertex[0].x=cen_x-wid; vertex[0].y=cen_y+height; vertex[0].z=cen_z-wid; vertex[0].w=1;
			vertex[1].x=cen_x+wid; vertex[1].y=cen_y+height; vertex[1].z=cen_z-wid; vertex[1].w=1;
			vertex[2].x=cen_x+wid; vertex[2].y=cen_y+height; vertex[2].z=cen_z+wid; vertex[2].w=1;
			vertex[3].x=cen_x-wid; vertex[3].y=cen_y+height; vertex[3].z=cen_z+wid; vertex[3].w=1;
			vertex[4].x=cen_x-wid; vertex[4].y=cen_y-height; vertex[4].z=cen_z-wid; vertex[4].w=1;
			vertex[5].x=cen_x+wid; vertex[5].y=cen_y-height; vertex[5].z=cen_z-wid; vertex[5].w=1;
			vertex[6].x=cen_x+wid; vertex[6].y=cen_y-height; vertex[6].z=cen_z+wid; vertex[6].w=1;
			vertex[7].x=cen_x-wid; vertex[7].y=cen_y-height; vertex[7].z=cen_z+wid; vertex[7].w=1;
			for(int i = 0; i<4 ; i++)
			{
				for(int j = 0; j<4 ; j++)
				{
					if(i==j)
					{
						state[i][j] = 1;
						mstate[i][j] = 1;
					}
					else
					{
						state[i][j] = 0;
						mstate[i][j] = 0;
					} 
				}
			}
		}
		void display()
		{
			glBegin(GL_POLYGON);  //left
				glColor3f(1,0,0);
				for(int i = 0 ; i<4 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //right
				glColor3f(1,0,0);
				for(int i = 4 ; i<8 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //front
				glColor3f(0,1,0);
				glVertex3f(vertex[4].x,vertex[4].y,vertex[4].z);
				glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //back
				glColor3f(0,1,0);
				glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
				glVertex3f(vertex[5].x,vertex[5].y,vertex[5].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
			glEnd();
			glBegin(GL_POLYGON);  //top
				glColor3f(0,1,0);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //bottom
				glColor3f(0,1,0);
				glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
				glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
				glVertex3f(vertex[5].x,vertex[5].y,vertex[5].z);
				glVertex3f(vertex[4].x,vertex[4].y,vertex[4].z);
			glEnd();
		}
		void load_state(double R[][4])
		{
			for(int i = 0; i<4 ; i++)
			{
				for(int j = 0 ; j<4 ; j++)
				{
					state[i][j] = R[i][j];
				} 
			}
		}
		void rotate(char cmd)
		{
			if(cmd == 'b' && axis>18) return;
			if(cmd == 'n' && axis<-18) return;
			double I[4][4],temp[4][4];
			double x = cen_x;
			double y = cen_y+height;
			inverse(mstate,I);
			for(int i = 0; i<8 ; i++)	go_back(I,-mx,-my,-mz,vertex[i]);
			inverse(state,I);
			double R[4][4] = {
				{1,0,0, -x},
				{0,1,0, -y},
				{0,0,1,0},
				{0,0,0,1}
			};
			for(int i = 0; i<8 ; i++)
			{
				mul(R,vertex[i]);
				mul(I,vertex[i]);
			}
			if(cmd == 'b')
			{
				axis++;
				double x = 5*3.14159/180;
				double R1 [4][4] = {{1,0,0,0},{0,cos(x),-sin(x),0},{0,sin(x),cos(x),0},{0,0,0,1}};
				for(int i = 0; i<8; i++) mul(R1,vertex[i]);
				mmul(state,R1,temp);
			}
			else if(cmd == 'n')
			{
				axis--;
				double x = 355*3.14159/180;
				double R1 [4][4] = {{1,0,0,0},{0,cos(x),-sin(x),0},{0,sin(x),cos(x),0},{0,0,0,1}};
				for(int i = 0; i<8; i++) mul(R1,vertex[i]);
				mmul(state,R1,temp);
			}
			double R1[4][4] = {
				{1,0,0, x},
				{0,1,0, y},
				{0,0,1,0},
				{0,0,0,1}
			};
			for(int i = 0; i<8 ; i++)
			{
				mul(state,vertex[i]);
				mul(R1,vertex[i]);
			}
			for(int i = 0; i<8 ; i++)	go_back(mstate,-mx,-my,-mz,vertex[i]);
			load_state(temp);
			
		}
};
class Leg
{
	public:
		double cen_x,cen_y,cen_z;
		double mx,my,mz;
		double wid,height;
		bool isleft;
		double state[4][4];
		double mstate[4][4];
		double axis;
		Vertex vertex[8];
		Feet feet;
		void get(double x,double y,double z,double x1,double y1,double z1,bool is)
		{
			cen_x = x;
			cen_y = y;
			cen_z = z;
			axis=0;
			mx = x1;
			my = y1;
			mz = z1;
			wid = 15;
			height = 50;
			if(is) feet.get(x,y-70,z,x,y+height,z,is);
			else feet.get(x,y-70,z,x,y+height,z,is);
			isleft = is;
			vertex[0].x=cen_x-wid; vertex[0].y=cen_y+height; vertex[0].z=cen_z-wid; vertex[0].w=1;
			vertex[1].x=cen_x+wid; vertex[1].y=cen_y+height; vertex[1].z=cen_z-wid; vertex[1].w=1;
			vertex[2].x=cen_x+wid; vertex[2].y=cen_y+height; vertex[2].z=cen_z+wid; vertex[2].w=1;
			vertex[3].x=cen_x-wid; vertex[3].y=cen_y+height; vertex[3].z=cen_z+wid; vertex[3].w=1;
			vertex[4].x=cen_x-wid; vertex[4].y=cen_y-height; vertex[4].z=cen_z-wid; vertex[4].w=1;
			vertex[5].x=cen_x+wid; vertex[5].y=cen_y-height; vertex[5].z=cen_z-wid; vertex[5].w=1;
			vertex[6].x=cen_x+wid; vertex[6].y=cen_y-height; vertex[6].z=cen_z+wid; vertex[6].w=1;
			vertex[7].x=cen_x-wid; vertex[7].y=cen_y-height; vertex[7].z=cen_z+wid; vertex[7].w=1;
			for(int i = 0; i<4 ; i++)
			{
				for(int j = 0; j<4 ; j++)
				{
					if(i==j)
					{
						state[i][j] = 1;
						mstate[i][j] = 1;
					}
					else
					{
						state[i][j] = 0;
						mstate[i][j] = 0;
					} 
				}
			}
		}
		void display()
		{
			feet.display();
			glBegin(GL_POLYGON);  //left
				glColor3f(1,0,0);
				for(int i = 0 ; i<4 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //right
				glColor3f(1,0,0);
				for(int i = 4 ; i<8 ; i++)
					glVertex3f(vertex[i].x,vertex[i].y,vertex[i].z);
			glEnd();
			glBegin(GL_POLYGON);  //front
				glColor3f(0,1,0);
				glVertex3f(vertex[4].x,vertex[4].y,vertex[4].z);
				glVertex3f(vertex[0].x,vertex[0].y,vertex[0].z);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //back
				glColor3f(0,1,0);
				glVertex3f(vertex[1].x,vertex[1].y,vertex[1].z);
				glVertex3f(vertex[5].x,vertex[5].y,vertex[5].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
			glEnd();
			glBegin(GL_POLYGON);  //top
				glColor3f(0,1,0);
				glVertex3f(vertex[3].x,vertex[3].y,vertex[3].z);
				glVertex3f(vertex[2].x,vertex[2].y,vertex[2].z);
				glVertex3f(vertex[6].x,vertex[6].y,vertex[6].z);
				glVertex3f(vertex[7].x,vertex[7].y,vertex[7].z);
			glEnd();
			glBegin(GL_POLYGON);  //bottom
				glColor3f(0,1,0);
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
				mul(R,feet.vertex[i]);
			}
		}
		void load_state(double R[][4])
		{
			for(int i = 0; i<4 ; i++)
			{
				for(int j = 0 ; j<4 ; j++)
				{
					state[i][j] = R[i][j];
					feet.mstate[i][j] = R[i][j];
				} 
			}
		}
		void rotate(char cmd)
		{
			if(cmd == 'i' && axis>18 || cmd == 'o' && axis<-18) return;
			double I[4][4],temp[4][4];
			double x = cen_x;
			double y = cen_y+height;
			if( (cmd == 'b' || cmd == 'n') && isleft)
			{
				feet.rotate(cmd);
				return;
			}
			if( (cmd == 'm' || cmd == ',') && !isleft)
			{
				if(cmd=='m') cmd = 'b';
				if(cmd==',') cmd = 'n';
				feet.rotate(cmd);
				return;
			}
			inverse(state,I);
			double R[4][4] = {
				{1,0,0, -x},
				{0,1,0, -y},
				{0,0,1,0},
				{0,0,0,1}
			};
			rotate_all(R);
			rotate_all(I);
			if(cmd == 'k')
			{
				double x = 5*3.14159/180;
				double R1 [4][4] = {{cos(x),0,sin(x),0},{0,1,0,0},{-sin(x),0,cos(x),0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);
			}
			else if(cmd == 'l')
			{
				double x = 355*3.14159/180;
				double R1 [4][4] = {{cos(x),0,sin(x),0},{0,1,0,0},{-sin(x),0,cos(x),0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);
			}
			else if(cmd == 'i')
			{
				axis++;
				double x = 5*3.14159/180;
				double R1 [4][4] = {{cos(x),-sin(x),0,0},{sin(x),cos(x),0,0},{0,0,1,0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);
			}
			else if(cmd =='o')
			{
				axis--;
				double x = 355*3.14159/180;
				double R1 [4][4] = {{cos(x),-sin(x),0,0},{sin(x),cos(x),0,0},{0,0,1,0},{0,0,0,1}};
				rotate_all(R1);
				mmul(state,R1,temp);
			}
			double R1[4][4] = {
				{1,0,0, x},
				{0,1,0, y},
				{0,0,1,0},
				{0,0,0,1}
			};
			rotate_all(state);
			rotate_all(R1);
			load_state(temp);
			
		}
};
