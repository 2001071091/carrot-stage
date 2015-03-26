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

	void render();
private:
	vector<GLfloat>* verts;		//����λ��
	vector<GLfloat>* norms;		//���㷨��
	vector<GLfloat>* texcoords;	//������ͼ����
	vector<GLuint>* bones;		//�����������,һ������4������
	vector<GLfloat>* weights;	//�������Ȩֵ,һ������4��Ȩֵ
	vector<GLuint>* indics;		//��������(��������,��ɫ��ͼ,������ͼ,Ԥ��һλ)

	GLuint vbo_indics[6];//6�ֶ������ݵĵ�VBO index(λ�á����ߡ���ͼ���ꡢ��������������Ȩֵ����������)
	GLuint pbo_indics[10];//֧�����10����ͼ��PBO index
};

