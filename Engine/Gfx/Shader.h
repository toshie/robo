#pragma once

#include <GL/glew.h>
#include <GL/glut.h>

#include <boost/optional.hpp>

#include <string>

class Shader
{
  enum Type : GLenum
  {
    Fragment = GL_FRAGMENT_SHADER,
    Vertex = GL_VERTEX_SHADER
  };

public:
  Shader(const std::string& vertexShaderPath,
         const std::string& fragmentShaderPath) :
    _fragmentShaderPath(fragmentShaderPath),
    _vertexShaderPath(vertexShaderPath) {}

  ~Shader()
  {
    if (_programId != 0)
      glDeleteProgram(_programId);
  }

  GLuint getUniformId(const std::string& name) const
  {
    glGetUniformLocation(_programId, name.c_str());
  }

  bool init();
  void use() { glUseProgram(_programId); }

private:
  boost::optional<GLuint> load(const std::string& path, Type type);

  std::string _fragmentShaderPath;
  std::string _vertexShaderPath;
  GLuint _programId{0};
};
