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
	void create_buffer_obj();//创建Buffer Object
	void delete_buffer_obj();//删除Buffer Object
	void render();
private:
	/*顶点数据*/
	vector<GLfloat> verts;		//顶点位置
	vector<GLfloat> norms;		//顶点法线
	vector<GLfloat> texcoords;	//顶点贴图坐标
	vector<GLfloat> indics;		//材质索引
	vector<GLuint> bones;		//顶点关联骨骼,一个顶点4个骨骼
	vector<GLfloat> weights;	//顶点骨骼权值,一个顶点4个权值

	/*贴图数据集合*/
	vector<void*> textures;

	/*材质数据*/
	unsigned int materials_count;
	GLfloat* materials;

	void load_dae(const char*);

	/*Buffer Object 相关属性*/
	bool in_bo;//是否在buffer object中
	GLuint vbo_indics[6];//6个VBO
	GLuint *tbo_indics;//动态数量的贴图索引
	GLuint *tex_ids;//动态数量的贴图ID

	GLenum front_face = GL_CCW;
	//GLuint ubo_indics[.];
};

