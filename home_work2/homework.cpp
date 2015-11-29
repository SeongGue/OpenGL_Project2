#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POINT_NUM	4
#define SPLINE_NUM	5
#define BUILDING_NUM	10
#define GROUND	-200

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Timerfunction(int value);
void Mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void Mouse_Move(int x, int y);
//-----------------------------------------------
void Print();

void Init();
void Cmera_change();
bool Collide_2D(float a_x, float a_z, float b_x, float b_z, float a_radious, float b_radious);
void ctrl_point();
void Hermite_Spline();
void draw_train();
void gen_building();
int gen_sign();


enum { XY_SURFACE = 0, XZ_SURFACE = 1, PERSPECTIVE = 2 };

struct Point
{
	float x;
	float y;
	float z;
	bool collison;
};

struct Building
{
	float x, y, z;
	float size;
	float color_r, color_g, color_b;
};

Point p[SPLINE_NUM][POINT_NUM];
Point test_point = { 0, 0, 0 };
Point save_spline_point[SPLINE_NUM][100];
Point v0[SPLINE_NUM];
Point v3[SPLINE_NUM];

Building building[BUILDING_NUM];

int camera_viewpoint = 0;
int w1 = 0, h1 = 0;
int point_num = 0;

int c_index = 0, s_index = 0; //롤러코스터 이동 변수
int spline_num = 0;

float angle = 0;

void main(int argc, char *argv[])
{
	Init();
	gen_building();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Homework");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
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
	ctrl_point();
	draw_train();

	for (int i = 0; i < BUILDING_NUM; i++)
	{
		glColor3f(building[i].color_r, building[i].color_g, building[i].color_b);
		glPushMatrix();
		glTranslatef(building[i].x, building[i].y, building[i].z);
		glutSolidCube(building[i].size);
		glPopMatrix();
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	w1 = w;
	h1 = h;
	glEnable(GL_DEPTH_TEST);
}

void Mouse(int button, int state, int x, int y)
{
	if (camera_viewpoint == XY_SURFACE)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && point_num < 20)
		{

			test_point.x = x - 400;
			test_point.y = 300 - y;
			for (int i = 0; i < BUILDING_NUM; i++)
			{
				if (Collide_2D(building[i].x, building[i].y, test_point.x, test_point.y, building[i].size / 2, 2.5))
					return;
			}
			p[spline_num % 5][point_num % 4].x = x - 400;
			p[spline_num % 5][point_num % 4].y = 300 - y;
			Print();
			if (point_num % 4 == 3 && spline_num % 5 != 4)
			{
				point_num++;
				spline_num = (int)(point_num / 4);
				p[spline_num % 5][point_num % 4].x = x - 400;
				p[spline_num % 5][point_num % 4].y = 300 - y;
			}
			point_num++;
			spline_num = (int)(point_num / 4);

		}
	}
	else if (camera_viewpoint == XZ_SURFACE)
	{
		glutMotionFunc(Mouse_Move);
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			test_point.x = x - 400;
			test_point.z = 300 - y;
			for (int i = 0; i < SPLINE_NUM; i++){
				for (int j = 0; j < POINT_NUM; j++)
				{
					if (Collide_2D(p[i][j].x, p[i][j].z, test_point.x, test_point.z, 2.5, 2.5))
					{
						p[i][j].collison = true;
					}
				}
			}
		}
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			for (int i = 0; i < SPLINE_NUM; i++)
				for (int j = 0; j < POINT_NUM; j++)
					p[i][j].collison = false;
		}
	}
}

void Mouse_Move(int x, int y)
{
	for (int i = 0; i < SPLINE_NUM; i++)
	{
		for (int j = 0; j < POINT_NUM; j++)
		{
			if (p[i][j].collison)
			{
				p[i][j].z = 300 - y;
				test_point.x = x - 400;
				test_point.z = 300 - y;
			}
		}
	}
	printf("move~~");
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
		s_index++;
		c_index = (int)(s_index / 100);
	}
	glutPostRedisplay();
	glutTimerFunc(40, Timerfunction, 1);
}


void Init()
{
	for (int i = 0; i < SPLINE_NUM; i++){
		v0[i].x = 0; v0[i].y = 0; v0[i].z = 0; v0[i].collison = false;
		v3[i].x = 0; v3[i].y = 0; v3[i].z = 0; v3[i].collison = false;
		for (int j = 0; j < POINT_NUM; j++){
			p[i][j].x = 0; p[i][j].y = 0; p[i][j].z = 0; p[i][j].collison = false;
		}
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

bool Collide_2D(float a_x, float a_z, float b_x, float b_z, float a_radious, float b_radious)
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

void ctrl_point()
{
	glColor3f(1, 1, 0);//ctrlpoint
	for (int i = 0; i < SPLINE_NUM; i++)
	{
		for (int j = 0; j < POINT_NUM; j++)
		{
			glPushMatrix();
			{
				glTranslatef(p[i][j].x, p[i][j].y, p[i][j].z);
				glutSolidCube(5);
			}
			glPopMatrix();
		}
	}

	glPointSize(5);//current mouse point
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(test_point.x, test_point.y, test_point.z);
	glEnd();
}
void Hermite_Spline()
{
	glColor3f(1, 1, 1);
	for (int i = 0; i < SPLINE_NUM; i++)
	{
		v0[i] = { 3 * (p[i][1].x - p[i][0].x), 3 * (p[i][1].y - p[i][0].y), 3 * (p[i][1].z - p[i][0].z) };
		v3[i] = { 3 * (p[i][3].x - p[i][2].x), 3 * (p[i][3].y - p[i][2].y), 3 * (p[i][3].z - p[i][2].z) };
	}

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < spline_num; i++)
	{
		for (float t = 0.0; t <= 1.0; t += 0.01)
		{
			float x = (1 - 3 * t*t + 2 * t*t*t)*p[i][0].x + t*(1 - t)*(1 - t)*v0[i].x + (3 * t*t - 2 * t*t*t)*p[i][3].x - t*t*(1 - t)*v3[i].x;
			float y = (1 - 3 * t*t + 2 * t*t*t)*p[i][0].y + t*(1 - t)*(1 - t)*v0[i].y + (3 * t*t - 2 * t*t*t)*p[i][3].y - t*t*(1 - t)*v3[i].y;
			float z = (1 - 3 * t*t + 2 * t*t*t)*p[i][0].z + t*(1 - t)*(1 - t)*v0[i].z + (3 * t*t - 2 * t*t*t)*p[i][3].z - t*t*(1 - t)*v3[i].z;
			glVertex3f(x, y, z);
			int index = (int)(t * 100);
			save_spline_point[i][index].x = x;
			save_spline_point[i][index].y = y;
			save_spline_point[i][index].z = z;

		}
	}
	glEnd();
}

void Print()
{
	//for (int i = 0; i < 100; i++)
	//	printf("save_spline_point[%d].x = %.f, save_spline_point[%d].y = %.f, save_spline_point[%d].z = %.f \n", i, save_spline_point[i].x, i, save_spline_point[i].y, i, save_spline_point[i].z);

	printf("p[%d][%d].x = %.f \n", spline_num % 5, point_num % 4, p[spline_num % 5][point_num % 4].x); //마우스 함수
	printf("p[%d][%d].y = %.f \n", spline_num % 5, point_num % 4, p[spline_num % 5][point_num % 4].y);
	printf("spline_num = %d \n", point_num);


}

void draw_train()
{
	glPushMatrix();
	{
		glTranslatef(save_spline_point[c_index % 5][s_index % 100].x, save_spline_point[c_index % 5][s_index % 100].y, save_spline_point[c_index % 5][s_index % 100].z);
		glutSolidCube(20);
	}
	glPopMatrix();
}

void gen_building()
{
	srand((unsigned)time(NULL));
	int sign = 0;
	int cnt = 0;
	int jb = 0;
	for (int i = 0; i < BUILDING_NUM; i++)//glOrtho(-400, 400, -300, 300, -300, 300);
	{
		sign = gen_sign();
		building[i].size = (rand() % 50) + 50;

		building[i].x = sign * (rand() % 400);
		building[i].z = sign * (rand() % 300);

		if (i > 0)
		{
			for (int j = 0; j < i; j++)
			{
				if (Collide_2D(building[j].x, building[j].z, building[i].x, building[i].z, building[j].size / 2, building[i].size / 2))
				{
					building[i].x = sign * (rand() % 400);
					building[i].z = sign * (rand() % 300);
					j = 0;
				}
				printf("i = %d, j = %d \n", i, j);
			}
		}


		building[i].y = GROUND + building[i].size / 2;
		sign = gen_sign();
		building[i].color_r = rand() % 2;
		building[i].color_g = rand() % 2;
		building[i].color_b = rand() % 2;
		while (building[i].color_r == 0 && building[i].color_g == 0 && building[i].color_b == 0)
		{
			building[i].color_r = rand() % 2;
			building[i].color_g = rand() % 2;
			building[i].color_b = rand() % 2;
		}
	}
}

int gen_sign()
{
	int sign = rand() % 2;
	if (sign % 2 == 0)
		return sign = 1;
	else
		return sign = -1;
}