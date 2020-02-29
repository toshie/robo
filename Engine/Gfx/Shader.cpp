#include "Shader.h"

#include <fstream>
#include <iostream>

bool Shader::init()
{
  boost::optional<GLuint> vertexShader = 
    load(_vertexShaderPath, Type::Vertex);
  boost::optional<GLuint> fragmentShader = 
    load(_fragmentShaderPath, Type::Fragment);

  if (!vertexShader || !fragmentShader)
    return false;


  _programId = glCreateProgram();
  glAttachShader(_programId, *vertexShader);
  glAttachShader(_programId, *fragmentShader);
  glBindAttribLocation(_programId, 0, "pos"); // TODO: needed?
  glBindFragDataLocation(_programId, 0, "FragColor");
  glLinkProgram(_programId);
 
  GLint status = GL_TRUE;
  glGetProgramiv(_programId, GL_LINK_STATUS, &status);
  if (status != GL_TRUE)
  {
    GLchar errorLog[256] = {0};
    GLsizei length = 0;
    glGetProgramInfoLog(
        _programId, sizeof(errorLog) / sizeof(GLchar), &length, errorLog);

    std::cerr << "ERROR: Shader '"
              << "' linking failed.\n"
              << "Message: \n"
              << errorLog << std::endl;

    return false;
  }

  return true;
}

boost::optional<GLuint> Shader::load(const std::string& path, Type type)
{
  std::ifstream srcFile(path);
  if (!srcFile.is_open())
  {
    std::cerr << ("ERROR: Cannot open a shader source: " + path + "\n");
    return boost::none;
  }

  std::string src, srcLine;
  while (std::getline(srcFile, srcLine))
  {
    src += srcLine + "\n";
  }

  srcFile.close();

  GLuint id = glCreateShader(type);
  if (id == 0)
  {
    std::cerr << "ERROR: Cannot create shader '" << path << std::endl;
    return boost::none;
  }

  const char* srcPtr = src.c_str();
  const GLint srcSize = src.size();
  glShaderSource(id, 1, &srcPtr, &srcSize);
  glCompileShader(id);

  GLint status = GL_TRUE;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE)
  {
    GLchar errorLog[256] = {0};
    GLsizei length = 0;
    glGetShaderInfoLog(
        id, sizeof(errorLog) / sizeof(GLchar), &length, errorLog);

    std::cerr << "ERROR: Shader '" << path
              << "' compilation failed.\n"
              << "Message: \n"
              << errorLog << std::endl;

    return boost::none;
  }

  return id;
}
