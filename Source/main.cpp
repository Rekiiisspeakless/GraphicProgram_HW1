#include "../Include/Common.h"
#include <cmath>

//For GLUT to handle 
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3
#define MENU_SLOW 4
#define MENU_FAST 5


//GLUT timer variable
GLubyte timer_cnt = 16;
bool timer_enabled = false;
unsigned int timer_speed = 0;

using namespace glm;
using namespace std;

mat4 view;					// V of MVP, viewing matrix
mat4 projection;			// P of MVP, projection matrix
mat4 model;					// M of MVP, model matrix
mat4 rotate_mat = mat4();

vec3 temp = vec3();
GLfloat speed = 0.1f;
GLfloat torsorDegree = 0;
GLfloat headDegree = 0;
GLfloat leftHandDegree = 0;
GLfloat rightHandDegree = 0;
GLfloat leftLegDegree = 0;
GLfloat rightLegDegree = 0;
GLfloat torsorBounce = 0.05f;
GLfloat headBounce = 0.03f;
GLfloat leftHandBounce = 0.05f;
GLfloat rightHandBounce = 0.05f;
GLfloat leftLegBounce = 0.05f;
GLfloat rightLegBounce = 0.05f;

//mouse position
GLfloat mousePressX = 0.0f;
GLfloat mousePressY = 0.0f;
GLfloat mouseRotateX = 0.0f;
GLfloat mouseRotateY = 0.0f;
GLfloat prevMouseRotateX = 0.0f;
GLfloat prevMouseRotateY = 0.0f;



GLint um4mvp;			// shader mvp uniform id 
GLuint program;			// shader program id

						// Load shader file to program
char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

// Free shader file
void freeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

// OpenGL initialization
void My_Init()
{
	// Clear display windows color to white color(1.0,1.0,1.0,1.0)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Enable depth test, will not draw pixels that are blocked by other pixels 
	glEnable(GL_DEPTH_TEST);

	// Set the type of depth test
	glDepthFunc(GL_LEQUAL);

	// Create Shader Program
	program = glCreateProgram();

	// Create customize shader by tell openGL specify shader type 
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load shader file
	char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");

	// Assign content of these shader files to those shaders we created before
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	// Free the shader file string(won't be used any more)
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);

	// Compile these shaders
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Logging
	shaderLog(vertexShader);
	shaderLog(fragmentShader);

	// Assign the program we created before with these shaders
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Get the id of inner variable 'um4mvp' in shader programs
	um4mvp = glGetUniformLocation(program, "um4mvp");

	// Tell OpenGL to use this shader program now
	glUseProgram(program);

	//Create vertex array object and bind it to  OpenGL (OpenGL will apply operation only to the vertex array objects it bind)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	//Enable shader layout location 0 and 1 for vertex and color   
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//Create buffer and bind it to OpenGL
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

vec3 Normalized(vec3 a, vec3 b, float length) {
	float dx, dy, dz;
	dx = a.x - b.x;
	dy = a.y - b.y;
	dz = a.z - b.z;

	dx = dx * length / distance(a, b);
	dy = dy * length / distance(a, b);
	dz = dz * length / distance(a, b);
	vec3 c = vec3(a.x + dx, a.y + dy, a.z + dz);
	return c;
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	// Clear display buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Create vertex and color data
	/*float data[18] = {

		-0.5f, -0.4f, 0.0f,
		0.5f, -0.4f, 0.0f,
		0.0f,  0.6f, 0.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};*/
	float cubeSize = 0.5f;
	int cubeVertNum = 36;
	int vertSize = 36 * 3;
	/*const int octahedronSize = 10;
	float octahedronRate = 1.0f;
	float octahedronData[octahedronSize * octahedronSize * 8 * 3 * 2];
	int  octahedronVect = octahedronSize * octahedronSize * 3;
	int v = 0;
	
	float unitLength = octahedronRate / octahedronSize;
	for (int i = 0; i < octahedronSize; ++i) {
		float upStartVect = (i + 1) / 2 - octahedronRate / 2;
		float downStartVect = i / 2 - octahedronRate / 2;
		float k = downStartVect;
		for (int j = 0; j < octahedronSize - i; ++j, ++v) {
			octahedronData[v * 3 + 2] = 3 * i * unitLength / 4;
			octahedronData[v * 3 + 0] = k;
			octahedronData[v * 3 + 1] = downStartVect;

			
			v++;

			octahedronData[v * 3 + 5] = 3 * i * unitLength / 4;
			octahedronData[v * 3 + 3] = k + unitLength;
			octahedronData[v * 3 + 4] = downStartVect ;

			v++;

			octahedronData[v * 3 + 8] = 3 * (i + 1) * unitLength / 4;
			octahedronData[v * 3 + 6] = k ;
			octahedronData[v * 3 + 7] = downStartVect + unitLength / 2;

			k += unitLength;
		}
		//TODO
		k = upStartVect;
		for (int j = 0; j < octahedronSize - i - 1; ++j, ++v) {
			octahedronData[v * 3 + 2] = 3 * (i + 1) * unitLength / 4;
			octahedronData[v * 3 + 0] = k ;
			octahedronData[v * 3 + 1] = upStartVect ;


			v++;

			octahedronData[v * 3 + 5] = 3 * (i + 1) * unitLength / 4;
			octahedronData[v * 3 + 3] = k + unitLength;
			octahedronData[v * 3 + 4] = upStartVect;

			v++;

			octahedronData[v * 3 + 8] = 3 * i * unitLength / 4;
			octahedronData[v * 3 + 6] = k + unitLength / 2;
			octahedronData[v * 3 + 7] = downStartVect - unitLength / 2;

			k += unitLength;
		}
	}*/
	const int gridNum = 20;
	const float unitCubeLength = 0.5f / gridNum;
	float gridCube[36 * 3 * 2 * gridNum * gridNum];
	float gridSample[36 * 3 * 2]{
		//cubefront 0 ~ 17
		-0.25f,  -0.25f + unitCubeLength, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f + unitCubeLength, -0.25f, -0.25f,

		-0.25f + unitCubeLength, -0.25f, -0.25f,
		-0.25f + unitCubeLength,  -0.25f + unitCubeLength, -0.25f,
		-0.25f,  -0.25f + unitCubeLength, -0.25f,

		//cuberight 18 ~ 35
		0.25f, -0.25f, -0.25f,
		0.25f, -0.25f,  -0.25f + unitCubeLength,
		0.25f,  -0.25f + unitCubeLength, -0.25f,

		0.25f, -0.25f,  -0.25f + unitCubeLength,
		0.25f,  -0.25f + unitCubeLength,  -0.25f + unitCubeLength,
		0.25f,  -0.25f + unitCubeLength, -0.25f,

		//cubeback 36 ~ 53
		-0.25f + unitCubeLength, -0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		-0.25f + unitCubeLength,  -0.25f + unitCubeLength,  0.25f,

		-0.25f, -0.25f,  0.25f,
		-0.25f,  -0.25f + unitCubeLength,  0.25f,
		-0.25f + unitCubeLength,  -0.25f + unitCubeLength,  0.25f,

		//cubeleft 54 ~ 71
		-0.25f, -0.25f,  -0.25f + unitCubeLength,
		-0.25f, -0.25f, -0.25f,
		-0.25f,  -0.25f + unitCubeLength,  -0.25f + unitCubeLength,

		-0.25f, -0.25f, -0.25f,
		-0.25f,  -0.25f + unitCubeLength, -0.25f,
		-0.25f,  -0.25f + unitCubeLength,  -0.25f + unitCubeLength,

		//cubedown 72 ~ 89
		-0.25f, -0.25f,  -0.25f + unitCubeLength,
		-0.25f + unitCubeLength, -0.25f,  -0.25f + unitCubeLength,
		-0.25f + unitCubeLength, -0.25f, -0.25f,

		-0.25f + unitCubeLength, -0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f,  -0.25f + unitCubeLength,

		//cubeup 90 ~ 107
		-0.25f,  0.25f, -0.25f,
		-0.25f + unitCubeLength,  0.25f, -0.25f,
		-0.25f + unitCubeLength,  0.25f,  -0.25f + unitCubeLength,

		-0.25f + unitCubeLength,  0.25f,  -0.25f + unitCubeLength,
		-0.25f,  0.25f,  -0.25f + unitCubeLength,
		-0.25f,  0.25f, -0.25f,

		//color
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f

	};
	//front
	int s = 0;
	for (int i = 0; i < gridNum; ++i) {
		for (int j = 0; j < gridNum; ++j) {
			for (int k = 0; k < 6; ++k) {
				gridCube[s] = gridSample[k * 3 + 0] + unitCubeLength * i;
				s++;
				gridCube[s] = gridSample[k * 3 + 1] + unitCubeLength * j;
				s++;
				gridCube[s] = gridSample[k * 3 + 2];
				s++;
			}
		}
	}
	//right
	for (int i = 0; i < gridNum; ++i) {
		for (int j = 0; j < gridNum; ++j) {
			for (int k = 6; k < 12; ++k) {
				gridCube[s] = gridSample[k * 3 + 0];
				s++;
				gridCube[s] = gridSample[k * 3 + 1] + unitCubeLength * j;
				s++;
				gridCube[s] = gridSample[k * 3 + 2] + unitCubeLength * i;
				s++;
			}
		}
	}
	//back
	for (int i = 0; i < gridNum; ++i) {
		for (int j = 0; j < gridNum; ++j) {
			for (int k = 12; k < 18; ++k) {
				gridCube[s] = gridSample[k * 3 + 0] + unitCubeLength * i;
				s++;
				gridCube[s] = gridSample[k * 3 + 1] + unitCubeLength * j;
				s++;
				gridCube[s] = gridSample[k * 3 + 2];
				s++;
			}
		}
	}
	//left
	for (int i = 0; i < gridNum; ++i) {
		for (int j = 0; j < gridNum; ++j) {
			for (int k = 18; k < 24; ++k) {
				gridCube[s] = gridSample[k * 3 + 0];
				s++;
				gridCube[s] = gridSample[k * 3 + 1] + unitCubeLength * j;
				s++;
				gridCube[s] = gridSample[k * 3 + 2] + unitCubeLength * i;
				s++;
			}
		}
	}
	//down
	for (int i = 0; i < gridNum; ++i) {
		for (int j = 0; j < gridNum; ++j) {
			for (int k = 24; k < 30; ++k) {
				gridCube[s] = gridSample[k * 3 + 0] + unitCubeLength * i;
				s++;
				gridCube[s] = gridSample[k * 3 + 1];
				s++;
				gridCube[s] = gridSample[k * 3 + 2] + unitCubeLength * j;
				s++;
			}
		}
	}
	//up
	for (int i = 0; i < gridNum; ++i) {
		for (int j = 0; j < gridNum; ++j) {
			for (int k = 30; k < 36; ++k) {
				gridCube[s] = gridSample[k * 3 + 0] + unitCubeLength * i;
				s++;
				gridCube[s] = gridSample[k * 3 + 1];
				s++;
				gridCube[s] = gridSample[k * 3 + 2] + unitCubeLength * j;
				s++;
			}
		}
	}

	for (int i = 36 * gridNum * gridNum; i < 36 * gridNum * gridNum * 2; i++) {
		gridCube[i * 3 + 0] = 1.0f;
		gridCube[i * 3 + 1] = 0.0f;
		gridCube[i * 3 + 2] = 0.0f;
	}

	const int cylinderNum = 10;
	const float radian = 0.5f;
	const float cylinderHeight = 1.5f;
	const int circleVertNum = cylinderNum;
	const float radPerTriangle = 360 / (float)cylinderNum;
	float cylinderData[(3 * cylinderNum * 2 + 6 * cylinderNum) * 3 * 2];
	int cylinder_tmp = 0;
	for (int i = 0; i < circleVertNum; ++i) {
		cylinderData[i * 9 + 0] = 0.0f;
		cylinderData[i * 9 + 1] = cylinderHeight / 2;
		cylinderData[i * 9 + 2] = 0.0f;

		cylinderData[i * 9 + 3] = radian * cos(deg2rad(radPerTriangle * i));
		cylinderData[i * 9 + 4] = cylinderHeight / 2;
		cylinderData[i * 9 + 5] = radian * sin(deg2rad(radPerTriangle * i));

		cylinderData[i * 9 + 6] = radian * cos(deg2rad(radPerTriangle * (i + 1)));
		cylinderData[i * 9 + 7] = cylinderHeight / 2;
		cylinderData[i * 9 + 8] = radian * sin(deg2rad(radPerTriangle * (i + 1)));
	}
	cylinder_tmp = 9 * circleVertNum;
	for (int i = 0; i < circleVertNum; ++i) {
		cylinderData[cylinder_tmp + i * 9 + 0] = 0.0f;
		cylinderData[cylinder_tmp + i * 9 + 1] = -cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 9 + 2] = 0.0f;

		cylinderData[cylinder_tmp + i * 9 + 3] = radian * cos(deg2rad(radPerTriangle * i));
		cylinderData[cylinder_tmp + i * 9 + 4] = -cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 9 + 5] = radian * sin(deg2rad(radPerTriangle * i));

		cylinderData[cylinder_tmp + i * 9 + 6] = radian * cos(deg2rad(radPerTriangle * (i + 1)));
		cylinderData[cylinder_tmp + i * 9 + 7] = -cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 9 + 8] = radian * sin(deg2rad(radPerTriangle * (i + 1)));
	}
	cylinder_tmp *= 2;
	for (int i = 0; i < circleVertNum; ++i) {
		cylinderData[cylinder_tmp + i * 18 + 0] = radian * cos(deg2rad(radPerTriangle * i));
		cylinderData[cylinder_tmp + i * 18 + 1] = cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 18 + 2] = radian * sin(deg2rad(radPerTriangle * i));

		cylinderData[cylinder_tmp + i * 18 + 3] = radian * cos(deg2rad(radPerTriangle * (i + 1)));
		cylinderData[cylinder_tmp + i * 18 + 4] = cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 18 + 5] = radian * sin(deg2rad(radPerTriangle * (i + 1)));

		cylinderData[cylinder_tmp + i * 18 + 6] = radian * cos(deg2rad(radPerTriangle * i));
		cylinderData[cylinder_tmp + i * 18 + 7] = -cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 18 + 8] = radian * sin(deg2rad(radPerTriangle * i));

		cylinderData[cylinder_tmp + i * 18 + 9] = radian * cos(deg2rad(radPerTriangle * i));
		cylinderData[cylinder_tmp + i * 18 + 10] = -cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 18 + 11] = radian * sin(deg2rad(radPerTriangle * i));

		cylinderData[cylinder_tmp + i * 18 + 12] = radian * cos(deg2rad(radPerTriangle * (i + 1)));
		cylinderData[cylinder_tmp + i * 18 + 13] = -cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 18 + 14] = radian * sin(deg2rad(radPerTriangle * (i + 1)));

		cylinderData[cylinder_tmp + i * 18 + 15] = radian * cos(deg2rad(radPerTriangle * (i + 1)));
		cylinderData[cylinder_tmp + i * 18 + 16] = cylinderHeight / 2;
		cylinderData[cylinder_tmp + i * 18 + 17] = radian * sin(deg2rad(radPerTriangle * (i + 1)));
	}
	cylinder_tmp *= 2;
	for (int i = 0; i < circleVertNum * 3  * 2 * 2; ++i) {
		cylinderData[cylinder_tmp + i * 3 + 0] = 0.0f;
		cylinderData[cylinder_tmp + i * 3 + 1] = 1.0f;
		cylinderData[cylinder_tmp + i * 3 + 2] = 3.0f;
	}

	float data[36 * 3 * 2]{
		//vertices
		-0.25f,  0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f, -0.25f,

		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		0.25f,  0.25f, -0.25f,

		0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f,  0.25f,

		-0.25f,  0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f,  0.25f,  0.25f,

		0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f, -0.25f,

		//color
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < 36 * gridNum * gridNum; ++i) {
		vec3 originVec = vec3(gridCube[i * 3 + 0], gridCube[i * 3 + 1], gridCube[i * 3 + 2]);
		vec3 center = vec3(0, 0, 0);
		vec3 normalizedVec = Normalized(originVec, center, 0.5f / 2);
		gridCube[i * 3 + 0] = normalizedVec.x;
		gridCube[i * 3 + 1] = normalizedVec.y;
		gridCube[i * 3 + 2] = normalizedVec.z;
	}

	float torsorData[36 * 3 * 2] = {0};
	for (int i = 0; i < cubeVertNum * 2; ++i) {
		
		if (i < cubeVertNum) {
			torsorData[i * 3 + 1] = data[i * 3 + 1] * 3.0f;
			torsorData[i * 3 + 0] = data[i * 3 + 0] * 2.0f;
		}
		else {
			torsorData[i * 3 + 1] = data[i * 3 + 1];
			torsorData[i * 3 + 0] = data[i * 3 + 0];
		}
		
		torsorData[i * 3 + 2] = data[i * 3 + 2];
	}
	
	
	/*for (int i = 0; i < cubeVertNum * gridNum * gridNum * 2; ++i) {

		if (i < cubeVertNum * gridNum * gridNum) {
			gridCube[i * 3 + 1] = gridCube[i * 3 + 1] * 3.0f;
			gridCube[i * 3 + 0] = gridCube[i * 3 + 0] * 2.0f;
		}
		else {
			gridCube[i * 3 + 1] = gridCube[i * 3 + 1];
			gridCube[i * 3 + 0] = gridCube[i * 3 + 0];
		}

		gridCube[i * 3 + 2] = gridCube[i * 3 + 2];
	}*/



	//Put data into buffer
	//glBufferData(GL_ARRAY_BUFFER, sizeof(torsorData), torsorData, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderData), cylinderData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 36 * 3));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * circleVertNum * 3 * 2 * 2 * 3));

	// Tell openGL to use the shader program we created before
	glUseProgram(program);

	//TODO :
	//Create rotate and translate martix and calculate model matrix
	mat4 translate_mat = translate(mat4(), temp);
	
	
	torsorDegree += torsorBounce;
	//cout << "torsorDegree: " << torsorDegree << endl;
	if (torsorDegree >= 45 || torsorDegree <= -45) {
		torsorBounce = -torsorBounce;
	}
	
	
	rotate_mat = rotate(mat4(), deg2rad (torsorDegree + mouseRotateX + prevMouseRotateX), vec3(0.0f, 1.0f, 0.0f));

	//mat4 mouseRotate = rotate(mat4(), mouseRotateDegree, vec3(0.0f, 1.0f, 0.0f));
	
	model =  translate_mat * rotate_mat;
	mat4 torsorRotate = rotate_mat;
	//mat4 torsorModel = translate_mat;*/
	mat4 torsorModel = model;

	//cout << "( " << temp.x << ", " << temp.y << ", " << temp.z << " )" << endl;
	//Calculate mvp matrix by ( projection * view * model ) 
	mat4 mvp = projection * view * model;

	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));



	// Tell openGL to draw the vertex array we had binded before
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawArrays(GL_TRIANGLES, 0, circleVertNum * 3  * 2 * 2);

	//float headData[36 * 3 * 2];
	float headData[36 * gridNum * gridNum * 3 * 2];
	/*for (int i = 0; i < cubeVertNum * 2; ++i) {
		if (i < cubeVertNum) {
			headData[i * 3 + 0] = data[i * 3 + 0];
			headData[i * 3 + 1] = data[i * 3 + 1];
			headData[i * 3 + 2] = data[i * 3 + 2];
		}
		else {
			headData[i * 3 + 0] = 0.0f;
			headData[i * 3 + 1] = 0.3f;
			headData[i * 3 + 2] = 0.5f;
		}
	}*/

	for (int i = 0; i < cubeVertNum * gridNum * gridNum * 2; ++i) {
		if (i < cubeVertNum * gridNum * gridNum) {
			headData[i * 3 + 0] = gridCube[i * 3 + 0];
			headData[i * 3 + 1] = gridCube[i * 3 + 1];
			headData[i * 3 + 2] = gridCube[i * 3 + 2];
		}
		else {
			headData[i * 3 + 0] = 0.0f;
			headData[i * 3 + 1] = 0.3f;
			headData[i * 3 + 2] = 0.5f;
		}
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(headData), headData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 36 * 3));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 36 * gridNum * gridNum * 3));
	
	headDegree += headBounce;
	//cout << "torsorDegree: " << torsorDegree << endl;
	if (headDegree >= 30 || headDegree <= -30) {
		headBounce = -headBounce;
	}
	mat4 headRotate = rotate(mat4(), deg2rad(headDegree), vec3(0.0f, 1.0f, 0.0f));
	translate_mat = translate(mat4(), vec3(0.0f, 1.25f, 0.0f));
	mat4 prev_model = torsorModel;
	model = translate_mat * headRotate;
	mat4 headModel = model;
	mvp = projection * view  * torsorModel *  model;

	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));



	// Tell openGL to draw the vertex array we had binded before
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawArrays(GL_TRIANGLES, 0, 36 * gridNum * gridNum);

	//hands
	//left hand

	float leftHandData[36 * 3 * 2];
	for (int i = 0; i < cubeVertNum * 2; ++i) {
		if (i < cubeVertNum) {
			leftHandData[i * 3 + 0] = data[i * 3 + 0] * 0.5f;
			leftHandData[i * 3 + 1] = data[i * 3 + 1] * 1.5f;
			leftHandData[i * 3 + 2] = data[i * 3 + 2];
		}
		else {
			leftHandData[i * 3 + 0] = 0.0f;
			leftHandData[i * 3 + 1] = 0.5f;
			leftHandData[i * 3 + 2] = 0.3f;
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(leftHandData), leftHandData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 36 * 3));

	translate_mat = translate(mat4(), vec3(-2.5f * 0.25f, 0.375f, 0.0f));
	mat4 temp_mat1 = translate(mat4(), vec3(0.0f, -1.5f * 0.25f, 0.0f));
	mat4 temp_mat2 = translate(mat4(), vec3(0.0f, 1.5f * 0.25f, 0.0f));

	leftHandDegree += leftHandBounce;
	//cout << "torsorDegree: " << torsorDegree << endl;
	if (leftHandDegree >= 45 || leftHandDegree <= -45) {
		leftHandBounce = -leftHandBounce;
	}


	mat4 leftHandRotate = rotate(mat4(), deg2rad(leftHandDegree), vec3(-1.0f, 0.0f, 0.0f));

	prev_model = torsorModel;
	/*model = translate_mat;
	mat4 leftHandModel = model;
	mvp = projection * view * torsorRotate  * model  * prev_model  * temp_mat2  * leftHandRotate  * temp_mat1 ;*/
	model = translate_mat * temp_mat2 * leftHandRotate * temp_mat1;
	mvp = projection * view * torsorModel * model;

	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));



	// Tell openGL to draw the vertex array we had binded before
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right hand

	float rightHeadData[36 * 3 * 2];
	for (int i = 0; i < cubeVertNum * 2; ++i) {
		if (i < cubeVertNum) {
			rightHeadData[i * 3 + 0] = data[i * 3 + 0] * 0.5f;
			rightHeadData[i * 3 + 1] = data[i * 3 + 1] * 1.5f;
			rightHeadData[i * 3 + 2] = data[i * 3 + 2];
		}
		else {
			rightHeadData[i * 3 + 0] = 0.0f;
			rightHeadData[i * 3 + 1] = 0.5f;
			rightHeadData[i * 3 + 2] = 0.3f;
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(rightHeadData), rightHeadData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 36 * 3));

	translate_mat = translate(mat4(), vec3(2.5f * 0.25f, 0.375f, 0.0f));

	rightHandDegree -= rightHandBounce;
	//cout << "torsorDegree: " << torsorDegree << endl;
	if (rightHandDegree >= 45 || rightHandDegree <= -45) {
		rightHandBounce = -rightHandBounce;
	}


	mat4 rightHandRotate = rotate(mat4(), deg2rad(rightHandDegree), vec3(-1.0f, 0.0f, 0.0f));
	prev_model = torsorModel;
	//model = translate_mat;
	model = translate_mat * temp_mat2 * rightHandRotate * temp_mat1;
	mat4 rightHandModel = model;
	//mvp = projection * view * torsorRotate * model * prev_model * temp_mat2 * rightHandRotate * temp_mat1;
	mvp = projection * view * torsorModel * model;


	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));

	// Tell openGL to draw the vertex array we had binded before
	glDrawArrays(GL_TRIANGLES, 0, 36);

	///leg
	//left leg

	float leftLegData[36 * 3 * 2];
	for (int i = 0; i < cubeVertNum * 2; ++i) {
		if (i < cubeVertNum) {
			leftLegData[i * 3 + 0] = data[i * 3 + 0] * 0.5f;
			leftLegData[i * 3 + 1] = data[i * 3 + 1] * 1.5f;
			leftLegData[i * 3 + 2] = data[i * 3 + 2];
		}
		else {
			leftLegData[i * 3 + 0] = 0.3f;
			leftLegData[i * 3 + 1] = 0.3f;
			leftLegData[i * 3 + 2] = 0.3f;
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(leftLegData), leftLegData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 36 * 3));

	translate_mat = translate(mat4(), vec3(-0.375f, -4.5 * 0.25, 0.0f));
	mat4 temp_mat3 = translate(mat4(), vec3(0.0f, -1.5f * 0.25f, 0.0f));
	mat4 temp_mat4 = translate(mat4(), vec3(0.0f, 1.5f * 0.25f, 0.0f));

	leftLegDegree -= leftLegBounce;
	//cout << "torsorDegree: " << torsorDegree << endl;
	if (leftLegDegree >= 45 || leftLegDegree <= -45) {
		leftLegBounce = -leftLegBounce;
	}


	mat4 leftLegRotate = rotate(mat4(), deg2rad(leftLegDegree), vec3(-1.0f, 0.0f, 0.0f));
	prev_model = torsorModel;
	//model = translate_mat;
	model = translate_mat * temp_mat4 * leftLegRotate * temp_mat3; 
	mat4 leftLegModel = model;
	//mvp = projection * view * torsorRotate * model * prev_model * temp_mat4 * leftLegRotate * temp_mat3;
	mvp = projection * view * torsorModel * model;

	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));



	// Tell openGL to draw the vertex array we had binded before
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right leg

	float rightLegData[36 * 3 * 2];
	for (int i = 0; i < cubeVertNum * 2; ++i) {
		if (i < cubeVertNum) {
			rightLegData[i * 3 + 0] = data[i * 3 + 0] * 0.5f;
			rightLegData[i * 3 + 1] = data[i * 3 + 1] * 1.5f;
			rightLegData[i * 3 + 2] = data[i * 3 + 2];
		}
		else {
			rightLegData[i * 3 + 0] = 0.3f;
			rightLegData[i * 3 + 1] = 0.3f;
			rightLegData[i * 3 + 2] = 0.3f;
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(rightLegData), rightLegData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 36 * 3));

	translate_mat = translate(mat4(), vec3(0.375f, -4.5 * 0.25, 0.0f));
	rightLegDegree += rightLegBounce;
	//cout << "torsorDegree: " << torsorDegree << endl;
	if (rightLegDegree >= 45 || rightLegDegree <= -45) {
		rightLegBounce = -rightLegBounce;
	}


	mat4 rightLegRotate = rotate(mat4(), deg2rad(rightLegDegree), vec3(-1.0f, 0.0f, 0.0f));
	prev_model = torsorModel;
	model = translate_mat * temp_mat4 * rightLegRotate * temp_mat3;
	mat4 rightLegModel = model;
	//mvp = projection * view * torsorRotate * model * prev_model * temp_mat4 * rightLegRotate * temp_mat3;
	mvp = projection * view * torsorModel * model;

	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));



	// Tell openGL to draw the vertex array we had binded before
	glDrawArrays(GL_TRIANGLES, 0, 36);

	const int ringPower = 6;
	const int ringVertNum = 64; // 2^ringPower
	const int ringMask = ringVertNum - 2;
	const int sectionInRing = ringVertNum / 2 - 1;
	const int sphereVertNum = sectionInRing * ringVertNum;
	const float sectionArc = 6.28 / sectionInRing;

	const float sphereRadius = -0.125f; // radius = 10
	float sphereData[sphereVertNum * 3 * 2];
	float angleX, angleY;
	for (int i = 0; i < sphereVertNum; ++i) {
		angleX = (float)(i & 1) + (i >> ringPower);
		angleY = (float)((i & ringMask) >> 1) + ((i >> ringPower) * (sectionInRing));

		angleX *= (float)sectionArc / 2.0f;
		angleY *= (float)sectionArc*-1;
		sphereData[i * 3 + 0] = sphereRadius * sin(angleX) * sin(angleY);
		sphereData[i * 3 + 1] = sphereRadius * cos(angleX);
		sphereData[i * 3 + 2] = sphereRadius * sin(angleX) * cos(angleY);
	}
	for (int i = sphereVertNum; i < 2 * sphereVertNum; ++i) {
		sphereData[i * 3 + 0] = 0.4f;
		sphereData[i * 3 + 1] = 0.1f;
		sphereData[i * 3 + 2] = 0.0f;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereData), sphereData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * sphereVertNum * 3));

	translate_mat = translate(mat4(), vec3(-0.25f, 0.0f, 0.5f));
	model = translate_mat;

	mvp = projection * view * torsorModel * headModel * model;


	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));

	// Tell openGL to draw the vertex array we had binded before
	glDrawArrays(GL_TRIANGLE_STRIP, 0, sphereVertNum);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * sphereVertNum * 3));

	translate_mat = translate(mat4(), vec3(0.25f, 0.0f, 0.5f));
	model = translate_mat;

	mvp = projection * view * torsorModel * headModel * model;


	// Transfer value of mvp to both shader's inner variable 'um4mvp'; 
	glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));

	// Tell openGL to draw the vertex array we had binded before
	glDrawArrays(GL_TRIANGLE_STRIP, 0, sphereVertNum);

	////////////////////test////////////////
	//Put data into buffer
	/*glBufferData(GL_ARRAY_BUFFER, sizeof(octahedronData), octahedronData, GL_STATIC_DRAW);

	//Select which range as vertex and which as color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * octahedronVect * 3));
	translate_mat = translate(mat4(), vec3(0.0f, 1.0f, 0.0f));
	prev_model = model;
	model = translate_mat;
	mvp = projection * view * model * prev_model;
	glDrawArrays(GL_TRIANGLES, 0, octahedronSize * octahedronSize * 3);*/


	// Change current display buffer to another one (refresh frame) 
	glutSwapBuffers();
}



// Setting up viewing matrix
// This function will be called whenever the size of windows is changing
void My_Reshape(int width, int height)
{
	// Setting where to display and size of OpenGL display area
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 1000.0f);
	// TODO :
	// Building a projection matrix by calling perspective
	// perspective(fov, aspect_ratio, near_plane_distance, far_plane_distance)
	// fov = field of view, it represent how much range(degree) is this camera could see 
	// projection = perspective( ... , ... , ... );
	view = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
	// TODO :
	// Building a viewing matrix by calling lookAt
	// lookAt(camera_position, camera_viewing_vector, up_vector)
	// up_vector represent the vector which define the direction of 'up'
	// view = lookAt( ... , ... , ... );
}


void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if (timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}



void My_Keyboard(unsigned char key, int x, int y)
{
	//TODO :
	//When button down translate the triangle 

	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	if (key == 'd')
	{
		temp += vec3(1.0f, 0.0f, 0.0f);
	}
	else if (key == 'a')
	{
		temp += vec3(-1.0f, 0.0f, 0.0f);
	}
	else if (key == 'w')
	{
		temp += vec3(0.0f, 1.0f, 0.0f);
	}
	else if (key == 's')
	{
		temp += vec3(0.0f, -1.0f, 0.0f);
	}
}

void My_SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id)
{
	switch (id)
	{
	case MENU_TIMER_START:
		if (!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
		//TODO :
		//You should add new case to deal with MENU_SLOW and MENU_FAST




	default:
		break;
	}
}
void My_Motion_Mouse(int x, int y) {
	mouseRotateX = x - mousePressX;
	mouseRotateY = y - mousePressY;
	glutPostRedisplay();
}

void My_Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mousePressX = x;
			mousePressY = y;
			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			prevMouseRotateX += x - mousePressX;
			prevMouseRotateY += y - mousePressY;

			mouseRotateX = 0.0f;
			mouseRotateY = 0.0f;
			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
}

void My_newMenu(int id)
{
	
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	glutInit(&argc, argv);

	// TODO :
	// Setting display mode for GLUT here
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	
	// TODO :
	// Creating windows here (remeber to set both size & position for it )
	
	glutInitWindowPosition( 500 , 100 );
	glutInitWindowSize( 600 , 600 );
	glutCreateWindow( "Practice2" );


#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();

	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);
	int menu_speed = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	//TODO :
	//Create new entry about MENU_SLOW and MENU_FAST under the speed menu


	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0);
	glutMouseFunc(My_Mouse);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}