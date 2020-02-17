#pragma once

#include "Object3d.h"

#include <glm/mat4x4.hpp>

class Camera : public Object3d
{
public:
  Camera() = default;

  inline glm::mat4 getView()
  {
    return glm::mat4(
      _vecRight.x, _vecUp.x, _vecForward.x, 0.0,
      _vecRight.y, _vecUp.y, _vecForward.y, 0.0,
      _vecRight.z, _vecUp.z, _vecForward.z, 0.0,
        -glm::dot(_vecRight, _position),
        -glm::dot(_vecUp, _position),
        -glm::dot(_vecForward, _position), 1.0
    );
  }

  void moveLook(GLfloat xOffset, GLfloat yOffset);
};
