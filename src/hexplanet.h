#ifndef HEXPLANET_H
#define HEXPLANET_H

#include <vector>

#include <Imath/ImathVec.h>
#include <Imath/ImathColor.h>

#include <GL/gl.h>
#ifdef WIN32
#include <GL/glaux.h>
#endif
#include <GL/glu.h>

struct HexTri;

// A Hextile is a single hex (or sometimes pentagon)
// in the hex tiled planet. It is a single vertex of
// the dual mesh.
struct HexTile
{
	HexTile( Imath::V3f p );	

	// the position of the vert in the tile,
	// and the center of the hex in space
	Imath::V3f m_vertPos;		
	Imath::V3f m_nrm;

	// Terrain Type
	enum {
		Terrain_WATER,
		Terrain_DESERT,
		Terrain_GRASSLAND,
		Terrain_FOREST,
		Terrain_MOUNTAIN
	};

	int m_terrain;
	
	// Triangle that share this hex
	std::vector<HexTri*> m_hextri;	
};

// A hextri is an element of the dual
// of the (mostly) hex tiling
struct HexTri
{
	HexTri( size_t a, size_t b, size_t c );

	// indices into the hex list
	size_t m_hexA, m_hexB, m_hexC;
	
	// Connectivity, links to neighbors
	// i.e. nbCA = neighbor across edge A<->C
	HexTri *m_nbAB, *m_nbBC, *m_nbCA;

	// Returns the center of the tri
	Imath::V3f getCenter( const std::vector<HexTile> &hexes  );

	// Temporaries that don't need to stick around	
	union {
		// Just used in the construction process
		size_t m_newvert;

		// angle around tile center
		float m_angle;
	} m_tmp;
	

	// Dbg color
	//Imath::C4f m_dbgColor;
};

class HexPlanet
{
public:
	HexPlanet( int subd_level, float trandom, float twatery );

	enum {
		DrawMode_CONSTRUCTION, // Show construction
		DrawMode_TERRAIN,      // Terrain with no grid
		DrawMode_TERRAINGRID,  // Terrain with grid
	};

	void draw( int draw_mode );

	size_t getNumHexes();

	void subdivide( float trandom, float twatery );

	// returns the polygon representation of this
	// hex. Usually 6-sided but could be a pentagon	
	void getPolygon( HexTile &tile, std::vector<Imath::V3f> &poly, float offset=0.0f );

	// returns the indices of the neighbors of this tile
	// Usually 6, could be 5. These aren't in any order
	void getNeighbors( size_t tileNdx, std::vector<size_t> &nbrs );
	
	// Returns a point on the planet's surface given a ray
	bool rayHitPlanet( Imath::V3f p, Imath::V3f dir, Imath::V3f &result );

	// Get a hex index from a 3d point .. projects the point
	// down to the planet surface and returns the index of the
	// hex containing it
	size_t getHexIndexFromPoint( Imath::V3f surfPos );

//protected:
	int m_subdLevel;

	// construction operations
	void buildLevel0( float twatery );	
	void projectToSphere();
	void findNeighbors();
	int getRandomTerrain( float twatery );

	// data
	std::vector<HexTile> m_hexes;
	std::vector<HexTri> m_hexdual;

	// static resources
	static bool m_initStaticRes;
	static GLuint g_texTemplate;
	static GLuint g_texTileset;
	static GLuint g_texTilesetGrid; // Tileset with outline

	static float kPlanetRadius;
};

#endif
