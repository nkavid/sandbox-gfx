#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

out vec2 texCoord;

uniform mat3 uMatrix;

void main()
{
  gl_Position = vec4(uMatrix * aPos, 1.0);
  texCoord    = aTex;
}
