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

void Sprite::load_model(const char* path){
	const char * ext =strrchr(path, '.');
	if (strcmp(ext, ".obj") == 0){
		loadFrom_obj(path);
	}
}

void Sprite::loadFrom_obj(const char* path) {
	char root[255];
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
		//
		if (strcmp(token, "g") == 0) {
			//cout << "��: " << strtok(NULL, " ") << "\n";
		}
		//v
		if (strcmp(token, "v") == 0) {
			verts.push_back(atof(strtok(NULL, " ")));
			verts.push_back(atof(strtok(NULL, " ")));
			verts.push_back(atof(strtok(NULL, " ")));
		}
		//vn ��������
		if (strcmp(token, "vn") == 0) {
			norms.push_back(atof(strtok(NULL, " ")));
			norms.push_back(atof(strtok(NULL, " ")));
			norms.push_back(atof(strtok(NULL, " ")));
		}
		//vn ��������
		if (strcmp(token, "vt") == 0) {
			texcoords.push_back(atof(strtok(NULL, " ")));
			texcoords.push_back(atof(strtok(NULL, " ")));
		}
		//f
		if (strcmp(token, "f") == 0) {
			char* cells[4];
			for (int i = 0; i < 4; i++) {
				cells[i] = strtok(NULL, " ");
			}
			GLint *_indics = new GLint[12];
			GLint is[3];
			for (int i = 0; i < 4; i++) {
				if (cells[i] != NULL) {
					obj_split_face_cell(cells[i], is);
					_indics[i * 3] = is[0]; //vertor
					_indics[i * 3 + 1] = is[1]; //uv
					_indics[i * 3 + 2] = is[2]; //norms
				}
				else {
					_indics[i * 3] = 0; //vertor
					_indics[i * 3 + 1] = 0; //uv
					_indics[i * 3 + 2] = 0; //norms
				}
			}
			indics.push_back(_indics);
		}
	}

	//std::cout << "verts:" << verts.size() << "\n";
	//std::cout << "norms:" << norms.size() << "\n";
	//std::cout << "texcoords:" << texcoords.size() << "\n";
	//std::cout << "indics:" << indics.size() << "\n";
	fin.close();
}

void Sprite::render() {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(rotaion, 0, 1, 0);
	if (scaleX != 1 || scaleY != 1 || scaleZ != 1)glScaled(scaleX, scaleY, scaleZ);
	for (int i = 0; i < indics.size(); i++) {
		GLint* _indics = indics[i];
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++) {
			int v_index = (_indics[j * 3] - 1) * 3;
			int t_index = (_indics[j * 3 + 1] - 1) * 2;
			int n_index = (_indics[j * 3 + 2] - 1) * 3;
			if (v_index <= 0)continue;
			glTexCoord2fv(&texcoords[t_index]);
			glNormal3fv(&norms[n_index]);
			glVertex3fv(&verts[v_index]);
		}
		glEnd();
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
