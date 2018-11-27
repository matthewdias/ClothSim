
#include "pch.h"
#include <iostream>

#include <AntTweakBar.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#include "Particle.h"
#include "Cloth.h"
#include "Constraint.h"
#include "vmath.h"

//Setup globals for use within our cloth
float G_MASS = 1;
float G_DAMPENING = 0.01;
float G_TIMESTEP = 0.5;
float G_CONSTRAINT_ITERATIONS = 15;

float G_GRAVITY = -0.075f;

//Setup globals for AntTweakBar
int g_scene = 1;
float g_gravityScale = 1;
float g_windScale = 1;

// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (height == 0) {
		gluPerspective(80, (float)width, 1.0, 5000.0);
	} else {
		gluPerspective(80, (float)width / (float)height, 1.0, 5000.0);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);
}

void Terminate(void)
{
	TwTerminate();
}

Cloth cloth = Cloth(14.0, 10.0, G_MASS, 55, 45, G_CONSTRAINT_ITERATIONS, G_DAMPENING, G_TIMESTEP, vmath::vec3(0, 0, 0), false);
Cloth cloth2 = Cloth(14.0, 10.0, G_MASS, 55, 45, G_CONSTRAINT_ITERATIONS, G_DAMPENING, G_TIMESTEP, vmath::vec3(200, 0, 0), true);
vmath::vec3 ballCenter = vmath::vec3(7.5, -13, -5);
float ballRadius = 2;
float ballTime = 0;

void Display(void)
{

	cloth.addForce(vmath::vec3(0, G_GRAVITY * g_gravityScale * cloth.getMass(), 0));
	cloth.timeStep();
	cloth.ballCollision(ballCenter, ballRadius);
	cloth.planeCollision(-15);


	cloth2.addForce(vmath::vec3(0, G_GRAVITY * g_gravityScale * cloth2.getMass(), 0));
	cloth2.timeStep();

	cloth2.windForce(vmath::vec3(g_windScale*(2 + (rand() % 10 + 1)*(rand() % 3 - 1)*0.1), 0, 0.01));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glBegin(GL_POLYGON);
	glColor3f(0.8f, 0.8f, 1.0f);
	glVertex3f(-200.0f, -100.0f, -100.0f);
	glVertex3f(200.0f, -100.0f, -100.0f);
	glColor3f(1.0f, 0.8f, 1.0f);
	glVertex3f(200.0f, 100.0f, -100.0f);
	glVertex3f(-200.0f, 100.0f, -100.0f);
	glEnd();
	glEnable(GL_LIGHTING);

	switch (g_scene)
	{
		case 1: {
			glTranslatef(-5, 5, -12);
			glRotatef(25, 0, 1, 0);
			break;
		}
		case 2: {
			glTranslatef(-205, 5, -18);
			//glRotatef(25, 0, 1, 0);
			break;
		}
		case 3: {
			glTranslatef(-25, 6, -15);
			//glRotatef(25, 0, 1, 0);
			break;
		}
		default: {
			glLoadIdentity();
			break;
		}
	}

	// Draw cloth
	cloth.drawShaded();
	cloth2.drawShaded();

	// Draw ball
	glPushMatrix();
	glTranslatef(5, -35, 0);
	glColor3f(0.2f, 0.7f, 0.4f);
	glutSolidCube(40);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(ballCenter[0], ballCenter[1], ballCenter[2]);
	glColor3f(0.2f, 0.7f, 0.4f);
	glutSolidSphere(ballRadius - 0.1, 50, 50);
	glPopMatrix();

	// Draw tweak bars
	TwDraw();

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	glutPostRedisplay();
}

unsigned char * texData;
unsigned int texWidth, texHeight;

//Load BMP and bind it into openGL
int loadCustomBMP(const char * imagepath) {
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int imageSize;   // = width*height*3 (For RGB)

	FILE *stream;
	errno_t err;

	// Open the file
	if ((err = fopen_s(&stream, imagepath, "rb")) != 0) {
		printf("Image could not be opened\n");
		return 0;
	}

	if (fread(header, 1, 54, stream) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}

	imageSize = *(int*)&(header[0x22]);
	texWidth = *(int*)&(header[0x12]);
	texHeight = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) {
		imageSize = texWidth * texHeight * 3;
	}

	// Create a buffer
	texData = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(texData, 1, imageSize, stream);

	//Everything is in memory now, the file can be closed
	fclose(stream);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, texData);

	printf("TEXTURE LOADED\n");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return textureID;
}

void init(GLvoid) {
	//Setup cloth particles which shouldn't move in each scene.
	cloth2.getParticle(0, 0)->setMovable(false);
	cloth2.getParticle(0, 1)->setMovable(false);
	cloth2.getParticle(0, 45 - 1)->setMovable(false);
	cloth2.getParticle(0, 45 - 2)->setMovable(false);


	//Setup lighting
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = { -1.0,1.0,0.5,0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *)&lightPos);

	glEnable(GL_LIGHT1);

	GLfloat lightAmbient1[4] = { 0.0,0.0,0.0,0.0 };
	GLfloat lightPos1[4] = { 1.0,0.0,-0.2,0.0 };
	GLfloat lightDiffuse1[4] = { 0.5,0.5,0.3,0.0 };

	glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat *)&lightPos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat *)&lightAmbient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat *)&lightDiffuse1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// Load textures in
	loadCustomBMP("image.bmp");   // Load pattern into image data array
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_BGR_EXT,
		GL_UNSIGNED_BYTE, texData);  // Create texture from image data
}

void TW_CALL RunCB(void * /*clientData*/)
{
	cloth.setPosition(vmath::vec3(0, 0, 0), false);
}

int main(int argc, char *argv[])
{
	TwBar *bar; // Pointer to the tweak bar

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Cloth Simulation");

	// Set GLUT callbacks
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	atexit(Terminate);

	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT key events to AntTweakBar
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	// - Directly redirect GLUT special key events to AntTweakBar
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(300, 100);

	bar = TwNewBar("TweakBar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); // Message added to the help bar.
	TwDefine(" TweakBar size='200 400' color='0 0 0' "); // change default tweak bar size and color

	TwAddVarRW(bar, "Scene", TW_TYPE_INT16, &g_scene,
		" min=1 max=2 step=1 keyIncr=z keyDecr=Z help='Change what cloth demo is currently running.' ");

	TwAddVarRW(bar, "Gravity", TW_TYPE_FLOAT, &g_gravityScale,
		" min=0.0 max=2.0 step=0.1 keyIncr=a keyDecr=A help='Change what cloth demo is currently running.' ");

	TwAddVarRW(bar, "WindScale", TW_TYPE_FLOAT, &g_windScale,
		" min=0.0 max=2.0 step=0.1 keyIncr=q keyDecr=Q help='Change what cloth demo is currently running.' ");

	TwAddButton(bar, "Reset", RunCB, NULL, "label='Reset Scene 1'");

	init();

	glutMainLoop();


	return 0;
}
