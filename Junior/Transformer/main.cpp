#include"glut.h" 
#include"function.cpp"
using namespace std;
int main()
{
	int mode = GLUT_RGB|GLUT_DOUBLE;;
	glutInitDisplayMode(mode);
	glutInitWindowSize(640,640);
	glutCreateWindow("The Homework 2");
	myinit();
	
	glutDisplayFunc( display );
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glOrtho(0,640,0,640,-320,320);
	//gluOrtho2D(0,640,0,640);
	glutMainLoop();
	return 0;
} 

