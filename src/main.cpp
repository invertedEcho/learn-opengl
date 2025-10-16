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
  0.5f, 0.5f, 0.0f, // top right
  0.5f, -0.5f, 0.0f, // bottom right
  -0.5f, -0.5f, 0.0f, // bottom left
  -0.5f, 0.5f, 0.0f // top left
};

// specify the order at which we want the vertices to be drawn
unsigned int indices[] = {
  0, 1, 3, // first triangle
  1, 2, 3 // second triangle
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

  // vertex buffer object
  unsigned int vbo;
  glGenBuffers(1, &vbo);

  // vertex array object -> remembers how attributes are read from our VBO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  

  // element buffer object
  unsigned int EBO;
  glGenBuffers(1, &EBO);


  // 1. bind vertex array object -> remembers our attributes
  glBindVertexArray(VAO);

  // 2. copy vertices array in a buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // 3. copy index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  // 4. set vertex attributes pointers
  // this tells opengl how to actually interpret our vertex shader
  // attribute(0) is of size 3, type float, we dont want it normalized, stride = offset between vertex attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
  // also actually enable attribute(0)
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

  glad_glUseProgram(shaderProgram);
  glBindVertexArray(VAO);

  while(!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
