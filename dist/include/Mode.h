#pragma once

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"carrot-stage.lib")
#endif

#include <GL\glew.h>
#include <vector>

using namespace std;

class Mode
{
public:
	Mode();
	~Mode();

	void load(const char*);
	void create_buffer_obj();//����Buffer Object
	void delete_buffer_obj();//ɾ��Buffer Object
	void render();
private:
	/*��������*/
	vector<GLfloat> verts;		//����λ��
	vector<GLfloat> norms;		//���㷨��
	vector<GLfloat> texcoords;	//������ͼ����
	vector<GLfloat> indics;		//��������
	vector<GLuint> bones;		//�����������,һ������4������
	vector<GLfloat> weights;	//�������Ȩֵ,һ������4��Ȩֵ

	/*��ͼ���ݼ���*/
	vector<void*> textures;

	/*��������*/
	unsigned int materials_count;
	GLfloat* materials;

	void load_dae(const char*);

	/*Buffer Object �������*/
	bool in_bo;//�Ƿ���buffer object��
	GLuint vbo_indics[6];//6��VBO
	GLuint *tbo_indics;//��̬��������ͼ����
	GLuint *tex_ids;//��̬��������ͼID

	GLenum front_face = GL_CCW;
	//GLuint ubo_indics[.];
};

