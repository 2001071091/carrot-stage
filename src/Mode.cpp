#include "Mode.h"
#include "utils.h"


Mode::Mode()
{
}


Mode::~Mode()
{
}
#define EQUAL(a,b) strcmp(a, b) == 0//字符串比较
#define FIND_BY_ID(name,arr,num,key) int zzz=0;for(int zzz=0;zzz<num;zzz++){if(EQUAL(arr[zzz].id,key+1))break;}auto name=arr[zzz];
#define SPLIT_STRING(var,count,src,type,convertFun) var = (type*)malloc(count*sizeof(type)); \
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
#define PUSH_ARRAY(vector,ptr,num) {int num_idx=0;while(num_idx<num)vector.push_back((ptr)[num_idx++]);}

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
	float* data;
};
using namespace com_yoekey_3d;

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

void Mode::load_dae(const char* path){
	
	//解析dae文件的xml格式
	auto root = load_xml(path);

	//遍历数据
	for (int i = 0; i < root->child_count; i++){
		auto node = root->children[i];
		printf("loop to node[%s]\n", node.name);
		if (EQUAL(node.name, "library_images")){//贴图
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "image")){
					auto id = get_xml_node_attrib(child, "id");
					//auto name = get_xml_node_attrib(child, "name");不需要处理name
					if (child.child_count>0 && EQUAL(child.children[0].name, "init_from")){//文件路径方式加载
						auto path = child.children[0].inner_text;
						if (path != NULL){
							auto image = loadImage(path);//加载
							if (image == NULL){
								printf("image load error %s\n", path);
							}
							else{
								textures.push_back(image);
								printf("image path=%s,(%d,%d)\n", path, image->w, image->h);
							}
						}
					}
					else if (child.child_count == 0 && child.inner_text != NULL){//HexBinary格式加载
						printf("not support hexbinary image data");
					}
					else{//没有数据的image节点
						printf("image[%s] no data\n", id);
					}
				}
			}
		}
		else if (EQUAL(node.name, "library_materials")){//材质

		}
		else if (EQUAL(node.name, "library_effects")){//效果

		}
		else if (EQUAL(node.name, "library_geometries")){//几何图形
			for (int j = 0; j < node.child_count; j++){
				auto child = node.children[j];
				if (EQUAL(child.name, "geometry")){
					auto id = get_xml_node_attrib(child, "id");
					if (child.child_count>0 && EQUAL(child.children[0].name, "mesh")){
						auto mesh = child.children[0];
						source* vertex;
						source sources[3];
						int idx_source=0;
						for (int k = 0; k<mesh.child_count; k++){
							auto mesh_child = mesh.children[k];
							if (EQUAL(mesh_child.name, "source")){
								sources[idx_source++] = load_dae_source(mesh_child);
							}
							else if (EQUAL(mesh_child.name, "vertices")){
								FIND_BY_ID(target, sources, 3, get_xml_node_attrib(mesh_child.children[0], "source"));
								vertex = &target;
							}
							else if (EQUAL(mesh_child.name, "triangles")){
								//定义三角形列表
								source *normal, *texcoord;
								int offset_v, offset_n, offset_t;
								int stride;
								int triangle_count = atoi(get_xml_node_attrib(mesh_child, "count"));
								for (int l = 0; l < mesh_child.child_count; l++){
									auto sub = mesh_child.children[l];
									if (EQUAL(sub.name, "input")){
										const char* semantic = get_xml_node_attrib(sub, "semantic");
										int offset = atoi(get_xml_node_attrib(sub, "offset"));
										if (EQUAL(semantic, "VERTEX")){
											offset_v = offset;
										}
										else if (EQUAL(semantic, "NORMAL")){
											offset_n = offset;
											FIND_BY_ID(target, sources, 3, get_xml_node_attrib(sub, "source"));
											normal = &target;
										}
										else if (EQUAL(semantic, "TEXCOORD")){
											offset_t = offset;
											FIND_BY_ID(target, sources, 3, get_xml_node_attrib(sub, "source"));
											texcoord = &target;
										}
									}
									else if (EQUAL(sub.name, "p")){
										stride = max(offset_v, max(offset_n, offset_t)) + 1;
										int* indics;
										int indics_count = triangle_count * stride * 3;
										int idx_p = 0;
										SPLIT_STRING(indics, indics_count, sub.inner_text, int, atoi);
										while (idx_p<indics_count){
											PUSH_ARRAY(verts, vertex->data + indics[idx_p + offset_v] * vertex->stride, vertex->stride);
											PUSH_ARRAY(norms, normal->data + indics[idx_p + offset_n] * normal->stride, normal->stride);
											PUSH_ARRAY(texcoords, texcoord->data + indics[idx_p + offset_t] * texcoord->stride, texcoord->stride);
											idx_p += stride;
										}
										printf("triangle_count:%d,verts.size():%d\n", triangle_count, verts.size());
										free(indics);//释放
									}
								}
								//处理input
								
								//处理p
							}
						}
					}
					else if (child.child_count>0){
						printf("not support geometry with %s\n", child.children[0].name);
					}
					else{
						printf("error geometry node[%s]\n", id);
					}
				}
			}
		}
		else if (EQUAL(node.name, "library_controllers")){//变形控制

		}
		else if (EQUAL(node.name, "library_animations")){//动画

		}

	}
	
	//printf("load textures:%d\n", textures.size());
	free_xml_node(root);//释放xml
}