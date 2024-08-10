#include "glut.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
using namespace std;
const struct OBJ_COLOR {
    GLfloat red, green, blue;
    OBJ_COLOR() : red(1.0), green(1.0), blue(1.0) {}
} OBJ_COLOR;

typedef struct vertex {
    double x, y, z;
} vertex;
typedef struct face_triangle {
    unsigned long v1, v2, v3;
} face_triangle;
typedef struct face_quad {
    unsigned long v1, v2, v3, v4;
} face_quad;

std::vector<vertex> vertices;
std::vector<face_quad> faces_quads;
std::vector<face_triangle> faces_triangles;
bool initial_state = true;
bool is_quad;
bool render_mode; // true = solid body, false = wireframe
bool sign = false;
GLfloat m[16];//={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
GLfloat pre[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};;
const float ZOOM_SPEED = 100.0f;
const float ROTATE_SPEED = 0.1f;
float       DISTANCE = 310.0f;
int height_x=720;
int height_y=720;
double old_x=0;
double old_y=0;
double old_z=0;
double pos_x=0;
double pos_y=0;
double max_x = -99999;
double max_y = -99999;
double max_z = -99999;
double avg_x = 0;
double avg_y = 0;
double avg_z = 0;
float angle=0;
int cnt = 0;
struct camera {
    GLfloat x, y, z, phi, theta;
    camera() : x(-4.0f), y(2.0f), z(10.0f), phi(0), theta(0) {}
} camera;
class coor {
	public:
		GLfloat x;
		GLfloat y;
		GLfloat z;
		void set(GLfloat x,GLfloat y,GLfloat z)
		{
			this->x = x;
			this->y = y;
			this->z = (z<1.0?sqrt(1.0-z):0);
		}
		coor cross(coor s)
		{
			coor w;
			w.x = y*s.z-z*s.y;
			w.y = z*s.x-x*s.z;
			w.z = x*s.y-y*s.x;
			return w;
		}
		GLfloat value()
		{
			return sqrt(x*x+y*y+z*z);
		}
};
coor u;
coor v;
coor w;
coor pre_w;
float pre_angle;
void init() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_COLOR);
    glEnable(GL_COLOR_MATERIAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT1);
    GLfloat lightAmbient1[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat lightPos1[4] = {0.5, 0.5, 0.5, 1.0};
    GLfloat lightDiffuse1[4] = {0.8, 0.8, 0.8, 1.0};
    GLfloat lightSpec1[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat lightLinAtten = 0.0f;
    GLfloat lightQuadAtten = 0.0f;
    glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat *) &lightPos1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat *) &lightAmbient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat *) &lightDiffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, (GLfloat *) &lightSpec1);
    glLightfv(GL_LIGHT1, GL_LINEAR_ATTENUATION, &lightLinAtten);
    glLightfv(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, &lightQuadAtten);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    pre_w.set(0,0,0);
    pre_angle=0;
}

void calculate_normal(face_triangle f, GLdouble *normal) {
    // x
    normal[0] = (vertices[f.v2 - 1].y - vertices[f.v1 - 1].y) * (vertices[f.v3 - 1].z - vertices[f.v1 - 1].z)
                - (vertices[f.v3 - 1].y - vertices[f.v1 - 1].y) * (vertices[f.v2 - 1].z - vertices[f.v1 - 1].z);
    // y
    normal[1] = (vertices[f.v2 - 1].z - vertices[f.v1 - 1].z) * (vertices[f.v3 - 1].x - vertices[f.v1 - 1].x)
                - (vertices[f.v2 - 1].x - vertices[f.v1 - 1].x) * (vertices[f.v3 - 1].z - vertices[f.v1 - 1].z);
    // z
    normal[2] = (vertices[f.v2 - 1].x - vertices[f.v1 - 1].x) * (vertices[f.v3 - 1].y - vertices[f.v1 - 1].y)
                - (vertices[f.v3 - 1].x - vertices[f.v1 - 1].x) * (vertices[f.v2 - 1].y - vertices[f.v1 - 1].y);
}

void draw_obj() {
    if (render_mode){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (is_quad) {
        for (face_quad f : faces_quads) {
            // Calculate normal by calculating two tri normals & averaging
            face_triangle f_tri1 = {f.v1, f.v2, f.v3};
            face_triangle f_tri2 = {f.v2, f.v3, f.v4};
            GLdouble normal_tri1[3];
            GLdouble normal_tri2[3];
            calculate_normal(f_tri1, normal_tri1);
            calculate_normal(f_tri2, normal_tri2);
            GLdouble normal[3] = {
                    (normal_tri1[0] + normal_tri2[0]) / 2,
                    (normal_tri1[1] + normal_tri2[1]) / 2,
                    (normal_tri1[2] + normal_tri2[2]) / 2
            };

            glBegin(GL_QUADS);
            glColor3f(OBJ_COLOR.red, OBJ_COLOR.green, OBJ_COLOR.blue);
            glNormal3dv(normal);
            glVertex3d(vertices[f.v1 - 1].x, vertices[f.v1 - 1].y, vertices[f.v1 - 1].z);
            glVertex3d(vertices[f.v2 - 1].x, vertices[f.v2 - 1].y, vertices[f.v2 - 1].z);
            glVertex3d(vertices[f.v3 - 1].x, vertices[f.v3 - 1].y, vertices[f.v3 - 1].z);
            glVertex3d(vertices[f.v4 - 1].x, vertices[f.v4 - 1].y, vertices[f.v4 - 1].z);
            glEnd();
        }
    } else {
        for (face_triangle f : faces_triangles) {
            GLdouble normal[3];
            calculate_normal(f, normal);
            
            glBegin(GL_TRIANGLES);
            glNormal3dv(normal);
            glVertex3d(vertices[f.v1 - 1].x, vertices[f.v1 - 1].y, vertices[f.v1 - 1].z);
            glVertex3d(vertices[f.v2 - 1].x, vertices[f.v2 - 1].y, vertices[f.v2 - 1].z);
            glVertex3d(vertices[f.v3 - 1].x, vertices[f.v3 - 1].y, vertices[f.v3 - 1].z);
            glEnd();
        }
    }
    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (h == 0) {
        gluPerspective(80, (float) w, 1.0, 5000.0);
    } else {
        gluPerspective(80, (float) w / (float) h, 1.0, 5000.0);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void arrow_keys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: {
            DISTANCE -= ZOOM_SPEED;
            break;
        }
        case GLUT_KEY_DOWN: {
            DISTANCE += ZOOM_SPEED;
            break;
        }
        case GLUT_KEY_LEFT: {
            camera.theta -= ROTATE_SPEED;
            break;
        }
        case GLUT_KEY_RIGHT:
            camera.theta += ROTATE_SPEED;
            break;
        default:
            break;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's':
        	break;
        default:
            break;
    }
}

void Rotate_object(float angle)
{
	GLfloat tmp[16];
	GLfloat tmp1[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glLoadIdentity();
	glRotatef(angle*180.0*0.5, w.x, w.y, w.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, tmp);
	glLoadIdentity();
	glMultMatrixf(m);
	glMultMatrixf(tmp);
	glMultMatrixf(pre);
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float a = 1/(max_x*2);
    float b = 1/(max_y*2);
    float c = 1/(max_z*2);
    glScalef(a,b,c);
    glTranslatef(-avg_x/vertices.size(),-avg_y/(vertices.size()),-avg_z/vertices.size());
	//glLoadMatrixf(m);
	camera.x =  0;
    camera.y =  0;
    camera.z =  DISTANCE;
    gluLookAt(camera.x, camera.y, camera.z, 0,0,0, 0.0f, 1.0f, 0.0f);
    Rotate_object(angle);
    draw_obj();
    glutSwapBuffers();
    glutPostRedisplay();
}

void obj_parse(char *file_line) {

    if (file_line[0] == '\0') {
        // End of file
        return;
    }

    char *tokenized;
    tokenized = strtok(file_line, " ");
    char first_char = tokenized[0];

    if (first_char == '#') {
        // Comment line, ignore
        return;
    } else if (first_char == 'v') {
        // Vertex

        double x = strtod(strtok(NULL, " "), NULL);
        double y = strtod(strtok(NULL, " "), NULL);
        double z = strtod(strtok(NULL, " "), NULL);
        max_x = std::max(std::abs(x),max_x);
        max_y = std::max(std::abs(y),max_y);
        max_z = std::max(std::abs(z),max_z);
        //std::cout << x << y << z << "\n";
        vertex v = {x , y , z };
        avg_x += x;
        avg_y += y;
        avg_z += z;
        vertices.push_back(v);
    } else if (first_char == 'f') {
        // Face
	
        unsigned long v1 = strtoul(strtok(NULL, " "), NULL, 0);
        unsigned long v2 = strtoul(strtok(NULL, " "), NULL, 0);
        unsigned long v3 = strtoul(strtok(NULL, " "), NULL, 0);
        unsigned long v4;
        char *v4_str = strtok(NULL, " ");
        if (v4_str != NULL) {
            // Face is a quad
            v4 = strtoul(v4_str, NULL, 0);

            face_quad f = {v1, v2, v3, v4};
            faces_quads.push_back(f);
        } else {
            // Face is a triangle
            face_triangle f = {v1, v2, v3};
            faces_triangles.push_back(f);
        }
    }
}
void ToSphere(GLint theX,GLint theY,coor& s)//height_x height_y
{
	GLfloat aX = (theX - 0.5*height_x)*2/height_x;
	GLfloat aY = (0.5*height_y - theY)*2/height_y;
	
	GLfloat asinX = aX;
	GLfloat asinY = aY;
	GLfloat aSxy2 = aX * aX + aY * aY;
	s.set(aX,aY,aSxy2);
}
void mouse(GLint theButton,GLint theState,GLint theX,GLint theY)
{
	ToSphere(theX,theY,u);
	if(theState == GLUT_UP)
	{
		sign = true;
	}
	glutPostRedisplay();
}
void idle()
{
	if(sign)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(angle*180.0*0.5, w.x, w.y, w.z);
		glMultMatrixf(pre);
		glPushMatrix();
		glGetFloatv(GL_MODELVIEW_MATRIX, pre);
		w.x = 0;
		w.y = 0;
		w.z = 0;
		angle = 0;
		sign = false;
		glutSwapBuffers();
    	glutPostRedisplay();
	}
}
void motion (int x,int y)
{
	
	ToSphere(x,y,v);
	w = u.cross(v);
	GLfloat aAng = w.value()/(u.value())*(v.value());
	angle=std::asin(aAng);
	
	glutSwapBuffers();
    glutPostRedisplay();
}
void open_obj(int argc) {

    //// Argument parsing ////

    if (argc < 2) {
        // No object name passed in, show help & quit
        std::cout << "Usage:" << std::endl;
        std::cout  << " <.obj filename>" << std::endl;
        exit(1);
    }

    std::string filename = "Dino.obj";
    std::regex obj_regex(".+(\\.obj)$");
    //std::cout << obj_regex << std::endl;
    /*if (!std::regex_match(filename, obj_regex)) {
        // Filename is invalid
        std::cout << "This application only accepts *.obj files, exiting..." << std::endl;
        exit(1);
    }*/

    //// Filename accepted, attempting to open ////

    std::cout << "Opening file: " << filename << std::endl;

    std::ifstream file_stream;
    file_stream.open(filename, std::ifstream::in);

    if ((file_stream.rdstate() & std::ifstream::failbit) != 0) {
        std::cerr << "Error opening " << filename << std::endl;
        exit(1);
    }

    while (file_stream.good()) {
        char file_line[100];
        file_stream.getline(file_line, 100);
        obj_parse(file_line);
    }

    file_stream.close();

    is_quad = (faces_quads.size() > faces_triangles.size());
}

int main() {
	int argc = 2;
    open_obj(argc);
    // initialize rendering with solid body
    render_mode = true;
    int window;
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize(720, 720);
    glutInitWindowPosition(0, 0);
    window = glutCreateWindow("109550150 §f«h¿Î");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutSpecialFunc(arrow_keys);
    //glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutMainLoop();
}
