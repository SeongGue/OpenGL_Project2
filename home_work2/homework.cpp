#include <GL/glut.h>
#include <stdio.h>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
//void Timerfunction(int value);
void Mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
//-----------------------------------------------
void Init();
void Cmera_change();

struct Point
{
	float x;
	float y;
	float z;
	bool collison;
};

Point p[4];


int a = 0;
int w1 = 0, h1 = 0;
int point_num = 0;


void main(int argc, char *argv[])
{
	Init();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Homework");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(keyboard);
	//glutTimerFunc(40, Timerfunction, 1);
	glutMainLoop();
}

GLvoid drawScene(GLvoid)
{
	Cmera_change();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 젂체를 칠하기
	glColor3f(1, 1, 1);
	Point v0 = { 3 * (p[1].x - p[0].x), 3 * (p[1].y - p[0].y), 3 * (p[1].z - p[0].z) };
	Point v3 = { 3 * (p[3].x - p[2].x), 3 * (p[3].y - p[2].y), 3 * (p[3].z - p[2].z) };

	glBegin(GL_LINE_STRIP);
	for (float t = 0.0; t <= 1.0; t += 0.01)
	{
		float x = (1 - 3 * t*t + 2 * t*t*t)*p[0].x + t*(1 - t)*(1 - t)*v0.x + (3 * t*t - 2 * t*t*t)*p[3].x - t*t*(1 - t)*v3.x;
		float y = (1 - 3 * t*t + 2 * t*t*t)*p[0].y + t*(1 - t)*(1 - t)*v0.y + (3 * t*t - 2 * t*t*t)*p[3].y - t*t*(1 - t)*v3.y;
		float z = (1 - 3 * t*t + 2 * t*t*t)*p[0].z + t*(1 - t)*(1 - t)*v0.z + (3 * t*t - 2 * t*t*t)*p[3].z - t*t*(1 - t)*v3.z;
		glVertex3f(x, y, z);
	}
	glEnd();

	//glPointSize(5);
	//glBegin(GL_POINTS);
	//glColor3f(1, 0, 0);
	//glVertex3f(p[0].x, p[0].y, p[0].z);
	//glColor3f(0, 1, 0);
	//glVertex3f(p[1].x, p[1].y, p[1].z);
	//glColor3f(0, 0, 1);
	//glVertex3f(p[2].x, p[2].y, p[2].z);
	//glColor3f(1, 1, 0);
	//glVertex3f(p[3].x, p[3].y, p[3].z);
	//glEnd();

	glColor3f(1, 1, 0);
	glPushMatrix();
	{
		glTranslatef(p[0].x, p[0].y, p[0].z);
		glutSolidCube(5);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(p[1].x, p[1].y, p[1].z);
		glutSolidCube(5);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(p[2].x, p[2].y, p[2].z);
		glutSolidCube(5);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(p[3].x, p[3].y, p[3].z);
		glutSolidCube(5);
	}
	glPopMatrix();


	printf("red = %.f, %.f, %.f \n", p[0].x, p[0].y, p[0].z);
	printf("green = %.f, %.f, %.f \n", p[1].x, p[1].y, p[1].z);
	printf("blue = %.f, %.f, %.f \n", p[2].x, p[2].y, p[2].z);
	printf("yellow = %.f, %.f, %.f \n\n", p[3].x, p[3].y, p[3].z);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	w1 = w;
	h1 = h;
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		p[point_num].x = x - 400;
		p[point_num].y = 300 - y;
		point_num++;
		glutPostRedisplay();
	}
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == '0')
	{
		a = 0;
		glutPostRedisplay();
	}
	if (key == '1')
	{
		a = 1;
		glutPostRedisplay();
	}
}

//void Timerfunction(int value)
//{
//	glutPostRedisplay();
//	glutTimerFunc(40, Timerfunction, 1);
//}

void Init()
{
	for (int i = 0; i < 4; i++){
		p[i].x = 0; p[i].y = 0; p[i].z = 0; p[i].collison = false;
	}
}

void Cmera_change()
{
	glLoadIdentity();
	if (a == 0)
		glOrtho(-400, 400, -300, 300, 0, 500);
	if (a == 1){
		gluPerspective(60.0f, w1 / h1, 1.0, 2000.0);
		gluLookAt(0.0, 1000, 0.0,
			0.0, 0.0, 0.0,
			0.0, 0.0, -1.0);
	}
}