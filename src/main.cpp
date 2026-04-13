// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
// clang-format on

#include <cmath>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// thanks to EBO (a buffer that stores indices to tell opengl in what order to
// draw the vertices), for a rectangle we dont have to make two triangles (each
// 3 vertices, e.g. 6 in total), but can just say the 4 vertices for the
// rectaclge
const float rectangleVertices[] = {
    // positions        // colors         // texture coords
    0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f  // top left
};

const float triangleVertces[] = {
    0.0,   0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // left
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f  // right
};

// specify the order at which we want the vertices to be drawn
// so each number refers to the index in our `vertices` array
unsigned int rectangleEboIndices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

unsigned int eboIndices[] = {0, 1, 2};

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

  // vertex buffer object -> this is the data that gets fed into vertex shader
  // attributes, the actual data "insertion" happens in
  // glBufferData(GL_ARRAY_BUFFER, ...)
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
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices,
               GL_STATIC_DRAW);

  // 3. copy index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleEboIndices),
               rectangleEboIndices, GL_STATIC_DRAW);

  // 4. set vertex attributes pointers
  // this tells opengl how to actually interpret our vertex shader
  // parameters:
  // 1. we want to configure the attribute 0, e.g. shader has location = 0
  // 2. attribute(0) is of size 3, because vec3
  // 3. the values are of type float
  // 4. we dont want it normalized (in our case, its already normalized)
  // 5. stride: tells us the space between consecutive vertex attributes.
  // 6. offset of where the position data begins in the buffer. since its at the
  //    very beginning, this is 0. this has this void cast, for 0, we could also
  //    leave it out, but this wouldnt work if its not 0, because the last
  //    argument has to be of type void as pointer. it works with 0 because 0 is
  //    a null pointer constant, and it can be implicility converted to any
  //    pointer type.

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
  glEnableVertexAttribArray(0);

  // color attrribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                        (void *)(sizeof(float) * 3));
  glEnableVertexAttribArray(1);

  // texture coords attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                        (void *)(sizeof(float) * 6));
  glEnableVertexAttribArray(2);

  Shader shaderProgram = Shader("../assets/shaders/vertexShader.glsl",
                                "../assets/shaders/fragmentShader.glsl");
  shaderProgram.use();

  // textures
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrChannels;
  unsigned int texture1, texture2;

  unsigned char *texture1Data =
      stbi_load("../assets/container.jpg", &width, &height, &nrChannels, 0);

  glGenTextures(1, &texture1);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);

  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (texture1Data) {
    // 1: texture target, meaning it will generate a texutre on the currently
    //    bound texture object, thats why we just called glBindTexture
    // 2: mip map level, in this case base level which is 0
    // 3: the format in which we want to store the texture, our image has only
    // RGB
    //    values
    // 6: should always be 0, "border"?
    // 7 and 8: format and datatype of our source image
    // 9: our actual image data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, texture1Data);

    // generate a mipmap for currently bound texture
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load container texture" << std::endl;
  }

  stbi_image_free(texture1Data);

  // awesome face texture
  unsigned char *texture2Data =
      stbi_load("../assets/awesomeface.png", &width, &height, &nrChannels, 0);

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (texture2Data) {
    // awesome face is png, which includes alpha channel, hence GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texture2Data);

    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load awesomeface texture" << std::endl;
  }

  stbi_image_free(texture2Data);

  shaderProgram.use();

  // "ourColor" is the uniform we declared in our fragment shader
  // int vertexColorLocation =
  //     glGetUniformLocation(shaderProgram.ID, "ourColor");
  //
  // int hOffsetUniformLoc = glGetUniformLocation(shaderProgram.ID,
  // "hOffset");

  // we first have to use the shader program, because glUniform4f operates on
  // the currently active program. remember, opengl is state machine, and thus
  // there is only one currently active shader program
  // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
  shaderProgram.setInt("texture1", 0);
  shaderProgram.setInt("texture2", 1);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    shaderProgram.use();

    // float timeValue = glfwGetTime();
    // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

    // glUniform1f(hOffsetUniformLoc, 0.2);

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
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
