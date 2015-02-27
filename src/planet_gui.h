#ifndef PLANET_GUI_H
#define PLANET_GUI_H

#include <GL/glut.h>
#include <GL/glui.h>

#include <OpenEXR/ImathVec.h>

#include "hexplanet.h"

// A star billboard in the starfield
struct Star
{
	int index;
	Imath::V3f p;
	float radius;
};

//=====================================================
// Main interface class, also draws the background
// starfield and lighting
//=====================================================
class PlanetGui
{
public:
	PlanetGui( int gluiMainWin );

	void buildInterface();

	void redraw();	

	void paintTile();

protected:

	// Updates the hex cursor pos
	void updateCursorPos();

	// font stuff
	GLuint m_glFontTexId_console;
	GLuint m_fntConsole;

	// Glut and GLUI widgets
	GLUI *m_glui;
	int m_gluiMainWin;

	// callbacks
	void cbControl( int id );

	// Widget IDs
	enum {
		ID_SUBDIVISION,
		ID_TERRRAND,
		ID_TWATER,
		ID_BTN_REBUILD,
		ID_BTN_SUBDIV,
		ID_DRAWMODE,

		ID_LAST
	};

	// Index of the hex the cursor is over
	size_t m_cursorHex;

	// location of the mouse cursor on the planet's surface
	// (last know location if mouse is not touching planet)
	Imath::V3f m_mouseSurfacePos;
	bool m_mouseOnSurface;

	// planet displaylist
	GLuint m_planetDlist;
	bool m_planetDirty;	

	// display params
	int m_beautyMode;
	int m_showStats;

	// planet params
	int m_subdLevel;  // subdivision level 
	int m_drawMode;	  // draw mode
	int m_paintTile;  // which tile to "paint"

	// "terraforming" params
	float m_terrRandom;
	float m_terrWatery;

	// the planet map
	HexPlanet *m_planet;

	// Planet Rotation
	float m_rotPlanet[16];
	float m_rotSun[16];

	// view from opengl
	double m_modelView[16], m_projection[16];
	int m_viewport[4];

	// default rotations
	static float _ident[16];

	// singleton
	static PlanetGui *s_theGUI;

	// helpers for drawing in beauty mode
	void drawAtmosphere();
	void drawStarfield();	
	
	GLuint m_texStars;
	std::vector<Star> m_stars;

public:
	
	// updated from main
	int m_mouseX, m_mouseY;

	//Callbacks
	static void control_cb( int control );

	// max subd level
	static const int kMaxSubdivisionLevel;	
};

#endif
