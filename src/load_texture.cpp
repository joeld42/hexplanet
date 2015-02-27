#ifdef WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <stdio.h>

//#include <SDL.h>
//#include <SDL_endian.h>
#include <sys/types.h>

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>

//#include "debug.h"
#include "load_texture.h"

typedef unsigned __int8 Uint8;
typedef unsigned __int16 Uint16;
typedef unsigned __int32 Uint32;

// Most of the DDS code comes from from the NVidia Whitepaper
// "Using Texture Compression in OpenGL" by Sebastien Domine

#define DDS_MAKEFOURCC(a, b, c, d) \
       ((Uint32)(Uint8)(a) | ((Uint32)(Uint8)(b) << 8) |   \
	   ((Uint32)(Uint8)(c) << 16) | ((Uint32)(Uint8)(d) << 24 ))

// Structs needed to read DDS files are described here:
//
// http://msdn.microsoft.com/archive/default.asp?url=
//         /archive/en-us/directx9_c/directx/graphics
//         /reference/ddsfilereference/ddsfileformat.asp

// FOURCC codes for DX compressed-texture pixel formats
 
#define DDS_FOURCC_DXT1  (DDS_MAKEFOURCC('D','X','T','1'))
#define DDS_FOURCC_DXT2  (DDS_MAKEFOURCC('D','X','T','2'))
#define DDS_FOURCC_DXT3  (DDS_MAKEFOURCC('D','X','T','3'))
#define DDS_FOURCC_DXT4  (DDS_MAKEFOURCC('D','X','T','4'))
#define DDS_FOURCC_DXT5  (DDS_MAKEFOURCC('D','X','T','5'))

#define DDS_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define DDS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define DDS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

// structs
struct DDPIXELFORMAT_INFO
{
	Uint32 dwSize; // should be 32
	Uint32 dwFlags;
	Uint32 dwFourCC;
	Uint32 dwRGBBitCount;
	Uint32 dwRBitMask;
	Uint32 dwGBitMask;
	Uint32 dwBBitMask;
	Uint32 dwAlphaBitMask;
};

struct DDCAPS2_INFO
{
	Uint32 dwCaps1;
	Uint32 dwCaps2;
	Uint32 dwReserved[2];
};

struct DDSURFACEDESC2_INFO
{
	Uint32 dwSize; // should be 124
	Uint32 dwFlags;
	Uint32 dwHeight, dwWidth;
	Uint32 dwLinearSize; // or pitch for uncompressed 
	Uint32 dwDepth;
	Uint32 dwMipMapCount;
	Uint32 dwReserved1[11];
	DDPIXELFORMAT_INFO ddpfPixelFormat;
	DDCAPS2_INFO ddsCaps;
	Uint32 dwReserved2;
};

struct GenericImage
{
	Uint32 w, h;
	Uint32 comp; // components, 3 or 4
	GLuint format;
	Uint8 *pixeldata;
};

GenericImage *ReadDDSFile( const char *filename, Uint32 *bufsize, Uint32 *numMipmaps )
{
	GenericImage *img;
	DDSURFACEDESC2_INFO ddsd;
	char *filecode[4];
	FILE *fp;

	// try to open the file
	fp = fopen( filename, "rb" );
	if (fp == NULL) return NULL;

	// verify the file type
	fread( filecode, 1, 4, fp );
	if (strncmp( (const char *)filecode, "DDS ", 4)!=0)
	{
		fclose(fp);
		return NULL;
	}

	// read the file
	fread( &ddsd, sizeof(ddsd), 1, fp );

	// Get the image data
	img = (GenericImage*)malloc(sizeof(GenericImage));
	memset( img, 0, sizeof(GenericImage));

	// calculate image size
	*bufsize = ddsd.dwMipMapCount > 1? ddsd.dwLinearSize * 2 : ddsd.dwLinearSize;
	
	// load image data
	img->pixeldata = (Uint8*)malloc( *bufsize * sizeof(Uint8) );
	fread( img->pixeldata, 1, *bufsize, fp );
	fclose( fp );

	// get image information
	img->w = ddsd.dwWidth;
	img->h = ddsd.dwHeight;
	img->comp = (ddsd.ddpfPixelFormat.dwFourCC == DDS_FOURCC_DXT1) ? 3 : 4;
	*numMipmaps = ddsd.dwMipMapCount;

	switch( ddsd.ddpfPixelFormat.dwFourCC)
	{
		case DDS_FOURCC_DXT1:
			img->format = DDS_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case DDS_FOURCC_DXT3:
			img->format = DDS_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case DDS_FOURCC_DXT5:
			img->format = DDS_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			//DBG::error( "Texture %s has unknown DDS pixel format! ", filename );
			free( img->pixeldata );
			free( img );
			img = NULL;
			break;
	}

	return img;
}

GLuint loadTextureDDS( const char *filename )
{
	GLuint glTexID;

	Uint32 ddsBufSize;
	Uint32 numMipmaps;
	GenericImage *ddsImage = ReadDDSFile( filename, &ddsBufSize, &numMipmaps );

	assert( ddsImage && "Could not load texture" );

	Uint32 width, height, offset, size;
	Uint32 blocksize = (ddsImage->format == DDS_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

	glGenTextures( 1, &glTexID );
	glBindTexture( GL_TEXTURE_2D, glTexID );

	// Load the mipmap levels
	offset = 0;
	width = ddsImage->w;
	height = ddsImage->h;
	for (Uint32 i=0; i < numMipmaps && (width || height); i++)
	{
		if (width==0)
		{
			width = 1;
		}

		if (height==0)
		{
			height = 1;
		}

		size = ((width+3)/4) * ((height+3)/4) * blocksize;
		glCompressedTexImage2DARB( GL_TEXTURE_2D, i, ddsImage->format, width, height,
									0, size, ddsImage->pixeldata + offset );

		//GLErrorReport() // probably a good idea
		offset += size;
		width >>= 1;
		height >>= 1;
	}

	return glTexID;
}
