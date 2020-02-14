#version 330 core

layout (location = 0) in vec3 pos;

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
  vertexColor = vec4(1.0, 1.0, 0.0, 1.0);
  gl_Position = proj * view * model * vec4(pos, 1.0);
  //gl_Position = proj * vec4(pos, 1.0);
}
