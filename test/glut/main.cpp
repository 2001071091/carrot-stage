
#ifdef _WIN32
#define	FREEGLUT_STATIC
#pragma comment(lib,"FreeImage.lib")
#endif

#include "GL/glut.h"
#include "Sprite.h" 
#include <utils.h>
#include <iostream>


using namespace com_yoekey_3d;

#define SPRITE_NUM 1

Sprite sprite[SPRITE_NUM];
//���ù���  

void draw_back() {
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_ONE, GL_ONE);
	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-10, 40, -30);
		glVertex3f(10, 40, -30);
		glVertex3f(0, 20, -30);
	glEnd();
	glDisable(GL_BLEND);
	glColor4f(1, 1, 1, 1);
}

void SetupLights() {
	glEnable(GL_LIGHTING);


    GLfloat ambientLight[] = {0.4f, 0.4f, 0.4f, 1.0f}; 
    GLfloat diffuseLight[] = {0.5f, 0.5f, 0.5f, 1.0f}; 
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f}; 
	GLfloat lightPos[] = { 1.0f, 1.0f, -1.0f, 1.0f };//x,y,z,w
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);



    GLfloat redAmbiLight[] = {0.2f, 0.0f, 0.0f, 1.0f}; //��ɫ����Դ������  
    GLfloat redDiffLight[] = {0.8f, 0.0f, 0.0f, 1.0f}; //��ɫ����Դ������  
    GLfloat redSpecLight[] = {1.0f, 0.0f, 0.0f, 1.0f}; //��ɫ����Դ�����? 


    glEnable(GL_COLOR_MATERIAL); //���ò��ʵ���ɫ����  
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); //ָ��������ɫ����  
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight); //ָ�����϶Ծ����ķ�Ӧ  
    glMaterialf(GL_FRONT, GL_SHININESS, 128); //ָ������ϵ��  

	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	/*
	GLfloat emissiveColor[] = {0.3,0.3,0.3,1.0};
	glMaterialfv(GL_FRONT,GL_EMISSION,emissiveColor);
	*/
}
GLuint tex = 0;
void render(void) {
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glColor3f(1.0, 0.0, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	
	gluLookAt(sprite[0].x, sprite[0].y + 50, sprite[0].z - 120,
		sprite[0].x, sprite[0].y + 50, sprite[0].z, 
            0.0, 1.0, 0.);

	SetupLights();

	//glColor3f(1, 0, 0);

	
	//glBindTexture(GL_TEXTURE_2D, tex);

    //float white[] = {0.0, 0.5, 0.5, 1};
    //glMaterialfv(GL_FRONT, GL_DIFFUSE,white);
    //glMaterialfv(GL_FRONT, GL_SPECULAR,white);
    //model.x += 0.1;
    //model.z += 0.1;
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < SPRITE_NUM; i++){
		sprite[i].rotaion +=(i%2==1?1:-0.5);
		sprite[i].render();
		sprite[i].scale(0.3);
	}

	draw_back();

    glFlush();
    glutSwapBuffers();
}


void changeSize(int w, int h) {
	glViewport(0, 0, w, h);
}

void timer(int arg) {
    //printf("%d",arg);
    render();
    glutTimerFunc(16, timer, 1);
}

void setupRC(void) {

	glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	tex = loadTexture("../../data/3_KDJH_01.png");

	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//glFrontFace(GL_CCW);
	//glEdgeFlag(GL_TRUE);
	//glEnable(GL_POLYGON_STIPPLE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_POLYGON_SMOOTH);
	//glEnable(GL_MULTISAMPLE);
}
/*
 *
 */
int main(int argc, char** argv) {

	//printf("version:%s\n", FreeImage_GetVersion());

    //const GLubyte* x =glGetString(GL_VENDOR);
    //std::cout << "vendor:" << glGetString(GL_VENDOR) << "\n";
    //std::cout << "render:" << glGetString(GL_RENDER) << "\n";
    //std::cout << "version:" << glGetString(GL_VERSION) << "\n";
    //std::cout << "extensions:" << glGetString(GL_EXTENSIONS) << "\n";

	for (int i = 0; i < SPRITE_NUM; i++){
		sprite[i].load_model("../../data/zz.obj");
		//sprite[i].x = rand() % 100;
		//sprite[i].z = rand() % 100;
		//sprite[i].scale(1.5);
	}
    
	//sprite2.load_model("../../data/z1.obj");
    //sprite1.loadFrom_obj("/mnt/hgfs/E/zz.obj");
    //sprite2.loadFrom_obj("/mnt/hgfs/E/z1.obj");


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("test win");

    glutReshapeFunc(changeSize);
    glutDisplayFunc(render);
    glutTimerFunc(16, timer, 1);


    glMatrixMode(GL_PROJECTION);
    gluPerspective(/* field of view in degree */ 60.0,
            /* aspect ratio */ 800.0 / 600.0,
            /* Z near */ 1.0, /* Z far */ 240.0);

    setupRC();





    //gluPerspective(60, 800 / 600, 1, 10);

    //glOrtho(-100, 100.0, -100, 100.0, 100, -100);
    glutMainLoop();

    return (EXIT_SUCCESS);
}
