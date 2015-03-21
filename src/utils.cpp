#include "utils.h"
#include <FreeImage.h>


#include <fstream>
#include <iostream>

using com_yoekey_3d::GLBITMAP;

namespace com_yoekey_3d{
	GLBITMAP* FIBitmap2GLBitmap(FIBITMAP *fibmp){
		int i, j, k;
		int pitch = FreeImage_GetPitch(fibmp);
		unsigned char *bits = FreeImage_GetBits(fibmp);
		int bpp = FreeImage_GetBPP(fibmp);
		GLBITMAP *glbmp = (GLBITMAP *)malloc(sizeof(GLBITMAP));
		RGBQUAD *palette = NULL;

		if (!glbmp) return NULL;

		glbmp->w = FreeImage_GetWidth(fibmp);
		glbmp->h = FreeImage_GetHeight(fibmp);

		switch (bpp) {
		case 8:
			if (!(palette = FreeImage_GetPalette(fibmp))) return NULL;
			if (!(glbmp->buf = (unsigned char *)malloc(glbmp->w*glbmp->h * 3))) return NULL;
			glbmp->rgb_mode = GL_RGB;
			for (i = 0; i < glbmp->h; ++i) {
				for (j = 0; j < glbmp->w; ++j) {
					k = bits[i*pitch + j];
					glbmp->buf[(i*glbmp->w + j) * 3 + 0] = palette[k].rgbRed;
					glbmp->buf[(i*glbmp->w + j) * 3 + 1] = palette[k].rgbGreen;
					glbmp->buf[(i*glbmp->w + j) * 3 + 2] = palette[k].rgbBlue;
				}
			}
			break;
		case 24:
			if (!(glbmp->buf = (unsigned char *)malloc(glbmp->w*glbmp->h * 3))) return NULL;
			glbmp->rgb_mode = GL_RGB;
			for (i = 0; i < glbmp->h; ++i) {
				for (j = 0; j < glbmp->w; ++j) {
					glbmp->buf[(i*glbmp->w + j) * 3 + 0] = bits[i*pitch + j * 3 + 2];
					glbmp->buf[(i*glbmp->w + j) * 3 + 1] = bits[i*pitch + j * 3 + 1];
					glbmp->buf[(i*glbmp->w + j) * 3 + 2] = bits[i*pitch + j * 3 + 0];
				}
			}
			break;
		case 32:
			if (!(glbmp->buf = (unsigned char *)malloc(glbmp->w*glbmp->h * 4))) return NULL;
			glbmp->rgb_mode = GL_RGBA;
			for (i = 0; i < glbmp->h; ++i) {
				for (j = 0; j < glbmp->w; ++j) {
					glbmp->buf[(i*glbmp->w + j) * 4 + 0] = bits[i*pitch + j * 4 + 2];
					glbmp->buf[(i*glbmp->w + j) * 4 + 1] = bits[i*pitch + j * 4 + 1];
					glbmp->buf[(i*glbmp->w + j) * 4 + 2] = bits[i*pitch + j * 4 + 0];
					glbmp->buf[(i*glbmp->w + j) * 4 + 3] = bits[i*pitch + j * 4 + 3];
				}
			}
			break;
		default: return NULL;
		}

		return glbmp;
	}

}
void com_yoekey_3d::obj_split_face_cell(char* cell, GLint* is){
	is[0] = atoi(strtok(cell, "/"));
	is[1] = atoi(strtok(NULL, "/"));
	is[2] = atoi(strtok(NULL, "/"));
}

void com_yoekey_3d::FreeGLBitmap(GLBITMAP *glbmp)
{
	if (glbmp) {
		if (glbmp->buf) free(glbmp->buf);
		free(glbmp);
	}
}


GLuint com_yoekey_3d::loadTexture(const char *filename)
{
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise(FALSE);
	#endif  
	GLuint tex = 0;
	FIBITMAP *bitmap = NULL;
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	
	GLBITMAP *glbmp = NULL;

	fif = FreeImage_GetFileType(filename, 0);
	if (FIF_UNKNOWN == fif) {
		fif = FreeImage_GetFIFFromFilename(filename);
		if (FIF_UNKNOWN == fif)
			return 0;
	}
	if (FreeImage_FIFSupportsReading(fif))
		bitmap = FreeImage_Load(fif, filename, 0);

	if (!bitmap)
		return 0;

	//printf("bit: %d\n", FreeImage_GetBPP(bitmap));

	glbmp = FIBitmap2GLBitmap(bitmap);
	if (!glbmp)
		return 0;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, glbmp->rgb_mode, glbmp->w, glbmp->h, 0, glbmp->rgb_mode, GL_UNSIGNED_BYTE, glbmp->buf);

	FreeGLBitmap(glbmp);
	FreeImage_Unload(bitmap);

	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif  
	return tex;
}
GLuint com_yoekey_3d::loadShaderFromFile(GLenum type, const char *path){

	FILE* fp;
	fp = fopen(path, "rb");
	if (fp == NULL){
		printf("file not found!\n");
		return 0;
	}
	//把文件的位置指针移到文件尾
	fseek(fp, 0L, SEEK_END);
		//获取文件长度;
	long length = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char *src = new char[length+1];
	fread(src, length, 1, fp);
	src[length] = 0;
	
	//printf("%s\n", src);
	GLuint result = loadShader(type, src);

	fclose(fp);
	delete src;
	return result;
}

GLuint com_yoekey_3d::loadShader(GLenum type,const char *src){
	// Create the shader object
	GLuint shader = glCreateShader(type);
	if (shader == 0) {
		std::cout << "Error: failed to create shader.\n";
		return 0;
	}

	// Load the shader source
	glShaderSource(shader, 1, &src, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1) {
			char * infoLog = new char[infoLen];
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			std::cout << infoLog << '\n';
			delete infoLog;
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}


GLuint com_yoekey_3d::setupProgram(GLuint vertexShader, GLuint fragmentShader){

	// Create program, attach shaders.
	GLuint _programHandle = glCreateProgram();
	if (!_programHandle) {
		std::cout<<"Failed to create program.\n";
		return 0;
	}

	glAttachShader(_programHandle, vertexShader);
	glAttachShader(_programHandle, fragmentShader);

	/*
	va_list attributeList;
	va_start(attributeList, fragmentShader);
	char *nextArg;
	int iArgNum = va_arg(attributeList, int);
	for (int i = 0; i < iArgNum; i++){
		int index = va_arg(attributeList, int);
		nextArg = va_arg(attributeList, char*);
		glBindAttribLocation(_programHandle, index, nextArg);
	}
	va_end(attributeList);
	*/

	// Link program
	//
	glLinkProgram(_programHandle);

	// Check the link status
	GLint linked;
	glGetProgramiv(_programHandle, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(_programHandle, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char * infoLog = new char[infoLen];
			glGetProgramInfoLog(_programHandle, infoLen, NULL, infoLog);
			std::cout << infoLog << '\n';
			delete infoLog;
		}

		glDeleteProgram(_programHandle);
		_programHandle = 0;
		return 0;
	}

	return _programHandle;

	//glUseProgram(_programHandle);

	// Get attribute slot from program
	//
	//_positionSlot = glGetAttribLocation(_programHandle, "vPosition");
}
