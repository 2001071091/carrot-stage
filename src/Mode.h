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
	int           id;                  //������ID
	int           parent_id;           //����������ID
	int			  joint_idx;				//��Ч���������ID,��Ƥ���Ĺ������е�ID,û��Ƥ���Ĺ���Ϊ-1
	unsigned int  child_count;         //�ӹ�������
	int			*children;           //�ӹ���ID����
	float		world_matrix[16];        //����������ϵ�ı任����
	float		skinning_matrix[16];      //Ƥ���ı任����
	float		joint_matrix[16];        // The Bind Pose Matrix
	float		inverse_bind_matrix[16];  // The Inverse Bind Pose Matrix
	unsigned int  keyframe_count;       // No Of key frames of animation for this bone
	Keyframe     **keyframes;         // All Key frames for this Bone��s animation
};

struct SkeletonData
{
	vector<Bone*> bones;            //���й���,���չ���ID����
	Bone         *root_bone;          // Root bone reference to one of the Bone in the m_Bones
	vector<Bone*> joints;			//��Ƥ���Ĺ���,������Ƥ���Ĺ�������
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
	void create_buffer_obj();//����Buffer Object
	void delete_buffer_obj();//ɾ��Buffer Object
	void render();
	void render_skelecton();
	void render_bind_skelecton();
private:
	/*��������*/
	vector<GLfloat> verts;		//����λ��
	vector<GLfloat> norms;		//���㷨��
	vector<GLfloat> texcoords;	//������ͼ����
	vector<GLfloat> indics;		//��������
	vector<GLfloat> joints;		//�����������,һ������4������
	vector<GLfloat> weights;	//�������Ȩֵ,һ������4��Ȩֵ

	/*��ͼ���ݼ���*/
	vector<void*> textures;
	GLuint *tex_ids;//��̬��������ͼID

	/*��������*/
	unsigned int materials_count;
	GLfloat* materials;

	/*�ؽ�����*/
	unsigned int joints_count;
	GLfloat* joints_matrics;
	GLuint tbo_idx;//TBO����
	GLuint tex_tbo;//TBO����ͼ����
	SkeletonData skeleton;

	void load_dae(const char*);

	/*Buffer Object �������*/
	bool in_bo;//�Ƿ���buffer object��
	GLuint vbo_indics[6];//6��VBO

	GLenum front_face = GL_CCW;
	//GLuint ubo_indics[.];

	void pose_skeleton(int frame);
	void pose_bone(int idx, int frame, const float* parent_world_matrix);
	void update_joints_matrics();
};

