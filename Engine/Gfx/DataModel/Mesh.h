#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <fstream>
#include <map>
#include <string>
#include <vector>

class Mesh
{
public:
  struct Vertex
  {
    std::array<GLfloat, 3> pos;
    std::array<GLfloat, 2> texturePos;
    std::array<GLfloat, 3> normalPos;
  } __attribute__((packed));

  void fromObjFile(std::ifstream& file);

  const std::string& getFileName() const { return _fileName; }
  void setFileName(const std::string& fileName) { _fileName = fileName; }

  const size_t getQuadFacesBytes() const { return _quadFaces.size() * sizeof(decltype(_quadFaces)::value_type); }
  const std::uint8_t* getQuadFacesPtr() const
  {
    return reinterpret_cast<const std::uint8_t*>(_quadFaces.data());
  }
  const size_t getQuadVerticesCount() const { return _quadFaces.size() * 4; }

  const size_t getTriangleFacesBytes() const { return _triangleFaces.size() * sizeof(decltype(_triangleFaces)::value_type); }
  const std::uint8_t* getTriangleFacesPtr() const
  {
    // TODO: reinterpret_cast is probably not the best idea. boost::serialize?
    return reinterpret_cast<const std::uint8_t*>(_triangleFaces.data());
  }
  const size_t getTriangleVerticesCount() const { return _triangleFaces.size() * 3; }

  const size_t getVerticesBytes() const { return _vertices.size() * sizeof(Vertex); }
  const std::vector<Vertex>& getVertices() const { return _vertices; }
  const std::uint8_t* getVerticesPtr() const
  {
    return reinterpret_cast<const std::uint8_t*>(_vertices.data());
  }

private:
  struct IndexedVertex
  {
    Vertex vertex;
    GLuint idx;
  };

  // TODO: map is slow, replace it with something else
  using VerticesMap = std::map<std::tuple<GLuint, GLuint, GLuint>, IndexedVertex>;
  using VertexPos = std::vector<std::array<GLfloat, 3>>;
  using TexturePos = std::vector<std::array<GLfloat, 2>>;
  using NormalPos = std::vector<std::array<GLfloat, 3>>;

  void addFace(const std::vector<std::string>& objFileLine,
               const VertexPos& vertexPos,
               const TexturePos& texturePos,
               const NormalPos& normalPos,
               VerticesMap& vertices);
  /**
   * @brief Takes one face index in the format that is typical to .obj 
   * files ("f" lines) and creates a vertex for them or gets the existing one and 
   * returns the index of it.
   * @param objIndices Indices set in position/texture/normal format.
   * @param vertexPos Vertex positions vector ("v" values from .obj file).
   * @param texturePos Texture positions vector ("vt" values from .obj file).
   * @param normalPos Normal positions vector ("vn" values from .obj file).
   * @param vertices Map of currently created vertices.
   * @return Index of found/created vertex.
   */
  GLuint getVertexIdx(const std::string& objFaceIndex,
                      const VertexPos& vertexPos,
                      const TexturePos& texturePos,
                      const NormalPos& normalPos,
                      VerticesMap& vertices);

  std::string _fileName;
  std::vector<std::array<GLuint, 4>> _quadFaces;
  std::vector<std::array<GLuint, 3>> _triangleFaces;
  std::vector<Vertex> _vertices;
};
