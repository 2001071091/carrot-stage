#include "Mode.h"
#include "utils.h"
using namespace com_yoekey_3d;

Mode::Mode()
{
	materials = NULL;
	tbo_indics = NULL;
	tex_ids = NULL;
}

Mode::~Mode()
{
	delete_buffer_obj();//删除buffer object
	//释放图形对象
	for (int i = 0; i < textures.size(); i++){
		FreeGLBitmap((GLBITMAP*)textures[i]);
	}
	if (tbo_indics != NULL)free(tbo_indics);
	if (tex_ids != NULL)free(tex_ids);
	if (materials != NULL)free(materials);
}
//字符串比较
#define EQUAL(a,b) strcmp(a, b) == 0//字符串比较
#define SPLIT_STRING_2(var,count,src,type,convertFun) \
	char type_tmp[12];\
	int idx_src = 0, idx_tmp = 0, idx_data = 0;\
	while (true){\
		char c = src[idx_src++];\
		if (c == 0 ||c == ' ' || c == '\n' || c == '\r'){\
			if (idx_tmp > 0){\
				type_tmp[idx_tmp] = 0;\
				idx_tmp = 0;\
				var[idx_data++] = convertFun(type_tmp); \
			}\
			if (c == 0)break;\
		}\
		else{\
			type_tmp[idx_tmp++] = c; \
		}\
	}\
	//printf("test:%f\n", var[0]);
//字符串拆分
#define SPLIT_STRING(var,count,src,type,convertFun) if(var==NULL)var = (type*)malloc(count*sizeof(type)); \
	char type_tmp[12];\
	int idx_src = 0, idx_tmp = 0, idx_data = 0;\
	while (true){\
		char c = src[idx_src++];\
		if (c == 0 ||c == ' ' || c == '\n' || c == '\r'){\
			if (idx_tmp > 0){\
				type_tmp[idx_tmp] = 0;\
				idx_tmp = 0;\
				var[idx_data++] = convertFun(type_tmp); \
			}\
			if (c == 0)break;\
		}\
		else{\
			type_tmp[idx_tmp++] = c; \
		}\
	}\
	//printf("test:%f\n", var[0]);
//向vector推入数据
#define PUSH_ARRAY(vector,ptr,num) {int num_idx=0;while(num_idx<num)vector.push_back((ptr)[num_idx++]);}
#define FIND_IN_VECTOR(var,vector,prop,key) \
{\
	for(int find_i=0;find_i<vector.size();find_i++){\
		if (EQUAL(vector[find_i].prop, key)){\
			var =vector[find_i];\
			break;\
		}\
	}\
}
#define FIND_IN_ARRAY(var,arr,len,prop,key) \
{\
	for(int find_i=0;find_i<len;find_i++){\
		if (EQUAL(arr[find_i].prop, key)){\
			var =arr[find_i];\
			break;\
		}\
	}\
}
#define FIND_INDEX_VECTOR(var,vector,prop,key) \
{\
if (key!=NULL)\
	for(int find_i=0;find_i<vector.size();find_i++){\
		if (EQUAL(vector[find_i].prop, key)){\
			var =find_i;\
			break;\
		}\
	}\
}

void Mode::create_buffer_obj(){
	if (in_bo)return;
	tbo_indics = (GLuint*)malloc(sizeof(GLuint)*textures.size());
	tex_ids = (GLuint*)malloc(sizeof(GLuint)*textures.size());

	/*VBO 顶点数据缓冲区*/
	glGenBuffers(6, vbo_indics);
	//写入顶点数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);
	//写入法线数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[1]);
	glBufferData(GL_ARRAY_BUFFER, norms.size()*sizeof(GLfloat), &norms[0], GL_STATIC_DRAW);
	//写入贴图坐标数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[2]);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
	//写入材质索引数据
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[3]);
	glBufferData(GL_ARRAY_BUFFER, indics.size()*sizeof(GLfloat), &indics[0], GL_STATIC_DRAW);
	
	/*TBO 贴图数据缓冲区*/
	
	//GenBuffers(textures.size(), tbo_indics);
	glGenTextures(textures.size(), tex_ids);
	for (int i = 0; i < textures.size(); i++){
		GLBITMAP* bitmap = (GLBITMAP*)textures[i];
		int psize = bitmap->rgb_mode == GL_RGBA ? 4 : 3;//RGBA的情况一个像素占用4个字节
		//glBindBuffer(GL_TEXTURE_BUFFER, tbo_indics[i]);
		//glBufferData(GL_TEXTURE_BUFFER, bitmap->w*bitmap->h*psize, bitmap->buf, GL_STATIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, tex_ids[i]);
		//glTexBuffer(GL_TEXTURE_BUFFER, bitmap->rgb_mode, tbo_indics[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, bitmap->rgb_mode, bitmap->w, bitmap->h, 0, bitmap->rgb_mode, GL_UNSIGNED_BYTE, bitmap->buf);
	}
	in_bo = true;
}

void Mode::delete_buffer_obj(){
	if (!in_bo)return;
	glDeleteBuffers(6, vbo_indics);

	in_bo = false;
}

void Mode::render(){
	if (!in_bo)return;
	glFrontFace(front_face);//设置正面顺序

	//设置uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_ids[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_ids[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_ids[2]);

	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if (program > 0){
		//最多支持6张贴图数据
		if (textures.size() > 0)glUniform1i(glGetUniformLocation(program, "color_map_0"), 0);
		if (textures.size() > 1)glUniform1i(glGetUniformLocation(program, "color_map_1"), 1);
		if (textures.size() > 2)glUniform1i(glGetUniformLocation(program, "color_map_2"), 2);
		if (textures.size() > 3)glUniform1i(glGetUniformLocation(program, "color_map_3"), 3);
		if (textures.size() > 4)glUniform1i(glGetUniformLocation(program, "color_map_4"), 4);
		if (textures.size() > 5)glUniform1i(glGetUniformLocation(program, "color_map_5"), 5);
		//材质
		glUniform4fv(glGetUniformLocation(program, "materials"), 6 * materials_count, materials);
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[3]);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, indics.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mode::load(const char* path){
	const char * ext = strrchr(path, '.');
	if (EQUAL(ext, ".dae")
		|| EQUAL(ext, ".DAE")){
		load_dae(path);
	}else
		printf("file %s not support", ext);
	return;
}

struct source{
	const char* id;
	int count;
	int stride;
	float* data =NULL;
};

void free_source(source src){
	free(src.data);
}

source load_dae_source(xml_node node){
	source result;
	result.id = get_xml_node_attrib(node, "id");
	for (int i = 0; i < node.child_count; i++){
		auto child = node.children[i];
		if (EQUAL(child.name, "float_array")){
			//读取float_array
			result.count = atoi(get_xml_node_attrib(child, "count"));
			auto src = child.inner_text;
			if (src == NULL){
				printf("no data in float_array\n");
			}
			else{
				SPLIT_STRING(result.data, result.count, src, float, atof);
			}
		}
		else if (EQUAL(child.name, "technique_common")){
			//TODO 读取技术细节
			result.stride = atoi(get_xml_node_attrib(child.children[0], "stride"));
		}
		else {
			printf("not support [%s]\n", child.name);
		}
	}
	printf("source[id:%s,count:%d,stride:%d]\n", result.id, result.count, result.stride);
	return result;
}

struct image{
	const char *id;
	GLBITMAP *data;//图形数据
};

void free_image(image image){
	FreeGLBitmap(image.data);
}

image load_dae_image(xml_node node){
	image result;
	result.id = get_xml_node_attrib(node, "id");
	//auto name = get_xml_node_attrib(child, "name");不需要处理name
	if (node.child_count>0 && EQUAL(node.children[0].name, "init_from")){//文件路径方式加载
		auto path = node.children[0].inner_text;
		if (path != NULL){
			result.data = loadImage(path);//加载
			if (result.data == NULL){
				printf("image load error %s\n", path);
			}
			else{
				//printf("image path=%s,(%d,%d)\n", path, result.data->w, result.data->h);
			}
		}
	}
	else if (node.child_count == 0 && node.inner_text != NULL){//HexBinary格式加载
		printf("not support hexbinary image data");
	}
	else{//没有数据的image节点
		printf("image[%s] no data\n", result.id);
	}
	printf("image[id:%s,w:%d,h:%d]\n", result.id, result.data->w, result.data->h);
	return result;
}

struct input{
	const char* semantic;
	const char* source;//source的ID
	unsigned int offset;
};

struct triangles{
	//const char* name;//名称没有用
	const char* material;
	unsigned int count;//三角形的数量
	unsigned int stride;//每个三角形顶点由几个索引组成,一般是3(位置\法线\贴图坐标),等于input的数量
	unsigned int p_count;
	input *inputs;
	int* p =NULL;//索引数组,数组长度=count*3*stride
};

void free_triangles(triangles triangles){
	free(triangles.inputs);
	free(triangles.p);
}

triangles load_dae_triangles(xml_node node){
	triangles result;
	result.material = get_xml_node_attrib(node, "material");
	result.count = atoi(get_xml_node_attrib(node, "count"));
	result.stride = 0;
	for (int i = 0; i < node.child_count; i++){
		auto child = node.children[i];
		if (EQUAL(child.name, "input")){
			result.stride++;
		}
	}
	result.inputs = (input*)malloc(sizeof(input)*result.stride);

	int idx_input = 0;
	for (int i = 0; i < node.child_count; i++){
		auto child = node.children[i];
		if (EQUAL(child.name, "input")){
			result.inputs[idx_input].semantic = get_xml_node_attrib(child, "semantic");
			result.inputs[idx_input].offset = atoi(get_xml_node_attrib(child, "offset"));
			result.inputs[idx_input].source = get_xml_node_attrib(child, "source");
			idx_input++;
		}
		else if (EQUAL(child.name, "p")){
			result.p_count= result.count * 3 * result.stride;
			SPLIT_STRING(result.p, result.p_count, child.inner_text, int, atoi);
		}
	}
	return result;
}

struct vertices{
	const char* id;
	input input;
};

vertices load_dae_vertices(xml_node node){
	vertices result;
	result.id = get_xml_node_attrib(node, "id");
	result.input.semantic = get_xml_node_attrib(node.children[0], "semantic");
	result.input.source = get_xml_node_attrib(node.children[0], "source");
	return result;
}

struct mesh{
	unsigned int source_count;
	unsigned int vertex_count;
	unsigned int triangle_count;
	source* sources;
	vertices* vertices;
	triangles* triangles;
};

void free_mesh(mesh mesh){
	int i;
	for (i = 0; i < mesh.source_count; i++){
		free_source(mesh.sources[i]);
	}
	for (i = 0; i < mesh.triangle_count; i++){
		free_triangles(mesh.triangles[i]);
	}
	free(mesh.sources);
	free(mesh.vertices);
	free(mesh.triangles);
}

struct geometry{
	const char* id;
	mesh mesh;//mesh的集合
};

void free_geometry(geometry geometry){
	free_mesh(geometry.mesh);
}

mesh load_dae_mesh(xml_node node){
	mesh result;
	result.source_count = 0;
	result.vertex_count = 0;
	result.triangle_count = 0;
	for (int i = 0; i < node.child_count; i++){
		auto child = node.children[i];
		if (EQUAL(child.name, "source")){
			result.source_count++;
		}
		else if (EQUAL(child.name, "vertices")){
			result.vertex_count++;
		}
		else if (EQUAL(child.name, "triangles")){
			result.triangle_count++;
		}
	}
	result.sources = (source*)malloc(sizeof(source)*result.source_count);
	result.vertices = (vertices*)malloc(sizeof(vertices)*result.vertex_count);
	result.triangles = (triangles*)malloc(sizeof(triangles)*result.triangle_count);
	int idx_source = 0, idx_vertex = 0, idx_triangle = 0;
	for (int i = 0; i < node.child_count; i++){
		auto child = node.children[i];
		if (EQUAL(child.name, "source")){
			result.sources[idx_source++] = load_dae_source(child);
		}
		else if (EQUAL(child.name, "vertices")){
			result.vertices[idx_vertex++] = load_dae_vertices(child);
		}
		else if (EQUAL(child.name, "triangles")){
			result.triangles[idx_triangle++] = load_dae_triangles(child);
		}
	}
	return result;
}

geometry load_dae_geometry(xml_node node){
	geometry result;
	result.id = get_xml_node_attrib(node, "id");
	if (node.child_count>0 && EQUAL(node.children[0].name, "mesh")){
		result.mesh = load_dae_mesh(node.children[0]);
	}
	else if (node.child_count>0){
		printf("not support geometry with %s\n", node.children[0].name);
	}
	else{
		printf("error geometry node[%s]\n", result.id);
	}

	return result;
}

struct material{
	const char* id;
	const char* effect_url;
};

material load_dae_material(xml_node node){
	material result;
	result.id = get_xml_node_attrib(node, "id");
	result.effect_url = get_xml_node_attrib(node.children[0], "url");
	return result;
}

struct effect{
	float emission[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;

	const char* id;
	const char* texture_emission =NULL;
	const char* texture_ambient = NULL;
	const char* texture_diffuse = NULL;
	const char* texture_specular = NULL;
};

effect load_dae_effect(xml_node node){
	effect result;
	result.id = get_xml_node_attrib(node, "id");
	//.profile.technique.phong
	xml_node list = node.children[0].children[0].children[0];
	for (int i = 0; i < list.child_count; i++){
		xml_node child = list.children[i];
		if (EQUAL(child.name, "emission")){
			if (EQUAL(child.children[0].name, "color")){//颜色
				SPLIT_STRING_2(result.emission, 4, child.children[0].inner_text, float, atof);
			}
			else if (EQUAL(child.children[0].name, "texture")){//贴图
				result.texture_emission = get_xml_node_attrib(child.children[0], "texture");
				memset(result.emission, 0, sizeof(float)* 4);
			}
		}
		else if (EQUAL(child.name, "ambient")){
			if (EQUAL(child.children[0].name, "color")){//颜色
				SPLIT_STRING_2(result.ambient, 4, child.children[0].inner_text, float, atof);
			}
			else if (EQUAL(child.children[0].name, "texture")){//贴图
				result.texture_ambient = get_xml_node_attrib(child.children[0], "texture");
				memset(result.ambient, 0, sizeof(float)* 4);
			}
		}
		else if (EQUAL(child.name, "diffuse")){
			if (EQUAL(child.children[0].name, "color")){//颜色
				SPLIT_STRING_2(result.diffuse, 4, child.children[0].inner_text, float, atof);
			}
			else if (EQUAL(child.children[0].name, "texture")){//贴图
				result.texture_diffuse = get_xml_node_attrib(child.children[0], "texture");
				memset(result.diffuse, 0, sizeof(float)* 4);
			}
		}
		else if (EQUAL(child.name, "specular")){
			if (EQUAL(child.children[0].name, "color")){//颜色
				SPLIT_STRING_2(result.specular, 4, child.children[0].inner_text, float, atof);
			}
			else if (EQUAL(child.children[0].name, "texture")){//贴图
				result.texture_specular = get_xml_node_attrib(child.children[0], "texture");
				memset(result.specular, 0, sizeof(float)* 4);
			}
		}
		else if (EQUAL(child.name, "shininess")){
			result.shininess = atof(child.children[0].inner_text);
			//printf("shininess:%f",result.shininess);
		}
	}
	return result;
}

struct node{
	const char* id;
	const char* sid;
	const char* type;
	const char* instance_controller;//带#号
	float matrix[16];

	unsigned int child_count;
	node* children =NULL;
};

void free_node(node node){
	if (node.children != NULL){
		for (int i = 0; i < node.child_count; i++)
			free_node(node.children[i]);
		free(node.children);
	}
}

node load_dae_node(xml_node n){
	node result;
	result.id = get_xml_node_attrib(n, "id");
	result.sid = get_xml_node_attrib(n, "sid");
	result.type = get_xml_node_attrib(n, "type");
	result.child_count = 0;
	for (int i = 0; i < n.child_count; i++){
		xml_node child = n.children[i];
		if (EQUAL(child.name, "node")){
			result.child_count++;
		}
	}

	result.children = (node*)malloc(sizeof(node)*result.child_count);
	int idx = 0;
	for (int i = 0; i < n.child_count; i++){
		xml_node child = n.children[i];
		if (EQUAL(child.name, "matrix")){
			SPLIT_STRING_2(result.matrix, 16, child.inner_text, float, atof);
		}
		else if (EQUAL(child.name,"node")){
			result.children[idx++] = load_dae_node(child);
		}
		else if (EQUAL(child.name, "instance_controller")){
			result.instance_controller = get_xml_node_attrib(child, "url");
		}
	}
	printf("load node[%s],type[%s]\n", result.id, result.type);
	return result;
}

struct visual_scene{
	const char* id;

	unsigned int node_count;
	node*  nodes = NULL;
};

void free_visual_scene(visual_scene visual_scene){
	if (visual_scene.nodes != NULL){
		for (int i = 0; i < visual_scene.node_count; i++)
			free_node(visual_scene.nodes[i]);
		free(visual_scene.nodes);
	}
}

visual_scene load_dae_visual_scene(xml_node n){
	visual_scene result;
	result.id = get_xml_node_attrib(n, "id");
	result.node_count = 0;
	for (int i = 0; i < n.child_count; i++){
		xml_node child = n.children[i];
		if (EQUAL(child.name, "node")){
			result.node_count++;
		}
	}

	result.nodes = (node*)malloc(sizeof(node)*result.node_count);
	int idx = 0;
	for (int i = 0; i < n.child_count; i++){
		xml_node child = n.children[i];
		if (EQUAL(child.name, "node")){
			result.nodes[idx++] = load_dae_node(child);
		}
	}

	return result;
}

struct joint{
	const char* id;
	float matrix[16];
};

void Mode::load_dae(const char* path){
	
	//解析dae文件的xml格式
	auto root = load_xml(path);
	vector<image> images;
	vector<geometry> geometries;
	vector<material> _materials;
	vector<effect> effects;
	vector<visual_scene> scenes;

	//遍历解析数据
	for (int i = 0; i < root->child_count; i++){
		auto node = root->children[i];
		printf("loop to node[%s]\n", node.name);
		if (EQUAL(node.name, "library_images")){//贴图
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "image")){
					images.push_back(load_dae_image(child));
				}
			}
		}
		else if (EQUAL(node.name, "library_materials")){//材质
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "material")){
					_materials.push_back(load_dae_material(child));
				}
			}
		}
		else if (EQUAL(node.name, "library_effects")){//效果
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "effect")){
					effects.push_back(load_dae_effect(child));
				}
			}
		}
		else if (EQUAL(node.name, "library_geometries")){//几何图形
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "geometry")){
					geometries.push_back(load_dae_geometry(child));
				}
			}
		}
		else if (EQUAL(node.name, "library_controllers")){//变形控制

		}
		else if (EQUAL(node.name, "library_animations")){//动画

		}
		else if (EQUAL(node.name, "library_visual_scenes")){//动画
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "visual_scene")){
					scenes.push_back(load_dae_visual_scene(child));
				}
			}
		}
	}

	//贴图转到数组中
	for (int i = 0; i < images.size(); i++){
		textures.push_back(images[i].data);
	}

	//几何图形转到 数组中
	for (int i = 0; i < geometries.size(); i++){
		mesh mesh = geometries[i].mesh;
		for (int j = 0; j < mesh.triangle_count; j++){
			triangles ta = mesh.triangles[j];
			source* srcs = (source*)malloc(sizeof(source)*ta.stride);
			int idx_m;
			FIND_INDEX_VECTOR(idx_m, _materials, id, ta.material);
			//printf("%d\n",ta.stride);
			for (int l = 0; l < ta.stride; l++){
				input input = ta.inputs[l];
				//printf("%d,%s\n", l, input.semantic);
				if (EQUAL(input.semantic, "VERTEX")){
					vertices v;
					FIND_IN_ARRAY(v, mesh.vertices, mesh.vertex_count, id, input.source + 1);//+1去掉#
					FIND_IN_ARRAY(srcs[l], mesh.sources, mesh.source_count, id, v.input.source + 1);
				}
				else {
					FIND_IN_ARRAY(srcs[l], mesh.sources, mesh.source_count, id, input.source + 1);
				}
			}
			int idx_p = 0;
			while (idx_p < ta.count * 3 * ta.stride){
				for (int l = 0; l < ta.stride; l++){
					input input = ta.inputs[l];
					int index = ta.p[idx_p + input.offset];
					source src = srcs[l];
					if (EQUAL(input.semantic, "VERTEX")){
						PUSH_ARRAY(verts, src.data + index*src.stride, src.stride);
						//y,z反转
						float tmp = verts[verts.size() - 1];
						verts[verts.size() - 1] = verts[verts.size() - 2];
						verts[verts.size() - 2] = tmp;
					}
					else if (EQUAL(input.semantic, "NORMAL")){
						PUSH_ARRAY(norms, src.data + index*src.stride, src.stride);
					}
					else if (EQUAL(input.semantic, "TEXCOORD")){
						PUSH_ARRAY(texcoords, src.data + index*src.stride, src.stride);
					}
				}
				indics.push_back(idx_m);
				idx_p += ta.stride;
			}
			free(srcs);
		}
		//释放
		free_geometry(geometries[i]);
	}
	/*

	*/
	materials = (GLfloat*)malloc(sizeof(GLfloat)* 4 * 6 * _materials.size());
	for (int i = 0; i < _materials.size(); i++){
		material mater = _materials[i];
		effect effect;
		FIND_IN_VECTOR(effect, effects, id, mater.effect_url + 1);
		int idx = i * 4 * 6;
		memcpy(materials + idx, &effect, sizeof(GLfloat)* 17);//直接拷贝4个颜色,和一个shininess
		//预留3个float做其他
		//最后4个float记录贴图索引,-1表示使用颜色
		materials[idx + 20] = -1;
		FIND_INDEX_VECTOR(materials[idx + 20], images, id, effect.texture_emission);
		materials[idx + 21] = -1;
		FIND_INDEX_VECTOR(materials[idx + 21], images, id, effect.texture_ambient);
		materials[idx + 22] = -1;
		FIND_INDEX_VECTOR(materials[idx + 22], images, id, effect.texture_diffuse);
		materials[idx + 23] = -1;
		FIND_INDEX_VECTOR(materials[idx + 23], images, id, effect.texture_specular);
		materials_count++;
	}

	printf("textures:%d\n", textures.size());
	printf("verts:%d\n", verts.size());
	printf("norms:%d\n", norms.size());
	printf("texcoords:%d\n", texcoords.size());
	printf("indics:%d\n", indics.size());
	printf("materials:%d\n", materials_count);
	printf("scene:%d\n", scenes.size());
	
	front_face = GL_CW;//存在YZ轴交换,改变正面顺序
	//printf("load textures:%d\n", textures.size());
	free_xml_node(root);//释放xml
}