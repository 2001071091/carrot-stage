#pragma once

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"carrot-stage.lib")
#endif

#include <GL\glew.h>
#include <vector>

using namespace std;

struct Material{
	GLfloat emission[4];//�Է���(ֱ�ӷ���,����������Ӱ��,��������Ӱ)
	GLfloat ambient[4];//������
	GLfloat diffuse[4];//������
	GLfloat specular[4];//�߹�
	GLfloat shininess;//�߹�ǿ��
	//GLfloat reflective[4];//����
	//GLfloat reflectivity;//��������
	//int transparent_opaue;//͸��
	//GLfloat transparency;//͸����
};

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
	vector<GLuint> bones;		//�����������,һ������4������
	vector<GLfloat> weights;	//�������Ȩֵ,һ������4��Ȩֵ
	vector<GLuint> indics;		//��������(��������,��ɫ��ͼ,������ͼ,Ԥ��һλ)

	/*��ͼ���ݼ���*/
	vector<void*> textures;

	/*��������*/


	GLuint vbo_indics[6];//6�ֶ������ݵĵ�VBO index(λ�á����ߡ���ͼ���ꡢ��������������Ȩֵ����������)
	GLuint pbo_indics[10];//֧�����10����ͼ��PBO index

	void load_dae(const char*);
};

