#define GLEW_STATIC

#include "Common/ResourceManager.h"
#include "Engine/Gfx/Renderer.h"

int main(int argc, char** args)
{
  ResourceManager::instance().loadMeshes();

  Renderer& renderer = Renderer::instance();
  renderer.setScreenDimensions(1920, 1080);

  if (!renderer.init(argc, args))
    return 1;
 
  renderer.start();

  return 0;
}
