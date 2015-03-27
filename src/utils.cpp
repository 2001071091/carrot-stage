#include "utils.h"
#include <FreeImage.h>


#include <fstream>
#include <iostream>
#include <vector>

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

	xml_node_attrib read_xml_node_attrib(FILE* fp){
		char* name = (char*)malloc(255);
		int index = 0;
		char* value = NULL;
		char c;
		//忽略开头的无效字符
		while (true){
			c = fgetc(fp);
			if (c == EOF)break;//读到文件尾
			if (c == ' ')continue;
			fseek(fp, -1,SEEK_CUR);
			break;
		}
		while (true){
			c = fgetc(fp);
			if (c == EOF)break;//读到文件尾
			if (c==' '||c == '/' || c == '>'){//属性结束
				break;
			}
			if (c == '='){
				//开始读取value
				int size = 255;
				int vindex = 0;
				value = (char*)malloc(size);
				//读取字符串使用的单引号还是双引号
				c = fgetc(fp);
				if (c == EOF)break;
				if (c == '\"' || c == '\''){
					char end = c;
					while (true){//读取属性
						c = fgetc(fp);
						if (c == EOF)break;
						if (c == end){//属性读取完毕
							break;
						}
						else if(c=='&'){//转义
							char token[4];
							int len = 0;
							for (int i = 0; i < 5; i++){
								c = fgetc(fp);
								if (c == EOF)break;
								if (c == ';')break;
								token[i] = c;
								len++;
							}
							char tc = ' ';
							switch (len){
							case 2:
								if (token[0] == 'l'&&token[1] == 't'){
									tc = '<';
								}
								else if (token[0] == 'g'&&token[1] == 't'){
									tc = '>';
								}
								break;
							case 3:
								if (token[0] == 'a'&&token[1] == 'm'&&token[2] == 'p'){
									tc = '&';
								}
								break;
							case 4:
								if (token[0] == 'a'&&token[1] == 'p'&&token[2] == 'o'&&token[3] == 's'){
									tc = '\'';
								}
								else if (token[0] == 'q'&&token[1] == 'u'&&token[2] == 'o'&&token[3] == 't'){
									tc = '\"';
								}
								break;
							}
							if (vindex == size){//动态扩容
								char* tmp = (char*)malloc(size * 2);
								memcpy(tmp, value, size);
								size *= 2;
								free(value);
								value = tmp;
							}
							value[vindex++] = tc;
						}
						else{
							if (vindex == size){//动态扩容
								char* tmp = (char*)malloc(size * 2);
								memcpy(tmp, value, size);
								size *= 2;
								free(value);
								value = tmp;
							}
							value[vindex++] = c;
						}
					}
				}
				value[vindex++] = 0;
				break;//属性读取完毕
			}
			else{
				name[index++] = c;
			}
		}
		name[index++] = 0;
		xml_node_attrib result;
		result.name = name;
		result.value = value;
		//printf("attrib\t%s:%s\n", name, value);
		return result;
	}

	xml_node_token read_xml_node_token(FILE* fp){
		char *name =(char*)malloc(255);
		xml_node_attrib* attribs = NULL;
		int index = 0;
		unsigned int count = 0;
		char c;
		bool end = false;
		bool single = false;
		long offset = 0;
		while (true){
			c = fgetc(fp);
			if (c == EOF)break;
			offset++;
			if (c == ' ' || c == '\n' || c == '\r')continue;//忽略掉节点外地无效字符
			if (c == '<'){//xml节点开始
				c = fgetc(fp);
				if (c == EOF)break;
				if (c == '/'){//结束标记
					end = true;
				}
				else if (c == '?'){
					//xml的版本信息节点
				}
				else{
					fseek(fp, -1, SEEK_CUR);
				}
				while (true){
					c = fgetc(fp);
					if (c == EOF)break;
					if (c == ' '){//xml节点名称结束,读取属性列表
						if (attribs == NULL){
							attribs = (xml_node_attrib*)malloc(sizeof(xml_node_attrib)*(count + 1));
						}
						else{
							//扩容
							xml_node_attrib* tmp = (xml_node_attrib*)malloc(sizeof(xml_node_attrib)*(count + 1));
							memcpy(tmp, attribs, sizeof(xml_node_attrib)*count);
							free(attribs);
							attribs = tmp;
						}
						attribs[count] = read_xml_node_attrib(fp);
						count++;

					}
					else if (c == '>'){//xml节点名称结束,左节点结束
						break;
					}
					else if (c == '/' || c == '?'){//无子节点和innerText的节点结束
						c = fgetc(fp);
						if (c == EOF)break;
						if (c != '>'){
							printf("error xml node end,'/' or '?' need '>' fellow");
							xml_node_token result;
							result.text = true;
							return result;
						}
						single = true;
						if (end){
							printf("error xml node end");
							xml_node_token result;
							result.text = true;
							return result;
						}
						//xml节点名称结束且节点结束
						break;
					}
					else{//记录xml节点名称
						name[index++] = c;
					}
				}
				break;
			}
			else{
				//回滚
				fseek(fp, -offset, SEEK_CUR);
				free(name);
				xml_node_token result;
				result.text = true;
				return result;//不是'<'开头的表示非节点标识
			}
		}
		name[index] = 0;
		xml_node_token result;
		result.name = name;
		result.end = end;
		result.single = single;
		result.attrib_count = count;
		result.attribs = attribs;
		result.text = false;
		//printf("node[%s],end[%d],single[%d],\n", name, end, single);
		return result;
	}
	void free_xml_node(xml_node node){
		//printf("free node[%s]\n", node.name);
		free(node.name);
		free(node.inner_text);
		for (int i = 0; i < node.attrib_count; i++){
			free(node.attribs[i].name);
			free(node.attribs[i].value);
		}
		free(node.attribs);
		for (int i = 0; i < node.child_count; i++){
			free_xml_node(node.children[i]);
		}
		free(node.children);
	}
	void free_xml_node(xml_node* node){
		//printf("free node[%s]\n", node->name);
		free(node->name);
		free(node->inner_text);
		for (int i = 0; i < node->attrib_count; i++){
			free(node->attribs[i].name);
			free(node->attribs[i].value);
		}
		free(node->attribs);
		for (int i = 0; i < node->child_count; i++){
			free_xml_node(node->children[i]);
		}
		free(node->children);
		free(node);
	}
	const char* get_xml_node_attrib(xml_node node, const char* name){
		for (int i = 0; i < node.attrib_count; i++){
			if (strcmp(node.attribs[i].name, name) == 0)
				return node.attribs[i].value;
		}
		return NULL;
	}

	xml_node* load_xml_node(FILE* fp){
		xml_node_token token =read_xml_node_token(fp);
		char* inner_text = NULL;
		unsigned int count = 0;
		xml_node* children = NULL;
		if (token.text){
			return (xml_node*)1;
		}
		else if(token.single){
			//独立的节点情况
		}
		else if(token.end){
			//结束节点
			return NULL;
		}
		else{
			//尝试获取子节点
			while (true){
				xml_node* sub = load_xml_node(fp);
				if ((int)sub == 1){
					int size = 255;
					int index = 0;
					inner_text = (char*)malloc(size);
					char c;
					//获取非子节点信息
					while (true){
						c = fgetc(fp);
						if (c == EOF)break;
						if (c == '<'){
							//读取结束
							fseek(fp, -1, SEEK_CUR);
							break;
						}
						else{
							if (index == size){//动态扩容
								char* tmp = (char*)malloc(size * 2);
								memcpy(tmp, inner_text, size);
								size *= 2;
								free(inner_text);
								inner_text = tmp;
							}
							inner_text[index++] = c;
						}
					}
					inner_text[index] = 0;
				}else if (sub == NULL)break;//遇到结束节点则跳出
				else{
					if (children == NULL){
						children = (xml_node*)malloc(sizeof(xml_node)*(count + 1));
					}
					else{
						//扩容
						xml_node* tmp = (xml_node*)malloc(sizeof(xml_node)*(count + 1));
						memcpy(tmp, children, sizeof(xml_node)*count);
						free(children);
						children = tmp;
					}
					children[count] = *sub;
					free(sub);//释放掉sub
					count++;
				}
			}
		}
		xml_node* result = (xml_node*)malloc(sizeof(xml_node));
		result->name = token.name;
		result->attribs = token.attribs;
		result->inner_text = inner_text;
		result->children = children;

		result->attrib_count = token.attrib_count;
		result->child_count = count;

		//printf("node[%s],attrib_count[%d],child_count[%d],has_inner[%d]\n", token->name, token->attrib_count, count, inner_text != NULL);
		return result;
	}
	xml_node* load_xml(const char* path){
		FILE* fp;
		fp = fopen(path, "rb");
		if (fp == NULL){
			printf("file not found!\n");
			return NULL;
		}
		//把文件的位置指针移到文件尾
		//fseek(fp, 0L, SEEK_END);
		//获取文件长度;
		//long length = ftell(fp);
		//fseek(fp, 0L, SEEK_SET);
		xml_node* version = load_xml_node(fp);
		free_xml_node(version);

		xml_node* root = load_xml_node(fp);
		fclose(fp);
		return root;
	}
}


void com_yoekey_3d::obj_split_face_cell(char* cell, GLint* is){
	char* tmp;
	tmp = strtok(cell, "/");
	is[0] = tmp[0] == 0 ? 0 : atoi(tmp);
	tmp = strtok(NULL, "/");
	is[1] = tmp[0] == 0 ? 0 : atoi(tmp);
	tmp = strtok(NULL, "/");
	is[2] = tmp[0] == 0 ? 0 : atoi(tmp);
}

void com_yoekey_3d::FreeGLBitmap(GLBITMAP *glbmp)
{
	if (glbmp) {
		if (glbmp->buf) free(glbmp->buf);
		free(glbmp);
	}
}

GLBITMAP* com_yoekey_3d::loadImage(const char *filename){
#ifdef FREEIMAGE_LIB
	FreeImage_Initialise(FALSE);
#endif  
	GLuint tex = 0;
	FIBITMAP *bitmap = NULL;
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	GLBITMAP *glbmp = NULL;

	fif = FreeImage_GetFileType(filename, 0);
	if (FIF_UNKNOWN == fif) {
		//printf("unknown type: %d\n", fif);
		fif = FreeImage_GetFIFFromFilename(filename);
		if (FIF_UNKNOWN == fif)
			return NULL;
	}
	if (FreeImage_FIFSupportsReading(fif))
		bitmap = FreeImage_Load(fif, filename, 0);

	if (!bitmap)
		return NULL;

	//printf("bit: %d\n", FreeImage_GetBPP(bitmap));

	glbmp = FIBitmap2GLBitmap(bitmap);
	if (!glbmp)
		return NULL;

	FreeImage_Unload(bitmap);

#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif  
	return glbmp;
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
