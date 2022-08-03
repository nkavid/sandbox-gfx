#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texSampler;
uniform vec3 uColor;

void main()
{
  fragColor = vec4(texture(texSampler, texCoord).xyz, 1.0f);
}
