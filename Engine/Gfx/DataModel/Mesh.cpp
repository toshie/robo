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
const std::string OBJ_INDICATOR_TEXTURE("vn");
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

// TODO: too big!! simplify this function!!!
void Mesh::addIndex(const std::vector<std::string>& values)
{
  const static size_t TRIANGLE_VALUES_COUNT = 4;
  const static size_t QUAD_VALUES_COUNT = 5;
  if (values.size() != TRIANGLE_VALUES_COUNT &&
      values.size() != QUAD_VALUES_COUNT)
  {
    throw std::range_error("WARNING: Mesh corrupted: bad count of poligon face's coords");
  }

  const static size_t VERTEX = 0;
  const static size_t TEXTURE = 1;
  const static size_t NORMAL = 2;

  // all indices in line of object file are grouped by vertex order
  //    B       B------C
  //   / \      |      |
  //  /   \     |      |
  // A-----C    A------D
  boost::optional<GLuint> vertexA, vertexB, vertexC, vertexD;
  boost::optional<GLuint> textureA, textureB, textureC, textureD;
  boost::optional<GLuint> normalA, normalB, normalC, normalD;

  std::vector<std::string> indicesA;
  boost::split(indicesA, values[1], boost::is_any_of("/"));

  if (indicesA.size() > VERTEX && !indicesA[VERTEX].empty())
    vertexA = boost::lexical_cast<unsigned int>(indicesA[VERTEX]);

  if (indicesA.size() > TEXTURE && !indicesA[TEXTURE].empty())
    textureA = boost::lexical_cast<unsigned int>(indicesA[TEXTURE]);

  if (indicesA.size() > NORMAL && !indicesA[NORMAL].empty())
    normalA = boost::lexical_cast<unsigned int>(indicesA[NORMAL]);


  std::vector<std::string> indicesB;
  boost::split(indicesB, values[2], boost::is_any_of("/"));

  if (indicesB.size() > VERTEX && !indicesB[VERTEX].empty())
    vertexB = boost::lexical_cast<unsigned int>(indicesB[VERTEX]);

  if (indicesB.size() > TEXTURE && !indicesB[TEXTURE].empty())
    textureB = boost::lexical_cast<unsigned int>(indicesB[TEXTURE]);

  if (indicesB.size() > NORMAL && !indicesB[NORMAL].empty())
    normalB = boost::lexical_cast<unsigned int>(indicesB[NORMAL]);


  std::vector<std::string> indicesC;
  boost::split(indicesC, values[3], boost::is_any_of("/"));

  if (indicesC.size() > VERTEX && !indicesC[VERTEX].empty())
    vertexC = boost::lexical_cast<unsigned int>(indicesC[VERTEX]);

  if (indicesC.size() > TEXTURE && !indicesC[TEXTURE].empty())
    textureC = boost::lexical_cast<unsigned int>(indicesC[TEXTURE]);

  if (indicesC.size() > NORMAL && !indicesC[NORMAL].empty())
    normalC = boost::lexical_cast<unsigned int>(indicesC[NORMAL]);

  // now get the indices for the figure's fourth vertex if this is a Quad
  if (values.size() == QUAD_VALUES_COUNT)
  {
    std::vector<std::string> indicesD;
    boost::split(indicesD, values[4], boost::is_any_of("/"));

    if (indicesD.size() > VERTEX && !indicesD[VERTEX].empty())
      vertexD = boost::lexical_cast<unsigned int>(indicesD[VERTEX]);

    if (indicesD.size() > TEXTURE && !indicesD[TEXTURE].empty())
      textureD = boost::lexical_cast<unsigned int>(indicesD[TEXTURE]);

    if (indicesD.size() > NORMAL && !indicesD[NORMAL].empty())
      normalD = boost::lexical_cast<unsigned int>(indicesD[NORMAL]);
  }



  if (vertexA && vertexB && vertexC)
  {
    // vertex indexes in .obj files starts with 1, not 0
    // so we need to decrement
    if (vertexD)
    {
      _quadVertexIndices.push_back({*vertexA - 1, *vertexB - 1, *vertexC - 1, *vertexD - 1});
    }
    else
    {
      _triangleVertexIndices.push_back({*vertexA - 1, *vertexB - 1, *vertexC - 1});
    }
  }

    //if (textureX && textureY && textureZ)

  if (normalA && normalB && normalC)
  {
    if (normalD)
    {
      _quadNormalIndices.push_back({*normalA - 1, *normalB - 1, *normalC - 1, *normalD - 1});
    }
    else
    {
      _triangleNormalIndices.push_back({*normalA - 1, *normalB - 1, *normalC - 1});
    }
  }
}

void Mesh::fromObjFile(std::ifstream& file)
{
  std::string line;
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
        _vertices.push_back(objLineTo3dCoords(values));
      }
      else if (lineIndicator == OBJ_INDICATOR_NORMAL)
      {
        _normals.push_back(objLineTo3dCoords(values));
      }
      else if (lineIndicator == OBJ_INDICATOR_TEXTURE)
      {
        _textures.push_back(objLineTo2dCoords(values));
      }
      else if (lineIndicator == OBJ_INDICATOR_INDEX)
      {
        addIndex(values);
      }
    }
    catch (std::range_error& re)
    {
      ResourceManager::instance().console() << re.what() << "\n";
    }
  }
}
