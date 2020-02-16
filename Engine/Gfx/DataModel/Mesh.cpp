#include "Engine/Gfx/DataModel/Mesh.h"

#include "Common/ResourceManager.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/tokenizer.hpp>

#include <exception>

namespace
{

const std::string OBJ_INDICATOR_VERTEX("v");
const std::string OBJ_INDICATOR_NORMAL("vn");
const std::string OBJ_INDICATOR_TEXTURE("vt");
const std::string OBJ_INDICATOR_INDEX("f");

std::array<GLfloat, 3> objLineTo3dCoords(const std::vector<std::string>& objFileLine)
{
  if (objFileLine.size() != 4)
    throw std::range_error("WARNING: Mesh corrupted: bad count of cooridantes in .obj file line. Line indicator: " + objFileLine[0]);

  return {boost::lexical_cast<float>(objFileLine[1]),   // x
          boost::lexical_cast<float>(objFileLine[2]),   // y
          boost::lexical_cast<float>(objFileLine[3])}; // z
}

std::array<GLfloat, 2> objLineTo2dCoords(const std::vector<std::string>& objFileLine)
{
  if (objFileLine.size() < 3) // it's possible that "vt" has "z" coordinate with 0
    throw std::range_error("WARNING: Mesh corrupted: bad count of cooridantes in .obj file line. Line indicator: " + objFileLine[0]);

  return {boost::lexical_cast<float>(objFileLine[1]),   // x
          boost::lexical_cast<float>(objFileLine[2])};   // y
}



} // anonymous namespace

GLuint Mesh::getVertexIdx(const std::string& objFaceIndex,
                          const VertexPos& vertexPos,
                          const TexturePos& texturePos,
                          const NormalPos& normalPos,
                          VerticesMap& vertices)
{
  std::vector<std::string> indices;
  boost::split(indices, objFaceIndex, boost::is_any_of("/"));

  constexpr size_t VERTEX = 0, TEXTURE = 1, NORMAL = 2;
  GLuint vertexPosIdx = 0, texturePosIdx = 0, normalPosIdx = 0;
  if (indices.size() > VERTEX && !indices[VERTEX].empty())
    vertexPosIdx = boost::lexical_cast<unsigned int>(indices[VERTEX]) - 1;

  if (indices.size() > TEXTURE && !indices[TEXTURE].empty())
    texturePosIdx = boost::lexical_cast<unsigned int>(indices[TEXTURE]) - 1;

  if (indices.size() > NORMAL && !indices[NORMAL].empty())
    normalPosIdx = boost::lexical_cast<unsigned int>(indices[NORMAL]) - 1;


  auto key = std::make_tuple(vertexPosIdx, texturePosIdx, normalPosIdx);
  auto found = vertices.find(key);

  if (found != vertices.end())
    return found->second.idx;

  if (vertexPosIdx > vertexPos.size() ||
      texturePosIdx > texturePos.size() ||
      normalPosIdx > normalPos.size())
  {
    throw std::out_of_range("ERROR: Input .obj corrupted: index out of range");
  }

  IndexedVertex v;
  v.vertex.pos = vertexPos[vertexPosIdx];
  v.vertex.texturePos = texturePos[texturePosIdx];
  v.vertex.normalPos = normalPos[normalPosIdx];
  v.idx = vertices.size();
  vertices.insert({key, v});

  return v.idx;
}

void Mesh::addFace(const std::vector<std::string>& objFileLine,
                   const VertexPos& vertexPos,
                   const TexturePos& texturePos,
                   const NormalPos& normalPos,
                   VerticesMap& vertices)
{
  constexpr size_t triangleVerticesCnt = 3;
  if (objFileLine.size() < triangleVerticesCnt + 1)
    throw std::invalid_argument("ERROR: .obj line doesn't have enough vertices to make a triangle or quad face");

  // all indices in line of object file are grouped by vertex order
  //    B       B------C
  //   / \      |      |
  //  /   \     |      |
  // A-----C    A------D
  GLuint indexA = getVertexIdx(objFileLine[1], vertexPos, texturePos, normalPos, vertices);
  GLuint indexB = getVertexIdx(objFileLine[2], vertexPos, texturePos, normalPos, vertices);
  GLuint indexC = getVertexIdx(objFileLine[3], vertexPos, texturePos, normalPos, vertices);

  constexpr size_t quadVerticesCnt = 4;
  if (objFileLine.size() >= quadVerticesCnt + 1)
  {
    GLuint indexD = getVertexIdx(objFileLine[4], vertexPos, texturePos, normalPos, vertices);
    _quadFaces.push_back({indexA, indexB, indexC, indexD});
  }
  else
  {
    _triangleFaces.push_back({indexA, indexB, indexC});
  }
}

void Mesh::fromObjFile(std::ifstream& file)
{
  std::string line;
  VertexPos vertexPos;
  TexturePos texturePos;
  NormalPos normalPos;
  VerticesMap vertices;
  while (std::getline(file, line))
  {
    boost::trim(line);
    boost::char_separator<char> separator(" ");
    boost::tokenizer<boost::char_separator<char>> delimited(line, separator);

    if (delimited.begin() == delimited.end())
      continue;

    // must copy into a container because token_iterator has only forward_iterator
    // TODO: optimize
    std::vector<std::string> values(delimited.begin(), delimited.end());

    try
    {
      std::string& lineIndicator = values[0];
      if (lineIndicator == OBJ_INDICATOR_VERTEX)
      {
        vertexPos.push_back(objLineTo3dCoords(values));
      }
      else if (lineIndicator == OBJ_INDICATOR_NORMAL)
      {
        normalPos.push_back(objLineTo3dCoords(values));
      }
      else if (lineIndicator == OBJ_INDICATOR_TEXTURE)
      {
        texturePos.push_back(objLineTo2dCoords(values));
      }
      else if (lineIndicator == OBJ_INDICATOR_INDEX)
      {
        addFace(values, vertexPos, texturePos, normalPos, vertices);
      }
    }
    catch (std::exception& re)
    {
      ResourceManager::instance().console() << re.what() << "\n";
    }
  }

  _vertices.resize(vertices.size());
  for (const auto& keyAndVertex : vertices)
  {
    const IndexedVertex& v = keyAndVertex.second;
    _vertices[v.idx] = std::move(v.vertex);
  }
}
