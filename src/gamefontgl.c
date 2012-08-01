#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <assert.h>

#include "gamefontgl.h"

#ifdef _MSC_VER /* MSVC++ */
#define vsnprintf _vsnprintf
#endif

/* a single glyph */
typedef struct {    
    int w, h;
    int baseline;
    int xpos, ypos;    
} gfGlyphImpl;

/* a bunch of glyphs at one pxlsize */
typedef struct gfCharset_struct {
    GLuint pxlsize;
	int numGlyphs;
	int leading;
    gfGlyphImpl *glyph[256];    
    signed char *kern; /* 256x256 kerning table */
    struct gfCharset_struct *next;    
} gfCharsetImpl;

/* a whole font */
typedef struct {
	int texId;

    gfCharsetImpl *charsetList;
    int numChars;        
    float widthScl;

    int do_kern:1;
    int do_blend:1;
    
} gfFontImpl;

/* TODO: make this resize dynamically, but 100 should be plenty */
#define MAX_FONTS (100)
static gfFontImpl * _gfFontIdTable[MAX_FONTS];
static unsigned int _gfNextFontID = 1;

static gfFontImpl *_activeFont = NULL;
static gfCharsetImpl *_activeCharset = NULL;

/* creates a font that uses the specified texture. */
GLuint gfCreateFont( GLuint texture_id )
{
	unsigned int id;
    gfFontImpl *font = (gfFontImpl*)malloc(sizeof(gfFontImpl));	
	font->do_blend = 1;
	font->numChars = 0;	
	font->widthScl = 1.0;	
	font->charsetList = NULL;
	font->texId = texture_id;

#ifdef DEBUG
	// Clear the table at first use
	if (_gfNextFontID==0) {
		for (id=0; id < MAX_FONTS; id++) {
			_gfFontIdTable[id] = NULL;
		}
	}
#endif
    
	id = _gfNextFontID++;
	_gfFontIdTable[id] = font;

	return id;
}


void gfLoadFontMetrics( GLuint fontId, const char *finfo_filename )
{
	FILE *fp;

	/* temporaries for file reading */
	char line[1000];
	int rx, ry, rw, rh;    
    int ch, baseline;
	GLuint pxlsz;
	int i;

	gfFontImpl *font;
	gfCharsetImpl *charset;
	gfGlyphImpl *g;	

	assert( (fontId) && (_gfFontIdTable[fontId]) );

	font = _gfFontIdTable[ fontId ];

	fp = fopen( finfo_filename, "rt" );
	assert( fp ); /* TODO: add some real error handling */

	while (fgets( line, 1000, fp )) {
		
		if (line[0]=='#') continue;

		sscanf( line, "%d %d %d %d %d %d %d", 
			&ch, &pxlsz, &baseline,
			&rx, &ry, &rw, &rh );

		/* find the charset in the font's charset list */
		charset = font->charsetList;
		while ((charset) && (charset->pxlsize != pxlsz)) {
			charset = charset->next;
		}

		/* not found */
		if (!charset) {

			/* create and initialize a charset */ 
			charset = (gfCharsetImpl*)malloc(sizeof(gfCharsetImpl));
			charset->pxlsize = pxlsz;
			charset->leading = pxlsz; /* TODO: better than this */
			charset->numGlyphs = 0;
			for (i=0; i < 256; i++) {
				charset->glyph[i] = NULL;
			}

			charset->kern = NULL; /* NOT IMPL */

			/* add to the font */
			charset->next = font->charsetList;
			font->charsetList = charset;
		}

		font->numChars++;
		
		/* create a glyph and add it to the charset */
		g = (gfGlyphImpl*)malloc(sizeof(gfGlyphImpl));
		g->baseline = baseline;
		g->xpos = rx;
		g->ypos = ry;
		g->h = rh;
		g->w = rw;
		
		/* add the glyph to the charset */
		assert( charset->glyph[ch]==NULL );
		charset->glyph[ch] = g;
		charset->numGlyphs++;
	}

}


/* adjust the font metrics */
int gfGetFontMetric( GLuint fontId, GLuint metric )
{
	gfFontImpl *font;
	assert( (fontId) && (_gfFontIdTable[fontId]) );

	font = _gfFontIdTable[fontId];
	switch ( metric ) {
	case GF_FONT_NUMCHARS:
		return font->numChars;	
	default:
		assert(0);
		break;
	}	

	/* not reached */
	assert(0);
	return 0;
}

int gfGetCharsetMetric( GLuint fontId, GLuint pxlsize, GLuint metric ) {
	gfFontImpl *font;
	gfCharsetImpl *charset;

	assert( (fontId) && (_gfFontIdTable[fontId]) );

	font = _gfFontIdTable[fontId];

	charset = font->charsetList;
	while ((charset) && (charset->pxlsize != pxlsize)) {
		charset = charset->next;
	}
	assert( charset ); /* todo: better error handling, or return defaults or something */
	
	switch ( metric ) {
	case GF_CHARSET_NUMGLYPHS:
		return charset->numGlyphs;
	case GF_CHARSET_LEADING:
		return charset->leading;
	default:
		assert(0);
		break;
	}	

	/* not reached */
	assert(0);
	return 0;
}

int gfGetGlyphMetric( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric )
{
	/* todo */
	return 0;
}

void gfSetFontMetric( GLuint fontId, GLuint metric, int value )
{
	gfFontImpl *font;
	assert( (fontId) && (_gfFontIdTable[fontId]) );

	font = _gfFontIdTable[fontId];
	switch ( metric ) {			
	
	default:
		assert(0);
		break;
	}	

	/* not reached */
	assert(0);	
}

void gfSetCharsetMetric( GLuint fontId, GLuint pxlsize, GLuint metric, int value )
{
	gfFontImpl *font;
	gfCharsetImpl *charset;

	assert( (fontId) && (_gfFontIdTable[fontId]) );

	font = _gfFontIdTable[fontId];

	charset = font->charsetList;
	while ((charset) && (charset->pxlsize != pxlsize)) {
		charset = charset->next;
	}
	assert( charset ); /* todo: better error handling, or return defaults or something */
	
	switch ( metric ) {	
	case GF_CHARSET_LEADING:
		charset->leading = value;	
	default:
		assert(0);
		break;
	}	

	/* not reached */
	assert(0);
}

void gfSetGlyphMetric( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric, int value ) 
{
	/* todo */
}

/* TODO: only float metric is widthscale rightnow... */
float gfGetFontMetricf( GLuint fontId, GLuint metric )
{
	return 0.0; /* todo */
}

float gfGetCharsetMetricf( GLuint fontId, GLuint pxlsize, GLuint metric )
{
	return 0.0; /* todo */
}

float gfGetGlyphMetricf( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric )
{
	return 0.0; /* todo */
}

void gfSetFontMetricf( GLuint fontId, GLuint metric, float value )
{
	/* todo */
}

void gfSetCharsetMetricf( GLuint fontId, GLuint pxlsize, GLuint metric, float value )
{
	/* todo */
}

void gfSetGlyphMetricf( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric, float value )
{
	/* todo */
}

void gfEnableFont( GLuint fontId, GLuint pxlsz )
{
	
	gfCharsetImpl *charset;
	
	assert( (fontId) && (_gfFontIdTable[fontId]) );

	/* activate the font and charset */
	_activeFont = _gfFontIdTable[fontId];

	charset = _activeFont->charsetList;
	while ((charset) && (charset->pxlsize != pxlsz)) {		
		charset = charset->next;
	}
	assert( charset );	/* todo: scale closest charset to approximate  */	
				 		/* non-existing ones */

	_activeCharset = charset;
}

void gfDisableFont()
{	
	_activeFont = NULL;
	_activeCharset = NULL;
}

void gfBeginText()
{
	GLuint vp[4], txw, txh;
	assert( _activeFont );

	/* set up texture mode */
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glBindTexture( GL_TEXTURE_2D, _activeFont->texId );

	/* Set up 2d for text drawing */	
	glGetIntegerv( GL_VIEWPORT, vp );
	
	glMatrixMode( GL_PROJECTION );	
	glPushMatrix();
	glLoadIdentity();

    glOrtho( 0.0f, vp[2], 0.0f, vp[3], -1.0f, 1.0f );	

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	

	glMatrixMode( GL_TEXTURE );
	glPushMatrix();
	glLoadIdentity();	

	// Scale the texture matrix to pixel coords
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &txw );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &txh );
	glScaled( 1.0/(float)txw, -1.0/(float)txh, 1.0 );	

	glMatrixMode( GL_MODELVIEW );


}

void gfEndText()
{

	glMatrixMode( GL_TEXTURE );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
}

/* Draws a simple string */
/* use display lists if you want this to be faster */
void gfDrawString( const char *s )
{
	const char *ch;	
	int curr_x, curr_y, bl;	
	gfGlyphImpl *g;

	assert(_activeCharset);
	curr_x = 0; curr_y = 0;

	glBegin( GL_QUADS );
	for ( ch=s; *ch; ch++) {
		
		if (*ch=='\n') {
			curr_x = 0;
			curr_y -= _activeCharset->leading;
		} else if (*ch==' ') {
			curr_x += (_activeCharset->pxlsize*0.5f); /* todo: fix this */
		} else {
			g = _activeCharset->glyph[*ch];
			if (g) {
				
			bl = curr_y - (g->h - g->baseline);

			glTexCoord2f( g->xpos, g->ypos + g->h );
			glVertex2f( curr_x, bl );

			glTexCoord2f( g->xpos + g->w, g->ypos + g->h );
			glVertex2f( curr_x + g->w, bl  );

			glTexCoord2f( g->xpos + g->w, g->ypos  );
			glVertex2f( curr_x + g->w, bl + g->h );

			glTexCoord2f( g->xpos, g->ypos );
			glVertex2f( curr_x, bl + g->h );
			}

			curr_x += g->w;
		}		
	}
	glEnd();
}

/* calculates the size of a string */
int gfGetStringWidth( const char *s ) 
{
	const char *ch;
	int maxW, lineW;
	maxW = 0;
	lineW = 0;

	assert(_activeCharset);

	for ( ch=s; *ch; ch++) {
		
		/* reset total on newline */
		if (*ch=='\n') {
			if (lineW > maxW) maxW = lineW;
			lineW = 0;
		} else if (*ch==' ') {
			lineW += (_activeCharset->pxlsize*0.5f); /* todo: fix this */
		} else {
			/* add this char to total */
			if (_activeCharset->glyph[*ch]) {
				lineW = lineW + _activeCharset->glyph[*ch]->w;
			}
		}
	}
	if (lineW > maxW) maxW = lineW;

	return maxW;
}

int gfGetStringHeight( const char *s)
{	
	const char *ch;
	int nlCount = 1;
	assert(_activeCharset);

	for ( ch=s; *ch; ch++) {
		if (*ch=='\n') nlCount++;
	}

	return nlCount * _activeCharset->leading;
}

void gfDrawStringFmt( const char *s, ...)
{
	char *buff;
	int bsz, sz;

	va_list args;
	va_start(args, s);
	
	// make space for the new string 
	sz = strlen( s ) + 100;

	// format the string
	while (1) {
		buff = malloc( sz );
		bsz = vsnprintf( buff, sz, s, args );

		if (bsz >= 0) {
			gfDrawString( buff );
			free(buff);
			break;
		}
				
		// try again with twice as big until it fits
		free( buff );
		sz <<= 1;
	}

	va_end(args);	
}

