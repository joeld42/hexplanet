#ifndef LOAD_TEXTURE
#define LOAD_TEXTURE

#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>

// creates and returns a GL textureID
GLuint loadTextureDDS( const char *filename );

#endif