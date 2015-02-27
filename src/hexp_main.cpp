#include <iostream>
#include <vector>

// must include windows.h before GL on win32
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// GL and related includes
#include <GL/glew.h>
#include <GL/gl.h>
#ifdef WIN32
#include <GL/glaux.h>
#endif
#include <GL/glu.h>


#include <GL/glut.h>
#include <GL/glui.h>

// Local includes
#include "planet_gui.h"

//=========================================
// globals (mostly for dealing with glut/gui)
//=========================================
int g_glutMainWin;
float g_aspect;
PlanetGui *g_planetGui;

//=========================================
// glut Display Func
//=========================================
void glut_Display( void )
{  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 50.0f, g_aspect, 1.0f, 5000.0f );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  g_planetGui->redraw();

  glutSwapBuffers(); 
}

//=========================================
// glut Reshape func
//=========================================
void glut_Reshape( int x, int y )
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
  glViewport( tx, ty, tw, th );

  g_aspect = (float)tw / (float)th;

  glutPostRedisplay();
}

//=========================================
// glut keyboard func
//=========================================
void glut_Keyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 27: 
  case 'q':
    exit(0);
    break;

#if 0
	// for debugging cursor thinnggy
  case '>':
  case '.':
	g_planetGui->m_cursorHex++;
	break;
   case '<':
   case ',':
	g_planetGui->m_cursorHex--;
	break;
#endif
  };  
  
  glutPostRedisplay();
}


//=========================================
// glut menu func
//=========================================
void glut_Menu( int value )
{
  glut_Keyboard( value, 0, 0 );
}


//=========================================
// glut idle func
//=========================================
void glut_Idle( void )
{
  // According to the GLUT specification, the current window is 
  // undefined during an idle callback.  So we need to explicitly change
  // it if necessary
  if ( glutGetWindow() != g_glutMainWin ) 
  {
	glutSetWindow(g_glutMainWin);  
  }

  // Sync widgets to live variables
  GLUI_Master.sync_live_all();  

  // update "sim"
  // No sim

  // redraw
  glutPostRedisplay();
}

//=========================================
// glut mouse click func
//=========================================
void glut_Mouse(int button, int button_state, int x, int y )
{
	g_planetGui->m_mouseX = x;
	g_planetGui->m_mouseY = y;    
	g_planetGui->paintTile();
}


//=========================================
// glut motion func
//=========================================
void glut_Motion(int x, int y )
{
	g_planetGui->m_mouseX = x;
	g_planetGui->m_mouseY = y;    
}

void glut_MotionDraw(int x, int y )
{
	g_planetGui->m_mouseX = x;
	g_planetGui->m_mouseY = y;    
	g_planetGui->paintTile();
}


//=========================================
// Main program
//=========================================
int main( int argc, char *argv[])
{
	// Todo: check args for fullscreen

	
	// Initialize glut
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( 800, 600 );
 
	g_glutMainWin = glutCreateWindow( "Hex Planet Demo" );

	// Initialize glew
	glewInit();

	glutDisplayFunc( glut_Display );
	GLUI_Master.set_glutReshapeFunc( glut_Reshape );  
	GLUI_Master.set_glutKeyboardFunc( glut_Keyboard );
	GLUI_Master.set_glutSpecialFunc( NULL );
	GLUI_Master.set_glutMouseFunc( glut_Mouse );
	glutMotionFunc( glut_MotionDraw );
	glutPassiveMotionFunc( glut_Motion );

	// Register idle func with glui
	GLUI_Master.set_glutIdleFunc( glut_Idle );

	// Build GLUI interface
	g_planetGui = new PlanetGui( g_glutMainWin );
	g_planetGui->buildInterface();	
	
	// opengl setup
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	// Call glut main loop  
	glutMainLoop();

	// clean up
	delete g_planetGui;

	return 1;
}
