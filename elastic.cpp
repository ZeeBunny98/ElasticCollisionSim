#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<iostream>
#include<cmath>
#include<string>
#include<utility>

//Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float posOffset;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x + posOffset, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

class box
{
	public:
		int mass;
		int initial_velocity;
		float left_side;
		float right_side;
};

class largeBox: public box{
	public:
		largeBox()
		{
			mass = 2;
			initial_velocity = 0.01;
			right_side = 1.0f;
			left_side = 1.0f - 0.5f;
		}
};

class smallBox: public box{
	public:
		smallBox()
		{
			mass = 1;
			initial_velocity = 0.01;
			left_side = -1.0f;
			right_side = -1.0f + 0.25f;
		}
};

class window
{
	public:
		int widthWindow = 800;
		int heightWindow = 800;
		const char* title = "Elastic or Inelastic Collision Sim";
		GLFWmonitor* full = NULL;
		GLFWwindow* share = NULL;
};

bool Collision_detect_large_wall(float side)
{
	if (side >= 1.0f)
	{
		return true;
	}
	else {return false;}
}
bool Collision_detect_small_wall(float side)
{
	if (side <= -1.0f)
	{
		return true;
	}
	else {return false;}
}
bool check_box_collision(float largeSide, float smallSide)
{
	if (largeSide <= smallSide)
	{
		return true;
	}
	return false;
}
std::pair<float, float> elastic_collision(int massL, int massS, float init_velocityL, float init_velocityS)
{
	float total_mass = float(massL + massS);
	float mass_diff = float(massL - massS);
	float final_velocityL = ((mass_diff/total_mass)*init_velocityL) + ((float)((2.0f*massS)/total_mass)*init_velocityS);
	float final_velocityS = (((2.0f*massL)/total_mass)*init_velocityL) - (((float)mass_diff/total_mass)*init_velocityS);
	return { final_velocityL, final_velocityS };
}

GLfloat vertices[] = {
	0.5f, -1.0f, 0.0f, //Bottom Left Corner
	0.5f, -0.5f, 0.0f, //Upper Left Corner
	1.0f, -0.5f, 0.0f, //Upper Right Corner
	1.0, -1.0f, 0.0f //Bottom Right Corner
};

GLfloat verticesSmall[] = {
	-1.0f, -1.0f, 0.0f, //Bottom Left Corner
	-1.0f, -0.75f, 0.0f, //Upper Left Corner
	-0.75f, -0.75f, 0.0f, //Upper Right Corner
	-0.75, -1.0f, 0.0f //Bottom Right Corner
};

GLuint indices[] = {
	0, 1, 2,
	3, 0, 2
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window glfwWindow;

	GLFWwindow* window = glfwCreateWindow(glfwWindow.widthWindow, glfwWindow.heightWindow, glfwWindow.title, glfwWindow.full, glfwWindow.share);
	if (window == NULL)
	{
		std::cout << "Window Error, Check GLFWwindow* window, check window class, and check glfwWindow Object" << std::endl;\
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0,0, glfwWindow.widthWindow, glfwWindow.heightWindow);
	//Shaders ----------------------------------------------------------------------------------------------------------------------

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//End of Shaders ---------------------------------------------------------------------------------------------------------------

	//VAOs + VBOs + EBOs -----------------------------------------------------------------------------------------------------------
	
	GLuint VAOs[2], VBOs[2], EBOs[2];

	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);
	glGenBuffers(2, EBOs);

	//1st Square

	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//2nd Square

	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSmall), verticesSmall, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Bind VAO + VBO + EBO to prevent accidental changes to them

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//End of VAOs + VBOs + EBOs -----------------------------------------------------------------------------------------------------

	largeBox large_box_obj;
	smallBox small_box_obj;

	float prevTime = float(glfwGetTime());
	float posL = 0.0f, posS = 0.0f;
	float deltaXL = 0.01f, deltaXS = 0.01f;

	GLuint posOffsetID = glGetUniformLocation(shaderProgram, "posOffset");

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		float currTime = float(glfwGetTime());
		if (currTime-prevTime >= 0.05)
		{
			posL += deltaXL;
			posS += deltaXS;
			large_box_obj.right_side += deltaXL;
			small_box_obj.right_side += deltaXS;
			small_box_obj.left_side += deltaXS;
			large_box_obj.left_side += deltaXL;
			prevTime = currTime;
		}

		if (Collision_detect_large_wall(large_box_obj.right_side))
		{
			deltaXL*= -1;
			float overlap1 = large_box_obj.right_side - 1.0f;
			posL -= overlap1;
			large_box_obj.left_side -= overlap1;
			large_box_obj.right_side -= overlap1;
		}
		if (Collision_detect_small_wall(small_box_obj.left_side))
		{
			deltaXS*= -1;
			float overlap2 = -1.0f - small_box_obj.left_side;
			posS += overlap2;
			small_box_obj.left_side += overlap2;
			small_box_obj.right_side += overlap2;
		}
		
		if (check_box_collision(large_box_obj.left_side, small_box_obj.right_side))
		{
			auto [large_obj_velocity, small_obj_velocity] = elastic_collision(large_box_obj.mass, small_box_obj.mass, deltaXL, deltaXS);
			deltaXL = large_obj_velocity;
			deltaXS = small_obj_velocity;

			float overlap = small_box_obj.right_side - large_box_obj.left_side;
			large_box_obj.left_side += overlap / 2.0f;
			large_box_obj.right_side += overlap / 2.0f;
			small_box_obj.left_side -= overlap / 2.0f;
			small_box_obj.right_side -= overlap / 2.0f;

			posL += overlap / 2.0f;
			posS -= overlap / 2.0f;
		}
		
		glUseProgram(shaderProgram);
		
		glUniform1f(posOffsetID, posL);
		glBindVertexArray(VAOs[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glUniform1f(posOffsetID, posS);
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}