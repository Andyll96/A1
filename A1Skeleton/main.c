/*******************************************************************
           Multi-Part Model Construction and Manipulation
********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "CubeMesh.h"
#include "QuadMesh.h"

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 1000;     // Viewport width in pixels
const int vHeight = 1000;    // Viewport height in pixels

static int currentButton;
static unsigned char currentKey;

// Lighting/shading and material properties for submarine - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat submarine_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat submarine_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat submarine_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat submarine_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground / sea floor 
static QuadMesh groundMesh;

//Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

float angle = 0.0f;

float propAngle = 0.0f;

float dx = 0.0f, dy = 0.0f, dz = -1.0f;



// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void drawDirigible(void);


int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 1000) / 2, 0);
	glutCreateWindow("Assignment 1");

    // Initialize GL
    initOpenGL(vWidth, vHeight);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
	glutIdleFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(functionKeys);

    // Start event loop, never returns
    glutMainLoop();

    return 0;
}

// Set up OpenGL. For viewport and projection setup see reshape(). */
/*Sets up lighting, other OpenGL stuff, and the ground floor*/
void initOpenGL(int w, int h)
{
    // Set up and enable lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);   // This light is currently off

    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
    glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

    // Set up ground/sea floor quad mesh
    Vector3D origin = NewVector3D(-8.0f, 0.0f, 8.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
    InitMeshQM(&groundMesh, meshSize, origin, 16.0, 16.0, dir1v, dir2v);

    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
    SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

    // Set up the bounding box of the scene
    // Currently unused. You could set up bounding boxes for your objects eventually.
    //Set(&BBox.min, -8.0f, 0.0, -8.0);
    //Set(&BBox.max, 8.0f, 6.0,  8.0);
}

// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
/*Sets up Camera, Sets material for submarine, creates cube, redraws floor*/
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
    gluLookAt(0.0, 6.0, 22.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);


    // Set submarine material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, submarine_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, submarine_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, submarine_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, submarine_mat_shininess);



	glPushMatrix();
		glPushMatrix();
			glScalef(0.5, 50.0, 0.5);
			glutSolidCube(0.5);
		glPopMatrix(); 
		glTranslatef(0, 0 + dy, 0);
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		drawDirigible();
	glPopMatrix();
    // Draw ground/sea floor
    DrawMeshQM(&groundMesh, meshSize);

    glutSwapBuffers();   // Double buffering, swap buffers
}

void drawDirigible(void) {
	glPushMatrix();
	//Main Body
	glTranslated(0 + dx, 0, 0 + dz);
		glPushMatrix(); //Main Body
			glTranslatef(0.0, 8.0, 0.0);
			glScalef(6.0, 1.0, 1.0);
			glutSolidSphere(1, 20, 50);
		glPopMatrix();

		//Propeller
		glPushMatrix();
			glPushMatrix(); //stud
				glTranslatef(5.0, 8.0, 0.0);
				glScalef(6.0, 0.5, 0.5);
				glutSolidCube(0.5);
			glPopMatrix();
			glPushMatrix(); //propeller 1
				glTranslatef(6.5, 8.0, 0.0);
				glScalef(0.2, 0.5, 3.0);
				glutSolidCube(0.5);
			glPopMatrix();
			glPushMatrix(); //propeller 2
				glTranslatef(6.5, 8.0, 0.0);
				glRotatef(90.0, 1.0, 0.0, 0.0);
				glScalef(0.2, 0.5, 3.0);
				glutSolidCube(0.5);
			glPopMatrix();
		glPopMatrix();

		//Passenger Deck
		glPushMatrix();
		glTranslatef(0.0, 7.0, 0.0);
		glScalef(4.0, 0.8, 0.8);
		glutSolidCube(1.0);
		glPopMatrix();
	glPopMatrix();
}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
    // Set up viewport, projection, then change to modelview matrix mode - 
    // display function will then set up camera and do modeling transforms.
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int mx, int my)
{
	float fraction = 0.6f;

    switch (key)
    {
	case 27:
		exit(0);
		break;
	case 'f':
		dx -= 0.2;
		break;
	case 'b':
		dx += 0.2;
		break;
	}

    glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_F1: //Help
		printf("F key: forward\n");
		printf("B key: backwards\n");
		printf("Left key: turn dirigible left\n");
		printf("Right key: turn dirigible right\n");
		printf("Up key: ascend dirigible vertically\n");
		printf("Down key: descend dirigible vertically\n");
		printf("esc key: close application");
		break;
	case GLUT_KEY_LEFT: //turn left
		printf("Left\n");
		angle += 2.0f;
		break;
	case GLUT_KEY_RIGHT: //turn right
		printf("Right\n");
		angle -= 2.0f;
		break;
	case GLUT_KEY_UP: //move vertically up
		printf("Up\n");
		dy += 0.2;
		break;
	case GLUT_KEY_DOWN: // move verticallly down
		printf("Down\n");
		dy -= 0.2;
		break;
	}

    glutPostRedisplay();   // Trigger a window redisplay
}

