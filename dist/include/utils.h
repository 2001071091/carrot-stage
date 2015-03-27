#ifndef COM_YOEKEY_3D_UTILS_H
#define	COM_YOEKEY_3D_UTILS_H


#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "glew32.lib")
#endif

#include <GL\glew.h>

namespace com_yoekey_3d {
	typedef struct
	{
		int         w;
		int             h;
		unsigned char           *buf;
		GLuint          rgb_mode;
	}GLBITMAP;
	struct xml_node_attrib{
		char* name;
		char* value;
	};
	struct xml_node{
		unsigned int attrib_count;
		unsigned int child_count;

		char* name;
		char* inner_text;

		xml_node_attrib* attribs;
		xml_node* children;
	};
	struct xml_node_token{
		char* name;
		unsigned int attrib_count;
		xml_node_attrib* attribs;
		bool end;//是否结束标签
		bool single;//是否是独立标签
		bool text;
	};
	void obj_split_face_cell(char* cell, GLint* is);
	GLuint loadTexture(const char *filename);
	GLBITMAP* loadImage(const char *filename);
	void FreeGLBitmap(GLBITMAP *glbmp);
	GLuint loadShader(GLenum type,const char *src);
	GLuint loadShaderFromFile(GLenum type,const char *path);
	GLuint setupProgram(GLuint vertexShader, GLuint fragmentShader);
	xml_node* load_xml(const char *filename);
	void free_xml_node(xml_node*);
	const char* get_xml_node_attrib(xml_node node, const char* name);
}

#endif