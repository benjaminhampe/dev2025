#include "ImageDDS.hpp"

#ifdef DE_IMAGE_CODEC_DDS_ENABLED

#include <de/Color.hpp>
#include <de/FileSystem.hpp>

namespace de {
namespace image {
} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_CODEC_DDS_ENABLED

#if 0

   // try to open the file
   FILE* file = ::fopen( fileName.c_str(), "rb");
   if ( !file )
   {
      printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", fileName.c_str() );
      getchar();
      return 0;
   }

   // verify the type of file
   char filecode[4];
   ::fread( filecode, 1, 4, file );
   if ( ::strncmp( filecode, "DDS ", 4 ) != 0 )
   {
      ::fclose( file );
      return 0;
   }

   // get the surface desc
   uint8_t header[124];
   ::fread( &header, 124, 1, file );

   uint32_t height      = *(uint32_t*)&(header[8 ]);
   uint32_t width       = *(uint32_t*)&(header[12]);
   uint32_t linearSize	= *(uint32_t*)&(header[16]);
   uint32_t mipMapCount = *(uint32_t*)&(header[24]);
   uint32_t fourCC      = *(uint32_t*)&(header[80]);

   // how big is it going to be including all mipmaps?
   uint32_t bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
   uint8_t* buffer = (uint8_t*)malloc( bufsize * sizeof( uint8_t ) );
   ::fread( buffer, 1, bufsize, file );
   /* close the file pointer */
   fclose(file);

   uint32_t components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
   uint32_t format;

   switch( fourCC )
   {
   case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
   case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
   case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
   default: free(buffer); return 0;
   }

   // Create one OpenGL texture
   GLuint textureID;
   glGenTextures(1, &textureID);

   // "Bind" the newly created texture : all future texture functions will modify this texture
   glBindTexture(GL_TEXTURE_2D, textureID);
   glPixelStorei(GL_UNPACK_ALIGNMENT,1);

   uint32_t blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
   uint32_t offset = 0;

   /* load the mipmaps */
   for (uint32_t level = 0; level < mipMapCount && (width || height); ++level)
   {
      uint32_t size = ((width+3)/4)*((height+3)/4)*blockSize;
      glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
         0, size, buffer + offset);

      offset += size;
      width  /= 2;
      height /= 2;

      // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
      if(width < 1) width = 1;
      if(height < 1) height = 1;

   }

   free(buffer);

   return textureID;


/// HEADER - dependencies dds definition

enum eDDSPixelFormat
{
	DDS_PF_ARGB8888,
	DDS_PF_DXT1,
	DDS_PF_DXT2,
	DDS_PF_DXT3,
	DDS_PF_DXT4,
	DDS_PF_DXT5,
	DDS_PF_UNKNOWN
};

// 16bpp stuff
#define DDS_LOW_5		0x001F;
#define DDS_MID_6		0x07E0;
#define DDS_HIGH_5		0xF800;
#define DDS_MID_555		0x03E0;
#define DDS_HI_555		0x7C00;


// byte-align structures
#include "irrpack.h"

/* structures */
struct ddsColorKey
{
   uint32_t		colorSpaceLowValue;
   uint32_t		colorSpaceHighValue;
} PACK_STRUCT;

struct ddsCaps
{
   uint32_t		caps1;
   uint32_t		caps2;
   uint32_t		caps3;
   uint32_t		caps4;
} PACK_STRUCT;

struct ddsMultiSampleCaps
{
   uint16_t		flipMSTypes;
   uint16_t		bltMSTypes;
} PACK_STRUCT;


struct ddsPixelFormat
{
   uint32_t		size;
   uint32_t		flags;
   uint32_t		fourCC;
	union
	{
      uint32_t	rgbBitCount;
      uint32_t	yuvBitCount;
      uint32_t	zBufferBitDepth;
      uint32_t	alphaBitDepth;
      uint32_t	luminanceBitCount;
      uint32_t	bumpBitCount;
      uint32_t	privateFormatBitCount;
	};
	union
	{
      uint32_t	rBitMask;
      uint32_t	yBitMask;
      uint32_t	stencilBitDepth;
      uint32_t	luminanceBitMask;
      uint32_t	bumpDuBitMask;
      uint32_t	operations;
	};
	union
	{
      uint32_t	gBitMask;
      uint32_t	uBitMask;
      uint32_t	zBitMask;
      uint32_t	bumpDvBitMask;
		ddsMultiSampleCaps	multiSampleCaps;
	};
	union
	{
      uint32_t	bBitMask;
      uint32_t	vBitMask;
      uint32_t	stencilBitMask;
      uint32_t	bumpLuminanceBitMask;
	};
	union
	{
      uint32_t	rgbAlphaBitMask;
      uint32_t	yuvAlphaBitMask;
      uint32_t	luminanceAlphaBitMask;
      uint32_t	rgbZBitMask;
      uint32_t	yuvZBitMask;
	};
} PACK_STRUCT;


struct ddsBuffer
{
	/* magic: 'dds ' */
   char				magic[ 4 ];

	/* directdraw surface */
   uint32_t		size;
   uint32_t		flags;
   uint32_t		height;
   uint32_t		width;
	union
	{
      int32_t				pitch;
      uint32_t	linearSize;
	};
   uint32_t		backBufferCount;
	union
	{
      uint32_t	mipMapCount;
      uint32_t	refreshRate;
      uint32_t	srcVBHandle;
	};
   uint32_t		alphaBitDepth;
   uint32_t		reserved;
	void				*surface;
	union
	{
		ddsColorKey	ckDestOverlay;
      uint32_t	emptyFaceColor;
	};
	ddsColorKey		ckDestBlt;
	ddsColorKey		ckSrcOverlay;
	ddsColorKey		ckSrcBlt;
	union
	{
		ddsPixelFormat	pixelFormat;
      uint32_t	fvf;
	};
	ddsCaps			caps;
   uint32_t		textureStage;

	/* data (Varying size) */
   uint8_t		data[ 4 ];
} PACK_STRUCT;


struct ddsColorBlock
{
   uint16_t		colors[ 2 ];
   uint8_t		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlockExplicit
{
   uint16_t		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlock3BitLinear
{
   uint8_t		alpha0;
   uint8_t		alpha1;
   uint8_t		stuff[ 6 ];
} PACK_STRUCT;


struct ddsColor
{
   uint8_t		r, g, b, a;
} PACK_STRUCT;

// Default alignment
#include "irrunpack.h"


/* endian tomfoolery */
typedef union
{
   float	f;
   char	c[ 4 ];
}
floatSwapUnion;


#ifndef __BIG_ENDIAN__
#ifdef _SGI_SOURCE
#define	__BIG_ENDIAN__
#endif
#endif


#ifdef __BIG_ENDIAN__

   int32_t   DDSBigLong( int32_t src ) { return src; }
   int16_t DDSBigShort( int16_t src ) { return src; }
   float DDSBigFloat( float src ) { return src; }

   int32_t DDSLittleLong( int32_t src )
	{
		return ((src & 0xFF000000) >> 24) |
			((src & 0x00FF0000) >> 8) |
			((src & 0x0000FF00) << 8) |
			((src & 0x000000FF) << 24);
	}

   int16_t DDSLittleShort( int16_t src )
	{
		return ((src & 0xFF00) >> 8) |
			((src & 0x00FF) << 8);
	}

   float DDSLittleFloat( float src )
	{
		floatSwapUnion in,out;
		in.f = src;
		out.c[ 0 ] = in.c[ 3 ];
		out.c[ 1 ] = in.c[ 2 ];
		out.c[ 2 ] = in.c[ 1 ];
		out.c[ 3 ] = in.c[ 0 ];
		return out.f;
	}

#else /*__BIG_ENDIAN__*/

   int32_t   DDSLittleLong( int32_t src ) { return src; }
   int16_t DDSLittleShort( int16_t src ) { return src; }
   float DDSLittleFloat( float src ) { return src; }

   int32_t DDSBigLong( int32_t src )
	{
		return ((src & 0xFF000000) >> 24) |
			((src & 0x00FF0000) >> 8) |
			((src & 0x0000FF00) << 8) |
			((src & 0x000000FF) << 24);
	}

   int16_t DDSBigShort( int16_t src )
	{
		return ((src & 0xFF00) >> 8) |
			((src & 0x00FF) << 8);
	}

   float DDSBigFloat( float src )
	{
		floatSwapUnion in,out;
		in.f = src;
		out.c[ 0 ] = in.c[ 3 ];
		out.c[ 1 ] = in.c[ 2 ];
		out.c[ 2 ] = in.c[ 1 ];
		out.c[ 3 ] = in.c[ 0 ];
		return out.f;
	}

#endif /*__BIG_ENDIAN__*/



// Copyright (C) 2002-2012 Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

/*
	Based on Code from Copyright (c) 2003 Randy Reddig
	Based on code from Nvidia's DDS example:
	http://www.nvidia.com/object/dxtc_decompression_code.html

	mainly c to cpp
*/


#include "CImageLoaderDDS.h"

#ifdef _IRR_COMPILE_WITH_DDS_LOADER_

#include "IReadFile.h"
#include "os.h"
#include "CColorConverter.h"
#include "CImage.h"
#include "irrString.h"


namespace irr
{

namespace video
{

namespace
{

/*!
	DDSDecodePixelFormat()
	determines which pixel format the dds texture is in
*/
void DDSDecodePixelFormat( ddsBuffer *dds, eDDSPixelFormat *pf )
{
	/* dummy check */
	if(	dds == NULL || pf == NULL )
		return;

	/* extract fourCC */
   const uint32_t fourCC = dds->pixelFormat.fourCC;

	/* test it */
	if( fourCC == 0 )
		*pf = DDS_PF_ARGB8888;
   else if( fourCC == *((uint32_t*) "DXT1") )
		*pf = DDS_PF_DXT1;
   else if( fourCC == *((uint32_t*) "DXT2") )
		*pf = DDS_PF_DXT2;
   else if( fourCC == *((uint32_t*) "DXT3") )
		*pf = DDS_PF_DXT3;
   else if( fourCC == *((uint32_t*) "DXT4") )
		*pf = DDS_PF_DXT4;
   else if( fourCC == *((uint32_t*) "DXT5") )
		*pf = DDS_PF_DXT5;
	else
		*pf = DDS_PF_UNKNOWN;
}


/*!
DDSGetInfo()
extracts relevant info from a dds texture, returns 0 on success
*/
int32_t DDSGetInfo( ddsBuffer *dds, int32_t *width, int32_t *height, eDDSPixelFormat *pf )
{
	/* dummy test */
	if( dds == NULL )
		return -1;

	/* test dds header */
   if( *((int32_t*) dds->magic) != *((int32_t*) "DDS ") )
		return -1;
	if( DDSLittleLong( dds->size ) != 124 )
		return -1;

	/* extract width and height */
	if( width != NULL )
		*width = DDSLittleLong( dds->width );
	if( height != NULL )
		*height = DDSLittleLong( dds->height );

	/* get pixel format */
	DDSDecodePixelFormat( dds, pf );

	/* return ok */
	return 0;
}


/*!
	DDSGetColorBlockColors()
	extracts colors from a dds color block
*/
void DDSGetColorBlockColors( ddsColorBlock *block, ddsColor colors[ 4 ] )
{
   uint16_t		word;


	/* color 0 */
	word = DDSLittleShort( block->colors[ 0 ] );
	colors[ 0 ].a = 0xff;

	/* extract rgb bits */
   colors[ 0 ].b = (uint8_t) word;
	colors[ 0 ].b <<= 3;
	colors[ 0 ].b |= (colors[ 0 ].b >> 5);
	word >>= 5;
   colors[ 0 ].g = (uint8_t) word;
	colors[ 0 ].g <<= 2;
	colors[ 0 ].g |= (colors[ 0 ].g >> 5);
	word >>= 6;
   colors[ 0 ].r = (uint8_t) word;
	colors[ 0 ].r <<= 3;
	colors[ 0 ].r |= (colors[ 0 ].r >> 5);

	/* same for color 1 */
	word = DDSLittleShort( block->colors[ 1 ] );
	colors[ 1 ].a = 0xff;

	/* extract rgb bits */
   colors[ 1 ].b = (uint8_t) word;
	colors[ 1 ].b <<= 3;
	colors[ 1 ].b |= (colors[ 1 ].b >> 5);
	word >>= 5;
   colors[ 1 ].g = (uint8_t) word;
	colors[ 1 ].g <<= 2;
	colors[ 1 ].g |= (colors[ 1 ].g >> 5);
	word >>= 6;
   colors[ 1 ].r = (uint8_t) word;
	colors[ 1 ].r <<= 3;
	colors[ 1 ].r |= (colors[ 1 ].r >> 5);

	/* use this for all but the super-freak math method */
	if( block->colors[ 0 ] > block->colors[ 1 ] )
	{
		/* four-color block: derive the other two colors.
		00 = color 0, 01 = color 1, 10 = color 2, 11 = color 3
		these two bit codes correspond to the 2-bit fields
		stored in the 64-bit block. */

      word = ((uint16_t) colors[ 0 ].r * 2 + (uint16_t) colors[ 1 ].r ) / 3;
		/* no +1 for rounding */
		/* as bits have been shifted to 888 */
      colors[ 2 ].r = (uint8_t) word;
      word = ((uint16_t) colors[ 0 ].g * 2 + (uint16_t) colors[ 1 ].g) / 3;
      colors[ 2 ].g = (uint8_t) word;
      word = ((uint16_t) colors[ 0 ].b * 2 + (uint16_t) colors[ 1 ].b) / 3;
      colors[ 2 ].b = (uint8_t) word;
		colors[ 2 ].a = 0xff;

      word = ((uint16_t) colors[ 0 ].r + (uint16_t) colors[ 1 ].r * 2) / 3;
      colors[ 3 ].r = (uint8_t) word;
      word = ((uint16_t) colors[ 0 ].g + (uint16_t) colors[ 1 ].g * 2) / 3;
      colors[ 3 ].g = (uint8_t) word;
      word = ((uint16_t) colors[ 0 ].b + (uint16_t) colors[ 1 ].b * 2) / 3;
      colors[ 3 ].b = (uint8_t) word;
		colors[ 3 ].a = 0xff;
	}
	else
	{
		/* three-color block: derive the other color.
		00 = color 0, 01 = color 1, 10 = color 2,
		11 = transparent.
		These two bit codes correspond to the 2-bit fields
		stored in the 64-bit block */

      word = ((uint16_t) colors[ 0 ].r + (uint16_t) colors[ 1 ].r) / 2;
      colors[ 2 ].r = (uint8_t) word;
      word = ((uint16_t) colors[ 0 ].g + (uint16_t) colors[ 1 ].g) / 2;
      colors[ 2 ].g = (uint8_t) word;
      word = ((uint16_t) colors[ 0 ].b + (uint16_t) colors[ 1 ].b) / 2;
      colors[ 2 ].b = (uint8_t) word;
		colors[ 2 ].a = 0xff;

		/* random color to indicate alpha */
		colors[ 3 ].r = 0x00;
		colors[ 3 ].g = 0xff;
		colors[ 3 ].b = 0xff;
		colors[ 3 ].a = 0x00;
	}
}


/*
DDSDecodeColorBlock()
decodes a dds color block
fixme: make endian-safe
*/

void DDSDecodeColorBlock( uint32_t *pixel, ddsColorBlock *block, int32_t width, uint32_t colors[ 4 ] )
{
   int32_t				r, n;
   uint32_t	bits;
   uint32_t	masks[] = { 3, 12, 3 << 4, 3 << 6 };	/* bit masks = 00000011, 00001100, 00110000, 11000000 */
   int32_t				shift[] = { 0, 2, 4, 6 };


	/* r steps through lines in y */
   for( r = 0; r < 4; r++, pixel += (width - 4) )	/* no width * 4 as uint32_t ptr inc will * 4 */
	{
		/* width * 4 bytes per pixel per line, each j dxtc row is 4 lines of pixels */

		/* n steps through pixels */
		for( n = 0; n < 4; n++ )
		{
			bits = block->row[ r ] & masks[ n ];
			bits >>= shift[ n ];

			switch( bits )
			{
			case 0:
				*pixel = colors[ 0 ];
				pixel++;
				break;

			case 1:
				*pixel = colors[ 1 ];
				pixel++;
				break;

			case 2:
				*pixel = colors[ 2 ];
				pixel++;
				break;

			case 3:
				*pixel = colors[ 3 ];
				pixel++;
				break;

			default:
				/* invalid */
				pixel++;
				break;
			}
		}
	}
}


/*
DDSDecodeAlphaExplicit()
decodes a dds explicit alpha block
*/
void DDSDecodeAlphaExplicit( uint32_t *pixel, ddsAlphaBlockExplicit *alphaBlock, int32_t width, uint32_t alphaZero )
{
   int32_t				row, pix;
   uint16_t	word;
	ddsColor		color;


	/* clear color */
	color.r = 0;
	color.g = 0;
	color.b = 0;

	/* walk rows */
	for( row = 0; row < 4; row++, pixel += (width - 4) )
	{
		word = DDSLittleShort( alphaBlock->row[ row ] );

		/* walk pixels */
		for( pix = 0; pix < 4; pix++ )
		{
			/* zero the alpha bits of image pixel */
			*pixel &= alphaZero;
			color.a = word & 0x000F;
			color.a = color.a | (color.a << 4);
         *pixel |= *((uint32_t*) &color);
			word >>= 4;		/* move next bits to lowest 4 */
			pixel++;		/* move to next pixel in the row */
		}
	}
}



/*
DDSDecodeAlpha3BitLinear()
decodes interpolated alpha block
*/
void DDSDecodeAlpha3BitLinear( uint32_t *pixel, ddsAlphaBlock3BitLinear *alphaBlock, int32_t width, uint32_t alphaZero )
{

   int32_t row, pix;
   uint32_t stuff;
   uint8_t bits[ 4 ][ 4 ];
   uint16_t alphas[ 8 ];
	ddsColor aColors[ 4 ][ 4 ];

	/* get initial alphas */
	alphas[ 0 ] = alphaBlock->alpha0;
	alphas[ 1 ] = alphaBlock->alpha1;

	/* 8-alpha block */
	if( alphas[ 0 ] > alphas[ 1 ] )
	{
		/* 000 = alpha_0, 001 = alpha_1, others are interpolated */
		alphas[ 2 ] = ( 6 * alphas[ 0 ] +     alphas[ 1 ]) / 7;	/* bit code 010 */
		alphas[ 3 ] = ( 5 * alphas[ 0 ] + 2 * alphas[ 1 ]) / 7;	/* bit code 011 */
		alphas[ 4 ] = ( 4 * alphas[ 0 ] + 3 * alphas[ 1 ]) / 7;	/* bit code 100 */
		alphas[ 5 ] = ( 3 * alphas[ 0 ] + 4 * alphas[ 1 ]) / 7;	/* bit code 101 */
		alphas[ 6 ] = ( 2 * alphas[ 0 ] + 5 * alphas[ 1 ]) / 7;	/* bit code 110 */
		alphas[ 7 ] = (     alphas[ 0 ] + 6 * alphas[ 1 ]) / 7;	/* bit code 111 */
	}

	/* 6-alpha block */
	else
	{
		/* 000 = alpha_0, 001 = alpha_1, others are interpolated */
		alphas[ 2 ] = (4 * alphas[ 0 ] +     alphas[ 1 ]) / 5;	/* bit code 010 */
		alphas[ 3 ] = (3 * alphas[ 0 ] + 2 * alphas[ 1 ]) / 5;	/* bit code 011 */
		alphas[ 4 ] = (2 * alphas[ 0 ] + 3 * alphas[ 1 ]) / 5;	/* bit code 100 */
		alphas[ 5 ] = (    alphas[ 0 ] + 4 * alphas[ 1 ]) / 5;	/* bit code 101 */
		alphas[ 6 ] = 0;										/* bit code 110 */
		alphas[ 7 ] = 255;										/* bit code 111 */
	}

	/* decode 3-bit fields into array of 16 bytes with same value */

	/* first two rows of 4 pixels each */
   stuff = *((uint32_t*) &(alphaBlock->stuff[ 0 ]));

   bits[ 0 ][ 0 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 0 ][ 1 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 0 ][ 2 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 0 ][ 3 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 1 ][ 0 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 1 ][ 1 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 1 ][ 2 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 1 ][ 3 ] = (uint8_t) (stuff & 0x00000007);

	/* last two rows */
   stuff = *((uint32_t*) &(alphaBlock->stuff[ 3 ])); /* last 3 bytes */

   bits[ 2 ][ 0 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 2 ][ 1 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 2 ][ 2 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 2 ][ 3 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 3 ][ 0 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 3 ][ 1 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 3 ][ 2 ] = (uint8_t) (stuff & 0x00000007);
	stuff >>= 3;
   bits[ 3 ][ 3 ] = (uint8_t) (stuff & 0x00000007);

	/* decode the codes into alpha values */
	for( row = 0; row < 4; row++ )
	{
		for( pix=0; pix < 4; pix++ )
		{
			aColors[ row ][ pix ].r = 0;
			aColors[ row ][ pix ].g = 0;
			aColors[ row ][ pix ].b = 0;
         aColors[ row ][ pix ].a = (uint8_t) alphas[ bits[ row ][ pix ] ];
		}
	}

	/* write out alpha values to the image bits */
	for( row = 0; row < 4; row++, pixel += width-4 )
	{
		for( pix = 0; pix < 4; pix++ )
		{
			/* zero the alpha bits of image pixel */
			*pixel &= alphaZero;

			/* or the bits into the prev. nulled alpha */
         *pixel |= *((uint32_t*) &(aColors[ row ][ pix ]));
			pixel++;
		}
	}
}


/*
DDSDecompressDXT1()
decompresses a dxt1 format texture
*/
int32_t DDSDecompressDXT1( ddsBuffer *dds, int32_t width, int32_t height, uint8_t *pixels )
{
   int32_t x, y, xBlocks, yBlocks;
   uint32_t *pixel;
	ddsColorBlock *block;
	ddsColor colors[ 4 ];

	/* setup */
	xBlocks = width / 4;
	yBlocks = height / 4;

	/* walk y */
	for( y = 0; y < yBlocks; y++ )
	{
		/* 8 bytes per block */
		block = (ddsColorBlock*) (dds->data + y * xBlocks * 8);

		/* walk x */
		for( x = 0; x < xBlocks; x++, block++ )
		{
			DDSGetColorBlockColors( block, colors );
         pixel = (uint32_t*) (pixels + x * 16 + (y * 4) * width * 4);
         DDSDecodeColorBlock( pixel, block, width, (uint32_t*) colors );
		}
	}

	/* return ok */
	return 0;
}


/*
DDSDecompressDXT3()
decompresses a dxt3 format texture
*/

int32_t DDSDecompressDXT3( ddsBuffer *dds, int32_t width, int32_t height, uint8_t *pixels )
{
   int32_t x, y, xBlocks, yBlocks;
   uint32_t *pixel, alphaZero;
	ddsColorBlock *block;
	ddsAlphaBlockExplicit *alphaBlock;
	ddsColor colors[ 4 ];

	/* setup */
	xBlocks = width / 4;
	yBlocks = height / 4;

	/* create zero alpha */
	colors[ 0 ].a = 0;
	colors[ 0 ].r = 0xFF;
	colors[ 0 ].g = 0xFF;
	colors[ 0 ].b = 0xFF;
   alphaZero = *((uint32_t*) &colors[ 0 ]);

	/* walk y */
	for( y = 0; y < yBlocks; y++ )
	{
		/* 8 bytes per block, 1 block for alpha, 1 block for color */
		block = (ddsColorBlock*) (dds->data + y * xBlocks * 16);

		/* walk x */
		for( x = 0; x < xBlocks; x++, block++ )
		{
			/* get alpha block */
			alphaBlock = (ddsAlphaBlockExplicit*) block;

			/* get color block */
			block++;
			DDSGetColorBlockColors( block, colors );

			/* decode color block */
         pixel = (uint32_t*) (pixels + x * 16 + (y * 4) * width * 4);
         DDSDecodeColorBlock( pixel, block, width, (uint32_t*) colors );

			/* overwrite alpha bits with alpha block */
			DDSDecodeAlphaExplicit( pixel, alphaBlock, width, alphaZero );
		}
	}

	/* return ok */
	return 0;
}


/*
DDSDecompressDXT5()
decompresses a dxt5 format texture
*/
int32_t DDSDecompressDXT5( ddsBuffer *dds, int32_t width, int32_t height, uint8_t *pixels )
{
   int32_t x, y, xBlocks, yBlocks;
   uint32_t *pixel, alphaZero;
	ddsColorBlock *block;
	ddsAlphaBlock3BitLinear *alphaBlock;
	ddsColor colors[ 4 ];

	/* setup */
	xBlocks = width / 4;
	yBlocks = height / 4;

	/* create zero alpha */
	colors[ 0 ].a = 0;
	colors[ 0 ].r = 0xFF;
	colors[ 0 ].g = 0xFF;
	colors[ 0 ].b = 0xFF;
   alphaZero = *((uint32_t*) &colors[ 0 ]);

	/* walk y */
	for( y = 0; y < yBlocks; y++ )
	{
		/* 8 bytes per block, 1 block for alpha, 1 block for color */
		block = (ddsColorBlock*) (dds->data + y * xBlocks * 16);

		/* walk x */
		for( x = 0; x < xBlocks; x++, block++ )
		{
			/* get alpha block */
			alphaBlock = (ddsAlphaBlock3BitLinear*) block;

			/* get color block */
			block++;
			DDSGetColorBlockColors( block, colors );

			/* decode color block */
         pixel = (uint32_t*) (pixels + x * 16 + (y * 4) * width * 4);
         DDSDecodeColorBlock( pixel, block, width, (uint32_t*) colors );

			/* overwrite alpha bits with alpha block */
			DDSDecodeAlpha3BitLinear( pixel, alphaBlock, width, alphaZero );
		}
	}

	/* return ok */
	return 0;
}


/*
DDSDecompressDXT2()
decompresses a dxt2 format texture (fixme: un-premultiply alpha)
*/
int32_t DDSDecompressDXT2( ddsBuffer *dds, int32_t width, int32_t height, uint8_t *pixels )
{
	/* decompress dxt3 first */
   const int32_t r = DDSDecompressDXT3( dds, width, height, pixels );

	/* return to sender */
	return r;
}


/*
DDSDecompressDXT4()
decompresses a dxt4 format texture (fixme: un-premultiply alpha)
*/
int32_t DDSDecompressDXT4( ddsBuffer *dds, int32_t width, int32_t height, uint8_t *pixels )
{
	/* decompress dxt5 first */
   const int32_t r = DDSDecompressDXT5( dds, width, height, pixels );

	/* return to sender */
	return r;
}


/*
DDSDecompressARGB8888()
decompresses an argb 8888 format texture
*/
int32_t DDSDecompressARGB8888( ddsBuffer *dds, int32_t width, int32_t height, uint8_t *pixels )
{
	/* setup */
   uint8_t* in = dds->data;
   uint8_t* out = pixels;

	/* walk y */
   for(int32_t y = 0; y < height; y++)
	{
		/* walk x */
      for(int32_t x = 0; x < width; x++)
		{
			*out++ = *in++;
			*out++ = *in++;
			*out++ = *in++;
			*out++ = *in++;
		}
	}

	/* return ok */
	return 0;
}


/*
DDSDecompress()
decompresses a dds texture into an rgba image buffer, returns 0 on success
*/
int32_t DDSDecompress( ddsBuffer *dds, uint8_t *pixels )
{
   int32_t width, height;
	eDDSPixelFormat pf;

	/* get dds info */
   int32_t r = DDSGetInfo( dds, &width, &height, &pf );
	if ( r )
		return r;

	/* decompress */
	switch( pf )
	{
	case DDS_PF_ARGB8888:
		/* fixme: support other [a]rgb formats */
		r = DDSDecompressARGB8888( dds, width, height, pixels );
		break;

	case DDS_PF_DXT1:
		r = DDSDecompressDXT1( dds, width, height, pixels );
		break;

	case DDS_PF_DXT2:
		r = DDSDecompressDXT2( dds, width, height, pixels );
		break;

	case DDS_PF_DXT3:
		r = DDSDecompressDXT3( dds, width, height, pixels );
		break;

	case DDS_PF_DXT4:
		r = DDSDecompressDXT4( dds, width, height, pixels );
		break;

	case DDS_PF_DXT5:
		r = DDSDecompressDXT5( dds, width, height, pixels );
		break;

	default:
	case DDS_PF_UNKNOWN:
		memset( pixels, 0xFF, width * height * 4 );
		r = -1;
		break;
	}

	/* return to sender */
	return r;
}

} // end anonymous namespace


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderDDS::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "dds" );
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderDDS::isALoadableFileFormat(io::IReadFile* file) const
{
	if (!file)
		return false;

	ddsBuffer header;
	file->read(&header, sizeof(header));

   int32_t width, height;
	eDDSPixelFormat pixelFormat;

	return (0 == DDSGetInfo( &header, &width, &height, &pixelFormat));
}


//! creates a surface from the file
IImage* CImageLoaderDDS::loadImage(io::IReadFile* file) const
{
   uint8_t *memFile = new uint8_t [ file->getSize() ];
	file->read ( memFile, file->getSize() );

	ddsBuffer *header = (ddsBuffer*) memFile;
	IImage* image = 0;
   int32_t width, height;
	eDDSPixelFormat pixelFormat;

	if ( 0 == DDSGetInfo( header, &width, &height, &pixelFormat) )
	{
      image = new CImage(ECF_A8R8G8B8, core::dimension2d<uint32_t>(width, height));

      if ( DDSDecompress( header, (uint8_t*) image->lock() ) == -1)
		{
			image->unlock();
			image->drop();
			image = 0;
		}
	}

	delete [] memFile;
	if ( image )
		image->unlock();

	return image;
}


//! creates a loader which is able to load dds images
IImageLoader* createImageLoaderDDS()
{
	return new CImageLoaderDDS();
}


} // end namespace video
} // end namespace irr

#endif


#endif
