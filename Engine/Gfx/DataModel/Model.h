#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Model
{
public:
  Model(const std::string& objPath,
        const std::string& mtlPath,
        const std::string& texturePath)
    : _objPath(objPath),
      _mtlPath(mtlPath),
      _texturePath(texturePath) {}

  bool read();

  const std::vector<glm::vec3>& vertices() const { return _vertices; }

private:
  std::string _objPath;
  std::string _mtlPath;
  std::string _texturePath;

  std::vector<glm::vec3> _vertices;
};
