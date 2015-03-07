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
	#pragma comment(lib,"opengl32.lib")
	#pragma comment(lib,"carrot-stage.lib")
#endif

#include <GL/gl.h>
#include <vector>

using namespace std;

namespace com_yoekey_3d {

	class Sprite {
	public:
		Sprite();
		virtual ~Sprite();

		void load_model(const char* file);
		int load_anim(const char* file);
		void render();

		float x, y, z;
		float rotaion;
		float scaleX, scaleY, scaleZ;
		void scale(GLfloat);
	private:
		vector<GLfloat> verts;
		vector<GLfloat> norms;
		vector<GLfloat> texcoords;

		vector<GLint*> indics;

		void loadFrom_obj(const char* path);
	};
}

#endif	/* COM_YOEKEY_3D_SPRITE_H */

