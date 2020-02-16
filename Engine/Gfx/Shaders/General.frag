#version 330 core
layout (location = 0) out vec4 FragColor;

in vec4 vertexColor;
in vec2 fragTextureCoord;

uniform sampler2D texSampler;

void main()
{
  // FragColor = vertexColor;
  float c = textureSize(texSampler, 0).x;
  if (c == 1024.0)
    c = 0.75;

//  if (c != 0.0)
//    c = c / 10000;
  
  if (c == 0.0)
  {
    c = textureSize(texSampler, 1).x;
    if (c != 0.0)
      c = 1.0;
  }

  if (c == 0.0)
  {
    c = textureSize(texSampler, 10).x;
    if (c != 0.0)
      c = 1.0;
  }
  
  if (c == 0.0)
  {
    c = textureSize(texSampler, 100).x;
    if (c != 0.0)
      c = 1.0;
  }
  
  if (c == 0.0)
  {
    c = textureSize(texSampler, 1000).x;
    if (c != 0.0)
      c = 1.0;
  }
  
  if (c == 0.0)
  {
    c = textureSize(texSampler, 10000).x;
    if (c != 0.0)
      c = 1.0;
  }

  //c = 0.0;
  //FragColor = vec4(c, c, c, c);
  //FragColor = texture(texSampler, vec2(0.7127029896, 0.5874789953));

  // flip it until it's not fixed in the renderer
  vec2 tex = fragTextureCoord;
  tex.y = 1.0 - tex.y;
  FragColor = texture(texSampler, tex);
}
