/*
* File:   Sprite.h
* Author: xiaolei
*
* Created on March 1, 2015, 5:20 PM
*/

#ifndef COM_YOEKEY_3D_SPRITE_H
#define	COM_YOEKEY_3D_SPRITE_H

#ifdef _WIN32
	#include <windows.h>
	#pragma comment(lib, "glew32.lib")
	#pragma comment(lib,"carrot-stage.lib")
#endif

#include <GL\glew.h>
#include <vector>

using namespace std;

namespace com_yoekey_3d {

	class Sprite {
	public:
		Sprite();
		virtual ~Sprite();

		void load_model(const char*);
		void render();

		float x, y, z;
		float rotaion;
		float scaleX, scaleY, scaleZ;
		void scale(GLfloat);
		void to_vbo();
		void clear_vbo();
	private:
		vector<GLfloat>* verts;		//顶点位置
		vector<GLfloat>* norms;		//顶点法线
		vector<GLfloat>* texcoords;	//顶点贴图坐标
		vector<GLuint>* bones;		//顶点关联骨骼
		vector<GLfloat>* weights;	//顶点骨骼权值

		bool use_vbo = false;
		GLuint vbo_indics[3];
		void loadFrom_obj(const char*);
		void loadFrom_dae(const char*);
		//void push(vector<GLfloat>* vsrc, vector<GLfloat>* vdes, int index, int num);
	};
}

#endif	/* COM_YOEKEY_3D_SPRITE_H */

