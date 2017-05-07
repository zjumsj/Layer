/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"

#include <iostream>
#include "fire.h"
#include <vector>
using namespace std;

/*
 glActiveTexture(GL_TEXTURE0) ��ͨ��ͼ
 glActiveTexture(GL_TEXTURE1) ��Ӱ����
 ϵͳ֧�ֵ���������Ⱥͳ��ȣ���ѯ glGetIntegerv(GL_MAX_TEXTURE_SIZE, GLint*)
 �ҵĻ�����GT240 1GB GDDR5 OpenGL 3.3���������ߴ�Ϊ 8192
*/
const float ratio = 0.15;
const int fire_num = 4;
ParticalSystem fire[fire_num];
Vector3D pos[fire_num] =
{
	{ (float)2.0,	(float)0.0,(float)0.1 },
	{ (float)-2.0,	(float)2.0,	(float)2.2 },
	{ (float)2.0,	(float)0.0,	(float)2.2 },
	{ (float)0,	(float)-2,	(float)4.3 }
};


struct obj {
	vector<glm::vec3> vertices;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normals;
};

obj wall_1, wall_2, wall_3, door, floor_1,sd,ms;

const int lights_num = 4;

GLuint tex_walls[2], tex_shadow[lights_num];
GLuint frame_buffer_s;
const int shadow_w=2048, shadow_h=2048;
glm::vec4 light_pos[lights_num]; // ��������
bool light_rotate=true, save_shadow=false;
static FTFont* ftgl;

int Mo::State = 2;//���ھ�̬�������г�ʼ�� �������״̬

int Mo::flash = 0;
int Mo::appear = 0;

float Mo::angle_d = 0;
int Mo::flag_d = 0;
int Mo::flag_d1 = 0;
float Mo::p_m = -2;
GLuint programID;

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = NULL;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader, 100);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			//cout << "Get v" << endl;
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z, 100);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			//cout << "Get vt" << endl;
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y, 10);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			//cout << "Get vn" << endl;
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z, 100);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			//cout << "Get f" << endl;
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], 100);
			if (matches != 6) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
		

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_normals.push_back(normal);

	}
	cout << "finished!\n" << endl;
	return true;
}

//bool loadOBJ_1(
//	const char * path,
//	std::vector<glm::vec3> & out_vertices,
//	std::vector<glm::vec2> & out_uvs,
//	std::vector<glm::vec3> & out_normals
//) {
//	printf("Loading OBJ file %s...\n", path);
//
//	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
//	std::vector<glm::vec3> temp_vertices;
//	std::vector<glm::vec2> temp_uvs;
//	std::vector<glm::vec3> temp_normals;
//
//
//	FILE * file = fopen(path, "r");
//	if (file == NULL) {
//		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
//		getchar();
//		return false;
//	}
//
//	while (1) {
//
//		char lineHeader[128];
//		 read the first word of the line
//		int res = fscanf(file, "%s", lineHeader);
//		if (res == EOF)
//			break; // EOF = End Of File. Quit the loop.
//
//				    else : parse lineHeader
//
//		if (strcmp(lineHeader, "v") == 0) {
//			glm::vec3 vertex;
//			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
//			temp_vertices.push_back(vertex);
//		}
//		else if (strcmp(lineHeader, "vt") == 0) {
//			glm::vec2 uv;
//			fscanf(file, "%f %f\n", &uv.x, &uv.y);
//			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
//			temp_uvs.push_back(uv);
//		}
//		else if (strcmp(lineHeader, "vn") == 0) {
//			glm::vec3 normal;
//			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
//			temp_normals.push_back(normal);
//		}
//		else if (strcmp(lineHeader, "f") == 0) {
//			std::string vertex1, vertex2, vertex3;
//			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
//			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
//			if (matches == 9) {
//				vertexIndices.push_back(vertexIndex[0]);
//				vertexIndices.push_back(vertexIndex[1]);
//				vertexIndices.push_back(vertexIndex[2]);
//				uvIndices.push_back(uvIndex[0]);
//				uvIndices.push_back(uvIndex[1]);
//				uvIndices.push_back(uvIndex[2]);
//				normalIndices.push_back(normalIndex[0]);
//				normalIndices.push_back(normalIndex[1]);
//				normalIndices.push_back(normalIndex[2]);
//			}
//			else {
//				fseek(file,,SEEK_CUR);
//				matches = fscanf_s(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], 100);
//				if (matches != 6) {
//					printf("File can't be read by our simple parser :-( Try exporting with other options\n");
//					return false;
//				}
//				vertexIndices.push_back(vertexIndex[0]);
//				vertexIndices.push_back(vertexIndex[1]);
//				vertexIndices.push_back(vertexIndex[2]);
//				normalIndices.push_back(normalIndex[0]);
//				normalIndices.push_back(normalIndex[1]);
//				normalIndices.push_back(normalIndex[2]);
//			}
//			
//		}
//		else {
//			 Probably a comment, eat up the rest of the line
//			char stupidBuffer[1000];
//			fgets(stupidBuffer, 1000, file);
//		}
//
//	}
//
//
//
//	// For each vertex of each triangle
//	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
//
//		// Get the indices of its attributes
//		unsigned int vertexIndex = vertexIndices[i];
//		unsigned int uvIndex = uvIndices[i];
//		unsigned int normalIndex = normalIndices[i];
//
//		// Get the attributes thanks to the index
//		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
//		glm::vec2 uv = temp_uvs[uvIndex - 1];
//		glm::vec3 normal = temp_normals[normalIndex - 1];
//
//		// Put the attributes in buffers
//		out_vertices.push_back(vertex);
//		out_uvs.push_back(uv);
//		out_normals.push_back(normal);
//
//	}
//
//	cout << "finished!\n" << endl;
//	return true;
//}

bool loadOBJ_1(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				//fprintf(file,"%s",10);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	cout << "finish!" << endl;
	return true;
}
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	cout << "finished!" << endl;

	return ProgramID;
}

//���� in English Texture
void tex_init()
{
	//�Ѿ����޸Ĺ���
	glActiveTexture(GL_TEXTURE0); // ��ͨ��ͼ
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//���õ�ǰ������ӳ�䷽ʽ
	void* data; int w, h;
	il_readImg(L"pic\\bb.jpg", &data, &w, &h);//��ȡ����
	glGenTextures(4, tex_walls);//����һ����������
	glBindTexture(GL_TEXTURE_2D, tex_walls[0]);//2D����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//��������
	delete data;
	//�����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//---------
	glActiveTexture(GL_TEXTURE2); // ��ͨ��ͼ
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//���õ�ǰ������ӳ�䷽ʽ
	il_readImg(L"pic\\zq.jpg", &data, &w, &h);//��ȡ����
	glBindTexture(GL_TEXTURE_2D, tex_walls[1]);//2D����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//��������
	delete data;
	//�����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//---------
	glActiveTexture(GL_TEXTURE3); // ��ͨ��ͼ
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//���õ�ǰ������ӳ�䷽ʽ
	il_readImg(L"pic\\js.jpg", &data, &w, &h);//��ȡ����
	glBindTexture(GL_TEXTURE_2D, tex_walls[2]);//2D����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//��������
	delete data;
	//�����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//---------
	glActiveTexture(GL_TEXTURE4); // ��ͨ��ͼ
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//���õ�ǰ������ӳ�䷽ʽ
	il_readImg(L"pic\\tms.bmp", &data, &w, &h);//��ȡ����
	glBindTexture(GL_TEXTURE_2D, tex_walls[3]);//2D����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//��������
	delete data;
	//�����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//---------
	glActiveTexture(GL_TEXTURE1); // ��Ӱ���� ����ǰ��Ծ��texture��Ϊtexture1��
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//���õ�ǰ����ӳ�䷽ʽ

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);//��������� S T R ����������
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);//��glTexGeni ���״��� ����������
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);

	glGenTextures(lights_num, tex_shadow);//���������ʹ洢ָ�� ������������  ������Ҫ������������������ �ԡ�
	for(int i=0; i<lights_num; ++i){
		glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);//lights_num=3; ����3����Դ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat c[4] = {1,1,1, 1};
		//��֮�������ò���
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_w, shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);//void * data=NULL ?  
	}
	// frame buffer object
	glGenFramebuffers(1, &frame_buffer_s);//����frame_buffer_s ����ͼ����������
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);//��
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow, 0);
}

void xoy_foor(float len, float tex_repeat, int subdivision)
{
	GLfloat color[]={.8f, 0.8f, 0.8f, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	
	glMatrixMode(GL_MODELVIEW);//ѡ�����ģʽ
	glPushMatrix();//BEGIN
	glScalef(len, len, 1);//ģ�����ź���
	glNormal3f(0,0,1);//���÷��߷���
	float u = 1.0f/subdivision;
	float tu = tex_repeat/subdivision;
	for(int i=0; i<subdivision; ++i)
	for(int j=0; j<subdivision; ++j){
		float ox = u*i, oy = u*j;
		float tox = tu*i, toy = tu*j;
		glBegin(GL_POLYGON);
		glTexCoord2f(tox   , toy);		glVertex3f(ox  , oy  , 0);//������ͼλ�� ���õ��λ��
		glTexCoord2f(tox+tu, toy);		glVertex3f(ox+u, oy  , 0);//
		glTexCoord2f(tox+tu, toy+tu);	glVertex3f(ox+u, oy+u, 0);
		glTexCoord2f(tox   , toy+tu);	glVertex3f(ox  , oy+u, 0);
		glEnd();
	}
	glPopMatrix();//END
}

void dlight(glm::vec4& light_pos) // �ڵ��ϻ���һ����
{
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	GLfloat cc[4]; glGetFloatv(GL_CURRENT_COLOR, cc);
	glColor3f(1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	glutSolidSphere(0.1f, 50, 50);
	glPopMatrix();
	if(li) glEnable(GL_LIGHTING);
	glColor4fv(cc);
}
inline void drawdoor_1();
inline void drawdoor_2();
inline void drawdoor_3();
inline void draw_tt();
inline void draw_goast();

void draw_world()
{
	glStaff::xyz_frame(2,2,2,false);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();



	//
	glLoadIdentity();
	glTranslatef(0.2, -0.1, -0.5);
	glRotatef(180, 0, 1, 0);
	glRotatef(20, 0, 0, 1);
	glScalef(0.2, 0.2, 0.2);

	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < sd.vertices.size(); i++)
	{

		glNormal3f(sd.normals[i].x, sd.normals[i].y, sd.normals[i].z);
		glVertex3f(sd.vertices[i].x, sd.vertices[i].y, sd.vertices[i].z);
	}
	glEnd();

	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);

	glActiveTexture(GL_TEXTURE3); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);
	glPopMatrix();
	//
	
	


	//draw_tt();
	
	glm::vec3 eye;
	eye = glStaff::Internal::get_eye();
	float t;
	t=eye.y;
	eye.y = eye.z;
	eye.z = t;
	//glTranslatef(eye.x + mat_view[0][0], eye.y + mat_view[2][0], eye.x + mat_view[1][0]);
	//glutSolidSphere(0.2, 50, 50);
	//glTranslatef(-eye.x - mat_view[0][0], -eye.y - mat_view[2][0], -eye.x - mat_view[1][0]);
	//glLoadIdentity();
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE4); glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	if (Mo::appear == 1&&Mo::angle_d>20)
		draw_goast();
	glEnable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE4); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
	//glPopMatrix();
	
	glRotatef(90, -1, 0, 0);
	//glRotatef(90, 0,  0, 1);
	//glTranslatef(-10, -10, 0);
		
	
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < floor_1.vertices.size(); i++)
	{

		glNormal3f(floor_1.normals[i].x, floor_1.normals[i].y, floor_1.normals[i].z);
		glVertex3f(floor_1.vertices[i].x, floor_1.vertices[i].y, floor_1.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glTranslatef(0, 0, 0.1);

	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < wall_1.vertices.size(); i++)
	{

		glNormal3f(wall_1.normals[i].x, wall_1.normals[i].y, wall_1.normals[i].z);
		glVertex3f(wall_1.vertices[i].x, wall_1.vertices[i].y, wall_1.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);



	drawdoor_1();
	glTranslatef(-2.5,-1,0);

	
	glDisable(GL_LIGHTING);
	fire_render(fire, 4, ratio);
	glEnable(GL_LIGHTING);
	


	glTranslatef(0, 0, 2);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < floor_1.vertices.size(); i++)
	{

		glNormal3f(floor_1.normals[i].x, floor_1.normals[i].y, floor_1.normals[i].z);
		glVertex3f(floor_1.vertices[i].x, floor_1.vertices[i].y, floor_1.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glTranslatef(0, 0, 0.1);

	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < wall_2.vertices.size(); i++)
	{

		glNormal3f(wall_2.normals[i].x, wall_2.normals[i].y, wall_2.normals[i].z);
		glVertex3f(wall_2.vertices[i].x, wall_2.vertices[i].y, wall_2.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	drawdoor_2();

	glTranslatef(1, -2.5, 0);



	glTranslatef(0, 0, 2);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < floor_1.vertices.size(); i++)
	{

		glNormal3f(floor_1.normals[i].x, floor_1.normals[i].y, floor_1.normals[i].z);
		glVertex3f(floor_1.vertices[i].x, floor_1.vertices[i].y, floor_1.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glTranslatef(0, 0, 0.1);

	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < wall_3.vertices.size(); i++)
	{

		glNormal3f(wall_3.normals[i].x, wall_3.normals[i].y, wall_3.normals[i].z);
		glVertex3f(wall_3.vertices[i].x, wall_3.vertices[i].y, wall_3.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);


	drawdoor_3();

	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	
	//xoy_foor(20, 3, 100);//���̸����ɴ���
	

	glPopMatrix();
}


GLint flag_d=0;
inline void drawdoor_1() {
	glTranslatef(0.5, 1, 0);

	glRotatef(Mo::angle_d, 0, 0, 1);

	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < door.vertices.size(); i++)
	{

		glNormal3f(door.normals[i].x, door.normals[i].y, door.normals[i].z);
		glVertex3f(door.vertices[i].x, door.vertices[i].y, door.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glRotatef(-Mo::angle_d, 0, 0, 1);





	glTranslatef(-3, 0, 0);
	glRotatef(180, 0, 0, 1);


	glRotatef(Mo::angle_d, 0, 0, 1);
	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < door.vertices.size(); i++)
	{

		glNormal3f(door.normals[i].x, door.normals[i].y, door.normals[i].z);
		glVertex3f(door.vertices[i].x, door.vertices[i].y, door.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glRotatef(-Mo::angle_d, 0, 0, 1);

	glTranslatef(-3.5, 3.5, 0);
	glRotatef(90, 0, 0, 1);

	glRotatef(Mo::angle_d, 0, 0, 1);
	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < door.vertices.size(); i++)
	{

		glNormal3f(door.normals[i].x, door.normals[i].y, door.normals[i].z);
		glVertex3f(door.vertices[i].x, door.vertices[i].y, door.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glRotatef(-Mo::angle_d, 0, 0, 1);

	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);

}

inline void drawdoor_2() {
	glTranslatef(1,0.5, 0);

	glRotatef(90-Mo::angle_d, 0, 0, 1);

	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < door.vertices.size(); i++)
	{

		glNormal3f(door.normals[i].x, door.normals[i].y, door.normals[i].z);
		glVertex3f(door.vertices[i].x, door.vertices[i].y, door.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glRotatef(Mo::angle_d-90, 0, 0, 1);





	glTranslatef(0, 1, 0);
	glRotatef(180, 0, 0, 1);


	glRotatef(90-Mo::angle_d, 0, 0, 1);
	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < door.vertices.size(); i++)
	{

		glNormal3f(door.normals[i].x, door.normals[i].y, door.normals[i].z);
		glVertex3f(door.vertices[i].x, door.vertices[i].y, door.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glRotatef(Mo::angle_d-90, 0, 0, 1);

	glTranslatef(2, -1, 0);
	glRotatef(180, 0, 0, 1);

	glRotatef(90-Mo::angle_d, 0, 0, 1);
	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < door.vertices.size(); i++)
	{

		glNormal3f(door.normals[i].x, door.normals[i].y, door.normals[i].z);
		glVertex3f(door.vertices[i].x, door.vertices[i].y, door.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glRotatef(Mo::angle_d-90, 0, 0, 1);

	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);

}

inline void drawdoor_3() {
	glTranslatef(-1.5, -1, 0);

	glRotatef(Mo::angle_d, 0, 0, 1);

	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < door.vertices.size(); i++)
	{

		glNormal3f(door.normals[i].x, door.normals[i].y, door.normals[i].z);
		glVertex3f(door.vertices[i].x, door.vertices[i].y, door.vertices[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glRotatef(-Mo::angle_d, 0, 0, 1);





	glTranslatef(0, 1, 0);
	glRotatef(180, 0, 0, 1);


	glRotatef(Mo::angle_d, 0, 0, 1);
	glActiveTexture(GL_TEXTURE2); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);


	

	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);

}
inline void draw_goast() {
	int lev = glStaff::Internal::in_which_l();
	int blo = glStaff::Internal::in_which_b();

	if (lev == 1 && ((blo == 2) || (blo == 3))) {
		glTranslatef(-2, 1.4, -1);
		glTranslatef(0, 0, Mo::p_m);
		glRotatef(180, 0, 1, 0);
		 
		glScalef(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < ms.vertices.size(); i++)
		{
			glTexCoord2f(ms.uvs[i].x, ms.uvs[i].y);
			glNormal3f(ms.normals[i].x, ms.normals[i].y, ms.normals[i].z);
			glVertex3f(ms.vertices[i].x, ms.vertices[i].y, ms.vertices[i].z);
		}
		glEnd();
		glScalef(1 / 0.7, 1 / 0.7, 1 / 0.7);
		 
		glRotatef(-180, 0, 1, 0);
		glEnable(GL_LIGHTING);
		glTranslatef(0, 0, -Mo::p_m);
		glTranslatef(2, -1.4, 1);
		return;
	}
	else if (lev == 1 && ((blo == 6) || (blo == 5))) {
		glTranslatef(0, 1.4, -1);
		glTranslatef(0, 0, -Mo::p_m);
		 
		glScalef(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < ms.vertices.size(); i++)
		{
			glTexCoord2f(ms.uvs[i].x, ms.uvs[i].y);
			glNormal3f(ms.normals[i].x, ms.normals[i].y, ms.normals[i].z);
			glVertex3f(ms.vertices[i].x, ms.vertices[i].y, ms.vertices[i].z);
		}
		glEnd();
		glScalef(1 / 0.7, 1 / 0.7, 1 / 0.7);
		 
		glEnable(GL_LIGHTING);
		glTranslatef(0, 0, Mo::p_m);
		glTranslatef(0, -1.4, 1);
		return;
	}
	else if (lev == 1 && ((blo == 4) || (blo == 7))) {
		glTranslatef(1, 1.4, 2);
		glTranslatef(Mo::p_m, 0, 0);
		glRotatef(-90, 0, 1, 0);
		 
		glScalef(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < ms.vertices.size(); i++)
		{
			glTexCoord2f(ms.uvs[i].x, ms.uvs[i].y);
			glNormal3f(ms.normals[i].x, ms.normals[i].y, ms.normals[i].z);
			glVertex3f(ms.vertices[i].x, ms.vertices[i].y, ms.vertices[i].z);
		}
		glEnd();
		glScalef(1 / 0.7, 1 / 0.7, 1 / 0.7);
		 
		glRotatef(90, 0, 1, 0);
		glEnable(GL_LIGHTING);
		glTranslatef(-Mo::p_m, 0, 0);
		glTranslatef(-1, -1.4, -2);
		return;
	}
	else if (lev == 2 && ((blo == 4) || (blo == 5))) {
		glTranslatef(0, 3.4, 1);
		glTranslatef(0, 0, Mo::p_m);
		glRotatef(180, 0, 1, 0);
		 
		glScalef(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < ms.vertices.size(); i++)
		{
			glTexCoord2f(ms.uvs[i].x, ms.uvs[i].y);
			glNormal3f(ms.normals[i].x, ms.normals[i].y, ms.normals[i].z);
			glVertex3f(ms.vertices[i].x, ms.vertices[i].y, ms.vertices[i].z);
		}
		glEnd();
		glScalef(1 / 0.7, 1 / 0.7, 1 / 0.7);
		 
		glRotatef(-180, 0, 1, 0);
		glEnable(GL_LIGHTING);
		glTranslatef(0, 0, -Mo::p_m);
		glTranslatef(0, -3.4, -1);
		return;
	}
	else if (lev == 2 && ((blo == 7) || (blo == 8) || (blo == 9))) {
		glTranslatef(2, 3.4, -1);
		glTranslatef(0, 0, Mo::p_m);
		glRotatef(-180, 0, 1, 0);
		 
		glScalef(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < ms.vertices.size(); i++)
		{
			glTexCoord2f(ms.uvs[i].x, ms.uvs[i].y);
			glNormal3f(ms.normals[i].x, ms.normals[i].y, ms.normals[i].z);
			glVertex3f(ms.vertices[i].x, ms.vertices[i].y, ms.vertices[i].z);
		}
		glEnd();
		glScalef(1 / 0.7, 1 / 0.7, 1 / 0.7);
		 
		glRotatef(180, 0, 1, 0);
		
		glTranslatef(0, 0, -Mo::p_m);
		glTranslatef(-2, -3.4, 1);

		glTranslatef(2, 3.4, 1);
		glTranslatef(0, 0, -Mo::p_m);
		glRotatef(180, 0, 1, 0);
		 
		glScalef(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < ms.vertices.size(); i++)
		{
			glTexCoord2f(ms.uvs[i].x, ms.uvs[i].y);
			glNormal3f(ms.normals[i].x, ms.normals[i].y, ms.normals[i].z);
			glVertex3f(ms.vertices[i].x, ms.vertices[i].y, ms.vertices[i].z);
		}
		glEnd();
		glScalef(1 / 0.7, 1 / 0.7, 1 / 0.7);
		 
		glRotatef(180, 0, 1, 0);
		glEnable(GL_LIGHTING);
		glTranslatef(0, 0, Mo::p_m);
		glTranslatef(-2, -3.4, -1);
		return;
	}
	else if (lev == 3 && ((blo == 6) || (blo == 3))) {
		glTranslatef(-1, 5.4, -2);
		glTranslatef(-Mo::p_m, 0, 0);
		glRotatef(90, 0, 1, 0);
		 
		glScalef(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < ms.vertices.size(); i++)
		{
			glTexCoord2f(ms.uvs[i].x, ms.uvs[i].y);
			glNormal3f(ms.normals[i].x, ms.normals[i].y, ms.normals[i].z);
			glVertex3f(ms.vertices[i].x, ms.vertices[i].y, ms.vertices[i].z);
		}
		glEnd();
		glScalef(1 / 0.7, 1 / 0.7, 1 / 0.7);
		 
		glRotatef(-90, 0, 1, 0);
		glEnable(GL_LIGHTING);
		glTranslatef(Mo::p_m, 0, 0);
		glTranslatef(1, -5.4, 2);
		return;	
	}
	else
		return;


}
void draw_model()
{	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(90,0,1,0);

	//glActiveTexture(GL_TEXTURE2);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);

	/*glBegin(GL_TRIANGLES);
	for (int i = 0; i < vertices1.size(); i++)
	{
		if (i % 3 == 0) {
			glColor3f(1, 0, 0);
			glTexCoord2f(1.0f  ,0.0f);
		}
	else if (i % 3 == 1) {
			glColor3f(0, 1, 0);
			glTexCoord2f(0.0f  , 1.0f);
	}else {
		glColor3f(0, 0, 1);
		glTexCoord2f(0.0f   , 0.0f);}

		glNormal3f(normals1[i].x, normals1[i].y, normals1[i].z);
		glVertex3f(vertices1[i].x, vertices1[i].y, vertices1[i].z);
	}
	glEnd();*/
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();
}
//�������⼸���������л���
//ģʽ�ƺ���ͬ ��������漴����
void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();//��õ�ǰʱ�䣿
	//-------------------------------- ��1�λ��ƣ������������ -------------------------
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);//frame_buffer_s a gluint ѡ��һ��Framebuffer
	//target framebuffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);//������ɫ���������ı�
	//Specify whether red, green, blue, and alpha can or cannot be written into the frame buffer
	glViewport(0, 0, shadow_w, shadow_h);//�����ӿ�
	//GLenum re = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	// ����Ҫ���պ�����
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);//��õ�ǰ����1 Disable?
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);//�����޳�Ч�� �����������Ч����

	glMatrixMode(GL_PROJECTION); glPushMatrix();//ѡ�����ģʽ Projection=ͶӰ
	glMatrixMode(GL_MODELVIEW); glPushMatrix();//�Ӿ����� ����ѹ��ջ����

	//DEBUG 12.8
	glm::mat4 shadow_mat_p = glm::perspective(glm::radians(100.0f), 1.0f, 0.2f, 1.0e10f);//����һ��͸�ӱ任
	//mat4 4��Ԫ�� ÿ��Ԫ����12��float
	glm::vec3 cen = glm::vec3(glStaff::get_mat_model()*glm::vec4(0,0,0,1));//get_mat_model ����һ�� {1 1 1 0} 3{0 0 0 0} 
	glm::mat4 shadow_mat_v[lights_num];
	for(int i=0; i<lights_num; ++i){//�Թ�Դ���в���
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow[i], 0);//tex_shadow �洢��Ӧ�������� texshadow[i] ѡ���Ӧ���ӽ�
		glClear(GL_DEPTH_BUFFER_BIT);//�ر����ѡ��
		shadow_mat_v[i] = glm::lookAt(glm::vec3(light_pos[i]), cen, glm::vec3(0,1,0));//ͷ������ ��һ�������ǵƵ�λ�� cen�ǣ�
		glMatrixMode(GL_PROJECTION); glLoadMatrixf(&shadow_mat_p[0][0]);//ͶӰ  replace the current matrix with the specified matrix
		glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&shadow_mat_v[i][0][0]);//�Ӿ����� ��������ջ
			draw_world();
		glMultMatrixf(&mat_model[0][0]);//����˷�
			//draw_model();//ע��֮������Ӱ������
	}

	glMatrixMode(GL_PROJECTION); glPopMatrix();//����������
	glMatrixMode(GL_MODELVIEW); glPopMatrix();//����������
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);//�����޳� ���ñ������

	//������ֿ��Բ��� �Ǹ��ӵĲ���
	if(save_shadow){
		save_shadow = false;
		glActiveTexture(GL_TEXTURE1);
		GLfloat* data = new GLfloat[shadow_w*shadow_h];//GL ��ǰ��������shadow_w=2048 shadow_h=2048
		for(int i=0; i<lights_num; ++i){
			glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);//������������ GL_TEXTURE_2D
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);//��ȡ�������� GL_DEPTH_COMPONENT ʹ����ȣ�
			//data��֮ǰ��ͼƬ��õ����� ���ڴ���
			wchar_t ss[50]; swprintf(ss, L"d%d.png", i);//����������
			il_saveImgDep(ss, data, shadow_w, shadow_w);//����ͼƬ�� ���� ָ�� �� ��
		}
		delete[] data;
	}

	// �ָ��ӿڡ���ɫģ�塢����
	glViewport(0, 0, glStaff::get_frame_width(), glStaff::get_frame_height());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(li) glEnable(GL_LIGHTING);//���ӹ���Ч��

	//-------------------------------- ��2�λ��ƣ����Ƴ��� ----------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//ָ����ȾpipelineΪĬ��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//�����ɫ ��Ȼ���
	// 1 ������
	for(int i=0; i<lights_num; ++i) glDisable(GL_LIGHT0+i);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);//����1 Disable 2D
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);// ����0 ʹ��2D
	//float gac2[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2); // black

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2); glEnable(GL_TEXTURE_2D);
	glMultMatrixf(&mat_model[0][0]);
		//glBindTexture(GL_TEXTURE_2D, tex_walls[0]);//2D����
		//draw_model();
	
	//glBindTexture(GL_TEXTURE_2D,tex_walls[1]);
	
	// 2 ���Դ
	GLfloat la[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black ��ʱֻ���Ƶ��Դ
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);//ʹ��
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);
	//����ָ����Ȼ���Ƚ�ֵ
	for(int i=0; i<((Mo::flash == 1) ? lights_num : 3); ++i){ // lights_num
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
		// When the eye planes are specified, the GL will automatically post-multiply them
		// with the inverse of the current modelview matrix.
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&mat_view[0][0]); //glLoadIdentity();
		glm::mat4 mat = glm::translate(glm::vec3(0.5f,0.5f,0.5f))
			* glm::scale(glm::vec3(0.5f,0.5f,0.5f)) * shadow_mat_p * shadow_mat_v[i];
		mat = glm::transpose(mat);
		glTexGenfv(GL_S, GL_EYE_PLANE, &mat[0][0]);
		glTexGenfv(GL_T, GL_EYE_PLANE, &mat[1][0]);
		glTexGenfv(GL_R, GL_EYE_PLANE, &mat[2][0]);
		glTexGenfv(GL_Q, GL_EYE_PLANE, &mat[3][0]);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&mat_view[0][0]);
			glLightfv(GL_LIGHT0+i, GL_POSITION, &light_pos[i][0]); // λ��ʽ��Դ
		glEnable(GL_LIGHT0+i);

		glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&mat_view[0][0]);
			draw_world();
		glMultMatrixf(&mat_model[0][0]); glDisable(GL_TEXTURE_2D);
			draw_model();//ע�͵�֮��ͼ���Ǻڵ� 

		glDisable(GL_LIGHT0+i);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // �ָ�������
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// 3 ���ƹ�Դ������
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	

	//���Ź���
	if (Mo::flag_d == 1)
		Mo::angle_d += 5.0f;
	else
		Mo::angle_d -= 5.0f;
	if (Mo::angle_d >= 20.0f&&Mo::flag_d1 == 0)
		Mo::angle_d = 20.0f;
	else if (Mo::flag_d1 == 1 && Mo::angle_d >= 90)
		Mo::angle_d = 90;
	if (Mo::angle_d <= 0.0f) {
		Mo::angle_d = 0.0f;
		Mo::flag_d1 = 0;
	}
	//���Ź���

	if (Mo::appear == 1 && Mo::angle_d > 20)
		Mo::p_m += 0.2f;
	if (Mo::p_m >= 0)
		Mo::p_m = 0;
	if (Mo::appear == 0||Mo::angle_d<=20)
		Mo::p_m = -2;
	
	//����
	fire_sim(fire, fire_num, pos, ratio);
	//����


	if(light_rotate){
		for(int i=0; i<3; ++i)
			light_pos[i] = glm::rotate(glm::radians(1.0f), glm::vec3(0,0.1,0)) * light_pos[i];
	}

	if (Mo::State) {
		//(eye[0]<1 && eye[0]>-1 && eye[1] < 2.1&&eye[1]>0 && eye[2]<3.2&&eye[2]>1.2)
		glm::vec3 eye;
		
		eye = glStaff::Internal::get_eye();

		light_pos[3] = glm::vec4(eye.x,eye.y,eye.z, 1);
		/*light_pos[3].x -= mat_view[0][0] * 2;
		light_pos[3].y -= mat_view[1][0] * 2;
		light_pos[3].z -= mat_view[2][0] * 2;*/
	}

	for(int i=0; i<((Mo::flash == 1) ? lights_num : 3); ++i) dlight(light_pos[i]);

	tt = omp_get_wtime()-tt;
	char ss[50];
	sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
}





void key_p(){light_rotate = !light_rotate;}
void key_j(){glMatrixMode(GL_PROJECTION);glRotatef(1, 0, -1, 0);}
void key_l(){glMatrixMode(GL_PROJECTION);glRotatef(-1, 0, -1, 0);}
void key_i(){glMatrixMode(GL_PROJECTION);glTranslatef(0.1, 0, 0);}
void key_k(){glMatrixMode(GL_PROJECTION);glTranslatef(-0.1, 0, 0);}

void key_u()
{
	save_shadow = true;
	std::cout << "shadow map saved" << '\n';
}



int main(void)
{
	

	//
	glStaff::Internal::_angle=0;
	//
	glStaff::init_win(1200, 800, "OpenGL     - YYF MSJ MYK ", "msyh.ttf");
	glStaff::init_gl();

	glStaff::set_mat_view(glm::lookAt( glm::vec3(-2.4,1.8,5.5), glm::vec3(-2.4,1.9,0), glm::vec3(0,1,0) ) );
	//eyex eyey eyez centerx centery centerz upx upy upz �����ӽǾ��� ���λ��12 12 12 ע�ӵ� 0 0 0  ͷ������y��
	glStaff::set_mat_model( glm::translate( glm::vec3(0,1,0) ) );
	bool res1 = loadOBJ("obj\\wall_1.obj", wall_1.vertices, wall_1.uvs, wall_1.normals);
	bool res2 = loadOBJ("obj\\wall_2.obj", wall_2.vertices, wall_2.uvs, wall_2.normals);
	bool res3 = loadOBJ("obj\\wall_3.obj", wall_3.vertices, wall_3.uvs, wall_3.normals);
	bool res4 = loadOBJ("obj\\door.obj", door.vertices, door.uvs, door.normals);
	bool res5 = loadOBJ("obj\\floor.obj", floor_1.vertices, floor_1.uvs, floor_1.normals);
	bool res6 = loadOBJ("obj\\sd.obj", sd.vertices, sd.uvs, sd.normals);
	bool res7 = loadOBJ_1("obj\\ONI.obj", ms.vertices, ms.uvs, ms.normals);
	//���ն���GL��ɫ��� ǰ������RGB ����һ����͸����
	//���ڹ�Դ����Ⱦ����
	GLfloat vec4f[4]={0};//GL Lightfv
	vec4f[0]=0.35f; vec4f[1]=0.35f; vec4f[2]=0.3f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4f); // red  ���ù�Դ���ɢ���
	glLightfv(GL_LIGHT0, GL_SPECULAR, vec4f);// ���ù�Դ��ľ��淴��
	vec4f[0]=0.35f; vec4f[1]=0.35f; vec4f[2]=0.3f;
	glLightfv(GL_LIGHT1, GL_DIFFUSE, vec4f); // green
	glLightfv(GL_LIGHT1, GL_SPECULAR, vec4f);
	vec4f[0]=0.35f; vec4f[1]=0.35f; vec4f[2]=0.3f; 
	glLightfv(GL_LIGHT2, GL_DIFFUSE, vec4f); // blue
	glLightfv(GL_LIGHT2, GL_SPECULAR, vec4f);
	vec4f[0]=0.35f; vec4f[1]=0.35f; vec4f[2]=0.3f; 
	glLightfv(GL_LIGHT3, GL_DIFFUSE, vec4f); // blue
	glLightfv(GL_LIGHT3, GL_SPECULAR, vec4f);

	

	vec4f[0]=0; vec4f[1]=0; vec4f[2]=0;//���û����� Ϊ��
	glLightfv(GL_LIGHT0, GL_AMBIENT, vec4f); // black AMBIENT
	glLightfv(GL_LIGHT1, GL_AMBIENT, vec4f);
	glLightfv(GL_LIGHT2, GL_AMBIENT, vec4f);
	glLightfv(GL_LIGHT3, GL_AMBIENT, vec4f);

	//���ó����Ļ�����RGBAǿ�� red green blue alpha
	float gac2[4]={0.05f,0.05f,0.05f,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2);//���û������ǿ��

	//��������λ������
	light_pos[0] = glm::vec4(-1.5, 6, 1.5, 1);
	light_pos[1] = glm::vec4(-1.8, 4, -1.8, 1);
	light_pos[2] = glm::vec4(2, 2, 1, 1);
	light_pos[3] = glm::vec4(0, 0, 0, 1);

	glStaff::add_key_callback('P', key_p, L"�ı��Դλ��");
	glStaff::add_key_callback('U', key_u, L"������Ӱͼ��d.png��");
	glStaff::add_key_callback('J', key_j, L"��ת");
	glStaff::add_key_callback('L', key_l, L"��ת");
	glStaff::add_key_callback('I', key_i, L"��ǰ");
	glStaff::add_key_callback('K', key_k, L"���");

	//glutSpecialFunc(keyboard);
	
	tex_init();//����ĳ�ʼ�� 
	
	ftgl = new FTExtrudeFont("msyh.ttf");
	ftgl->FaceSize(1);
	ftgl->Depth(0.1f);
	fire_init(fire, fire_num, ratio);
	/*
	glMatrixMode(GL_PROJECTION); 
	glm::mat4 shadow_mat_p = glm::perspective(glm::radians(50.0f), 2.0f, 0.1f, 1.0e10f);//����һ��͸�ӱ任
	glLoadMatrixf(&shadow_mat_p[0][0]);
	*/
	
	glStaff::renderLoop(draw);//���õĻص����� ���л�ͼ
	//ÿ�ζ������½�ģ�ģ�����
}




inline void draw_tt() {


	glActiveTexture(GL_TEXTURE4); glEnable(GL_TEXTURE_2D);


	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_S);
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, ms.vertices.size() * sizeof(glm::vec3), &ms.vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, ms.uvs.size() * sizeof(glm::vec2), &ms.uvs[0], GL_STATIC_DRAW);

	{

	


		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, ms.vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_S);
	
}
