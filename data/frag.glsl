#version 330 compatibility

in vec2 tex_coord;
in vec3 pix_normal;
flat in float indics;
in vec4 test;

//最大6张贴图
uniform sampler2D color_map_0;
uniform sampler2D color_map_1;
uniform sampler2D color_map_2;
uniform sampler2D color_map_3;
uniform sampler2D color_map_4;
uniform sampler2D color_map_5;
uniform samplerBuffer joint_matrics;

//材质
uniform vec4 materials[60];
//光源
uniform vec3 light_dir;//方向
uniform vec4 light_color;//颜色

//根据索引获取贴图颜色值
vec4 getTexture(float index,vec2 coord){
	if(index==0)return texture(color_map_0,coord);
	if(index==1)return texture(color_map_1,coord);
	if(index==2)return texture(color_map_2,coord);
	if(index==3)return texture(color_map_3,coord);
	if(index==4)return texture(color_map_4,coord);
	if(index==5)return texture(color_map_5,coord);
	return vec4(1,0,0,1);
}

void main(void){
	//通过材质索引获取材质
	int m_idx =int(indics)*6;
	vec4 emission =materials[m_idx];
	vec4 ambient =materials[m_idx+1];
	vec4 diffuse =materials[m_idx+2];
	vec4 specular =materials[m_idx+3];
	float shininess =materials[m_idx+4][0];
	vec4 texture_idx =materials[m_idx+5];
	
	//辐射光
	vec4 e_color =texture_idx[0]==-1?emission:getTexture(texture_idx[0],tex_coord);
	//环境光
	vec4 a_color =texture_idx[1]==-1?ambient:getTexture(texture_idx[1],tex_coord);
	//漫反射
	vec4 d_color =texture_idx[2]==-1?diffuse:getTexture(texture_idx[2],tex_coord);
	if(d_color.x==0&&d_color.y==0&&d_color.z==0){
		d_color =vec4(1,1,1,1);
	}
	float diff =max(0,dot(normalize(light_dir),normalize(pix_normal)));
	d_color =d_color*light_color*diff;
	
	//高光反射
	vec4 s_color =texture_idx[3]==-1?specular:getTexture(texture_idx[3],tex_coord);

	gl_FragColor =test;//vec4(0.5,0.5,0.5,1)+d_color;
}

