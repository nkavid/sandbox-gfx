#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat3 uMatrix;

void main()
{
  gl_Position = vec4(uMatrix * aPos, 1.0);
}
