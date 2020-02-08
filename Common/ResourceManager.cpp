#include "Common/ResourceManager.h"

#include <fstream>
#include <vector>

void ResourceManager::loadMeshes()
{
  _meshes.emplace_back();
  Mesh& mesh = _meshes.back();
  std::string testFileName = "ARC170.obj";
  std::ifstream objFile("Resource/Obj/" + testFileName); // TODO: change obj file path to the global settings
  if (objFile.is_open())
  {
    mesh.setFileName(testFileName);
    mesh.fromObjFile(objFile);
    objFile.close();
  }
  else
  {
    _console << "ERROR: Can't open an Obj file: " << testFileName << "\n";
  }
}
