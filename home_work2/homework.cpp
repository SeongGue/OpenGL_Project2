#include <GL/glut.h>
#include <stdio.h>

#define POINT_NUM	4

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Timerfunction(int value);
void Mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void Mouse_Move(int x, int y);
//-----------------------------------------------
void Init();
void Cmera_change();
bool Collide_XZ(float a_x, float a_z, float b_x, float b_z, float a_radious, float b_radious);
void draw_point();
void Hermite_Spline();

enum { XY_SURFACE = 0, XZ_SURFACE = 1, PERSPECTIVE = 2 };

struct Point
{
	float x;
	float y;
	float z;
	bool collison;
};

Point p[POINT_NUM];
Point test_point = { 0, 0, 0 };
Point save_spline_point[100];

int camera_viewpoint = 0;
int w1 = 0, h1 = 0;
int point_num = 0;

int s_index = 0;

float angle = 0;


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
	glutMotionFunc(Mouse_Move);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(40, Timerfunction, 1);
	glutMainLoop();
}

GLvoid drawScene(GLvoid)
{
	Cmera_change();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 젂체를 칠하기

	Hermite_Spline();
	draw_point();

	for (int i = 0; i < 100; i++)
		printf("save_spline_point[%d].x = %.f, save_spline_point[%d].y = %.f, save_spline_point[%d].z = %.f \n", i, save_spline_point[i].x, i, save_spline_point[i].y, i, save_spline_point[i].z);

	glPushMatrix();
	glTranslatef(save_spline_point[s_index].x, save_spline_point[s_index].y, save_spline_point[s_index].z);
	glutSolidCube(20);
	glPopMatrix();
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
	if (camera_viewpoint == XY_SURFACE)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			p[point_num].x = x - 400;
			p[point_num].y = 300 - y;
			point_num++;
		}
	}
	else if (camera_viewpoint == XZ_SURFACE)
	{ 
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			test_point.x = x - 400;
			test_point.z = 300 - y;
			for (int i = 0; i < POINT_NUM; i++){
				if (Collide_XZ(p[i].x, p[i].z, test_point.x, test_point.z, 2.5, 2.5))
				{
					p[i].collison = true;
				}
			}
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			for (int i = 0; i < POINT_NUM; i++)
				p[i].collison = false;
		}
	}
}

void Mouse_Move(int x, int y)
{
	for (int i = 0; i < POINT_NUM; i++)
	{
		if (p[i].collison)
		{
			p[i].z = 300 - y;
			test_point.x = x - 400;
			test_point.z = 300 - y;
		}
	}
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == '1')
	{
		camera_viewpoint = XY_SURFACE;
	}
	if (key == '2')
	{
		camera_viewpoint = XZ_SURFACE;
	}
	if (key == '3')
	{
		camera_viewpoint = PERSPECTIVE;
	}
}

void Timerfunction(int value)
{
	angle += 1;
	if (camera_viewpoint == PERSPECTIVE){
		if (s_index > 100)
			s_index = 0;
		s_index++;
	}
	glutPostRedisplay();
	glutTimerFunc(40, Timerfunction, 1);
}

void Init()
{
	for (int i = 0; i < 4; i++){
		p[i].x = 0; p[i].y = 0; p[i].z = 0; p[i].collison = false;
	}
}

void Cmera_change()
{
	glLoadIdentity();
	if (camera_viewpoint == XY_SURFACE)
		glOrtho(-400, 400, -300, 300, -300, 300);
	if (camera_viewpoint == XZ_SURFACE){
		glRotatef(90, 1, 0, 0);
		glOrtho(-400, 400, -300, 300, -300, 300);
	}
	if (camera_viewpoint == PERSPECTIVE){
		gluPerspective(60.0f, w1 / h1, 1.0, 2000.0);
		gluLookAt(0.0, 100, 1000.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);
		glRotatef(angle, 0, 1, 0);
	}
}

bool Collide_XZ(float a_x, float a_z, float b_x, float b_z, float a_radious, float b_radious)
{
	float left_a = a_x - a_radious, right_a = a_x + a_radious, top_a = a_z + a_radious, bottom_a = a_z - a_radious;
	float left_b = b_x - b_radious, right_b = b_x + b_radious, top_b = b_z + b_radious, bottom_b = b_z - b_radious;

	if (left_a > right_b)
		return false;
	if (right_a < left_b)
		return false;
	if (bottom_a > top_b)
		return false;
	if (top_a < bottom_b)
		return false;
	return true;
}

void draw_point()
{
	glColor3f(1, 1, 0);//ctrlpoint
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

	glPointSize(5);//current mouse point
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(test_point.x, test_point.y, test_point.z);
	glEnd();
}
void Hermite_Spline()
{
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
		int index = (int)(t * 100);
		save_spline_point[index].x = x;
		save_spline_point[index].y = y;
		save_spline_point[index].z = z;
	
	}
	glEnd();


}