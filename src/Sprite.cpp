/*
* File:   ModelPaser.cpp
* Author: Administrator
*
* Created on 2015��2��28��, ����12:56
*/

#include "Sprite.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace com_yoekey_3d;

inline void push(vector<GLfloat>* vsrc, vector<GLfloat>* vdes, int index, int num){
	for (int i = 0; i < num; i++){
		(*vdes).push_back((*vsrc)[index*num + i]);
	}
}

void Sprite::clear_vbo(){
	if (!use_vbo)return;
	glDeleteBuffers(3, vbo_indics);
	use_vbo = false;
}

void Sprite::to_vbo(){
	if (use_vbo)return;
	glGenBuffers(3, vbo_indics);
	//写入顶点数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[0]);
	glBufferData(GL_ARRAY_BUFFER, verts->size()*sizeof(GLfloat), &(*verts)[0], GL_STATIC_DRAW);
	//写入法线数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[1]);
	glBufferData(GL_ARRAY_BUFFER, norms->size()*sizeof(GLfloat), &(*norms)[0], GL_STATIC_DRAW);
	//写入贴图坐标数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[2]);
	glBufferData(GL_ARRAY_BUFFER, texcoords->size()*sizeof(GLfloat), &(*texcoords)[0], GL_STATIC_DRAW);
	use_vbo = true;
}



void Sprite::load_model(const char* path){
	const char * ext =strrchr(path, '.');
	if (strcmp(ext, ".obj") == 0
		|| strcmp(ext, ".OBJ") == 0){
		loadFrom_obj(path);
	}
	else if (strcmp(ext, ".dae") == 0 
		|| strcmp(ext, ".DAE") == 0){
		loadFrom_dae(path);
	}
}

void Sprite::loadFrom_dae(const char* path){
	ifstream fin;
	fin.open(path, ios_base::in);
	if (fin.fail()) {
		cout << "file open error\n";
		return;
	}


	fin.close();
}

void Sprite::loadFrom_obj(const char* path) {
	char root[255];
	vector<GLfloat> _verts;
	vector<GLfloat> _norms;
	vector<GLfloat> _texcoords;
	verts = new vector<GLfloat>;
	norms = new vector<GLfloat>;
	texcoords = new vector<GLfloat>;
	memcpy(root, path, strlen(path) + 1);
	strrchr(root, '/')[1] = 0;

	ifstream fin;
	fin.open(path, ios_base::in);
	if (fin.fail()) {
		cout << "file open error\n";
		return;
	}
	char buff[255];
	while (true) {
	#ifdef _WIN32
			fin.getline(buff, 255, '\n');
			if (fin.fail()) {
				break;
			}
	#else
			fin.getline(buff, 255,'\r');
			if (fin.fail()) {
				break;
			}
			fin.get();
	#endif // _WIN32

		if (buff[0] == '#' || buff[0] == 0)continue;
		char* token = strtok(buff, " ");
		if (strcmp(token, "mtllib") == 0) {
			//import mtllib
			//strcat(root, strtok(NULL, " "));
			//cout<<root;
		}
		if (strcmp(token, "g") == 0) {
			token = strtok(NULL, " ");
			if (strcmp(token, "Bip01")==0)break;
		}
		if (strcmp(token, "v") == 0) {
			_verts.push_back(atof(strtok(NULL, " ")));
			_verts.push_back(atof(strtok(NULL, " ")));
			_verts.push_back(atof(strtok(NULL, " ")));
		}
		if (strcmp(token, "vn") == 0) {
			_norms.push_back(atof(strtok(NULL, " ")));
			_norms.push_back(atof(strtok(NULL, " ")));
			_norms.push_back(atof(strtok(NULL, " ")));
		}
		if (strcmp(token, "vt") == 0) {
			_texcoords.push_back(atof(strtok(NULL, " ")));
			_texcoords.push_back(atof(strtok(NULL, " ")));
		}
		if (strcmp(token, "f") == 0) {
			char* cells[4];
			for (int i = 0; i < 4; i++) {
				cells[i] = strtok(NULL, " ");
			}
			GLint is[3];
			for (int i = 0; i < 4; i++) {
				if (cells[i] != NULL) {
					obj_split_face_cell(cells[i], is);
					//vertor is[0]
					//uv is[1]
					//norms is[2]
					push(&_verts, verts, is[0] - 1, 3);
					push(&_texcoords, texcoords, is[1] - 1, 2);
					push(&_norms, norms, is[2] - 1, 3);
				}
				else {
					push(verts, verts, verts->size() / 3 - 1, 3);
					push(texcoords, texcoords, texcoords->size() / 2 - 1, 2);
					push(norms, norms, norms->size() / 3 - 1, 3);
				}
			}
		}
	}

	//std::cout << "verts:" << verts->size() << "\n";
	fin.close();
}

void Sprite::render() {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(rotaion, 0, 1, 0);
	if (scaleX != 1 || scaleY != 1 || scaleZ != 1)glScaled(scaleX, scaleY, scaleZ);

	/*
	处理纹理
	*/

	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if (program > 0){
		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		glUniformMatrix4fv(glGetUniformLocation(program, "model_mat"), 1, GL_TRUE, matrix);
		glUniform1i(glGetUniformLocation(program, "color_map"), 0);
	}
	if (use_vbo){
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_QUADS, 0, verts->size()/3);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}else{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &(*verts)[0]);
		glNormalPointer(GL_FLOAT, 0, &(*norms)[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &(*texcoords)[0]);

		glDrawArrays(GL_QUADS, 0, verts->size() / 3);
	}

	glPopMatrix();
}

void Sprite::scale(GLfloat scale){
	scaleX = scale;
	scaleY = scale;
	scaleZ = scale;
}

Sprite::Sprite() {
	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;
}

Sprite::~Sprite() {
}
