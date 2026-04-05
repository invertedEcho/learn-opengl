#version 330 core

// in means we have an input of type vec3, called 'aPos'
layout(location = 0) in vec3 aPos;

void main()
{
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
