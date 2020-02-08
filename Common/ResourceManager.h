#pragma once

#include "Engine/Gfx/DataModel/Mesh.h"

#include <iostream>
#include <string>
#include <vector>

class ResourceManager
{
public:
  static ResourceManager& instance()
  {
    static ResourceManager instance;
    return instance;
  }

  std::ostream& console() { return _console; }

  void loadMeshes();
  const std::vector<Mesh>& meshes() { return _meshes; }

private:
  ResourceManager() : _console(std::cout) {}
  ResourceManager(ResourceManager& rhs) = delete;

  std::ostream& _console;

  std::vector<Mesh> _meshes;
};
