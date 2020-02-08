#define GL_GLEXT_PROTOTYPES

#include "Common/ResourceManager.h"
#include "Engine/Gfx/DataModel/Mesh.h"
#include "Engine/Gfx/Renderer.h"

#include <functional>

GLfloat cameraAngleX = 0.0f;
GLfloat cameraAngleY = 0.0f;
GLfloat cameraDistance = 5.0f;
GLfloat mouseX = 0.0f;
GLfloat mouseY = 0.0f;
bool mouseLeftDown = false;
bool mouseRightDown = false;

Renderer::~Renderer()
{
  if (_vertexBufferId != 0)
    glDeleteBuffersARB(1, &_vertexBufferId);

  if (_indexBufferId != 0)
    glDeleteBuffersARB(1, &_indexBufferId);
}

bool Renderer::init(int& argc, char**& argv)
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

  glutInitWindowSize(_screenWidth, _screenHeight);    // window size

  glutInitWindowPosition(100, 100);                   // window location

  // finally, create a window with openGL context
  // Window will not displayed until glutMainLoop() is called
  // it returns a unique ID
  _screenHandle = glutCreateWindow(argv[0]);          // param is the title of window

  // register GLUT callback functions
  glutDisplayFunc(Renderer::displayWrapper);
  glutTimerFunc(33, Renderer::timerWrapper, 33);      // redraw only every given millisec
  //glutIdleFunc(idleCB);                             // redraw when idle
  glutReshapeFunc(Renderer::reshapeWrapper);
  //glutKeyboardFunc(keyboardCB);
  glutMouseFunc(mouseWrapper);
  glutMotionFunc(mouseMotionWrapper);


  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // for lights testing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);

  glClearColor(0.5f, 0.5f, 0.5f, 0);                   // background color
  glClearStencil(0);                          // clear stencil buffer
  glClearDepth(1.0f);                         // 0 is near, 1 is far


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
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // show only mesh edges

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
  glGenTextures(1, &_textureBufferId);


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






  glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, 0);
  //glNormalPointer(GL_FLOAT,
  glDrawElements(GL_TRIANGLES, res.meshes()[0].getTriangleVertexIndices().size() * 3, GL_UNSIGNED_INT, 0);
  glDrawElements(GL_QUADS, res.meshes()[0].getQuadVertexIndices().size() * 4, GL_UNSIGNED_INT,
                 (void*)(res.meshes()[0].getTriangleVertexIndices().size() * 3 * sizeof(GLuint))); // offset

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  //glVertexPointer(3, GL_FLOAT, 0, &(*res.meshes()[0].vertices().begin()) );
  //glDrawArrays(GL_TRIANGLES, 0, res.meshes()[0].vertices().size());
  //glDrawElements(GL_TRIANGLES, res.meshes()[0].indices().size() * 3, GL_UNSIGNED_INT, &(*res.meshes()[0].indices().begin()) );



  glPopMatrix();

  glutSwapBuffers();
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
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
}


void Renderer::mouseMotion(int x, int y)
{
    if(mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if(mouseRightDown)
    {
        cameraDistance -= (y - mouseY) * 0.2f;
        mouseY = y;
    }
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
