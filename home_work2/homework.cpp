#pragma warning(disable : 4996)
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>


#define POINT_NUM	4
#define SPLINE_NUM	5
#define BUILDING_NUM	10
#define GROUND	-200
#define SNOW_NUM	20
#define PIE	3.14

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Timerfunction(int value);
void Mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void Mouse_Move(int x, int y);
void Light();
GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info);
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
void draw_building();
void Setting();
void make_background();
void Init_snow();
void select_weather();
void Bresenham(float s_x, float s_y, float e_x, float e_y);
void draw_all();
void firewalk_line(float s_x, float s_y, float e_x, float e_y, float z);

enum { XY_SURFACE = 0, XZ_SURFACE = 1, PERSPECTIVE = 2, MULTI_VIEW = 3 };
enum { SUNNY = 0, SNOW = 1, RAIN = 2 };

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

GLubyte *pBytes; // 데이터를 가리킬 포인터
BITMAPINFO *info; // 비트맵 헤더 저장핛 변수

Point p[SPLINE_NUM][POINT_NUM];
Point test_point = { 0, 0, 0 };
Point save_spline_point[SPLINE_NUM][100];
Point v0[SPLINE_NUM];
Point v3[SPLINE_NUM];
Point t[SNOW_NUM];

Building building[BUILDING_NUM];

int camera_viewpoint = 0;
int w1 = 0, h1 = 0;
int point_num = 0;

int c_index = 0, s_index = 0; //롤러코스터 이동 변수
int spline_num = 0;
int aa = -1;
int weather = SUNNY;
int fire_level = 0;


float angle = 0;
float b_time = 0;
float c_time = 0;


GLuint textures[5]; // 텍스처 이름

void main(int argc, char *argv[])
{
	Init();
	Init_snow();
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
//===============OpenGL Fuc=====================//

GLvoid drawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 젂체를 칠하기
	Light();
	Cmera_change();
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	w1 = w;
	h1 = h;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MAP2_VERTEX_3);
	Setting();
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
	if (key == '4')
	{
		camera_viewpoint = MULTI_VIEW;
	}
	if (key == 'q')
	{
		weather = SUNNY;
	}
	if (key == 'w')
	{
		weather = RAIN;
	}
	if (key == 'e')
	{
		weather = SNOW;
	}
}

void Timerfunction(int value)
{
	angle += 1;
	if (fire_level == 0)
		b_time += 1;
	if (b_time == 100){
		fire_level = 1;	
	}
	if (fire_level == 1)
		c_time++;
	if (c_time == 70){
		b_time = 0;
		c_time = 0;
		fire_level = 0;
	}

	if (camera_viewpoint == PERSPECTIVE && point_num >= 19){
		s_index++;
		c_index = (int)(s_index / 100);
	}
	for (int i = 0; i < SNOW_NUM; i++)
	{
		t[i].y -= 5;
		if (t[i].y < -100)
			t[i].y = 150;
	}
	glutPostRedisplay();
	glutTimerFunc(40, Timerfunction, 1);
}

//========================MyFuc=====================================//
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
	if (camera_viewpoint == XY_SURFACE){
		glViewport(0, 0, w1, h1);
		glOrtho(-400, 400, -300, 300, -300, 300);
		draw_all();
	}
	if (camera_viewpoint == XZ_SURFACE){
		glViewport(0, 0, w1, h1);
		glRotatef(90, 1, 0, 0);
		glOrtho(-400, 400, -300, 300, -300, 300);
		draw_all();
	}
	if (camera_viewpoint == PERSPECTIVE){
		glViewport(0, 0, w1, h1);
		gluPerspective(60.0f, w1 / h1, 1.0, 2000.0);
		gluLookAt(0.0, 100, 1000.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);
		glRotatef(angle, 0, 1, 0);
		draw_all();
	}
	if (camera_viewpoint == MULTI_VIEW){
		glLoadIdentity();
		glViewport(0, 300, w1 / 2, h1 / 2); //좌상단
		glOrtho(-400, 400, -300, 300, -300, 300);
		draw_all();

		glLoadIdentity();
		glViewport(w1 / 2, 300, w1 /2 , h1 / 2); //우상단
		glRotatef(90, 1, 0, 0);
		glOrtho(-400, 400, -300, 300, -300, 300);
		draw_all();

		glLoadIdentity();
		glViewport(0, 0, w1/2, h1/2); //우하단
		gluPerspective(60.0f, w1 / h1, 1.0, 2000.0);
		gluLookAt(0.0, 100, 1000.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);
		glRotatef(angle, 0, 1, 0);
		draw_all();
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
	glColor3f(0, 0, 0);
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
	srand((unsigned)(NULL));
	int sign = 0;
	int cnt = 0;
	int jb = 0;
	for (int i = 0; i < BUILDING_NUM; i++)//glOrtho(-400, 400, -300, 300, -300, 300);
	{
		sign = gen_sign();
		building[i].size = (rand() % 50) + 50;

		building[i].x = sign * (rand() % 350);
		building[i].z = sign * (rand() % 250);

		if (i > 0)
		{
			for (int j = 0; j < i; j++)
			{
				if (Collide_2D(building[j].x, building[j].z, building[i].x, building[i].z, building[j].size / 2, building[i].size / 2))
				{
					building[i].x = sign * (rand() % 350);
					building[i].z = sign * (rand() % 250);
					j = 0;
				}
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

void draw_building()
{
	for (int i = 0; i < BUILDING_NUM; i++)
	{
		glColor3f(building[i].color_r, building[i].color_g, building[i].color_b);
		glPushMatrix();
		glTranslatef(building[i].x, building[i].y, building[i].z);
		glutSolidCube(building[i].size);
		glPopMatrix();
	}
}

GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	//바이너리 읽기 모드로 파일을 연다.
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	//비트맵 파일 헤드를 읽는다.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1){
		fclose(fp);
		return NULL;
	}

	//파일이 BMP파일인지 확인한다.
	if (header.bfType != 'MB'){
		fclose(fp);
		return NULL;
	}

	//BITMAPINFOHEADER 위치로 간다.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	//비트맵 이미지 데이터를 넣을 메모리 할당을 한다.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL){
		fclose(fp);
		exit(0);
		return NULL;
	}

	//비트맵 인포 헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned)infosize){
		free(*info);
		fclose(fp);
		return NULL;
	}

	//비트맵 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth) *
		((*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);

	// 비트맵의 크기만큼 메모리를 핛당핚다.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵 데이터를 bit(GLubyte 타입)에 저장핚다.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return bits;
}

void Setting()
{
	glGenTextures(7, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	{
		pBytes = LoadDIBitmap("33floor.bmp", &info);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	{
		pBytes = LoadDIBitmap("34front.bmp", &info);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	{
		pBytes = LoadDIBitmap("34left.bmp", &info);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	{
		pBytes = LoadDIBitmap("34right.bmp", &info);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	{
		pBytes = LoadDIBitmap("34back.bmp", &info);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	// 텍스처 모드 설정
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
	// 텍스처 매핑 홗성화
	glEnable(GL_TEXTURE_2D);
}


void Light()
{
	GLfloat AmbientLight0[] = { 0.5, 0.5, 0.5, 1.0f };
	GLfloat DiffuseLight0[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat SpecularLight0[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightPos0[] = { -400, 300, 0.0, 1.0 };

	GLfloat AmbientLight1[] = { 0.5, 0.5, 0.5, 1.0f };
	GLfloat DiffuseLight1[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat SpecularLight1[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightPos1[] = { 0, 300, -300.0, 1.0 };

	GLfloat specref[] = { 0.0f, 1.0f, 1.0f, 1.0f };
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT1);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientLight1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, SpecularLight1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	glPushMatrix();//조명 0
	{
		glColor3f(1, 0, 0);
		glTranslatef(-400, 300, 0);
		glutSolidSphere(20, 20, 20);
	}
	glPopMatrix();

	glPushMatrix();//조명 1
	{
		glColor3f(1, 1, 0);
		glTranslatef(0, 300, -300);
		glutSolidSphere(20, 20, 20);
	}
	glPopMatrix();

	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientLight0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);
}

void make_background()
{
	glPushMatrix();//glOrtho(-400, 400, -300, 300, -300, 300);
	{
		glBindTexture(GL_TEXTURE_2D, textures[0]); //front, left, right, back
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);//바닥
		{
			glTexCoord3f(0, 1, 0);
			glVertex3f(-400, GROUND, -300);
			glTexCoord3f(0, 0, 0);
			glVertex3f(-400, GROUND, 300);
			glTexCoord3f(1, 0, 0);
			glVertex3f(400, GROUND, 300);
			glTexCoord3f(1, 1, 0);
			glVertex3f(400, GROUND, -300);
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, textures[1]);//앞벽
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		{
			glTexCoord3f(1, 1, 0);
			glVertex3f(-400, 300, 300);
			glTexCoord3f(0, 1, 0);
			glVertex3f(400, 300, 300);
			glTexCoord3f(0, 0, 0);
			glVertex3f(400, GROUND, 300);
			glTexCoord3f(1, 0, 0);
			glVertex3f(-400, GROUND, 300);
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, textures[2]);//왼쪽
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		{
			glTexCoord3f(1, 1, 0);
			glVertex3f(400, 300, 300);
			glTexCoord3f(0, 1, 0);
			glVertex3f(400, 300, -300);
			glTexCoord3f(0, 0, 0);
			glVertex3f(400, GROUND, -300);
			glTexCoord3f(1, 0, 0);
			glVertex3f(400, GROUND, 300);
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, textures[3]);//오른쪽
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		{
			glTexCoord3f(0, 1, 0);
			glVertex3f(-400, 300, 300);
			glTexCoord3f(0, 0, 0);
			glVertex3f(-400, GROUND, 300);
			glTexCoord3f(1, 0, 0);
			glVertex3f(-400, GROUND, -300);
			glTexCoord3f(1, 1, 0);
			glVertex3f(-400, 300, -300);
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, textures[4]);//뒤
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		{
			glTexCoord3f(0, 1, 0);
			glVertex3f(-400, 300, -300);
			glTexCoord3f(0, 0, 0);
			glVertex3f(-400, GROUND, -300);
			glTexCoord3f(1, 0, 0);
			glVertex3f(400, GROUND, -300);
			glTexCoord3f(1, 1, 0);
			glVertex3f(400, 300, -300);
		}
		glEnd();
	}
	glPopMatrix();
}

void Init_snow()
{
	srand(1000);
	for (int i = 0; i < SNOW_NUM; i++)
	{
		if (rand() % 2 == 0)
			aa *= -1;
		t[i].x = (rand() % 300) * aa;
		t[i].y = (rand() % 150) * aa;
		t[i].z = (rand() % 300) * aa;
	}
}

//----------------------------------------------------------------------------
void Bresenham(float s_x, float s_y, float e_x, float e_y)//http://bestend.co.kr/282
{
	if (e_x - s_x < 0){//x변화량이 음의 값
		float tmp_x = s_x, tmp_y = s_y;
		s_x = e_x;
		e_x = tmp_x;
		s_y = e_y;
		e_y = tmp_y;
	}

	bool minus = false;
	float dx = e_x - s_x, dy = e_y - s_y;
	float end_x = 0, end_y = 0;
	float x = 0, y = 0;
	float m = dy / dx;
	float p = 0;
	int k = 0;
	float c1 = 0, c2 = 0;



	//============기울기가 다를때================//
	//=======전처리==========//
	if (0 <= m && m <= 1)//기본 브레즌함
	{
		x = s_x, y = s_y;
		p = 2 * dy - dx;
		end_x = e_x, end_y = e_y;
		c1 = 2 * dy;
		c2 = 2 * (dy - dx);
	}
	else if (m > 1)//y=x에 반사
	{
		x = s_y, y = s_x;
		p = 2 * dx - dy;
		end_x = e_y, end_y = e_x;
		c1 = 2 * dx;
		c2 = 2 * (dx - dy);
	}
	else if (-1 <= m && m <= 0)//y=0에 반사
	{
		x = s_x, y = -s_y;
		p = 2 * -dy - dx;
		end_x = e_x, end_y = -e_y;
		c1 = 2 * -dy;
		c2 = 2 * (-dy - dx);
	}
	else if (m < -1)//y=0에 반사 후 y=x반사
	{
		x = -s_y, y = s_x;
		p = 2 * dx + dy;
		end_x = -e_y, end_y = e_x;
		c1 = 2 * dx;
		c2 = 2 * (dx + dy);
	}

	glPointSize(2.5);
	glColor3f(0, 0, 1);
	

	glBegin(GL_POINTS);
	while (x <= end_x && y <= end_y)
	{
		++k;
		//========기본 브레즈넘======//
		if (p >= 0)
		{
			x += 1;
			y += 1;
			p += c2;
		}
		else if (p < 0)
		{
			x += 1;
			p += c1;
		}
		//=======후처리=========//
		if (minus == false)//x변화량이 음수
		{
			if (0 <= m && m <= 1)
				glVertex2f(x, y);
			else if (m > 1)
				glVertex2f(y, x);
			else if (-1 <= m && m <= 0)
				glVertex2f(x, -y);
			else if (m < -1)
				glVertex2f(y, -x);
		}
		else
		{
			if (0 <= m && m <= 1)
				glVertex2f(-x, y);
			else if (m > 1)
				glVertex2f(-y, x);
			else if (-1 <= m && m <= 0)
				glVertex2f(-x, -y);
			else if (m < -1)
				glVertex2f(-y, -x);
		}
	}
	glEnd();
}


void select_weather()
{
	if (camera_viewpoint != XZ_SURFACE)
	{
		draw_building();

		if (weather == SNOW)
		{
			for (int i = 0; i < SNOW_NUM; i++){
				glPushMatrix();
				{
					glColor3f(1, 1, 1);
					glTranslatef(t[i].x, t[i].y, t[i].z);
					glutSolidCube(10);
				}
				glPopMatrix();
			}
		}
		else if (weather == RAIN)
		{
			for (int i = 0; i < SNOW_NUM; i++){
				glPushMatrix();
				{
					glColor3f(1, 1, 1);
					glTranslatef(t[i].x, t[i].y, t[i].z);
					glScalef(1, 3, 1);
					glutSolidCube(10);
				}
				glPopMatrix();
			}
		}
		else
			weather = SUNNY;
	}
}
void draw_all()
{
	Hermite_Spline();
	ctrl_point();
	draw_train();
	select_weather();
	make_background();
	firewalk_line(0, GROUND, 100, 100, 0);
}

void firewalk_line(float s_x, float s_y, float e_x, float e_y, float z)
{
	float dx = e_x - s_x, dy = e_y - s_y;
	float m = dy / dx;
	float current_end_x = s_x + b_time;
	float current_end_y = s_y + m * b_time;
	glPushMatrix();
	{
		glTranslatef(0, 0, z);
		//======연화========//
		Bresenham(s_x, s_y, current_end_x, current_end_y);
		//=====불꽃파티클======//
		if (fire_level == 1)
		{
			for (float i = 0.0; i < 2; i += 0.1)
			{
				Bresenham(current_end_x, current_end_y, current_end_x + cos(i * PIE) * c_time, current_end_y + sin(i * PIE) * c_time);
			}
		}

	}
	glPopMatrix();
}