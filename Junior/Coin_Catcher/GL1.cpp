#include<iostream>
#include<GL/glut.h>
#include"function.cpp"
using namespace std;

int main()
{
	int mode = GLUT_RGB|GLUT_DOUBLE; 
	glutInitDisplayMode(mode);
 	glutInitWindowSize(640,640);
	glutCreateWindow( "The Homework 1" );
	myinit();
	
	glutDisplayFunc( display );
	//glutReshapeFunc( resize ); 
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	gluOrtho2D(0,640,0,640);
	glutMainLoop();
	return 0;
} 
