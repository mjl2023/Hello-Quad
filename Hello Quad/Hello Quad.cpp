// Hello Quad.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*

	CREATES A QUAD - TWO TRIANGLES
    
    WITH INDEXING

*/
#include <iostream>
#include <sstream>

// Includes for OpenGL - open a window on Windows
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Constants for creating a window
const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Quad";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;
// wireframe mode
bool gWireframe = false;

// shaders - normally held in files on hard drive
const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"void main()"
"{"
"    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);"
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"out vec4 frag_color;"
"void main()"
"{"
"    frag_color = vec4(0.5f, 0.0f, 0.2f, 1.0f);"
"}";

// Keyboard callback
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
// fps
void showFPS(GLFWwindow* window);
// clean up initialization
bool initOpenGL();

int main()
{
    if (!initOpenGL())
    {
        std::cerr << "GLFW initialization failed" << std::endl;
        return -1;
    }

    // vertices for triangle
	GLfloat vertices[] = {
        // tri 0
       -0.5f,  0.5f, 0.0f, // top
        0.5f,  0.5f, 0.0f, // right
        0.5f, -0.5f, 0.0f, // left
	   -0.5f,  -0.5f, 0.0f // top
	   
    };

	// indices for triangle
	GLuint indices[] = {
		0, 1, 2, // first triangle
		0, 2, 3  // second triangle
	};

    //vbo - vertex buffer object
    GLuint vbo, ibo, vao;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
       
    //vao - vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // set vertex attributes pointer - position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    //ibo - index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // create and compile vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSrc, NULL);
    glCompileShader(vs);

    //check for shader compile errors
    GLint result;
    GLchar infoLog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
    }

    // create and compile fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fs);

    //check for shader compile errors
    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Error: Fragment shader compilation failed\n" << infoLog << std::endl;
    }

    // create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
    }

    //  delete shaders - we don't need them anymore
    glDeleteShader(vs);
    glDeleteShader(fs);

    // main loop
    while (!glfwWindowShouldClose(gWindow))
    {
        // get fps
        showFPS(gWindow);

        glfwPollEvents();

        // clear color
        /**/
        glClear(GL_COLOR_BUFFER_BIT);

        // use shader program
        glUseProgram(shaderProgram);

        // draw triangle
        glBindVertexArray(vao);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // buffer on screen - buffer we are currently drawing on
        glfwSwapBuffers(gWindow);
    }

    // clean up vbo and vao and shader program
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

    // clean up glfw Init
    glfwTerminate();
    return 0;

}

// clean up initialization
bool initOpenGL()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW Initialization failed" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);


    gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);


    if (gWindow == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        // opposite of Init - destroy object
        glfwTerminate();
        return false;
    }

    // make window focus
    glfwMakeContextCurrent(gWindow);

    glfwSetKeyCallback(gWindow, glfw_onKey);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW Initialization failed" << std::endl;

        return false;
    }

    glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

    return true;
}

// implement callback Key function
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//show fps
void showFPS(GLFWwindow* window)
{
    static double previousSeconds = 0.0;
    static int frameCount = 0;
    double elapsedSeconds;
    double currentSeconds = glfwGetTime(); // returns number of seconds GLFW started

    elapsedSeconds = currentSeconds - previousSeconds;

    // limit text 4 time per second
    if (elapsedSeconds > 0.25)
    {
        previousSeconds = currentSeconds;
        double fps = (double)frameCount / elapsedSeconds;
        double msPerFrame = 1000.0 / fps;

        std::ostringstream outs;
        outs.precision(3);
        outs << std::fixed
            << APP_TITLE << "    "
            << "FPS: " << fps << "     "
            << "Frame Time: " << msPerFrame << " (ms)";
        glfwSetWindowTitle(window, outs.str().c_str());
        frameCount = 0;

    }

    frameCount++;
}


