#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <GL/gl.h>
#include <iostream>

const char *vertexShaderSource = "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "void main()\n"
  "{\n"
  " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
  "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}";

const float vertices[] = {
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f, 0.5f, 0.0f
};

void framebuffer_size_change_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_change_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, 800, 600);

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  
  unsigned int VBO;
  glGenBuffers(1, &VBO);

  // 1. bind vertex array object
  glBindVertexArray(VAO);
  // 2. copy vertices array (VBO) in a buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // 3. set vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
  glEnableVertexAttribArray(0);

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int compileVertexShaderSuccess;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileVertexShaderSuccess);

  if (!compileVertexShaderSuccess) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR: Vertex shader compilation failed!\n" << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  int compileFragmentShaderSuccess;
  char infoLogFragmentShader[512];
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileFragmentShaderSuccess);

  if (!compileFragmentShaderSuccess) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogFragmentShader);
    std::cout << "ERROR: Fragment shader compilation failed!\n" << infoLogFragmentShader << std::endl;
  }

  // TODO: check if shader program failed
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glad_glAttachShader(shaderProgram, vertexShader);
  glad_glAttachShader(shaderProgram, fragmentShader);
  glad_glLinkProgram(shaderProgram);


  // we no longer need the shader objects as they are now linked in the program object
  glad_glDeleteShader(vertexShader);
  glad_glDeleteShader(fragmentShader);

  while(!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glad_glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
