#version 330 core

layout (location = 0) in vec3 pos;

out vec4 vertexColor;

uniform mat4 u_projection_matrix;

void main()
{
  gl_Position = u_projection_matrix * vec4(pos, 1.0);
  vertexColor = vec4(1.0, 0.0, 0.0, 1.0);
}
