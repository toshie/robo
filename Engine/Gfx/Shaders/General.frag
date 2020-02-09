#version 330 core
out vec4 FragColor;

in vec4 vertexColor;

void main()
{
  FragColor = vertexColor;
  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
