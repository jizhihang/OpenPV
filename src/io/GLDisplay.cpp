/*
 * GLDisplay.cpp
 *
 *  Created on: Jan 9, 2010
 *      Author: Craig Rasmussen
 */

#include "GLDisplay.hpp"
#include "io.h"
#include "../layers/HyPerLayer.hpp"

#include <assert.h>

#ifdef PV_USE_OPENGL
#  include <GLUT/glut.h>
#else
#  include "../include/gl_stubs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// glut callbacks and gl prototypes
//
void glut_resize(int wWidth, int wHeight);
void glut_display(void);
void glut_timer_func(void);
static int glut_init(int * argc, char * argv[], int wWidth, int wHeight);
static void gl_init();
static void gl_draw_texture(int id);

#ifdef __cplusplus
}
#endif

class PV::GLDisplay;

// global variables
//

PV::GLDisplay * glProbe = NULL;

float g_msecs = 0.0;  /* timer interval */

int g_xshift = -25;
int g_yshift = -25;

int glwWidth  = 512;   /* window width */
int glwHeight = 512;   /* window height */

bool glFirst = true;

namespace PV {

/**
 * This class runs an OpenGL probe.  It MUST be a singleton (only one instance).
 */
GLDisplay::GLDisplay(int * argc, char * argv[], HyPerCol * hc, int numRows, int numCols)
{
   this->parent = hc;
   hc->setDelegate(this);

   this->numRows = numRows;
   this->numCols = numCols;
   this->numDisplays = 0;

   // start with time before start for initial display refresh
   lastUpdateTime = hc->simulationTime() - hc->getDeltaTime();

   glProbe = this;
   g_msecs = 0.0f;    // glut timer delay

   time     = 0.0;
   stopTime = 0.0;

   image = NULL;

   const int maxDisplays = numRows * numCols;

   displays = (LayerDataInterface **) calloc(maxDisplays, sizeof(LayerDataInterface *));
   textureIds = (int *) calloc(maxDisplays, sizeof(int));

   glut_init(argc, argv, glwWidth, glwHeight);
}

GLDisplay::~GLDisplay()
{
   free(displays);
   free(textureIds);
}

int GLDisplay::addDisplay(LayerDataInterface * l)
{
   numDisplays += 1;
   assert(numDisplays < numRows * numCols);

   displays[numDisplays-1] = l;
   textureIds[numDisplays-1] = this->getTextureId(l);

   return 0;
}

int GLDisplay::addLayer(HyPerLayer * l)
{
   int status = addDisplay(l);
   if (status == 0) {
      status = l->insertProbe(this);
   }

   return status;
}

int GLDisplay::setImage(Image * image)
{
   this->image = image;
   return addDisplay(image);
}

void GLDisplay::setDelay(float msecs)
{
   g_msecs = msecs;    // glut timer delay
}

void GLDisplay::run(float time, float stopTime)
{
   this->time = time;
   this->stopTime = stopTime;
   glutMainLoop(); // we never return...
}

int GLDisplay::loadTexture(int id, LayerDataInterface * l)
{
   int status = 0;

   if (l == NULL) return -1;

   const PVLayerLoc * loc = l->getLayerLoc();

   const int n = loc->nx * loc->ny * loc->nBands;
   unsigned char * buf = new unsigned char[n];
   assert(buf != NULL);

   status = l->copyToInteriorBuffer(buf);

   const int width  = loc->nx * loc->nBands;  // increase width to overlay features
   const int height = loc->ny;

   glBindTexture(GL_TEXTURE_2D, id);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buf);

   delete buf;

   return 0;
}

void GLDisplay::drawDisplays()
{
   const bool exitOnFinish = true;

   time = parent->advanceTime(time);
   if (time >= stopTime) {
      parent->exitRunLoop(exitOnFinish);
   }

   if (image != NULL && lastUpdateTime < image->lastUpdate()) {
      loadTexture(getTextureId(image), image);
      lastUpdateTime = time;
   }

   if (!glwHeight) {
      return;
   }

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // PetaVision coordinate system is y==0 at top
   //
   glRotatef(180, 0, 0, 1);

   // this transformation for lower left-hand corner
   glScalef(0.75, 0.75, 1);
   glTranslatef(13, 13, -20);

   // this frame takes up entire display
   //glTranslatef(0, 0, -11);

   for (int i = 0; i < numDisplays; i++) {
      int dx = xTranslate(i);
      int dy = yTranslate(i);
      glTranslatef(dx, dy, 0);
      gl_draw_texture(textureIds[i]);
   }

   glutSwapBuffers();

   // force redraw to advance PetaVision time
   //
   if (g_msecs == 0.0f) {
      glutPostRedisplay();
   }
}

// implement LayerProbe interface
//
int GLDisplay::outputState(float time, HyPerLayer * l)
{
   return loadTexture(getTextureId(l), l);
}

int GLDisplay::getTextureId(LayerDataInterface * l)
{
   for (int i = 0; i < this->numDisplays; i++) {
      if (l == this->displays[i]) {
         // not sure what texture ids are valid
         return (10 + i);
      }
   }

   fprintf(stderr, "GLDisplay::getTextureId: couldn't find layer %p\n", l);
   exit(1);

   return 0;
}

int GLDisplay::xTranslate(int index)
{
   if (index == 0)            return 0;
   else if (index % numCols)  return g_xshift;
   else                       return -1 * (numCols - 1) * g_xshift;
}

int GLDisplay::yTranslate(int index)
{
   if (index == 0)            return 0;
   else if (index % numCols)  return 0;
   else                       return g_yshift;
}

} // namespace PV

static
void gl_draw_texture(int id)
{
   glEnable(GL_TEXTURE_2D); /* enable texture mapping */
   glBindTexture(GL_TEXTURE_2D, id); /* bind to our texture */

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f,0.0f); /* lower left corner of image */
   glVertex3f(-10.0f, -10.0f, 0.0f);
   glTexCoord2f(1.0f, 0.0f); /* lower right corner of image */
   glVertex3f(10.0f, -10.0f, 0.0f);
   glTexCoord2f(1.0f, 1.0f); /* upper right corner of image */
   glVertex3f(10.0f, 10.0f, 0.0f);
   glTexCoord2f(0.0f, 1.0f); /* upper left corner of image */
   glVertex3f(-10.0f, 10.0f, 0.0f);
   glEnd();

   glDisable(GL_TEXTURE_2D); /* disable texture mapping */
}

void glut_keyboard(unsigned char key, int x, int y)
{
   switch (key) {
   /* exit the program */
   case 27:
   case 'q':
   case 'Q':
//      parent->finish();
      exit(1);
      break;
   }
}

void glut_display(void)
{
   glProbe->drawDisplays();
}

void glut_timer_func(int value)
{
   glProbe->drawDisplays();
   glutTimerFunc(g_msecs, glut_timer_func, value);
}

/**
 * Resize function.  Called when window is created and resized.
 */
void glut_resize(int wWidth, int wHeight)
{
   glwWidth  = wWidth;
   glwHeight = wHeight;

   glViewport(0, 0, glwWidth, glwHeight);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(90, glwWidth / glwHeight, 1, 9999);

   glutPostRedisplay();
}

static
void gl_init()
{
   glEnable(GL_DEPTH_TEST);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static
int glut_init(int * argc, char * argv[], int wWidth, int wHeight)
{
   glutInit(argc, argv);

   // initialize display window
   //
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(wWidth, wHeight);
   glutInitWindowPosition(0, 0);
   glutCreateWindow("PetaVision Realtime Display");

   // register callbacks
   //
   glutKeyboardFunc(glut_keyboard);
   glutDisplayFunc(glut_display);
   glutReshapeFunc(glut_resize);
   if (g_msecs > 0.0f) {
      glutTimerFunc(g_msecs, glut_timer_func, 3);
   }

   gl_init();

   return 0;
}
