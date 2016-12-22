// opengl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include "lib\glut.h"
#include "lib\glui.h"

using namespace std;
/* hàm thực hiện các thao tác vẽ theo yêu cầu của chương trình */

static int year = 0, day = 0; // thông số chỉ thời gian trong năm và thời gian trong ngày để xác định vị trí của trái đất trên quỹ đạo và xác định góc quay của nó quanh tâm
int vpw = 0;
int vph = 0;

/* hàm thực hiện các khởi tạo */
void init(void)
{
	/* chọn màu để xóa nền (tức là sẽ phủ nền bằng màu này) */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST); // bật chức năng cho phép loại bỏ một phần của đối tượng bị che bởi đối tượng khác
	glShadeModel(GL_FLAT);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // xóa color buffer và depth buffer
	glPushMatrix(); // lưu lại ma trận hiện hành
	glColor3f(1.0, 0, 0); // thiết lập màu vẽ là màu đỏ
	glutWireSphere(1.0, 20, 16); // vẽ mặt trời là một lưới cầu có tâm tại gốc tọa độ
								 /* di chuyển đến vị trí mới để vẽ trái đất */
	glRotatef((GLfloat)year, 0.0, 1.0, 0.0); // quay một góc tương ứng với thời gian trong năm
	glTranslatef(2.0, 0.0, 0.0); // tịnh tiến đến vị trí hiện tại của trái đất trên quỹ đạo quanh mặt trời
	glRotatef((GLfloat)day, 0.0, 1.0, 0.0); // quay trái đất tương ứng với thời gian trong ngày
	glColor3f(0, 0, 1.0); // thiết lập màu vẽ là màu blue
	glutWireSphere(0.2, 10, 8); // vẽ trái đất
	glPopMatrix(); // phục hồi lại ma trận hiện hành cũ: tương ứng với quay lại vị trí ban đầu

	// world position, in case it is local position, need to convert it into world position
	GLdouble wx = 1.4, wy = 1.5, wz = 0;
	GLdouble sx, sy, sz;
	
	GLdouble projection[16];
	GLdouble modelview[16];
	GLint viewport[4] = {0,0,vpw,vph};
	// for camera specification --------------------------------------------------------------------
	glGetDoublev(GL_PROJECTION_MATRIX, projection); // fov, aspect w/h, near, far
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); // eye poit, look at point, upvector
	// ----------------------------------------------------------------------------------------------

	// APPROACH 1: using gluProject ----------------------------------------------------------------------------------------------
	gluProject(wx, wy, wz,
	modelview, projection, viewport,
	&sx, &sy, &sz);
	cout << " sx: " << sx << " sy: " << sy << " sz: " << sz << endl;

	// APPROACH 2: using gluProject manual ----------------------------------------------------------------------------------------
	glPushMatrix();
	GLdouble p1[4] = { wx,wy,wz,1 };
	glMatrixMode(GL_MODELVIEW);
	
	glLoadMatrixd(projection);
	glMultMatrixd(modelview);
	
	GLdouble p2[4];
	GLdouble m[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, m);
	// maxtrix = m11 m21 m31 m41 m21 m22..
	// m11 m12 m13 m14
	// m21 m22 m23 m24
	// m31 m32 m33 m34
	// m41 m42 m43 m44 
	for (int i = 0; i < 4; i++)
	{
		p2[i] = 0;
		for (int j = 0; j < 4; j++) p2[i] += m[i + j*4] * p1[j];
	}
	for (int i = 0; i < 4; i++) p2[i] = p2[i] / p2[3];
	sx = viewport[0] + viewport[2] * (p2[0] + 1) / 2;
	sy = viewport[1] + viewport[3] * (p2[1] + 1) / 2;
	
	sz = (p2[2] + 1) / 2;
	glPopMatrix();
	cout << " sx: " << sx << " sy: " << sy << " sz: " << sz << endl;
	glutSwapBuffers();
}

/* xử lý khi cửa sổ bị thay đổi */
void reshape(int w, int h)
{
	vpw = w;
	vph = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); // thay đổi kích thước viewport
											  /* xét thao tác trên chiếu */
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0); // thực hiện phép chiếu phối cảnh
															  // xét thao tác trên ModelView 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // thiết lập view
	*/
	const GLfloat zNear = 0.01f, zFar = 100.0f, fieldOfView = 40.0f*3.14159/180.0;
	glMatrixMode(GL_PROJECTION);
	GLfloat size = zNear * tanf(fieldOfView / 2.0f);
	GLfloat aspectRatio = w / h;
	glFrustum(-size, size, -size / aspectRatio, size / aspectRatio, zNear, zFar);
	glTranslatef(0.0f, -0.5f, -2.5f);
}

/* xử lý sự kiện keyboard */
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
		day = (day + 10) % 360;
		glutPostRedisplay();
		break;
	case 's':
		day = (day - 10) % 360;
		glutPostRedisplay();
		break;
	case 'a':
		year = (year + 5) % 360;
		glutPostRedisplay();
		break;
	case 'd':
		year = (year - 5) % 360;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	cout << "hi openGL" << endl;;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	int main_window = glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}

