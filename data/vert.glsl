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
out vec4 test;

uniform samplerBuffer joint_matrics;

mat4 load_matrix(float idx){
	mat4 m;
	for(int i=0;i<4;i++)
		m[i]=texelFetch(joint_matrics,int(idx)*4+i);
	return transpose(m);
}

void main(void){
	tex_coord = texcoord;
	indics =material_idx;
	pix_normal =normal;
	
	vec4 mod_pos =vec4(position,1);
	
	//骨骼变换
	float tw =0;
	vec4 dst_pos =vec4(0,0,0,0);
	for(int i=0;i<4;i++){
		mat4  m=load_matrix(bone_idx[i]);
		float w=bone_weight[i];
		if(w==0)continue;
		dst_pos+=(m*mod_pos)*w;
		tw+=w;
	}
	dst_pos/=tw;
	mod_pos =dst_pos;
	if(bone_idx[0]>=118&&bone_idx[0]<=125)test=vec4(1,0,0,1);
	else if(bone_idx[0]>=115&&bone_idx[0]<=117)test=vec4(0,1,0,1);
	else if(bone_idx[0]>=126&&bone_idx[0]<=130)test=vec4(0,1,0,1);
	else
		test=vec4(0.2,0.2,0.2,1);
	
	//模型变化+投影变换
	mod_pos= gl_ProjectionMatrix*gl_ModelViewMatrix*mod_pos;
	gl_Position =mod_pos;
}
