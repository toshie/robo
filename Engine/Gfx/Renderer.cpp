#define GL_GLEXT_PROTOTYPES
#define GLEW_STATIC

#include "Renderer.h"

#include "Common/ResourceManager.h"
#include "DataModel/Mesh.h"

#include <functional>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>

GLfloat cameraAngleX = 0.0f;
GLfloat cameraAngleY = 0.0f;
GLfloat cameraDistance = 5.0f;
GLfloat mouseX = 0.0f;
GLfloat mouseY = 0.0f;
bool mouseLeftDown = false;
bool mouseRightDown = false;
glm::vec3 cameraPos = glm::vec3(2.5f, 2.5f, 2.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

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

  if (_textureBufferId != 0)
    glDeleteBuffersARB(1, &_textureBufferId);

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
  if (!_vertexShader.init() || !_fragmentShader.init())
  {
    std::cerr << "Could not load needed shaders\n";
    return false;
  }

  _shaderProgramId = glCreateProgram();
  glAttachShader(_shaderProgramId, _vertexShader.getId());
  glAttachShader(_shaderProgramId, _fragmentShader.getId());
  glBindAttribLocation(_shaderProgramId, 0, "pos");
  glBindFragDataLocation(_shaderProgramId, 0, "FragColor");
  glLinkProgram(_shaderProgramId);
  glUseProgram(_shaderProgramId);
  
  GLint status = GL_TRUE;
  glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &status);
  if (status != GL_TRUE)
  {
    GLchar errorLog[256] = {0};
    GLsizei length = 0;
    glGetProgramInfoLog(
        _shaderProgramId, sizeof(errorLog) / sizeof(GLchar), &length, errorLog);

    std::cerr << "ERROR: Shader '"
              << "' linking failed.\n"
              << "Message: \n"
              << errorLog << std::endl;

    return false;
  }


  // test lights
  // set up light colors (ambient, diffuse, specular)
  //GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};  // ambient light
  //GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
  //GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
  //glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
  //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
  //glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);





  // position the light
  float lightPos[4] = {0, 0, 20, 1}; // positional light
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration

  return true;
}

void Renderer::start()
{
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // show only mesh edges */

  cameraDistance = ResourceManager::instance().meshes()[0].getVertices()[0][0];

  ResourceManager& res = ResourceManager::instance();
  const Mesh& mesh = res.meshes()[0];


  // vertex buffer
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER,
               mesh.getVertices().size() * 3 * sizeof(GLfloat),
               &(*mesh.getVertices().begin()),
               GL_STATIC_DRAW);

  // index buffer
  glGenBuffers(1, &_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh.getTriangleVertexIndices().size() * 3 * sizeof(GLuint) + mesh.getQuadVertexIndices().size() * 4 * sizeof(GLuint),
               0,
               GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                  0,
                  mesh.getTriangleVertexIndices().size() * 3 * sizeof(GLuint),
                  &(*mesh.getTriangleVertexIndices().begin()));
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                  mesh.getTriangleVertexIndices().size() * 3 * sizeof(GLuint),
                  mesh.getQuadVertexIndices().size() * 4 * sizeof(GLuint),
                  &(*mesh.getQuadVertexIndices().begin()));


  // texture buffer
  glGenBuffers(1, &_textureBufferId);
  glBindBuffer(GL_TEXTURE_BUFFER, _textureBufferId);
  glBufferData(GL_TEXTURE_BUFFER,
               mesh.getTextures().size() * 3 * sizeof(GLfloat),
               &(*mesh.getTextures().begin()),
               GL_STATIC_DRAW);

  // texture
  glGenTextures(1, &_textureId);
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
        SOIL_load_image("Resource/Obj/maps/ARC170_TXT_VERSION_4_D.jpg", &width, &height, 0, SOIL_LOAD_RGB);
  if (!image)
  {
    std::cerr << "Couldn't load a texture\n";
    return;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                    GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);


  // glBindTexture(GL_TEXTURE_2D, 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_TEXTURE_BUFFER, 0);

  glutMainLoop();
}

void Renderer::display()
{
  // clear buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // save the initial ModelView matrix before modifying ModelView matrix
  glPushMatrix();

  // tramsform camera
  glTranslatef(0, 0, -cameraDistance);
  glRotatef(cameraAngleX, 1, 0, 0);   // pitch
  glRotatef(cameraAngleY, 0, 1, 0);   // heading

  ResourceManager& res = ResourceManager::instance();
  //res.console() << "MESH: " << res.meshes()[0].vertices[100][1];

  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBindBuffer(GL_TEXTURE_BUFFER, _textureBufferId);






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


    GLint posAttrib = glGetAttribLocation(_shaderProgramId, "pos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(posAttrib);


  GLint uniModel = glGetUniformLocation(_shaderProgramId, "model");
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(
            model,
            glm::radians(180.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

  /* glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); */
  /* glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); */
  /* glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); */
  /* glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); */
  /* glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection)); */
  /* glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight); */

    // Set up projection
    glm::mat4 view = glm::lookAt(
        cameraPos,
        cameraTarget,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    GLint uniView = glGetUniformLocation(_shaderProgramId, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

  glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)_screenWidth / (float)_screenHeight, 1.0f, 10000.0f);
  GLint uniProj = glGetUniformLocation(_shaderProgramId, "proj");
  glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
  /* GLuint u_projection_matrix = glGetUniformLocation(_shaderProgramId, "u_projection_matrix"); */
  /* glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE, glm::value_ptr(proj)); */




  glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, 0);
  //glNormalPointer(GL_FLOAT,

  /* glDrawArrays(GL_TRIANGLES, 0, 3); */
  glDrawElements(GL_TRIANGLES, res.meshes()[0].getTriangleVertexIndices().size() * 3, GL_UNSIGNED_INT, 0);
  glDrawElements(GL_QUADS, res.meshes()[0].getQuadVertexIndices().size() * 4, GL_UNSIGNED_INT,
                 (void*)(res.meshes()[0].getTriangleVertexIndices().size() * 3 * sizeof(GLuint))); // offset

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_TEXTURE_BUFFER, 0);
  //glVertexPointer(3, GL_FLOAT, 0, &(*res.meshes()[0].vertices().begin()) );
  //glDrawArrays(GL_TRIANGLES, 0, res.meshes()[0].vertices().size());
  //glDrawElements(GL_TRIANGLES, res.meshes()[0].indices().size() * 3, GL_UNSIGNED_INT, &(*res.meshes()[0].indices().begin()) );



  glPopMatrix();

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
  constexpr GLfloat speed = 25.;

  glm::vec3 backwardDirection = glm::normalize(cameraPos - cameraTarget);
  glm::vec3 forwardDirection = glm::normalize(-backwardDirection);
  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), backwardDirection));
  glm::vec3 left = -right;
  glm::vec3 up = glm::cross(backwardDirection, right);

  bool stateChanged = false;
  if (_keyboard.isKeyDown('w'))
  {
    cameraPos += forwardDirection * speed;
    cameraTarget += forwardDirection * speed;
    stateChanged = true;
  }
  else if (_keyboard.isKeyDown('s'))
  {
    cameraPos += backwardDirection * speed;
    cameraTarget += backwardDirection * speed;
    stateChanged = true;
  }

  if (_keyboard.isKeyDown('a'))
  {
    cameraPos += left * speed;
    cameraTarget += left * speed;
    stateChanged = true;
  }
  else if (_keyboard.isKeyDown('d'))
  {
    cameraPos += right * speed;
    cameraTarget += right * speed;
    stateChanged = true;
  }
  
  if (_keyboard.isKeyDown(' '))
  {
    cameraPos += up * speed;
    cameraTarget += up * speed;
    stateChanged = true;
  }
  else if (_keyboard.isKeyDown('c'))
  {
    cameraPos -= up * speed;
    cameraTarget -= up * speed;
    stateChanged = true;
  }
      /* cameraPos.x -= speed; */
      /* cameraTarget.x -= speed; */

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

void Renderer::mouseMotion(int x, int y)
{
  GLfloat sensitivity = 0.5f;
  GLfloat xOffset = -(x - mouseX);
  GLfloat yOffset = -(y - mouseY);
  
  glm::vec3 backwardDirection = glm::normalize(cameraPos - cameraTarget);
  glm::vec3 forwardDirection = -backwardDirection;
  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), backwardDirection));
  glm::vec3 left = -right;
  glm::vec3 up = glm::cross(backwardDirection, right);
  
  glm::mat4 rotation = glm::mat4(1.0f);
  rotation = glm::rotate(
            rotation,
            glm::radians(xOffset * sensitivity),
            up
        );
  rotation = glm::rotate(
            rotation,
            glm::radians(yOffset * sensitivity),
            right
        );

  /* glm::vec3 forwardDirection = glm::normalize(cameraTarget - cameraPos); */
  cameraTarget = cameraPos + glm::vec3(rotation * glm::vec4(forwardDirection, 1.0));

  mouseX = x;
  mouseY = y;
  
  // TODO: fix cameraTarget nan's when goin on the window border
  /* std::cout << "DBG: cameraPos: " << cameraPos << std::endl; */
  /* std::cout << "DBG: cameraTarget: " << cameraTarget << std::endl; */
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
