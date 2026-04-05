// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <iostream>

const char *vertexShaderSource =
    "#version 330 core\n"
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

// thanks to EBO (a buffer that stores indices to tell opengl in what order to
// draw the vertices), for a rectangle we dont have to make two triangles (each
// 3 vertices, e.g. 6 in total), but can just say the 4 vertices for the
// rectaclge
const float vertices[] = {
    0.5f,  0.5f,  0.0f, // top right
    0.5f,  -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f  // top left
};

// specify the order at which we want the vertices to be drawn
// so each number refers to the index in our vertices array
unsigned int eboIndices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

void framebuffer_size_change_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void error_callback(int error, const char *description) {
  std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

int main() {
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    // GLenum code;
    // code = glGetError();
    // printf("%u", code);
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
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // vertex array object -> remembers how attributes are read from our VBO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);

  // element buffer object
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  // 1. bind vertex array object -> remembers our attributes
  glBindVertexArray(VAO);

  // 2. copy vertices array in a buffer for OpenGL to use
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // 3. copy index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eboIndices), eboIndices,
               GL_STATIC_DRAW);

  // 4. set vertex attributes pointers
  // this tells opengl how to actually interpret our vertex shader
  // parameters:
  // 1. we want to configure the attribute 0, e.g. shader has location = 0
  // 2. attribute(0) is of size 3, because vec3
  // 3. the values are of type float
  // 4. we dont want it normalized (in our case, its already normalized)
  // 5. stride: tells us the space between consecutive vertex attributes.
  //    in our case, the next set of position data is located exactly 3 times
  //    the size of a float.
  // 6. offset of where the position data begins in the buffer. since its at the
  //    very beginning, this is 0. this has this void cast, for 0, we could also
  //    leave it out, but this wouldnt work if its not 0, because the last
  //    argument has to be of type void as pointer. it works with 0 because 0 is
  //    a null pointer constant, and it can be implicility converted to any
  //    pointer type.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
  // also actually enable attribute(0)
  glEnableVertexAttribArray(0);

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

  // the vertex shader gets dynamically compiled at run-time
  glCompileShader(vertexShader);

  int compileVertexShaderSuccess;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileVertexShaderSuccess);

  if (!compileVertexShaderSuccess) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR: Vertex shader compilation failed!\n"
              << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  int compileFragmentShaderSuccess;
  char infoLogFragmentShader[512];
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS,
                &compileFragmentShaderSuccess);

  if (!compileFragmentShaderSuccess) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogFragmentShader);
    std::cout << "ERROR: Fragment shader compilation failed!\n"
              << infoLogFragmentShader << std::endl;
  }

  // the shader program is the final linked version of multiple shaders
  // combined, e.g. in our case the vertex shader together with the fragment
  // shader.

  // TODO: check if shader program failed
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glad_glAttachShader(shaderProgram, vertexShader);
  glad_glAttachShader(shaderProgram, fragmentShader);
  glad_glLinkProgram(shaderProgram);

  // we no longer need the shader objects as they are now linked in the program
  // object
  glad_glDeleteShader(vertexShader);
  glad_glDeleteShader(fragmentShader);

  glad_glUseProgram(shaderProgram);
  glBindVertexArray(VAO);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // glPolygonMode: tells OpenGL how to draw its primitives
    // wireframe mode:
    // 1. GL_FRONT_AND_BACK: apply it to the front and back of all triangles
    // (front and back of triangle in 3D space)
    // 2. GL_LINE: draw them as lines
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 1. we want to draw triangles
    // 2. 6 vertices
    // 3. the type of our indices is int (How can it be something other than
    //    int?)
    // 4. last argument is offset for EBO
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
