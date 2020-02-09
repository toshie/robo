#pragma once

#include <GL/glew.h>

#include <string>

class Shader
{
public:
  Shader(const std::string& path, GLenum type) : 
    _path(path),
    _type(type) {}

  GLuint getId() const { return _id; }
  bool init();

private:
  GLuint _id = 0;
  std::string _path;
  GLenum _type;
};
