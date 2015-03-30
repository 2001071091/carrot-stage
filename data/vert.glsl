#version 330 compatibility

layout (location = 0) in vec3 position;			//位置
layout (location = 1) in vec3 normal;			//法线
layout (location = 2) in vec2 texcoord;			//贴图坐标
layout (location = 3) in float material_idx;	//材质索引
layout (location = 4) in vec4 bone_idx;			//关联骨骼索引
layout (location = 5) in vec4 bone_weight;		//骨骼权值

out vec2 tex_coord;		//贴图坐标发送给片段着色器(插值)
out vec3 pix_normal;	//用来计算光照的插值法线
flat out float indics;	//材质索引发送给片段着色器(不插值)

void main(void){
	tex_coord = texcoord;
	indics =material_idx;
	pix_normal =normal;
	
	vec4 mod_pos=ftransform();//默认的变换管线
	
	//骨骼变换
	 
	gl_Position =mod_pos;
}