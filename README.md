# learn-opengl

Requirements:

Fedora:
`sudo dnf install libXcursor-devel libXrandr-devel mesa-libGL-devel`

## Basics
- vertex: single point in 3d
- vertices: plural, multiple corner points like describing a triangle
- mesh: outer box of an object

## Buffers
- a buffer object in opengl is some data on memory on the gpu, can store arbitray data
- create by using `glGenBuffer` -> this actually just allocates an ID/handle, no actual memory yet
- fill with data by using `glBufferData` -> this actually allocates gpu memory and fills the buffer with our given data, e.g. uploading from CPU to GPU
- bind buffer: by binding a buffer, we tell opengl what buffer we are using for the given buffer type, e.g. we create a "temporary symlink" from a target (buffer type) to a specific buffer id
- many buffers can exist, but only one buffer can be bound per target at a time
- swap buffer: neuer buffer wird als aktiv gesetzt und alter wird resetted
- framebuffer: das ziel wo reingerendert wird, alles was im derzeitigen frame visible ist

## Shaders
- vertex shader: will be executed per pixel every frame

## another section

step-by-step:
1. we have data, e.g. a list of vertices
```
const float vertices[] = {
  0.5f, 0.5f, 0.0f, // top right
  0.5f, -0.5f, 0.0f, // bottom right
  -0.5f, -0.5f, 0.0f, // bottom left
  -0.5f, 0.5f, 0.0f // top left
};
```

2. now we create a buffer, in this case a VBO (vertex buffer object) that stores our data

```
unsigned int vbo; // declare variable to hold buffer id
glGenBuffers(1, &vbo); // allocate handle for our buffer
glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the vbo buffer to GL_ARRAY_BUFFER type
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // now we actually allocate memory on our GPU, filling our buffer with data from vertices variable 

   ```

3. now we create a VAO, it sores no data but rather remembers vertex attributes are read from our VBO(s)
	1. Vertex array object -> `glGenVertexArrays(1, &vao)`
	2. bind to vertex array type -> `glBindVertexArray(vao)`
4. attributes are important because we specify stuff in our vertex shader, like:
	1. `layout (location = 0) in vec2 aPos;
	2. `layout (location = 1) in vec3 aColor;`
```cpp
glBindBuffer(GL_ARRAY_BUFFER, vbo);

// position attribute
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// color attribute
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
glEnableVertexAttribArray(1);
```
this tells opengl:
- attribute 0 uses 2 floats, starting at byte 0, stride (offset between vertex attributes) 5 floats
- attribute 1 uses 3 floats per vertex, starting after 2 floats
the currently bound VAO remembers:
When drawing, if shader wants attribute 0, fetch it from this buffer using that layout
```cpp
glBindVertexArray(vao);
glDrawArrays(GL_TRIANGLES, 0, 3);
```
opengl uses:
- the bound VAO (to know how to interpret attributes)
- the buffers it references (to fetch vertex data)
- the currently active shader (to use those inputs)

### EBO Buffers:
- if we wanna draw a rectangle for example, we would need two triangles (OpenGL mainly works with triangles)
- would look like this:

```
float vertices[] = {
    // first triangle
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, 0.5f, 0.0f, // top left
    // second triangle
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
};
```

- this has duplicate data, we specify bottom right and top left twice (this will quickly get worse as soon as we have more complex models)
- better solution: store only unique vertices and then specify the order at which the vertices should be drawn
- and thats exactly what EBOs do, this "technique" is also called "indexed drawing"

1. We specify the vertices and indices:
```cpp
float vertices[] = {
  0.5f, 0.5f, 0.0f, // top right
  0.5f, -0.5f, 0.0f, // bottom right
  -0.5f, -0.5f, 0.0f, // bottom left
  -0.5f, 0.5f, 0.0f // top left
};

unsigned int indices[] = {
  0, 1, 3, // first triangle
  1, 2, 3  // second triangle
}
```
2. Now we create the EBO
```cpp
unsigned int EBO; // handle for our EBO
glGenBuffers(1, &EBO); // generate a buffer for our EBO

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind element array buffer type to our EBO
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // fill our buffer with data and allocate memory with our indices
```

3. Use the EBO to draw
```cpp
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```
