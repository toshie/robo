#pragma once

#include <glm/glm.hpp>

#include <vector>

class ObjLoader
{
public:
  ObjLoader() = default;

  static bool load(const char * path,
                   std::vector < glm::vec3 > & out_vertices,
                   std::vector < glm::vec2 > & out_uvs,
                   std::vector < glm::vec3 > & out_normals);
};
