#pragma once

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"carrot-stage.lib")
#endif

#include <GL\glew.h>
#include <vector>

using namespace std;

struct Keyframe
{
	float          time;                // Time of this key frame
	float      transform[16];          // Transformation of this Key frame
};


struct Bone
{
	int           id;                  //骨骼的ID
	int           parent_id;           //父级骨骼的ID
	int			  joint_idx;				//有效骨骼的序号ID,有皮肤的骨骼才有的ID,没有皮肤的骨骼为-1
	unsigned int  child_count;         //子骨骼数量
	int			*children;           //子骨骼ID集合
	float		world_matrix[16];        //到世界坐标系的变换矩阵
	float		skinning_matrix[16];      //皮肤的变换矩阵
	float		joint_matrix[16];        // The Bind Pose Matrix
	float		inverse_bind_matrix[16];  // The Inverse Bind Pose Matrix
	unsigned int  keyframe_count;       // No Of key frames of animation for this bone
	Keyframe     **keyframes;         // All Key frames for this Bone’s animation
};

struct SkeletonData
{
	vector<Bone*> bones;            //所有骨骼,按照骨骼ID索引
	Bone         *root_bone;          // Root bone reference to one of the Bone in the m_Bones
	vector<Bone*> joints;			//有皮肤的骨骼,按照有皮肤的骨骼索引
	vector<Bone*> roots;


	unsigned int  keyframe_count;      // Another Copy of the No Of Key frames of animation
	//float		 *bind_shape_matrix;   // Bind Shape Matrix
};

class Mode
{
public:
	Mode();
	~Mode();

	void load(const char*);
	void create_buffer_obj();//创建Buffer Object
	void delete_buffer_obj();//删除Buffer Object
	void render();
	void render_skelecton();
	void render_bind_skelecton();
private:
	/*顶点数据*/
	vector<GLfloat> verts;		//顶点位置
	vector<GLfloat> norms;		//顶点法线
	vector<GLfloat> texcoords;	//顶点贴图坐标
	vector<GLfloat> indics;		//材质索引
	vector<GLfloat> joints;		//顶点关联骨骼,一个顶点4个骨骼
	vector<GLfloat> weights;	//顶点骨骼权值,一个顶点4个权值

	/*贴图数据集合*/
	vector<void*> textures;
	GLuint *tex_ids;//动态数量的贴图ID

	/*材质数据*/
	unsigned int materials_count;
	GLfloat* materials;

	/*关节数据*/
	unsigned int joints_count;
	GLfloat* joints_matrics;
	GLuint tbo_idx;//TBO索引
	GLuint tex_tbo;//TBO的贴图索引
	SkeletonData skeleton;

	void load_dae(const char*);

	/*Buffer Object 相关属性*/
	bool in_bo;//是否在buffer object中
	GLuint vbo_indics[6];//6个VBO

	GLenum front_face = GL_CCW;
	//GLuint ubo_indics[.];

	void pose_skeleton(int frame);
	void pose_bone(int idx, int frame, const float* parent_world_matrix);
	void update_joints_matrics();
};

