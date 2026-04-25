// Copyright (C) 2019, Benjamin Hampe
#pragma once
#include <de/image/ImageConfig.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_WAL_ENABLED)

// ===========================================================================
class ImageReaderWAL : public IImageReader
// ===========================================================================
{
public:
    std::vector< std::string > getSupportedReadExtensions() const override { return { "wal" }; }

    bool isSupportedReadExtension( std::string const & ext ) const override { return ext == "wal"; }

    bool load( Image & img, const uint8_t* p, size_t n, const std::string& uri = "" ) override;
};

#endif // DE_IMAGE_READER_WAL_ENABLED

// ===========================================================================
// ===========================================================================
// ===========================================================================

#if defined(DE_IMAGE_WRITER_WAL_ENABLED)

// ===========================================================================
class ImageWriterWAL : public IImageWriter
// ===========================================================================
{
public:
    std::vector< std::string >
    getSupportedWriteExtensions() const override { return std::vector< std::string >{ "wal" }; }

    bool
    isSupportedWriteExtension( std::string const & ext ) const override { return ext == "wal"; }

    bool
    save( Image const & img, std::string const & uri, uint32_t quality = 0 ) override;
};

#endif // DE_IMAGE_WRITER_WAL_ENABLED

} // end namespace image.
} // end namespace de.

#if 0
// Copyright (C) 2004 Murphy McCauley
// Copyright (C) 2007-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
/*
 Thanks to:
 Max McGuire for his Flipcode article about WAL textures
 Nikolaus Gebhardt for the Irrlicht 3D engine
*/

#ifndef __C_IMAGE_LOADER_WAL_H_INCLUDED__
#define __C_IMAGE_LOADER_WAL_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "IImageLoader.h"

namespace irr
{
namespace video
{

#ifdef _IRR_COMPILE_WITH_LMP_LOADER_

// byte-align structures
#include "irrpack.h"

   struct SLMPHeader {
      u32	width;	// width
      u32	height;	// height
      // variably sized
   } PACK_STRUCT;

// Default alignment
#include "irrunpack.h"

//! An Irrlicht image loader for Quake1,2 engine lmp textures/palette
class CImageLoaderLMP : public irr::video::IImageLoader
{
public:
   virtual bool isALoadableFileExtension(const io::path& filename) const;
   virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;
   virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;
};

#endif

#ifdef _IRR_COMPILE_WITH_WAL_LOADER_

//! An Irrlicht image loader for quake2 wal engine textures
class CImageLoaderWAL : public irr::video::IImageLoader
{
public:
   virtual bool isALoadableFileExtension(const io::path& filename) const;
   virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;
   virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;
};

//! An Irrlicht image loader for Halflife 1 engine textures
class CImageLoaderWAL2 : public irr::video::IImageLoader
{
public:
   virtual bool isALoadableFileExtension(const io::path& filename) const;
   virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const;
   virtual irr::video::IImage* loadImage(irr::io::IReadFile* file) const;
};

// byte-align structures
#include "irrpack.h"

   // Halfelife wad3 type 67 file
   struct miptex_halflife
   {
      c8  name[16];
      u32 width, height;
      u32 mipmap[4];		// four mip maps stored
   } PACK_STRUCT;

   //quake2 texture
   struct miptex_quake2
   {
      c8 name[32];
      u32 width;
      u32 height;
      u32 mipmap[4];		// four mip maps stored
      c8  animname[32];	// next frame in animation chain
      s32 flags;
      s32 contents;
      s32 value;
   } PACK_STRUCT;

// Default alignment
#include "irrunpack.h"

#endif

}
}

#endif

#endif
