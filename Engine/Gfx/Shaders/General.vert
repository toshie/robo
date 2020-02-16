#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 texturePos;
layout (location = 2) in vec3 asdnormalPos;

out vec4 vertexColor;
out vec2 fragTextureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
  vertexColor = vec4(1.0, 0.0, 1.0, 1.0);
  fragTextureCoord = texturePos;
  gl_Position = proj * view * model * vec4(vertexPos, 1.0);
}
