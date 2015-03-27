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
	GLfloat emission[4];//自发光(直接发光,不受其他光影响,不接受阴影)
	GLfloat ambient[4];//环境光
	GLfloat diffuse[4];//漫反射
	GLfloat specular[4];//高光
	GLfloat shininess;//高光强度
	//GLfloat reflective[4];//反射
	//GLfloat reflectivity;//反射能力
	//int transparent_opaue;//透明
	//GLfloat transparency;//透明度
};

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
	vector<GLuint> bones;		//顶点关联骨骼,一个顶点4个骨骼
	vector<GLfloat> weights;	//顶点骨骼权值,一个顶点4个权值
	vector<GLuint> indics;		//索引集合(材质索引,颜色贴图,法线贴图,预留一位)

	/*贴图数据集合*/
	vector<void*> textures;

	/*材质数据*/


	GLuint vbo_indics[6];//6种顶点数据的的VBO index(位置、法线、贴图坐标、骨骼关联、骨骼权值、索引集合)
	GLuint pbo_indics[10];//支持最多10张贴图的PBO index

	void load_dae(const char*);
};

