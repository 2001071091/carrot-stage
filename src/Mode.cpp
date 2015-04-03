#include "Mode.h"
#include "utils.h"
using namespace com_yoekey_3d;

void Mode::update_joints_matrics(){
	for (int i = 0; i < skeleton.joints.size(); i++){
		Bone* bone = skeleton.joints[i];
		memcpy(joints_matrics + i * 16, bone->skinning_matrix, 16 * sizeof(float));
	}
}

void Mode::pose_skeleton(int frame){
	float wm[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	for (int i = 0; i < skeleton.roots.size();i++){
		pose_bone(skeleton.roots[i]->id, frame, wm);
	}
}

void Mode::pose_bone(int idx, int frame, const float* parent_world_matrix){
	Bone* bone = skeleton.bones[idx];
	//bone->pose(frame, parent_world_matrix);
	if (bone->keyframe_count > 0){//存在帧信息则使用帧中的骨骼数据
		if (frame >= bone->keyframe_count){
			frame = frame%bone->keyframe_count;
		}
		//TODO 可以处理插值信息
		mat4x4_mul(parent_world_matrix, bone->keyframes[frame]->transform, bone->world_matrix);
	}
	else{//没有帧数据则直接使用骨骼原始的信息
		mat4x4_mul(parent_world_matrix, bone->joint_matrix, bone->world_matrix);
		//mat4x4_mul(bone->joint_matrix, parent_world_matrix, bone->world_matrix);
	}
	//影响皮肤的矩阵还需要乘上一个inverse_bind_matrix
	mat4x4_mul(bone->world_matrix, bone->inverse_bind_matrix, bone->skinning_matrix);

	//printf("idx[%d]\n", bone->joint_idx);
	//matrix_printf(bone->skinning_matrix,4,4);
	//printf("\n");
	
	//处理子骨骼
	if (bone->child_count>0)
		for (int i = 0; i < bone->child_count; i++)
			pose_bone(bone->children[i], frame, bone->world_matrix);
}

Mode::Mode()
{
	materials = NULL;
	tex_ids = NULL;
	joints_matrics = NULL;
}

Mode::~Mode()
{
	delete_buffer_obj();//删除buffer object
	//释放图形对象
	for (int i = 0; i < textures.size(); i++){
		FreeGLBitmap((GLBITMAP*)textures[i]);
	}
	if (tex_ids != NULL)free(tex_ids);
	if (materials != NULL)free(materials);
	if (joints_matrics != NULL)free(joints_matrics);
}
//字符串比较
#define EQUAL(a,b) ((a==NULL||b==NULL)?false:(strcmp(a, b) == 0))//字符串比较
#define SPLIT_STRING_2(var,count,src,type,convertFun) \
	char type_tmp[50];\
	int idx_src = 0, idx_tmp = 0, idx_data = 0;\
	while (true){\
		if (idx_data >= count)break; \
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
	char type_tmp[50];\
	int idx_src = 0, idx_tmp = 0, idx_data = 0;\
	while (true){\
		if (idx_data >= count)break; \
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
#define FIND_PTR_IN_VECTOR(var,vector,prop,key) \
{\
	for(int find_i=0;find_i<vector.size();find_i++){\
		if (EQUAL(vector[find_i]->prop, key)){\
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
	//写入关节索引
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[4]);
	glBufferData(GL_ARRAY_BUFFER, joints.size()*sizeof(GLfloat), &joints[0], GL_STATIC_DRAW);
	//写入关节权重
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[5]);
	glBufferData(GL_ARRAY_BUFFER, weights.size()*sizeof(GLfloat), &weights[0], GL_STATIC_DRAW);
	
	/*TBO 贴图数据缓冲区*/
	glEnable(GL_TEXTURE_2D);
	glGenTextures(textures.size(), tex_ids);
	for (int i = 0; i < textures.size(); i++){
		GLBITMAP* bitmap = (GLBITMAP*)textures[i];
		glBindTexture(GL_TEXTURE_2D, tex_ids[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, bitmap->rgb_mode, bitmap->w, bitmap->h, 0, bitmap->rgb_mode, GL_UNSIGNED_BYTE, bitmap->buf);
	}

	/*关节数据*/
	glGenBuffers(1, &tbo_idx);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo_idx);
	glBufferData(GL_TEXTURE_BUFFER, joints_count * 16 * sizeof(GLfloat), joints_matrics, GL_STATIC_DRAW);//每帧都会更新
	glGenTextures(1, &tex_tbo);
	glBindTexture(GL_TEXTURE_BUFFER, tex_tbo);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tbo_idx);
	in_bo = true;
}

void Mode::delete_buffer_obj(){
	if (!in_bo)return;
	glDeleteBuffers(6, vbo_indics);

	in_bo = false;
}

void Mode::render_bind_skelecton(){
	glEnable(GL_BLEND);
	glColor4f(1, 0, 0, 1);
	glBegin(GL_TRIANGLES);
	float v1[4] = { 0, 0.5, 0, 1 };
	float v2[4] = { 0, -0.3, 0.5, 1 };
	float v3[4] = { 0, -0.3, -0.5, 1 };
	float v4[4] = { 0, 0, 0, 1 };
	float v_1[4];
	float v_2[4];
	float v_3[4];
	float v_4[4];
	for (int i = 0; i < skeleton.joints.size(); i++){
		Bone* bone = skeleton.joints[i];
		if (bone->parent_id == -1)continue;
		Bone* parent = skeleton.bones[bone->parent_id];
		float mat[16];
		mat4x4_inverse(parent->inverse_bind_matrix, mat);
		mat4x4_mul_vector(mat, v1, v_1);
		mat4x4_mul_vector(mat, v2, v_2);
		mat4x4_mul_vector(mat, v3, v_3);
		mat4x4_inverse(bone->inverse_bind_matrix, mat);
		mat4x4_mul_vector(mat, v4, v_4);

		glVertex3fv(v_1);
		glVertex3fv(v_2);
		glVertex3fv(v_3);

		glVertex3fv(v_1);
		glVertex3fv(v_2);
		glVertex3fv(v_4);

		glVertex3fv(v_2);
		glVertex3fv(v_3);
		glVertex3fv(v_4);

		glVertex3fv(v_3);
		glVertex3fv(v_1);
		glVertex3fv(v_4);
		//glEnd();
	}
	glEnd();
	glDisable(GL_BLEND);
}

void Mode::render_skelecton(){
	glEnable(GL_BLEND);
	glColor4f(1, 0, 0, 1);
	glBegin(GL_TRIANGLES);
	float v1[4] = { 0, 0.5, 0, 1 };
	float v2[4] = { 0, -0.3, 0.5, 1 };
	float v3[4] = { 0, -0.3, -0.5, 1 };
	float v4[4] = { 0, 0, 0, 1 };
	float v_1[4];
	float v_2[4];
	float v_3[4];
	float v_4[4];
	for (int i = 0; i < skeleton.joints.size(); i++){
		Bone* bone = skeleton.joints[i];
		if (bone->parent_id == -1)continue;
		Bone* parent = skeleton.bones[bone->parent_id];
		mat4x4_mul_vector(parent->world_matrix, v1, v_1);
		mat4x4_mul_vector(parent->world_matrix, v2, v_2);
		mat4x4_mul_vector(parent->world_matrix, v3, v_3);
		mat4x4_mul_vector(bone->world_matrix, v4, v_4);

		glVertex3fv(v_1);
		glVertex3fv(v_2);
		glVertex3fv(v_3);

		glVertex3fv(v_1);
		glVertex3fv(v_2);
		glVertex3fv(v_4);

		glVertex3fv(v_2);
		glVertex3fv(v_3);
		glVertex3fv(v_4);

		glVertex3fv(v_3);
		glVertex3fv(v_1);
		glVertex3fv(v_4);
		//glEnd();
	}
	glEnd();
	glDisable(GL_BLEND);
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
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex_ids[3]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, tex_ids[4]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, tex_ids[5]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_BUFFER, tex_tbo);

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
		//关节矩阵
		glUniform1i(glGetUniformLocation(program, "joint_matrics"), 6);
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[3]);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[4]);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_indics[5]);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);

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
	char** name_data = NULL;
};

void free_source(source src){
	if (src.name_data != NULL){
		for (int i = 0; i < src.count; i++){
			//printf("release %s\n", src.name_data[i]);
			//free(src.name_data[i]);
		}
		free(src.name_data);
	}
	free(src.data);
}

char* clonestr(const char* str){
	size_t len = 0;
	while (str[len++]!='\0'){
	}
	char* result = (char*)malloc(len*sizeof(char));
	memcpy(result, str, len*sizeof(char));
	//printf("len:%d,%s\n", len, result);
	return result;
}

void replace(char* src,char o,char t,size_t len){
	char* c;
	size_t l = len;
	for (c = src; *c != '\0'&&l > 0; c++&&l--){
		if (*c == o){
			*c = t;
		}
	}
}

char* indexof(const char* src,const char* tar){
	const char* str = src;
	const char *c, *c2;
	while (str!=NULL){
		str = strchr(str, tar[0]);//检索第一个字符
		if (str != NULL){
			bool test = true;
			for ((c =str)&&(c2=tar); *c != '\0'&&*c2!='\0'; c++&&c2++){
				if (*c != *c2){
					str++;
					test = false;
					break;
				}
			}
			if (test)
				return (char*)str;
		}
	}
	return NULL;
}

source load_dae_source(xml_node node,const char** ref_names,int ref_name_count){
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
		}else if (EQUAL(child.name, "Name_array")){
			//读取float_array
			result.count = atoi(get_xml_node_attrib(child, "count"));
			auto src = child.inner_text;
			if (src == NULL){
				printf("no data in Name_array\n");
			}
			else{
				if (ref_names != NULL&&ref_name_count > 0){
					char* tmpstr = strdup(src);
					//进行替换
					for (int j = 0; j < ref_name_count; j++){
						if (strchr(ref_names[j], ' ') != NULL){
							char* tmp = tmpstr;
							while (tmp != NULL){
								tmp = indexof(tmp, ref_names[j]);
								if (tmp != NULL){
									size_t len = strlen(ref_names[j]);
									replace(tmp, ' ', '\\', len);
									tmp += len;
								}
							}
						}
					}
					//printf("%s\n", tmpstr);
					SPLIT_STRING(result.name_data, result.count, tmpstr, char*, strdup);
					for (int j = 0; j < result.count; j++){
						if (strchr(result.name_data[j], '\\') != NULL){
							replace(result.name_data[j], '\\', ' ', strlen(result.name_data[j]));
						}
					}
					free(tmpstr);
				}
				else{
					SPLIT_STRING(result.name_data, result.count, src, char*, strdup);
				}
			}
		}
		else if (EQUAL(child.name, "technique_common")){
			//TODO 读取技术细节
			const char* stride_s = get_xml_node_attrib(child.children[0], "stride");
			result.stride = 1;
			if (stride_s!=NULL)result.stride = atoi(stride_s);
		}
		else {
			printf("not support [%s]\n", child.name);
		}
	}
	//printf("source[id:%s,count:%d,stride:%d]\n", result.id, result.count, result.stride);
	//free_source(result);
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
	//printf("image[id:%s,w:%d,h:%d]\n", result.id, result.data->w, result.data->h);
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
			result.sources[idx_source++] = load_dae_source(child, NULL, 0);
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
	const char* parent_sid;
	const char* sid;
	const char* type;
	const char* instance_controller;//带#号
	//表示关节到父级关节坐标系的变化
	float matrix[16];
	bool has_matrix = false;

	unsigned int child_count;
	node* children =NULL;

	int bone_idx;//临时数据
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
	result.parent_sid = NULL;
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
			result.has_matrix = true;
			//printf("%s:\n", result.sid);
			SPLIT_STRING_2(result.matrix, 16, child.inner_text, float, atof);
			//for (int j = 0; j < 16; j++){
			//	if (j % 4 == 0)
			//		printf("\n");
			//	printf("%f,", result.matrix[j]);
			//}
			//printf("\n\n");
		}
		else if (EQUAL(child.name,"node")){
			node c = load_dae_node(child);
			c.parent_sid = result.sid;
			result.children[idx++] = c;
		}
		else if (EQUAL(child.name, "instance_controller")){
			result.instance_controller = get_xml_node_attrib(child, "url");
		}
	}
	//printf("load node[%s],type[%s]\n", result.id, result.type);
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
	unsigned int idx;//关节的顺序索引
	const char* id;
	//关节绑定矩阵的逆矩阵,表示世界坐标系到这个关节坐标系的变换
	float matrix[16];
};

struct skin{
	const char* geometry;//对应几何图形的ID

	float bind_shape_matrix[16];

	unsigned int joint_count;
	joint* joints;

	unsigned int vertex_count;	//顶点数量
	float* joint_indics;		//顶点关联关节索引(每个顶点最多绑4个骨骼,如果数据存在大于4个的保留权重最大的4个)
	float* joint_weights;		//顶点关联关节权重(和joint_indics对应)
};

void free_skin(skin skin){
	free(skin.joints);
	free(skin.joint_indics);
	free(skin.joint_weights);
}

skin load_dae_skin(xml_node node,int joint_idx_offset,const char** sids,int sid_count){
	skin result;
	source* sources;
	unsigned int source_count = 0;
	for (int i = 0; i < node.child_count; i++){
		xml_node child = node.children[i];
		if (EQUAL(child.name, "source")){
			source_count++;
		}
	}
	sources = (source*)malloc(sizeof(source)*source_count);
	int source_idx = 0;
	result.geometry = get_xml_node_attrib(node, "source") + 1;
	for (int i = 0; i < node.child_count; i++){
		xml_node child = node.children[i];
		if (EQUAL(child.name, "bind_shape_matrix")){
			result.bind_shape_matrix;
			SPLIT_STRING_2(result.bind_shape_matrix, 16, child.inner_text, float, atof);
		}
		else if (EQUAL(child.name, "source")){
			sources[source_idx++] = load_dae_source(child, sids, sid_count);
		}
		else if (EQUAL(child.name, "joints")){
			for (int k = 0; k < child.child_count; k++){
				xml_node input = child.children[k];
				const char* semantic = get_xml_node_attrib(input, "semantic");
				const char* key = get_xml_node_attrib(input, "source");
				if (EQUAL(semantic, "JOINT")){
					//不处理
					source source;
					FIND_IN_ARRAY(source, sources, source_count, id, key + 1);
					result.joint_count = source.count;
					result.joints = (joint*)malloc(sizeof(joint)*result.joint_count);
					for (int j = 0; j < result.joint_count; j++){
						result.joints[j].id = source.name_data[j];
					}
				}
				else if (EQUAL(semantic, "INV_BIND_MATRIX")){
					source source;
					FIND_IN_ARRAY(source, sources, source_count, id, key + 1);
					for (int j = 0; j < result.joint_count; j ++){
						result.joints[j].idx = j + joint_idx_offset;
						memcpy(result.joints[j].matrix, source.data + j*source.stride, 16*sizeof(float));
						printf("joint id[%s] idx[%d]\n", result.joints[j].id, result.joints[j].idx);
						//matrix_printf(result.joints[j].matrix, 4, 4);
						//printf("\n");
					}
				}
			}
		}
		else if (EQUAL(child.name, "vertex_weights")){
			result.vertex_count = atoi(get_xml_node_attrib(child, "count"));
			result.joint_indics = (float*)malloc(sizeof(float)*result.vertex_count * 4);
			result.joint_weights = (float*)malloc(sizeof(float)*result.vertex_count * 4);
			source weights;
			int* vcounts = NULL;
			int* vs = NULL;
			int v_count = 0;
			for (int k = 0; k < child.child_count; k++){
				xml_node sub = child.children[k];
				if (EQUAL(sub.name, "input")){
					const char* semantic = get_xml_node_attrib(sub, "semantic");
					const char* key = get_xml_node_attrib(sub, "source");
					if (EQUAL(semantic,"WEIGHT")){
						FIND_IN_ARRAY(weights, sources, source_count, id, key + 1);
					}
				}
				else if (EQUAL(sub.name, "vcount")){
					SPLIT_STRING(vcounts, result.vertex_count, sub.inner_text, int, atoi);
					for (int j = 0; j < result.vertex_count; j++){
						v_count += vcounts[j] * 2;
					}
				}
				else if (EQUAL(sub.name, "v")){
					int v_idx = 0;
					SPLIT_STRING(vs, v_count, sub.inner_text, int, atoi);
					for (int j = 0; j < result.vertex_count; j++){
						int vcount = vcounts[j];
						float* v_indics = result.joint_indics + j * 4;
						float* v_weights = result.joint_weights + j * 4;
						memset(v_indics, 0, sizeof(float)* 4);
						memset(v_weights, 0, sizeof(float)* 4);
						for (int l = 0; l < vcount; l++){
							int r_idx = vs[v_idx++] + joint_idx_offset;
							float r_w = weights.data[vs[v_idx++]];
							if (l>3){
								for (int t = 0; t < 4; t++){
									if (r_w>v_weights[t]){
										v_indics[t] = r_idx;
										v_weights[t] = r_w;
										break;
									}
								}
							}
							else{
								v_indics[l] = r_idx;
								v_weights[l] = r_w;
							}
						}
						/*
						for (int l = 0; l < 4; l++){
							printf("%f(%f),",v_indics[l],v_weights[l]);
						}
						printf("\n");
						*/
					}
				}
			}
		}
	}

	//释放资源
	for (int i = 0; i < source_count; i++){
		free_source(sources[i]);
	}
	free(sources);
	return result;
}

void make_node_map(vector<node*>* map,node *n){
	map->push_back(n);
	for (int i = 0; i < n->child_count; i++){
		make_node_map(map, &n->children[i]);
	}
}
Bone* create_bone(node *n,SkeletonData* skeleton){
	Bone* bone = (Bone*)malloc(sizeof(Bone));
	skeleton->bones.push_back(bone);
	bone->id = skeleton->bones.size() - 1;
	bone->parent_id = -1;
	n->bone_idx = bone->id;
	bone->child_count = n->child_count;
	bone->children = (int*)malloc(sizeof(int)*n->child_count);
	memcpy(bone->joint_matrix, n->matrix, sizeof(float)* 16);
	bone->joint_idx = -1;//默认记录没有关联皮肤
	bone->keyframe_count = 0;
	if (n->child_count>0)
	for (int i = 0; i < n->child_count; i++){
		Bone* sub = create_bone(&n->children[i], skeleton);
		sub->parent_id = bone->id;
		bone->children[i] = sub->id;
	}
	return bone;
}

void Mode::load_dae(const char* path){
	
	//解析dae文件的xml格式
	auto root = load_xml(path);
	vector<image> images;
	vector<geometry> geometries;
	vector<material> _materials;
	vector<effect> effects;
	vector<visual_scene> scenes;
	vector<skin> skins;
	vector<node*> node_map;
	const char** node_sids;

	//提前读取节点数据(处理sid存在空格的问题,提前知道有哪些sid,在加载JOINT的Name_array时进行智能分析)
	for (int i = 0; i < root->child_count; i++){
		auto node = root->children[i];
		if (EQUAL(node.name, "library_visual_scenes")){//场景节点
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "visual_scene")){
					scenes.push_back(load_dae_visual_scene(child));
				}
			}
		}
	}
	for (int i = 0; i < scenes.size(); i++){
		auto scene = scenes[i];
		for (int j = 0; j < scene.node_count; j++){
			node* node = &scene.nodes[j];
			if (node->has_matrix){
				//printf("%s\n",node->type);
				make_node_map(&node_map, node);
				Bone* rb =create_bone(node, &skeleton);
				skeleton.roots.push_back(rb);
			}
		}
	}

	//printf("node_count:%d\n", node_map.size());
	node_sids = (const char**)malloc(sizeof(const char*)*node_map.size());
	for (int i = 0; i < node_map.size(); i++){
		node_sids[i] = node_map[i]->sid;
		//printf("%s\n", node_sids[i]);
	}

	//遍历解析数据
	for (int i = 0; i < root->child_count; i++){
		auto node = root->children[i];
		//printf("loop to node[%s]\n", node.name);
		if (EQUAL(node.name, "asset")){
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "up_axis")){
					if (EQUAL(child.inner_text, "Z_UP")){
						front_face = GL_CW;//存在YZ轴交换,改变正面顺序
					}
				}
			}
		}
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
			int joint_idx_offset = 0;
			joints_count = 0;
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "controller")){
					auto first = child.children[0];
					if (EQUAL(first.name, "skin")){
						skin skin = load_dae_skin(first, joint_idx_offset, node_sids, node_map.size());
						joint_idx_offset += skin.joint_count;
						skins.push_back(skin);
						joints_count += skin.joint_count;
					}
				}
			}
		}
		else if (EQUAL(node.name, "library_animations")){//动画

		}
	}

	//贴图转到数组中
	for (int i = 0; i < images.size(); i++){
		textures.push_back(images[i].data);
	}

	//几何图形转到 数组中
	for (int i = 0; i < geometries.size(); i++){
		mesh mesh = geometries[i].mesh;
		skin skin;
		FIND_IN_VECTOR(skin, skins, geometry, geometries[i].id);
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
					int index = ta.p[idx_p + input.offset];//点的索引
					source src = srcs[l];
					if (EQUAL(input.semantic, "VERTEX")){
						PUSH_ARRAY(verts, src.data + index*src.stride, src.stride);
						//y,z反转
						if (front_face == GL_CW){
							float tmp = verts[verts.size() - 1];
							verts[verts.size() - 1] = verts[verts.size() - 2];
							verts[verts.size() - 2] = tmp;
						}
						if (skin.geometry != NULL){
							PUSH_ARRAY(joints, skin.joint_indics + index * 4, 4);
							PUSH_ARRAY(weights, skin.joint_weights + index * 4, 4);
						}
						else{
							//没有蒙皮的情况
							joints.push_back(0);
							joints.push_back(0);
							joints.push_back(0);
							joints.push_back(0);
							weights.push_back(0);
							weights.push_back(0);
							weights.push_back(0);
							weights.push_back(0);
						}
					}
					else if (EQUAL(input.semantic, "NORMAL")){
						PUSH_ARRAY(norms, src.data + index*src.stride, src.stride);
					}
					else if (EQUAL(input.semantic, "TEXCOORD")){
						PUSH_ARRAY(texcoords, src.data + index*src.stride, src.stride);
					}
				}
				indics.push_back(idx_m);//处理材质索引
				idx_p += ta.stride;
			}
			free(srcs);
		}
		//释放
		free_geometry(geometries[i]);
	}

	//序列化关节矩阵
	joints_matrics = (GLfloat*)malloc(sizeof(GLfloat)* 16 * joints_count);
	//构造骨骼数据
	//printf("skeleton_count:%d\n", skeleton_count);
	//printf("-------------\n");
	for (int i = 0; i < skins.size(); i++){
		skin skin = skins[i];
		for (int j = 0; j < skin.joint_count; j++){
			joint joint = skin.joints[j];
			node *n;
			FIND_PTR_IN_VECTOR(n, node_map, sid, joint.id);//找到对应的关节信息
			//printf("joint[%s],bone_idx[%d]\n", skin.joints[j].id, n->bone_idx);
			//补充骨骼的数据
			Bone* bone = skeleton.bones[n->bone_idx];
			bone->joint_idx = joint.idx;
			//将bind_shape_matrix和joint_matrix相乘作为该骨骼的inverse_bind_matrix
			mat4x4_mul(joint.matrix, skin.bind_shape_matrix, bone->inverse_bind_matrix);
			float mattest[16];
			if (mat4x4_inverse(bone->inverse_bind_matrix, mattest) == 0){
				printf("error joint[%s]\n", joint.id);
				float *pmat = bone->inverse_bind_matrix;
					printf("%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n\n",
						pmat[0], pmat[1], pmat[2], pmat[3],
						pmat[4], pmat[5], pmat[6], pmat[7],
						pmat[8], pmat[9], pmat[10], pmat[11],
						pmat[12], pmat[13], pmat[14], pmat[15]
						);
			}
			//printf("joint[%d,%s],bone[%d,%s]\n", joint.idx, joint.id, n->bone_idx, n->sid);
			skeleton.joints.push_back(bone);
		}
		free_skin(skin);
	}
	pose_skeleton(0);
	update_joints_matrics();
	printf("bones:[%d]\n",skeleton.bones.size());
	printf("joints:[%d]\n", skeleton.joints.size());

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

	//printf("textures:%d\n", textures.size());
	//printf("verts:%d\n", verts.size());
	//printf("norms:%d\n", norms.size());
	//printf("texcoords:%d\n", texcoords.size());
	//printf("indics:%d\n", indics.size());
	//printf("skin:%d\n", skins.size());
	//printf("weights:%d\n", weights.size());
	//printf("materials:%d\n", materials_count);
	//printf("joints_count:%d\n", joints_count);
	//printf("joints_matrics:%d\n", joint_idx*16);
	//printf("scene:%d\n", scenes.size());
	
	
	//printf("load textures:%d\n", textures.size());
	free_xml_node(root);//释放xml
	free(node_sids);
}