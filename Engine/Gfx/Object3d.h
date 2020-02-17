#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/vec3.hpp>

class Object3d
{
public:
  Object3d() = default;
  virtual ~Object3d() = default;

  void addX(GLfloat value) { _position.x += value; }
  void addY(GLfloat value) { _position.y += value; }
  void addZ(GLfloat value) { _position.z += value; }

  void moveBackward(GLfloat distance) { _position += _vecForward * distance; }
  void moveForward(GLfloat distance) { _position -= _vecForward * distance; }
  void moveLeft(GLfloat distance) { _position -= _vecRight * distance; }
  void moveRight(GLfloat distance) { _position += _vecRight * distance; }

  void subX(GLfloat value) { _position.x -= value; }
  void subY(GLfloat value) { _position.y -= value; }
  void subZ(GLfloat value) { _position.z -= value; }

protected:
  glm::vec3 _position{0.0, 0.0, 0.0};
  glm::vec3 _vecForward{0.0, 0.0, 1.0};
  glm::vec3 _vecRight{1.0, 0.0, 0.0};
  glm::vec3 _vecUp{0.0, 1.0, 0.0};
};
