#pragma once

#include "Keyboard.h"
#include "Shader.h"

#include <GL/glew.h>
#include <GL/glut.h>

class Renderer
{
public:
  ~Renderer();

  static Renderer& instance()
  {
    static Renderer instance;
    return instance;
  }

  bool init(int& argc, char**& argv);
  void start();

  void setScreenDimensions(int width, int height)
  {
    _screenWidth = width;
    _screenHeight = height;
  }

private:
  Renderer() = default;

  // GL callbacks
  void display();
  static void displayWrapper() { instance().display(); }

  void idle();
  static void idleWrapper() { instance().idle(); }

  void reshape(int width, int height);
  static void reshapeWrapper(int w, int h) { instance().reshape(w, h); }
  
  void timer(int millisec);
  static void timerWrapper(int millisec) { instance().timer(millisec); }

  bool handleKeyboard();

  static void keyboardDownWrapper(unsigned char key, int x, int y)
  {
    instance()._keyboard.keyDown(key, x, y);
  }

  static void keyboardUpWrapper(unsigned char key, int x, int y)
  {
    instance()._keyboard.keyUp(key, x, y);
  } 

  void mouse(int button, int state, int x, int y);
  static void mouseWrapper(int button, int state, int x, int y)
  {
    instance().mouse(button, state, x, y);
  }

  void mouseMotion(int x, int y);
  static void mouseMotionWrapper(int x, int y) { instance().mouseMotion(x, y); }


  void setPerspective();

  bool loadModels();

  Keyboard _keyboard;

  int _screenHandle{0};
  int _screenHeight{0};
  int _screenWidth{0};

  GLuint _vertexBufferId{0};
  GLuint _indexBufferId{0};
  GLuint _textureCoordBufId{0};
  GLuint _textureBufferId{0};
  GLuint _textureId{0};

  Shader _vertexShader{"Engine/Gfx/Shaders/General.vert", GL_VERTEX_SHADER};
  Shader _fragmentShader{"Engine/Gfx/Shaders/General.frag", GL_FRAGMENT_SHADER};
  GLuint _shaderProgramId{0};
};
