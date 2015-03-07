#ifndef COM_YOEKEY_3D_UTILS_H
#define	COM_YOEKEY_3D_UTILS_H


#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

namespace com_yoekey_3d {
	typedef struct
	{
		int         w;
		int             h;
		unsigned char           *buf;
		GLuint          rgb_mode;
	}GLBITMAP;

	void obj_split_face_cell(char* cell, GLint* is);
	GLuint loadTexture(const char *filename);
	void FreeGLBitmap(GLBITMAP *glbmp);
}

#endif