
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

float G_MASS = 1;
float G_DAMPENING = 0.01;
float G_TIMESTEP = 0.5;
float G_CONSTRAINT_ITERATIONS = 15;

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

Cloth cloth = Cloth(14.0, 10.0, G_MASS, 55, 45, G_CONSTRAINT_ITERATIONS, G_DAMPENING, G_TIMESTEP);
vmath::vec3 ballCenter = vmath::vec3(7, -5, 0);
float ballRadius = 2;
float ballTime = 0;

void Display(void)
{
	//Calculate positions
	ballTime++;
	ballCenter[2] = cos(ballTime / 50.0) * 7;

	cloth.addForce(vmath::vec3(0, -0.2, 0));
	cloth.windForce(vmath::vec3(0.5, 0, 0.2));
	cloth.timeStep();
	cloth.ballCollision(ballCenter, ballRadius);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glDisable(GL_LIGHTING); // drawing some smooth shaded background - because I like it ;)
	glBegin(GL_POLYGON);
	glColor3f(0.8f, 0.8f, 1.0f);
	glVertex3f(-200.0f, -100.0f, -100.0f);
	glVertex3f(200.0f, -100.0f, -100.0f);
	glColor3f(1.0f, 0.8f, 1.0f);
	glVertex3f(200.0f, 100.0f, -100.0f);
	glVertex3f(-200.0f, 100.0f, -100.0f);
	glEnd();
	glEnable(GL_LIGHTING);

	glTranslatef(-6.5, 6, -9.0f); // move camera out and center on the cloth
	glRotatef(25, 0, 1, 0); // rotate a bit to see the cloth from the side

	// Draw cloth
	cloth.drawShaded();
	// Draw ball
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

void init(GLvoid) {
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
}

int main(int argc, char *argv[])
{
	TwBar *bar; // Pointer to the tweak bar

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Cloth Simulation");

	init();

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

	float g_zoom = 0.0;
	TwAddVarRW(bar, "Zoom", TW_TYPE_FLOAT, &g_zoom,
		" min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");

	glutMainLoop();


	return 0;
}