#pragma once

#include <GL/gl.h>

#include <array>
#include <fstream>
#include <string>
#include <vector>

class Mesh
{
public:
  void fromObjFile(std::ifstream& file);

  const std::string& getFileName() const { return _fileName; }
  void setFileName(const std::string& fileName) { _fileName = fileName; }

  const std::vector<std::array<GLfloat, 3>> getVertices() const { return _vertices; }
  const std::vector<std::array<GLuint, 3>> getTriangleVertexIndices() const { return _triangleVertexIndices; }
  const std::vector<std::array<GLuint, 4>> getQuadVertexIndices() const { return _quadVertexIndices; }

  const std::vector<std::array<GLfloat, 3>> getNormals() const { return _normals; }
  const std::vector<std::array<GLuint, 3>> getTriangleNormalIndices() const { return _triangleNormalIndices; }
  const std::vector<std::array<GLuint, 4>> getQuadNormalIndices() const { return _quadNormalIndices; }

private:
  void addIndex(const std::vector<std::string>& values);

  std::string _fileName;
  std::vector<std::array<GLfloat, 3>> _vertices;
  std::vector<std::array<GLuint, 3>> _triangleVertexIndices;
  std::vector<std::array<GLuint, 4>> _quadVertexIndices;

  std::vector<std::array<GLfloat, 3>> _normals;
  std::vector<std::array<GLuint, 3>> _triangleNormalIndices;
  std::vector<std::array<GLuint, 4>> _quadNormalIndices;

  std::vector<std::array<GLfloat, 2>> _textures;
  std::vector<std::array<GLuint, 3>> _triangleTextureIndices;
  std::vector<std::array<GLuint, 4>> _quadTextureIndices;
};
