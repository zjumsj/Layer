
#include "gl_inc.h"
#include "ft_inc.h"
#include "il_inc.h"
#include <vector>
using namespace std;


class Mo;
class manage;
class object;

//ȫ�ֱ���
class Mo{
public: 
	static int State;//1 ��ʾ��Ϸ״̬ 0 ��ʾ����״̬
	static POINT mousepoint;
	static int flash;
	static int flag_d;
	static int flag_d1;
	static float angle_d;
	static int appear;
	static float p_m;

public:
	static void Mode(void);
	static void level(float &x, float &y, float &z);
};
/*
  1 | 2 | 3
  4 | 5 | 6
  7 | 8 | 9
  */
class manage{
public:
	//�������
	int num;
	object * fp;
	//���λ��
	int grid;
	float x;
	float y;//�߼�����
	



};

class object{
public:
	manage * parent;//ͨ��ָ�����ϼ���Ϣ
	int grid;//�������µ�λ��
	float physic_x;//��������
	float physic_y;//��������
	float x;//�߼�����
	float y;//�߼�����
	float speed;
	vector<glm::vec3> vertices;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normals;
	


public:
	void appear();//�������
	void amaze();//���˵Ķ�����
};


