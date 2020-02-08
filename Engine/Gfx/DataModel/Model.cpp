#include "Engine/Gfx/DataModel/Model.h"

bool Model::read()
{
  _vertices = {
    {-1.0f, -1.0f, 0.0f},
    { 1.0f, -1.0f, 0.0f},
    { 0.0f,  1.0f, 0.0f},
  };

  return true;
}
