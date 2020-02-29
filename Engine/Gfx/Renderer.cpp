#define GL_GLEXT_PROTOTYPES
#define GLEW_STATIC

#include "Renderer.h"

#include "Common/ResourceManager.h"
#include "DataModel/Mesh.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>

#include <functional>

GLfloat mouseX = 0.0f;
GLfloat mouseY = 0.0f;
bool mouseLeftDown = false;
bool mouseRightDown = false;

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

Renderer::~Renderer()
{
  if (_vertexBufferId != 0)
    glDeleteBuffersARB(1, &_vertexBufferId);

  if (_indexBufferId != 0)
    glDeleteBuffersARB(1, &_indexBufferId);

  /* if (_textureBufferId != 0) */
  /*   glDeleteBuffersARB(1, &_textureBufferId); */

  if (_textureId != 0)
    glDeleteTextures(1, &_textureId);
}

bool Renderer::init(int& argc, char**& argv)
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode
  /* glutInitDisplayMode(GLUT_RGBA);   // display mode */

  glutInitWindowSize(_screenWidth, _screenHeight);    // window size

  glutInitWindowPosition(100, 100);                   // window location

  // finally, create a window with openGL context
  // Window will not displayed until glutMainLoop() is called
  // it returns a unique ID
  _screenHandle = glutCreateWindow(argv[0]);          // param is the title of window

  GLenum glewStatus = glewInit();
  if (glewStatus != GLEW_OK)
  {
    std::cerr << "ERROR: GLEW init fail: "
              << glewGetErrorString(glewStatus) 
              << std::endl;

    return false;
  }

  // register GLUT callback functions
  glutDisplayFunc(Renderer::displayWrapper);
  glutTimerFunc(33, Renderer::timerWrapper, 33);      // redraw only every given millisec
  //glutIdleFunc(idleCB);                             // redraw when idle
  glutReshapeFunc(Renderer::reshapeWrapper);
  glutIdleFunc(idleWrapper);
  glutKeyboardFunc(keyboardDownWrapper);
  glutKeyboardUpFunc(keyboardUpWrapper);
  glutMouseFunc(mouseWrapper);
  glutMotionFunc(mouseMotionWrapper);


  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // for lights testing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  
  glEnable              ( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );

  glClearColor(0.5f, 0.5f, 0.5f, 0);                   // background color
  glClearStencil(0);                          // clear stencil buffer
  glClearDepth(1.0f);                         // 0 is near, 1 is far

  // load general shaders and use them
  if (!_generalShader.init())
  {
    std::cerr << "Could not load needed shaders\n";
    return false;
  }
  
  _generalShader.use();

  // test lights
  // set up light colors (ambient, diffuse, specular)
  //GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};  // ambient light
  //GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
  //GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
  //glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
  //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
  //glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);





  // position the light
  /* float lightPos[4] = {0, 0, 20, 1}; // positional light */
  /* glLightfv(GL_LIGHT0, GL_POSITION, lightPos); */

  /* glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration */

  return true;
}

void Renderer::start()
{
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // show only mesh edges */

  ResourceManager& res = ResourceManager::instance();
  const Mesh& mesh = res.meshes()[0];

  // vertex buffer
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER,
               mesh.getVerticesBytes(),
               mesh.getVerticesPtr(),
               GL_STATIC_DRAW);

  // index buffer
  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh.getTriangleFacesBytes() + mesh.getQuadFacesBytes(),
               0,
               GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                  0,
                  mesh.getTriangleFacesBytes(),
                  mesh.getTriangleFacesPtr());
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                  mesh.getTriangleFacesBytes(),
                  mesh.getQuadFacesBytes(),
                  mesh.getQuadFacesPtr());

  /* GLfloat vertices[] = {0.1f, 0.1f, 0.1f, */
  /*                       0.2f, 0.2f, */
  /*                       0.3f, 0.3f, 0.3f}; */
  /* GLuint indices[] = {0}; */
  /* glGenBuffers(1, &_vertexBufferId); */
  /* glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId); */
  /* glBufferData(GL_ARRAY_BUFFER, */
  /*              32, */
  /*              vertices, */
  /*              GL_STATIC_DRAW); */

  // index buffer
  /* glGenBuffers(1, &_indexBufferId); */
  /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId); */
  /* glBufferData(GL_ELEMENT_ARRAY_BUFFER, */
  /*              sizeof(GLuint), */
  /*              indices, */
  /*              GL_STATIC_DRAW); */
  /* glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, */
  /*                 0, */
  /*                 mesh.getTriangleFacesBytes(), */
  /*                 mesh.getTriangleFacesPtr()); */
  /* glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, */
  /*                 mesh.getTriangleFacesBytes(), */
  /*                 mesh.getQuadFacesBytes(), */
  /*                 mesh.getQuadFacesPtr()); */

  // texture buffer
  /* glGenBuffers(1, &_textureBufferId); */
  /* glBindBuffer(GL_TEXTURE_BUFFER, _textureBufferId); */
  /* glBufferData(GL_TEXTURE_BUFFER, */
  /*              mesh.getTextures().size() * 2 * sizeof(GLfloat), */
  /*              &(*mesh.getTextures().begin()), */
  /*              GL_STATIC_DRAW); */

  // texture
  glGenTextures(1, &_textureId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureId);

  // texture wrapping: repeat on X (S) and Y (T)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // texture filtering: GL_NEAREST on scale down and up
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // load texture
  int width, height;
  unsigned char* image =
        SOIL_load_image("Resource/Obj/maps/ARC170_TXT_VERSION_4_D.jpg", &width, &height, 0 , SOIL_LOAD_RGB);
  if (!image)
  {
    std::cerr << "Couldn't load a texture\n";
    return;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                    GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  /* glBindBuffer(GL_TEXTURE_BUFFER, 0); */

  glutMainLoop();
}


void test()
{
}

void Renderer::display()
{
  // clear buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  ResourceManager& res = ResourceManager::instance();
  //res.console() << "MESH: " << res.meshes()[0].vertices[100][1];

  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  /* glBindBuffer(GL_TEXTURE_BUFFER, _textureBufferId); */






/* //when diffuse and specular will be ready
	float shininess = 15.0f;
	float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
	float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};

	// set specular and shiniess using glMaterial (gold-yellow)
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

	// set ambient and diffuse color using glColorMaterial (gold-yellow)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3fv(diffuseColor);
*/


  // TODO: would be nice if these offsets were calculated by Mesh::Vertex
  /* constexpr GLint vertexPosAttrib = 0; */
  /* glVertexAttribPointer(vertexPosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), 0); */
  /* glEnableVertexAttribArray(vertexPosAttrib); */

  /* constexpr GLint texturePosAttrib = 1; */
  /* void* textureOffset = reinterpret_cast<void*>(sizeof(Mesh::Vertex::pos)); */
  /* glVertexAttribPointer(texturePosAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), textureOffset); */
  /* glEnableVertexAttribArray(texturePosAttrib); */
  
  /* constexpr GLint normalPosAttrib = 2; */
  /* void* normalOffset = reinterpret_cast<void*>((size_t)textureOffset + sizeof(Mesh::Vertex::texturePos)); */
  /* glVertexAttribPointer(normalPosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), normalOffset); */
  /* glEnableVertexAttribArray(normalPosAttrib); */

  constexpr GLint vertexPosAttrib = 0;
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, 0);
  glEnableVertexAttribArray(0);

  constexpr GLint texturePosAttrib = 1;
  void* textureOffset = reinterpret_cast<void*>(sizeof(Mesh::Vertex::pos));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void*)(3 * 4));
  glEnableVertexAttribArray(1);
  
  constexpr GLint normalPosAttrib = 2;
  void* normalOffset = reinterpret_cast<void*>((size_t)textureOffset + sizeof(Mesh::Vertex::texturePos));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void*)(3 * 4 + 2 * 4));
  glEnableVertexAttribArray(2);

  glm::mat4 model = glm::mat4(1.0f);
  /* glUniformMatrix4fv(_generalShader.getUniformId("model"), 1, GL_FALSE, glm::value_ptr(model)); */
  _generalShader.setUniform("model", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(model));

  glm::mat4 view = _camera.getView();
  /* glUniformMatrix4fv(_generalShader.getUniformId("view"), 1, GL_FALSE, glm::value_ptr(view)); */
  _generalShader.setUniform("view", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(view));

  glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)_screenWidth / (float)_screenHeight, 1.0f, 10000.0f);
  /* glUniformMatrix4fv(_generalShader.getUniformId("proj"), 1, GL_FALSE, glm::value_ptr(proj)); */
  _generalShader.setUniform("proj", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(proj));

  // bind texture to sampler in shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureId);
  /* glUniform1i(_generalShader.getUniformId("texSampler"), 0); // TODO: GL_TEXTURE0 ? */
  _generalShader.setUniform("texSampler", glUniform1i, 0);

  glDrawElements(GL_TRIANGLES, res.meshes()[0].getTriangleVerticesCount(), GL_UNSIGNED_INT, 0);
  glDrawElements(GL_QUADS, res.meshes()[0].getQuadVerticesCount(), GL_UNSIGNED_INT,
                 (void*)(res.meshes()[0].getTriangleFacesBytes())); // offset
  glDrawElements(GL_QUADS, res.meshes()[0].getQuadVerticesCount(), GL_UNSIGNED_INT,
                 (void*)(res.meshes()[0].getTriangleFacesBytes())); // offset

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glutSwapBuffers();
}

void Renderer::idle()
{
  if (handleKeyboard())
  {
    glutPostRedisplay();
  }
}

void Renderer::reshape(int width, int height)
{
  _screenWidth = width;
  _screenHeight = height;
  setPerspective();
}

void Renderer::timer(int millisec)
{
    glutTimerFunc(millisec, Renderer::timerWrapper, millisec);
    glutPostRedisplay();
}

std::ostream& operator<<(std::ostream& s, glm::vec3 v)
{
  s << "x: " << v.x << ", y: " << v.y << ", z: " << v.z;
  return s;
}

std::ostream& operator<<(std::ostream& s, glm::vec4 v)
{
  s << "x: " << v.x << ", y: " << v.y << ", z: " << v.z << ", w: " << v.w;
  return s;
}

bool Renderer::handleKeyboard()
{
  constexpr GLfloat speed = 10.0;
  bool stateChanged = false;
  if (_keyboard.isKeyDown('w'))
  {
    _camera.moveForward(speed);
    stateChanged = true;
  }
  else if (_keyboard.isKeyDown('s'))
  {
    _camera.moveBackward(speed);
    stateChanged = true;
  }

  if (_keyboard.isKeyDown('a'))
  {
    _camera.moveLeft(speed);
    stateChanged = true;
  }
  else if (_keyboard.isKeyDown('d'))
  {
    _camera.moveRight(speed);
    stateChanged = true;
  }
  
  if (_keyboard.isKeyDown(' '))
  {
    _camera.addY(speed);
    stateChanged = true;
  }
  else if (_keyboard.isKeyDown('c'))
  {
    _camera.subY(speed);
    stateChanged = true;
  }

  /* std::cout << "DBG: after: cameraPos: " << cameraPos << std::endl; */
  /* std::cout << "DBG: after: cameraTarget: " << cameraTarget << std::endl; */
  return stateChanged;
}

void Renderer::mouse(int button, int state, int x, int y)
{
  mouseX = x;
  mouseY = y;

  if(button == GLUT_LEFT_BUTTON)
  {
    if(state == GLUT_DOWN)
    {
        mouseLeftDown = true;
    }
    else if(state == GLUT_UP)
    {
        mouseLeftDown = false;
    }
  }
  else if(button == GLUT_RIGHT_BUTTON)
  {
    if(state == GLUT_DOWN)
    {
        mouseRightDown = true;
    }
    else if(state == GLUT_UP)
    {
        mouseRightDown = false;
    }
  }
}

GLfloat pitch = 0;

void Renderer::mouseMotion(int x, int y)
{
  GLfloat sensitivity = 0.5f;
  GLfloat xOffset = -(x - mouseX);
  GLfloat yOffset = -(y - mouseY);

  _camera.moveLook(xOffset * sensitivity, yOffset * sensitivity);

  /* std::cout << "DBG: viewForward: " << viewForward << std::endl; */
  /* std::cout << "DBG: viewRight: " << viewRight << std::endl; */
  /* std::cout << "DBG: viewUp: " << viewUp << std::endl; */

  mouseX = x;
  mouseY = y;
}

void Renderer::setPerspective()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)_screenWidth, (GLsizei)_screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)(_screenWidth)/_screenHeight, 1.0f, 10000.0f); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
