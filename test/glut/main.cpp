
#ifdef _WIN32
#define	FREEGLUT_STATIC
#pragma comment(lib,"FreeImage.lib")
#endif


#include "Sprite.h" 
#include "Mode.h"
#include <utils.h>
#include <iostream>
#include "GL/glut.h"


using namespace com_yoekey_3d;

#define SPRITE_NUM 1

Sprite sprite[SPRITE_NUM];
//���ù���  
Mode mod;
void draw_back() {
	glEnable(GL_BLEND);
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);
	//glBlendFunc(GL_ONE, GL_ONE);
	glColor4f(1, 0, 0,1);
	glBegin(GL_TRIANGLES);
		glVertex3f(1, 1, 0);
		glVertex3f(-1, 1, 0);
		glVertex3f(0, 0, 0);
	glEnd();
	glDisable(GL_BLEND);
	//glColor4f(1, 1, 1, 1);
}

void SetupLights() {
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);


    GLfloat ambientLight[] = {0.4f, 0.4f, 0.4f, 1.0f}; 
    GLfloat diffuseLight[] = {0.8, 0.8, 0.8, 1.0f}; 
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPos[] = { 100,0,0,1.0f };//x,y,z,w
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);



    //GLfloat redAmbiLight[] = {0.2f, 0.0f, 0.0f, 1.0f}; //��ɫ����Դ������  
    //GLfloat redDiffLight[] = {0.8f, 0.0f, 0.0f, 1.0f}; //��ɫ����Դ������  
    //GLfloat redSpecLight[] = {1.0f, 0.0f, 0.0f, 1.0f}; //��ɫ����Դ�����? 


    //glEnable(GL_COLOR_MATERIAL); //���ò��ʵ���ɫ����  
    //glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE); //ָ��������ɫ����  
	//glColorMaterial(GL_FRONT, GL_SPECULAR);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientLight);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLight);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight); //ָ�����϶Ծ����ķ�Ӧ  
    glMaterialf(GL_FRONT, GL_SHININESS,6); //ָ������ϵ��  


	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	/*
	GLfloat emissiveColor[] = {0.3,0.3,0.3,1.0};
	glMaterialfv(GL_FRONT,GL_EMISSION,emissiveColor);
	*/
}
unsigned char letterA[] = {
	0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe
};
void render(void) {
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	//draw_back();
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, tex);
	//for (int i = 0; i < SPRITE_NUM; i++){
	//	sprite[i].rotaion +=(i%2==1?1:-0.5);
	//	sprite[i].scale(0.7);
		//sprite[i].render();
		
	//}

	//draw_back();
	glRotatef(0.1, 0, 0, 1);

	mod.render();
	//mod.render_bind_skelecton();
	//mod.render_skelecton();
	/*
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100,100, -100, 100, -100, 100);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glColor3f(1, 1, 1);
	glRasterPos2i(0, 0);
	glBitmap(8, 13, 0, 0, 0, 0, letterA);
	*/
	//draw_back();

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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);

	//glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	//tex = loadTexture("../../data/3_KDJH_01.png");
	//tex = loadTexture("../../data/hr_hair.dds");
	//tex = loadTexture("../../data/hr_body.dds");

	glewInit();

	//mod.load("../../data/huangrong.DAE");
	mod.load("../../data/walk.DAE");
	mod.create_buffer_obj();

	//for (int i = 0; i < SPRITE_NUM; i++){
	//	sprite[i].load_model("../../data/huangrong.obj");
		//sprite[i].load_model("../../data/body.obj");
	//	sprite[i].to_vbo();
		//sprite[i].x = rand() % 100;
		//sprite[i].z = rand() % 100;
		//sprite[i].scale(((double)(50+rand() % 50))/100);
	//}


	GLuint vertexShader = loadShaderFromFile(GL_VERTEX_SHADER, "../../data/vert.glsl");
	GLuint fragmentShader = loadShaderFromFile(GL_FRAGMENT_SHADER, "../../data/frag.glsl");
	GLuint program = setupProgram(vertexShader, fragmentShader);

	glUseProgram(program);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 800.0 / 600.0, 1.0, 1240.0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//cout << sprite[0].x << ',' << sprite[0].y + 50 << ',' << sprite[0].z - 120<<'\n';

	gluLookAt(0, 60,60,
		0,0,40,
		0,0,1);
	//SetupLights();

	//GLfloat view_matrix[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "view_mat"), 1, GL_TRUE, view_matrix);

	//GLfloat projection_matrix[16];
	//glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "proj_mat"), 1, GL_TRUE, projection_matrix);
	//	GLint program;

	glUniform3f(glGetUniformLocation(program, "light_dir"), 1, 0, 0);
	glUniform4f(glGetUniformLocation(program, "light_color"), 1, 1, 1, 1);

	// Get attribute slot from program
	//
	
	//GLint _positionSlot = glGetAttribLocation(program, "position");

	//cout << "vVertex" << _positionSlot << '\n';
	
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
	/*
	float mat1[16] =
	{ 1, 1, 0, 100,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 };
	float mat1_inv[16];
	mat4x4_inverse(mat1, mat1_inv);
	float vector[4] =
	{ 1, 0.5, 0, 1};
	float result[4];
	float result2[4];
	mat4x4_mul_vector(mat1, vector, result);
	printf("%f,%f,%f,%f\n", result[0], result[1], result[2], result[3]);
	mat4x4_mul_vector(mat1_inv, result, result2);
	printf("%f,%f,%f,%f\n", result2[0], result2[1], result2[2], result2[3]);
	*/
	//auto node =load_xml("../../data/skin.DAE");

	//std::cout << node->name << '\n';
	

	//printf("version:%s\n", FreeImage_GetVersion());

    //const GLubyte* x =glGetString(GL_VENDOR);
    //std::cout << "vendor:" << glGetString(GL_VENDOR) << "\n";
    //std::cout << "render:" << glGetString(GL_RENDER) << "\n";
    //std::cout << "version:" << glGetString(GL_VERSION) << "\n";
    //std::cout << "extensions:" << glGetString(GL_EXTENSIONS) << "\n";
    
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

    setupRC();



    //gluPerspective(60, 800 / 600, 1, 10);

    //glOrtho(-100, 100.0, -100, 100.0, 100, -100);
    glutMainLoop();

    return (EXIT_SUCCESS);
}
