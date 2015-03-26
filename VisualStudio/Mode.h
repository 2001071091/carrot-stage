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
	vector<GLfloat>* verts;		//顶点位置
	vector<GLfloat>* norms;		//顶点法线
	vector<GLfloat>* texcoords;	//顶点贴图坐标
	vector<GLuint>* bones;		//顶点关联骨骼,一个顶点4个骨骼
	vector<GLfloat>* weights;	//顶点骨骼权值,一个顶点4个权值
	vector<GLuint>* indics;		//索引集合(材质索引,颜色贴图,法线贴图,预留一位)

	GLuint vbo_indics[6];//6种顶点数据的的VBO index(位置、法线、贴图坐标、骨骼关联、骨骼权值、索引集合)
	GLuint pbo_indics[10];//支持最多10张贴图的PBO index
};

