#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::moveLook(GLfloat xOffset, GLfloat yOffset)
{
  glm::mat4 rotation = glm::mat4(1.0f);
  rotation = glm::rotate(
                rotation,
                glm::radians(xOffset),
                _vecUp
              );
  rotation = glm::rotate(
                rotation,
                glm::radians(yOffset),
                _vecRight
              );
  
  _vecForward = glm::normalize(glm::vec3(rotation * glm::vec4(_vecForward, 1.0)));
  _vecRight =
    glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), _vecForward));
  _vecUp = glm::normalize(glm::cross(_vecForward, _vecRight));
}
