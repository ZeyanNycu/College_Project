#include"role2.cpp"
#include<iostream>
#include<math.h>
#include<string.h>
#include<windows.h>
#include<wingdi.h>
using namespace std;
double x,y,z;
char cmd;
Robot robot;
int cnt = 0;
bool change= false;
char walk[] ={'.','d','a','a','a','a','a','a','l','l','l','l','l','l','l','l','l','l','l','l','i','i','i','i','i','i','i','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','i','i','i','i','i','i','i','i','i','i','i',
'\'','\'','\'','\'','\'','\'','\'','\'','\'','\'','\'','\'','p','p','p','p','p','p','p','p','p','p','p','[',
'[','[','[','[','[','[','[','[','[','[','o','p','o','p','o','p','o','p','o','p','o','p','o','p','o','i','[','i','[','i','[','i','[','i','[','i',
'[','i','[','i','[','i','[','i','[','i','[','i','[','i','[','i','[','i','o','p','o','p','o','p','o','p','o','p','o','p','o','p',
'o','p','o','p','o','p','o','p','o','p','o','p','o','p','i','[','i','[','i','[','i','[','i','[','i','[','i','[','i','[','i','[',
'i','[','i','[','i','[','i','[','[','i','[','i','[','o','p','o','p','o','p','p','o','p','o','p','o','p','o','p','o','p','o','p',
'o','p','o','p','o','p','o','p','o','p','o','p','o','p','o','p','o','p',};
int cnt1 = 0;
bool state1 = false;
char dance[] ={'.','t','t','t','t','t','t','t','t','h','h','g','g','g','g','g','g','g','g','h','h','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','h','g','j','j','j','j','y','y','y','u','u','u','u','u','u','u','u','u','u','u','u','u','v','v','c','c','c','c','c','c','c','c','c','c','c','c','c','v','v','v','v','v','v','v','v','v','v','v','v','v','v','v','v','v','v','v','v','v','j','j','u','u','u','y','y','y','y','y','y','y','y','y','y','y','y','y','y','y','y','u','y','y','y','y','y','y','y','y','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','j','h','y','y','u','u','u','u','u','u','u','u','u','u','u','u','u','u','d','a','a','a','j','j','j','u','u','u','u','u','u','j','j','y','y','j','h','h','h','v','v','v','v','v','v','v','v','g','g','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','r','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','g','g','g','g','g','g','g','g','g','g','g','g','g','z','z','z','z','z','z','g','g','g','g','g','g','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','x','x','x','g','g','g','g','g','g','g','g','g','g','x','g','x','g','x','g','x','g','x','g','g','x','g','x','g','z','f','z','f','z','f','z','z','f','z','f','z','f','z','f','z','f','z','f','z','f','z','f','f','f','f','f','z','f','z','f','z','f','z','f','z','f','z','g','x','g','x','g','x','g','x','x','g','x','g','x','x','g','g','x','g','x','x','g','x','g','x','g','g','x','g','x','g','x','g','x','g','x','g','g','x','e','e','e','e','e','e','q','q','q','q','q','q','q','q','q','q','q','q','q','e','q','q','q','q','q','q','q','q','q','g','e','e','e','e','e','e','e','e','e','e','e','e','e','e',};
int cnt2 = 0;
bool state2 = false;
char salute[] = {'.','5','5','5','5','5','5','5','5','5','5','5','5','5','5','5','5','5','5','z','z','z','z','z','z','z','z','z'
,'z','z','z','z','z','z','t','h','g','u','f','u','f','u','f','u','f','u','u','f','u','f','u','f','u','f','u','f','u','f'
,'u','f','f','x','z','z','z','z','z','g','f','u','f','u','f','u','f','u','f','u','f','u','f','y','f','y','g','y','g','y','g','y'
,'y','g','y','g','y','g','y','g','y','g','y','g','y','g','y','g','y','y','g','y','g','y','g','g','y','g','g','y','x','x','x'
,'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','g','g','g','g','u','g','u','g','u','g'
,'u','g','u','u','g','u','g','u','g','u','g','u','g','u','u','g','u','g','u','g','u','g','u','g','u','g','u','u'};
//char salute[] = {'.','z','z','z','z','z','z','z','z','z','v','v','v','v','v','v','c','c','c','c','c','c','c','c','c','c','c','c','c','c','c','c','c','c','c','z','z','s','s','s','s','w','w','w','w','w','w','w','w','s','w','w','r','t','t','t','t','t','t','t','y','y','y','y','y','u','u','u','u','j','h','h','t','h','u','u','u','u','u','u','u','u','u','u','u','j','h','g','h','f','h','f','h','f','h','f','h','h','f','h','f','f','h','f','h','f','h','h','f','f','h','f','h','f','h','f','h','f','h','f','h','f','h','f','h','f','h','f','h','f','h','f','j','j','f','j','f','j','g','j','g','j','g','j','g','j','j','g','j','g','j','g','j','g','j','g','j','g','j','j','g','j','g','j','g','j','g','j','j','g','j','g','j','g','j','g','j','j','g','g','j','j','g','g','j','u','y','y','r','y','r','y','r','y','y','r','y','r','y','r','y','r','y','r','r','y','x','x','u','x','z','x','x','v','x','x','v','x','v','v','x','v','x','v','x','v','x','v','x','v','x','v','x','v','x','v','r','j','u','u','r','r','u','r','u','u','r','u','r','u','r','u','r','u','r','u','r','u','u','r','u','r','u','r','u','r','u','r','u','w','s','s','s','s','s'};

int cnt3 = 0;
bool state3 = false;
void myinit()
{
	x = 320;
	y = 320;
	z = 0;
	robot.get(x,y,z);
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClearDepth( 1.0 );
	//glEnable( GL_LIGHT0 );
	//glEnable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );
}
void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                // Reset the model-view matrix              // Begin drawing the color cube with 6 quads
    robot.display();
	glutSwapBuffers();
}
void option(unsigned char w)
{
	switch (w)
	{
		case 'a':
		case 'd':
		case 'w':
		case 's':
		case 'q':
		case 'e':
		case 'f':
		case 'g':
		case 'r':
		case 't':
		case 'y':
		case 'u':
		case 'h':
		case 'j':
		case 'z':
		case 'x':
		case 'c':
		case 'v':
		case '4':
		case '5':
			robot.upper.rotate(w);
			change = true;
			break;
		case 'k':
		case 'l':
		case 'i':
		case 'o':
		case 'b':
		case 'n':
			robot.left.rotate(w);
			change = true;
			break;
		case ';':
		case '\'':
		case 'p' :
		case '[':
		case 'm':
		case ',':
			if(w==';') w = 'k';
			if(w=='\'') w = 'l';
			if(w=='p') w = 'i';
			if(w=='[') w = 'o';
			robot.right.rotate(w);
			change = true;
			break;
		case '.':
			robot.get(320,320,0);
			change = true;
			break;
		case '1':
			cnt1 = 0;
			break;
	}
}
void idle()
{
	cnt++;
	if(change) 
	{
		glFlush();
		glutPostRedisplay();
		change = false;
	} 
	else if(state1 && cnt%200000 == 0)
	{
		option(walk[cnt1]);
		cnt1++;
		int size = sizeof(walk)/sizeof(walk[0]);
		cnt1 = cnt1 % size;
		change = true;
	}
	else if(state2 && cnt%200000 == 0)
	{
		option(dance[cnt2]);
		cnt2++;
		int size = sizeof(dance)/sizeof(dance[0]);
		cnt2 = cnt2 % size;
		change = true;
	}
	else if(state3 && cnt%200000 == 0)
	{
		option(salute[cnt3]);
		cnt3++;
		int size = sizeof(salute)/sizeof(salute[0]);
		cnt3 = cnt3 % size;
		change = true;
	}
}

void key(unsigned char w,int x,int y)
{
	switch (w)
	{
		case 'a':
		case 'd':
		case 'w':
		case 's':
		case 'q':
		case 'e':
		case 'f':
		case 'g':
		case 'r':
		case 't':
		case 'y':
		case 'u':
		case 'h':
		case 'j':
		case 'z':
		case 'x':
		case 'c':
		case 'v':
		case '4':
		case '5':
			state1 = false;
			state2 = false;
			state3 = false;
			robot.upper.rotate(w);
			change = true;
			break;
		case 'k':
		case 'l':
		case 'i':
		case 'o':
		case 'b':
		case 'n':
			state1 = false;
			state2 = false;
			state3 = false;
			robot.left.rotate(w);
			change = true;
			break;
		case ';':
		case '\'':
		case 'p' :
		case '[':
		case 'm':
		case ',':
			if(w==';') w = 'k';
			if(w=='\'') w = 'l';
			if(w=='p') w = 'i';
			if(w=='[') w = 'o';
			state1 = false;
			state2 = false;
			state3 = false;
			robot.right.rotate(w);
			change = true;
			break;
		case '.':
			state1 = false;
			state2 = false;
			state3 = false;
			robot.get(320,320,0);
			change = true;
			break;
		case '1':
			state1 = true;
			state2 = false;
			state3 = false;
			cnt1 = 0;
			break;
		case '2':
			state1 = false;
			state2 = true;
			state3 = false;
			cnt2 = 0;
			break;
		case '3':
			state1 = false;
			state2 = false;
			state3 = true;
			cnt3 = 0;
			break;
	}
}
