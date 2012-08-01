#ifndef GL_GAMEFONT_H
#define GL_GAMEFONT_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* font metrics */
enum {
    GF_FONT_NUMCHARS,   /* number of characters in the font */        
    GF_FONT_WIDTHSCL,   /* float, width scaling of the text */

	GF_CHARSET_LEADING,    /* The leading (height + vertical space) of the font */
	GF_CHARSET_NUMGLYPHS,  /* number of glyphs in the font */    

    GF_GLYPH_WIDTH,     /* width of the character */
    GF_GLYPH_HEIGHT,    /* height of the character */
    GF_GLYPH_PXLSIZE,   /* pixelsize of this glyph */
    GF_GLYPH_BASELINE,  /* baseline offset of the character */
    GF_GLYPH_XPOS,      /* x position in texture pixels */
    GF_GLYPH_YPOS,      /* y position in texture pixels */    
};

/* setting that can be enabled/disabled */
enum {
    GF_KERN_TEXT,    /* default: enabled (not implemented yet)*/    
};

/* creates a font that uses the specified texture. */
GLuint gfCreateFont( GLuint texture_id );

/* load the font metrics from the specified file */
void gfLoadFontMetrics( GLuint fontId, const char *finfo_filename );

/* adjust the font metrics */
int gfGetFontMetric( GLuint fontId, GLuint metric );
int gfGetCharsetMetric( GLuint fontId, GLuint pxlsize, GLuint metric );
int gfGetGlyphMetric( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric );

void gfSetFontMetric( GLuint fontId, GLuint metric, int value );
void gfSetCharsetMetric( GLuint fontId, GLuint pxlsize, GLuint metric, int value );
void gfSetGlyphMetric( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric, int value );

float gfGetFontMetricf( GLuint fontId, GLuint metric );
float gfGetCharsetMetricf( GLuint fontId, GLuint pxlsize, GLuint metric );
float gfGetGlyphMetricf( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric );

void gfSetFontMetricf( GLuint fontId, GLuint metric, float value );
void gfSetCharsetMetricf( GLuint fontId, GLuint pxlsize, GLuint metric, float value );
void gfSetGlyphMetricf( GLuint fontId, GLuint pxlsize, char glyph, GLuint metric, float value );


/* returns the kerning pair value. This is how much glyphB is  */
/* adjusted horizontally when drawn after glyphA. For example, */
/* "AV" would probably have a kerning value. Negative values   */
/* indicate that glyphB should be moved away from glyphA.      */
/* The glyphs must have the same PxlSize or this will generate */
/* an error. */
/* TODO: These are not implemented */
int gfGetKerningPair( GLuint fontId, int glyphIdA, int glyphIdB );
int gfSetKerningPair( GLuint fontId, int glyphIdA, int glyphIdB );

/* Like glBegin( ... ) / glEnd(); This sets up the modelview, projection */
/* and texture matrix for text drawing. Place these around gfDrawString() */
void gfBeginText();
void gfEndText();

/* enables the given font at the specified pxlsz. If the font does   */
/* not contain glyphs of the requested pxlsize, the nearest one will */
/* be used with scaling. */

/* TODO: right now only pixelsz that are present are used, it should */
/* scale the font polys to approximate non-existing sizes */
void gfEnableFont( GLuint fontId, GLuint pxlsz );
void gfDisableFont();

/* enable/disable various settings */
void gfEnable( GLenum setting );
void gfDisable( GLenum setting );

/* Draws a simple string */
void gfDrawString( const char *s );

/* Draws a string with printf-like formatting */
void gfDrawStringFmt( const char *s, ... );

/* calculates the size of a string */
int gfGetStringWidth( const char *s );
int gfGetStringHeight( const char *s);

/* Following the gl/glu style of splitting stuff that is just "convienance" */
/* into GLU, the gfu functions offer added convienance functions */

// NOTE: None of the gfu stuff is implemented yet....

/* Draws a string with formatting. You probably want to use this   */
/* with display lists, as parsing the formatting can take a while, */
/* relatively speaking. */
/****
 Format strings is a simple, html-like format:

  <i>Italic</i> - char rectangle will be skewed when drawing 
  <b>Bold</b>   - Widen, really. char rectangles will be x-scaled 110% 
  <#ffee2g>     - color will be set to (0xff, 0xee, 0x2g)
  </#>          - shorthand, color will be set to white (0xffffff)

  For example "<#ffeegg>Red Text</#> is <i>extra</i> good."


 if wrapWidth is non-zero, text will word-wrap at wrapWidth. 

 ***/
GLuint gfuDrawFormattedString( const char *string, int wrapWidth );


/* specifify the font and the font metrics at once */
GLuint gfuCreateFont( GLuint texture_id, const char *finfo_filename );

/* sets up the projection for 2D font rendering */
void gfuDrawString2D( int sx, int sy, const char *s );
void gfuDrawFormattedString2D( int sx, int sy, const char *s );

/* draws the string at the current local transform with additional */
/* scaling such that the current pxlsize is 1.0 in local space */
void gfuDrawString3D( const char *s );

/* draws the string along the given bezier curve */
/* if "normalizeLength" is true, the text will be stretched to */
/* fill the entire length of the path */
void gfuDrawStringAlongPath( const char *s,
			     int x0, int y0,
			     int x1, int y1,
			     int x2, int y2,
			     int x3, int y3,
			     int normalizeLength );

#ifdef __cplusplus
}
#endif

#endif

