#version 330 compatibility

out vec4 color;
out vec2 tex_coords;
out vec3 pix_normal;
uniform mat4 view_mat;//视角坐标系到世界坐标系的 变换矩阵
uniform mat4 proj_mat;
uniform mat4 model_mat;//视角坐标系到模型坐标系的 变换矩阵
layout (location = 0) in vec4 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
vec4 light =vec4(0,0,1,1);//世界坐标,光源方向
vec3 ambient =vec3(0.2,0.2,0.2);
vec3 diffuse =vec3(0.8,0.8,0.8);
void main(void){
	gl_Position =ftransform();//默认的变换管线
	tex_coords =texcoord;//设置贴图坐标
	
	//光线方向需要转换到模型坐标才能和模型坐标的法线进行运算
	light =model_mat*inverse(view_mat)*light;//世界坐标->视角坐标->模型坐标,由于OpenGL是列向量,所以矩阵变换顺序是 CBA*V
	
	vec4 s_color =vec4(0,0,0,0);
	//环境光
	s_color.xyz+=ambient;
	
	//漫反射
	float diff =max(0,dot(light.xyz,normal));//法线和光源方向的点乘就是漫反射的强度值(-1到1),两个方向垂直时为0,相同时最大
	s_color.xyz+=(diff*diffuse);
	
	pix_normal =normal;
	
	color =s_color;
}