#version 330 compatibility
in vec4 color;
in vec2 tex_coords;
in vec3 pix_normal;
vec4 light =vec4(0,0,1,1);//世界坐标,光源方向
vec3 specular =vec3(0.3,0.3,0.3);
float shininess =1;

uniform sampler2D color_map;
uniform mat4 view_mat;//视角坐标系到世界坐标系的 变换矩阵
uniform mat4 proj_mat;
uniform mat4 model_mat;//视角坐标系到模型坐标系的 变换矩阵
void main(void){
	vec4 s_color =color;

	//光线方向需要转换到模型坐标才能和模型坐标的法线进行运算
	light =model_mat*inverse(view_mat)*light;//世界坐标->视角坐标->模型坐标,由于OpenGL是列向量,所以矩阵变换顺序是 CBA*V
	
	
	//镜面光
	vec4 eye_normal =vec4(0,0,1,1);
	eye_normal=model_mat*eye_normal;
	vec3 vref =reflect(-light.xyz,pix_normal);
	float angle =max(0,dot(eye_normal.xyz,vref));
	float spec =pow(angle,shininess);
	s_color.xyz+=(spec*specular);

	gl_FragColor =texture(color_map,tex_coords)*s_color;
}
