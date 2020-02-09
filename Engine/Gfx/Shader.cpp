#include "Shader.h"

#include <fstream>
#include <iostream>

bool Shader::init()
{
  std::ifstream srcFile(_path);
  if (!srcFile.is_open())
  {
    std::cerr << ("ERROR: Cannot open a shader source: " + _path + "\n");
    return false;
  }

  std::string src, srcLine;
  while (std::getline(srcFile, srcLine))
  {
    src += srcLine + "\n";
  }

  srcFile.close();

  _id = glCreateShader(_type);
  if (_id == 0)
  {
    std::cerr << "ERROR: Cannot create shader '" << _path
              << "' of type " << _type << std::endl;

    return false;
  }

  const char* srcPtr = src.c_str();
  const GLint srcSize = src.size();
  glShaderSource(_id, 1, &srcPtr, &srcSize);
  glCompileShader(_id);

  GLint status = GL_TRUE;
  glGetShaderiv(_id, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE)
  {
    GLchar errorLog[256] = {0};
    GLsizei length = 0;
    glGetShaderInfoLog(
        _id, sizeof(errorLog) / sizeof(GLchar), &length, errorLog);

    std::cerr << "ERROR: Shader '" << _path
              << "' compilation failed.\n"
              << "Message: \n"
              << errorLog << std::endl;

    return false;
  }

  return true;
}
