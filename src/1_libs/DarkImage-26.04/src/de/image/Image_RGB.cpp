#include <de/image/Image_RGB.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_RGB_ENABLED)

bool
ImageReaderRGB::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
{
    return false;
}

#endif // DE_IMAGE_READER_RGB_ENABLED

// ===========================================================================
// ===========================================================================
// ===========================================================================

#if defined(DE_IMAGE_WRITER_RGB_ENABLED)

#pragma pack(push, 1)
struct SGIHeader
{
    uint16_t magic;      // 0x01DA
    uint8_t  storage;    // 0 = uncompressed, 1 = RLE
    uint8_t  bpc;        // bytes per channel (1)
    uint16_t dimension;  // 3 = width/height/channels
    uint16_t xsize;      // width
    uint16_t ysize;      // height
    uint16_t zsize;      // channels (3=RGB, 4=RGBA)
    uint32_t min;        // min pixel value (0)
    uint32_t max;        // max pixel value (255)
    uint32_t dummy;      // unused
    char     name[80];   // image name (optional)
    uint32_t colormap;   // 0 = normal
    uint8_t  pad[404];   // pad to 512 bytes
};
#pragma pack(pop)


static void write_be16(File & file, uint16_t v)
{
    uint8_t b[2];
    b[0] = (uint8_t)(v >> 8);
    b[1] = (uint8_t)(v & 0xFF);
    file.write(b, 2);
}

static void write_be32(File & file, uint32_t v)
{
    uint8_t b[4];
    b[0] = (uint8_t)(v >> 24);
    b[1] = (uint8_t)(v >> 16);
    b[2] = (uint8_t)(v >> 8);
    b[3] = (uint8_t)(v & 0xFF);
    file.write(b, 4);
}

bool
ImageWriterRGB::save( Image const & img, std::string const & uri, uint32_t quality )
{
    if ((img.pixelFormat() != PixelFormat::R8G8B8) &&
        (img.pixelFormat() != PixelFormat::R8G8B8A8))
    {
        DE_ERROR("Only supports RGB24 and RGBA32 format, not ", img.pixelFormatStr())
        return false;
    }

    File file(uri, eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot open ",uri)
        return false;
    }

    const uint32_t width = img.w();
    const uint32_t height = img.h();
    const uint32_t channels = img.channelCount();

    SGIHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.magic     = 0x01DA;
    hdr.storage   = 0;          // uncompressed
    hdr.bpc       = 1;          // 1 byte per sample
    hdr.dimension = 3;          // width/height/channels
    hdr.xsize     = (uint16_t)width;
    hdr.ysize     = (uint16_t)height;
    hdr.zsize     = (uint16_t)channels;
    hdr.min       = 0;
    hdr.max       = 255;
    hdr.colormap  = 0;          // normal

    // Write header in big-endian where required
    write_be16(file, hdr.magic);
    file.write(&hdr.storage, 1);
    file.write(&hdr.bpc, 1);
    write_be16(file, hdr.dimension);
    write_be16(file, hdr.xsize);
    write_be16(file, hdr.ysize);
    write_be16(file, hdr.zsize);
    write_be32(file, hdr.min);
    write_be32(file, hdr.max);
    write_be32(file, hdr.dummy);
    file.write(hdr.name, sizeof(hdr.name));
    write_be32(file, hdr.colormap);
    file.write(hdr.pad, sizeof(hdr.pad)); // pad to 512 bytes

    //const int pixelsPerImage = width * height;
    const int stride = width * channels;

    const uint8_t* __restrict__ pixels = img.data();

    // Planar R, G, B, [A]
    for (int c = 0; c < channels; ++c)
    {
        for (int y = 0; y < height; ++y)
        {
            const uint8_t* __restrict__ row = pixels + y * stride;
            for (int x = 0; x < width; ++x)
            {
                uint8_t v = row[x * channels + c];
                file.write(&v, 1);
            }
        }
    }

    return true;
}

#endif // DE_IMAGE_WRITER_RGB_ENABLED

} // end namespace image.
} // end namespace de.




/*
 *
Here’s a minimal, correct SGI RGB writer that produces real .rgb / .rgba files IrfanView will open automatically.

It writes:

    SGI header (512 bytes, big‑endian)

    uncompressed, planar channels (R… G… B… [A…])

SGI RGB writer (RGB or RGBA, from interleaved input)
cpp

#include <stdint.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <string.h> // memset

#pragma pack(push, 1)
struct SGIHeader {
    uint16_t magic;      // 0x01DA
    uint8_t  storage;    // 0 = uncompressed, 1 = RLE
    uint8_t  bpc;        // bytes per channel (1)
    uint16_t dimension;  // 3 = width/height/channels
    uint16_t xsize;      // width
    uint16_t ysize;      // height
    uint16_t zsize;      // channels (3=RGB, 4=RGBA)
    uint32_t min;        // min pixel value (0)
    uint32_t max;        // max pixel value (255)
    uint32_t dummy;      // unused
    char     name[80];   // image name (optional)
    uint32_t colormap;   // 0 = normal
    uint8_t  pad[404];   // pad to 512 bytes
};
#pragma pack(pop)


bool save_sgi_rgb(const wchar_t* path,
                  const uint8_t* pixels, // interleaved RGB or RGBA
                  int width,
                  int height,
                  int channels) // 3 = RGB, 4 = RGBA
{
    if (!path || !pixels || width <= 0 || height <= 0)
        return false;
    if (channels != 3 && channels != 4)
        return false;

    int fd = _wopen(path,
                    _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY,
                    0666);
    if (fd < 0)
        return false;

    SGIHeader hdr;
    memset(&hdr, 0, sizeof(hdr));

    hdr.magic     = 0x01DA;
    hdr.storage   = 0;          // uncompressed
    hdr.bpc       = 1;          // 1 byte per sample
    hdr.dimension = 3;          // width/height/channels
    hdr.xsize     = (uint16_t)width;
    hdr.ysize     = (uint16_t)height;
    hdr.zsize     = (uint16_t)channels;
    hdr.min       = 0;
    hdr.max       = 255;
    hdr.colormap  = 0;          // normal

    // Write header in big-endian where required
    write_be16(fd, hdr.magic);
    _write(fd, &hdr.storage, 1);
    _write(fd, &hdr.bpc, 1);
    write_be16(fd, hdr.dimension);
    write_be16(fd, hdr.xsize);
    write_be16(fd, hdr.ysize);
    write_be16(fd, hdr.zsize);
    write_be32(fd, hdr.min);
    write_be32(fd, hdr.max);
    write_be32(fd, hdr.dummy);
    _write(fd, hdr.name, sizeof(hdr.name));
    write_be32(fd, hdr.colormap);
    _write(fd, hdr.pad, sizeof(hdr.pad)); // pad to 512 bytes

    const int pixelsPerImage = width * height;
    const int stride = width * channels;

    // Planar R, G, B, [A]
    for (int c = 0; c < channels; ++c)
    {
        for (int y = 0; y < height; ++y)
        {
            const uint8_t* row = pixels + y * stride;
            for (int x = 0; x < width; ++x)
            {
                uint8_t v = row[x * channels + c];
                _write(fd, &v, 1);
            }
        }
    }

    _close(fd);
    return true;
}

This produces proper SGI .rgb (3 channels) or .rgba (4 channels) files; IrfanView will open them directly by extension.

Here is the clean, minimal, correct SGI RGB writer you asked for, Benjamin — no fluff, no abstractions, no STL except for memset, and fully compatible with IrfanView, ImageMagick, and old IRIX tools.

This writes real SGI .rgb/.rgba files, not raw RGB dumps.
It produces:

    512‑byte SGI header (big‑endian fields)

    planar channel data (RRR… GGG… BBB… [AAA…])

    uncompressed storage (storage=0)

    1 byte per channel (bpc=1)

    dimension=3 (width/height/channels)

This is the exact format IrfanView expects when opening .rgb by extension.
🟥 SGI RGB Writer (RGB or RGBA, uncompressed, planar)
cpp

#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#pragma pack(push, 1)
struct SGIHeader {
    uint16_t magic;      // 0x01DA (big-endian)
    uint8_t  storage;    // 0 = uncompressed
    uint8_t  bpc;        // bytes per channel = 1
    uint16_t dimension;  // 3 = width/height/channels
    uint16_t xsize;      // width
    uint16_t ysize;      // height
    uint16_t zsize;      // channels (3 or 4)
    uint32_t min;        // 0
    uint32_t max;        // 255
    uint32_t dummy;      // unused
    char     name[80];   // optional image name
    uint32_t colormap;   // 0 = normal
    uint8_t  pad[404];   // pad to 512 bytes
};
#pragma pack(pop)

static void be16(int fd, uint16_t v)
{
    uint8_t b[2] = { uint8_t(v >> 8), uint8_t(v & 0xFF) };
    _write(fd, b, 2);
}

static void be32(int fd, uint32_t v)
{
    uint8_t b[4] = {
        uint8_t(v >> 24),
        uint8_t(v >> 16),
        uint8_t(v >> 8),
        uint8_t(v & 0xFF)
    };
    _write(fd, b, 4);
}

bool save_sgi_rgb(const wchar_t* path,
                  const uint8_t* interleaved, // RGB or RGBA
                  int width,
                  int height,
                  int channels) // 3 or 4
{
    if (!path || !interleaved || width <= 0 || height <= 0)
        return false;
    if (channels != 3 && channels != 4)
        return false;

    int fd = _wopen(path,
                    _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY,
                    0666);
    if (fd < 0)
        return false;

    SGIHeader h;
    memset(&h, 0, sizeof(h));

    h.magic     = 0x01DA;
    h.storage   = 0;
    h.bpc       = 1;
    h.dimension = 3;
    h.xsize     = (uint16_t)width;
    h.ysize     = (uint16_t)height;
    h.zsize     = (uint16_t)channels;
    h.min       = 0;
    h.max       = 255;
    h.colormap  = 0;

    // --- Write header (big-endian fields) ---
    be16(fd, h.magic);
    _write(fd, &h.storage, 1);
    _write(fd, &h.bpc, 1);
    be16(fd, h.dimension);
    be16(fd, h.xsize);
    be16(fd, h.ysize);
    be16(fd, h.zsize);
    be32(fd, h.min);
    be32(fd, h.max);
    be32(fd, h.dummy);
    _write(fd, h.name, sizeof(h.name));
    be32(fd, h.colormap);
    _write(fd, h.pad, sizeof(h.pad));

    // --- Write planar channel data ---
    const int stride = width * channels;

    for (int c = 0; c < channels; c++)
    {
        for (int y = 0; y < height; y++)
        {
            const uint8_t* row = interleaved + y * stride;
            for (int x = 0; x < width; x++)
            {
                uint8_t v = row[x * channels + c];
                _write(fd, &v, 1);
            }
        }
    }

    _close(fd);
    return true;
}

🧠 What this writer produces
Header (512 bytes)

    Big‑endian magic 0x01DA

    Uncompressed storage

    1 byte per channel

    Dimension=3

    Width/height/channels

    No colormap

    Zero‑padded to 512 bytes

Pixel data (planar)
Code

RRRRRRR...
GGGGGGG...
BBBBBBB...
AAAAAAA... (if RGBA)

This is exactly what SGI .rgb files are.
🟦 Why IrfanView opens these automatically

Because .rgb is treated as SGI RGB, not raw RGB.
Your raw .rgb files failed because they lacked:

    magic number

    header

    planar layout

This writer fixes all of that.

If you want, I can also generate:

    SGI RLE‑compressed writer

    SGI loader

    RGB(A) → SGI planar converter

    SGI → raw RGB converter
*/
























#if 0

//! Copyright (C) 2009-2012 Gary Conway
//! This file is part of the "Irrlicht Engine".
//! For conditions of distribution and use, see copyright notice in irrlicht.h

/*
    Author:	Gary Conway (Viper) - co-author of the ZIP file format, Feb 1989,
                         see the story at http://www.idcnet.us/ziphistory.html
    Website:	http://idcnet.us
    Email:		codeslinger@vipergc.com
    Created:	March 1, 2009
    Version:	1.0
    Updated:

    This module will load SGI .rgb files (along with the other extensions). The module complies
    with version 1.0 of the SGI Image File Format by Paul Haeberli of Silicon Graphics Computer Systems
    The module handles BW, RGB and RGBA images.

    RGB images are stored with either 8 bits per COLOR VALUE, one each for red,green,blue (24bpp)
    or 16 bits per COLOR VALUE, again one each for red,green,blue  (48 bpp), not including the alpha channel


    OPTIONS NOT SUPPORTED

    1.	16 bit COLOR VALUES (48bpp modes)
    2.	COLORMAP = DITHERED mode



For non- run length encoded files, this is the structure

    The Header
    The Image Data

If the image is run length encoded, this is the structure:
    The Header
    The Offset Tables
    The Image Data

The Header consists of the following:

        Size  | Type   | Name      | Description

      2 bytes | short  | MAGIC     | IRIS image file magic number
      1 byte  | char   | STORAGE   | Storage format
      1 byte  | char   | BPC       | Number of bytes per pixel channel
      2 bytes | ushort | DIMENSION | Number of dimensions
      2 bytes | ushort | XSIZE     | X size in pixels
      2 bytes | ushort | YSIZE     | Y size in pixels
      2 bytes | ushort | ZSIZE     | Number of channels
      4 bytes | long   | PIXMIN    | Minimum pixel value
      4 bytes | long   | PIXMAX    | Maximum pixel value
      4 bytes | char   | DUMMY     | Ignored
     80 bytes | char   | IMAGENAME | Image name
      4 bytes | long   | COLORMAP  | Colormap ID
    404 bytes | char   | DUMMY     | Ignored

Here is a description of each field in the image file Header:

MAGIC - This is the decimal value 474 saved as a short. This identifies the file as an SGI image file.

STORAGE -	specifies whether the image is stored using run length encoding (RLE) or not (VERBATIM).
            If RLE is used, the value of this byte will be 1. Otherwise the value of this byte will
            be 0. The only allowed values for this field are 0 or 1.

BPC -		describes the precision that is used to store each channel of an image. This is the number of
            bytes per pixel component. The majority of SGI image files use 1 byte per pixel component,
            giving 256 levels. Some SGI image files use 2 bytes per component. The only allowed values
            for this field are 1 or 2.

DIMENSION - described the number of dimensions in the data stored in the image file.
            The only allowed values are 1, 2, or 3. If this value is 1, the image file
            consists of only 1 channel and only 1 scanline (row). The length of this
            scanline is given by the value of XSIZE below. If this value is 2, the file
            consists of a single channel with a number of scanlines. The width and height
            of the image are given by the values of XSIZE and YSIZE below.
            If this value is 3, the file consists of a number of channels.
            The width and height of the image are given by the values of XSIZE and YSIZE below.
            The number of channels is given by the value of ZSIZE below.

XSIZE -		The width of the image in pixels

YSIZE -		The height of the image in pixels

ZSIZE -		The number of channels in the image. B/W (greyscale) images are stored as 2 dimensional
            images with a ZSIZE of 1. RGB color images are stored as 3 dimensional images with a
            ZSIZE of 3. An RGB image with an ALPHA channel is stored as a 3 dimensional image with
            a ZSIZE of 4. There are no inherent limitations in the SGI image file format that would
            preclude the creation of image files with more than 4 channels.

PINMIN -	The minimum pixel value in the image. The value of 0 may be used if no pixel has a value
            that is smaller than 0.

PINMAX -	The maximum pixel value in the image. The value of 255 may be used if no pixel has a
            value that is greater than 255. This is the value that is considered to be full
            brightness in the image.

DUMMY -		This 4 bytes of data should be set to 0.

IMAGENAME - An null terminated ascii string of up to 79 characters terminated by a null may be
            included here. This is not commonly used.

COLORMAP -	This controls how the pixel values in the file should be interpreted. It can have one
            of these four values:

0: NORMAL - The data in the channels represent B/W values for images with 1 channel, RGB values
            for images with 3 channels, and RGBA values for images with 4 channels. Almost all
            the SGI image files are of this type.

1: DITHERED - The image will have only 1 channel of data. For each pixel, RGB data is packed
            into one 8 bit value. 3 bits are used for red and green, while blue uses 2 bits.
            Red data is found in bits[2..0], green data in bits[5..3], and blue data in
            bits[7..6]. This format is obsolete.

2: SCREEN - The image will have only 1 channel of data. This format was used to store
            color-indexed pixels. To convert the pixel values into RGB values a colormap
            must be used. The appropriate color map varies from image to image. This format is obsolete.

3: COLORMAP - The image is used to store a color map from an SGI machine. In this case the
            image is not displayable in the conventional sense.

DUMMY -		This 404 bytes of data should be set to 0. This makes the Header exactly 512 bytes.
*/

#include "CImageLoaderRGB.h"

#ifdef _IRR_COMPILE_WITH_RGB_LOADER_

#include "IReadFile.h"
#include "SColor.h"
#include "CColorConverter.h"
#include "CImage.h"
#include "os.h"
#include "irrString.h"


namespace irr
{
namespace video
{

//! constructor
CImageLoaderRGB::CImageLoaderRGB()
{
    #ifdef _DEBUG
    setDebugName("CImageLoaderRGB");
    #endif
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extensions listed here
bool CImageLoaderRGB::isALoadableFileExtension(const io::path& filename) const
{
    return core::hasFileExtension( filename, "rgb", "rgba", "sgi" ) ||
           core::hasFileExtension( filename, "int", "inta", "bw" );
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderRGB::isALoadableFileFormat(io::IReadFile* file) const
{
    rgbStruct rgb;
    return checkFormat(file, rgb);
}


/** The main entry point, read and format the image file.
\return Pointer to the image data on success
                null pointer on fail */
IImage* CImageLoaderRGB::loadImage(io::IReadFile* file) const
{
    IImage* image = 0;
    s32* paletteData = 0;

    rgbStruct rgb;   // construct our structure for holding data

    // read Header information
    if (checkFormat(file, rgb))
    {
        // 16 bits per COLOR VALUE, not supported, this is 48bpp mode
        if (rgb.Header.BPC != 1)
        {
            os::Printer::log("Only one byte per pixel RGB files are supported", file->getFileName(), ELL_ERROR);
        }
        else if (rgb.Header.Colormap != 0)
        {
            os::Printer::log("Dithered, Screen and Colormap RGB files are not supported", file->getFileName(), ELL_ERROR);
        }
        else if (rgb.Header.Storage == 1 && !readOffsetTables(file, rgb))
        {
            os::Printer::log("Failed to read RLE table in RGB file", file->getFileName(), ELL_ERROR);
        }
        else if (!rgb.allocateTemps())
        {
            os::Printer::log("Out of memory in RGB file loader", file->getFileName(), ELL_ERROR);
        }
        else
        {
            // read and process the file to rgbData
            processFile(file, rgb);

/*
          ZSIZE		Description
            1		BW (grayscale) image
            3		RGB image
            4		RGBa image with one alpha channel

            When the Alpha channel is present, I am not sure with RGB files if
            it's a precomputed RGB color or it needs to be completely calculated. My guess
            would be that it's not precomputed for two reasons.

            1. the loss of precision when calculating the fraction, then storing the result as an int
            2. the loss of the original color data when the image might be composited with another. Yes
                the original color data could be computed, however, not without another loss in precision

            Also, I don't know where to find the background color
            Pixmin and Pixmax are apparently the min and max alpha blend values (0-100%)

            Complete Alpha blending computation
            The actual resulting merged color is computed this way:
            (image color ◊ alpha) + (background color ◊ (100% - alpha)).

            Using precomputed blending
            (image color) + (background color ◊ (100% - alpha)).

            Alternatively, the RGB files could use another blending technique entirely
*/

            switch (rgb.Header.Zsize)
            {
            case 1:
                // BW (grayscale) image
                paletteData = new s32[256];
                for (int n=0; n<256; n++)
                    paletteData[n] = n;

                image = new CImage(ECF_A1R5G5B5, core::dimension2d<u32>(rgb.Header.Xsize, rgb.Header.Ysize));
                if (image)
                    CColorConverter::convert8BitTo16Bit(rgb.rgbData, (s16*)image->lock(), rgb.Header.Xsize, rgb.Header.Ysize, paletteData, 0, true);
                break;
            case 3:
                // RGB image
                // one byte per COLOR VALUE, eg, 24bpp
                image = new CImage(ECF_R8G8B8, core::dimension2d<u32>(rgb.Header.Xsize, rgb.Header.Ysize));
                if (image)
                    CColorConverter::convert24BitTo24Bit(rgb.rgbData, (u8*)image->lock(), rgb.Header.Xsize, rgb.Header.Ysize, 0, true, false);
                break;
            case 4:
                // RGBa image with one alpha channel (32bpp)
                // image is stored in rgbData as RGBA

                converttoARGB(reinterpret_cast<u32*>(rgb.rgbData), 	rgb.Header.Ysize * rgb.Header.Xsize);

                image = new CImage(ECF_A8R8G8B8, core::dimension2d<u32>(rgb.Header.Xsize, rgb.Header.Ysize));
                if (image)
                    CColorConverter::convert32BitTo32Bit((s32*)rgb.rgbData, (s32*)image->lock(), rgb.Header.Xsize, rgb.Header.Ysize, 0, true);

                break;
            default:
                // Format unknown
                os::Printer::log("Unsupported pixel format in RGB file", file->getFileName(), ELL_ERROR);
            }

            if (image)
                image->unlock();
        }
    }

    // and tidy up allocated memory
    delete [] paletteData;

    return image;
}

// returns true on success
bool CImageLoaderRGB::readHeader(io::IReadFile* file, rgbStruct& rgb) const
{
    if ( file->read(&rgb.Header, sizeof(rgb.Header)) < s32(sizeof(rgb.Header)) )
        return false;

    // test for INTEL or BIG ENDIAN processor
    // if INTEL, then swap the byte order on 16 bit INT's to make them BIG ENDIAN
    // because that is the native format for the .rgb file
#ifndef __BIG_ENDIAN__
    rgb.Header.Magic     = os::Byteswap::byteswap(rgb.Header.Magic);
    rgb.Header.Storage   = os::Byteswap::byteswap(rgb.Header.Storage);
    rgb.Header.Dimension = os::Byteswap::byteswap(rgb.Header.Dimension);
    rgb.Header.Xsize     = os::Byteswap::byteswap(rgb.Header.Xsize);
    rgb.Header.Ysize     = os::Byteswap::byteswap(rgb.Header.Ysize);
    rgb.Header.Zsize     = os::Byteswap::byteswap(rgb.Header.Zsize);
    rgb.Header.Pixmin    = os::Byteswap::byteswap(rgb.Header.Pixmin);
    rgb.Header.Pixmax    = os::Byteswap::byteswap(rgb.Header.Pixmax);
    rgb.Header.Colormap  = os::Byteswap::byteswap(rgb.Header.Colormap);
#endif

    // calculate the size of the buffer needed: XSIZE * YSIZE * ZSIZE * BPC
    rgb.ImageSize = (rgb.Header.Xsize)*(rgb.Header.Ysize)*(rgb.Header.Zsize)*(rgb.Header.BPC);

    return true;
}


bool CImageLoaderRGB::checkFormat(io::IReadFile* file, rgbStruct& rgb) const
{
    if (!readHeader(file, rgb))
        return false;

    return (rgb.Header.Magic == 0x1DA);
}

/*
If the image is stored using run length encoding, offset tables follow the Header that
describe what the file offsets are to the RLE for each scanline. This information only
applies if the value for STORAGE above is 1.

         Size  | Type   | Name      | Description

  tablen longs | long   | STARTTAB  | Start table
  tablen longs | long   | LENGTHTAB | Length table

One entry in each table is needed for each scanline of RLE data. The total number of scanlines in the image (tablen) is determined by the product of the YSIZE and ZSIZE. There are two tables of longs that are written. Each consists of tablen longs of data. The first table has the file offsets to the RLE data for each scanline in the image. In a file with more than 1 channel (ZSIZE > 1) this table first has all the offsets for the scanlines in the first channel, followed be offsets for the scanlines in the second channel, etc. The second table has the RLE data length for each scanline in the image. In a file with more than 1 channel (ZSIZE > 1) this table first has all the RLE data lengths for the scanlines in the first channel, followed be RLE data lengths for the scanlines in the second channel, etc.

To find the the file offset, and the number of bytes in the RLE data for a particular scanline, these
two arrays may be read in and indexed as follows:

To read in the tables:

    unsigned long *starttab, *lengthtab;

    tablen = YSIZE*ZSIZE*sizeof(long);
    starttab = (unsigned long *)mymalloc(tablen);
    lengthtab = (unsigned long *)mymalloc(tablen);
    fseek(rgb->inf,512,SEEK_SET);
    readlongtab(rgb->inf,starttab);
    readlongtab(rgb->inf,lengthtab);

To find the file offset and RLE data length for a scanline:

rowno is an integer in the range 0 to YSIZE-1 channo is an integer in the range 0 to ZSIZE-1

    rleoffset = starttab[rowno+channo*YSIZE]
    rlelength = lengthtab[rowno+channo*YSIZE]

It is possible for two identical rows (scanlines) to share compressed data. A completely
white image could be written as a single compressed row and having all table entries point
to that row. Another little hack that should work is if you are writing out a RGB RLE file,
and a particular scanline is achromatic (greyscale), you could just make the r, g and b rows
point to the same data!!

  RETURNS:	on success true, else returns false
*/

bool CImageLoaderRGB::readOffsetTables(io::IReadFile* file, rgbStruct& rgb) const
{
    rgb.TableLen = rgb.Header.Ysize * rgb.Header.Zsize ; // calc size of tables

    // return error if unable to allocate tables
    rgb.StartTable = new u32[rgb.TableLen];
    if (!rgb.StartTable)
        return false;
    rgb.LengthTable = new u32[rgb.TableLen];
    if (!rgb.LengthTable)
        return false;

    file->seek(512);
    file->read(rgb.StartTable, rgb.TableLen* sizeof(u32));
    file->read(rgb.LengthTable, rgb.TableLen* sizeof(u32));

    // if we are on an INTEL platform, swap the bytes
#ifndef __BIG_ENDIAN__
    const u32 length = rgb.TableLen;
    for (u32 i=0; i<length; ++i)
    {
        rgb.StartTable[i] = os::Byteswap::byteswap(rgb.StartTable[i]);
        rgb.LengthTable[i] = os::Byteswap::byteswap(rgb.LengthTable[i]);
    }
#endif

    return true;
}


/*
    The Header has already been read into rgb structure
    The Tables have been read if necessary
    Now process the actual data
*/
void CImageLoaderRGB::processFile(io::IReadFile* file, rgbStruct& rgb) const
{
    u16 *tempShort;

    // calculate the size of the buffer needed: XSIZE * YSIZE * ZSIZE * BPC
    rgb.rgbData = new u8 [(rgb.Header.Xsize)*(rgb.Header.Ysize)*(rgb.Header.Zsize)*(rgb.Header.BPC)];
    u8 *ptr = rgb.rgbData;

    // cycle through all scanlines

#ifdef _IRR_RGB_FILE_INVERTED_IMAGE_
    // preserve the image as stored, eg, inverted
    for (u16 i = 0; i < rgb.Header.Ysize; ++i)
#else
    // invert the image to make it upright
    for (s32 i = (s32)(rgb.Header.Ysize)-1; i>=0; --i)
#endif
    {
        // check the number of channels and read a row of data
        if (rgb.Header.Zsize >= 1)
            readRGBrow( rgb.tmpR, i, 0, file, rgb);
        if (rgb.Header.Zsize >= 2)
            readRGBrow( rgb.tmpG, i, 1, file, rgb);
        if (rgb.Header.Zsize >= 3)
            readRGBrow( rgb.tmpB, i, 2, file, rgb);
        if (rgb.Header.Zsize >= 4)
            readRGBrow( rgb.tmpA, i, 3, file, rgb);

        // cycle thru all values for this row
        for (u16 j = 0; j < rgb.Header.Xsize; ++j)
        {
            if(rgb.Header.BPC == 1)
            {
                // ONE byte per color
                if (rgb.Header.Zsize >= 1)
                    *ptr++ = rgb.tmpR[j];
                if (rgb.Header.Zsize >= 2)
                    *ptr++ = rgb.tmpG[j];
                if (rgb.Header.Zsize >= 3)
                    *ptr++ = rgb.tmpB[j];
                if (rgb.Header.Zsize >= 4)
                    *ptr++ = rgb.tmpA[j];
            }
            else
            {
                // TWO bytes per color
                if( rgb.Header.Zsize >= 1 )
                {
                    // two bytes of color data
                    tempShort  = (u16 *) (ptr);
                    *tempShort = *(  (u16 *) (rgb.tmpR) + j);
                    tempShort++;
                    ptr = ( u8 *)(tempShort);
                }
                if( rgb.Header.Zsize >= 2 )
                {
                    tempShort  = ( u16 *) (ptr);
                    *tempShort = *( ( u16 *) (rgb.tmpG) + j);
                    tempShort++;
                    ptr = ( u8 *) (tempShort);
                }
                if( rgb.Header.Zsize >= 3 )
                {
                    tempShort  = ( u16 *) (ptr);
                    *tempShort = *( ( u16 *) (rgb.tmpB) + j);
                    tempShort++;
                    ptr = ( u8 *)(tempShort);
                }
                if( rgb.Header.Zsize >= 4 )
                {
                    tempShort  = ( u16 *) (ptr);
                    *tempShort = *( ( u16 *) (rgb.tmpA) + j);
                    tempShort++;
                    ptr = ( u8 *)(tempShort);
                }
            } // end if(rgb.Header.BPC == 1)
        } // end for
    } // end for
}


/*
    This information only applies if the value for STORAGE is 1. If the image is
    stored using run length encoding, the image data follows the offset/length tables.
    The RLE data is not in any particular order. The offset tables are used to
    locate the rle data for any scanline.

    The RLE data must be read in from the file and expanded into pixel data in the following manner:

    If BPC is 1, then there is one byte per pixel. In this case the RLE data should be
    read into an array of chars. To expand data, the low order seven bits of the first
    byte: bits[6..0] are used to form a count. If the high order bit of the first byte
    is 1: bit[7], then the count is used to specify how many bytes to copy from the RLE
    data buffer to the destination. Otherwise, if the high order bit of the first byte
    is 0: bit[7], then the count is used to specify how many times to repeat the value
    of the following byte, in the destination. This process continues until a count
    of 0 is found. This should decompress exactly XSIZE pixels.


    One entry in each table is needed for each scanline of RLE data. The total number of
    scanlines in the image (tablen) is determined by the product of the YSIZE and ZSIZE.
    There are two tables of longs that are written. Each consists of tablen longs of data.
    The first table has the file offsets to the RLE data for each scanline in the image. In
    a file with more than 1 channel (ZSIZE > 1) this table first has all the offsets for the
    scanlines in the first channel, followed be offsets for the scanlines in the second
    channel, etc. The second table has the RLE data length for each scanline in the image.
    In a file with more than 1 channel (ZSIZE > 1) this table first has all the RLE data
    lengths for the scanlines in the first channel, followed be RLE data lengths for the
    scanlines in the second channel, etc.

    Return a row of data, expanding RLE compression if necessary
*/
void CImageLoaderRGB::readRGBrow(u8 *buf, int y, int z, io::IReadFile* file, rgbStruct& rgb) const
{
    if (rgb.Header.Storage != 1)
    {
        // stored VERBATIM

        file->seek(512+(y*rgb.Header.Xsize * rgb.Header.BPC)+(z* rgb.Header.Xsize * rgb.Header.Ysize * rgb.Header.BPC));
        file->read(buf, rgb.Header.Xsize * rgb.Header.BPC);

#ifndef __BIG_ENDIAN__
        if (rgb.Header.BPC != 1)
        {
            u16* tmpbuf = reinterpret_cast<u16*>(buf);
            for (u16 i=0; i<rgb.Header.Xsize; ++i)
                tmpbuf[i] = os::Byteswap::byteswap(tmpbuf[i]);
        }
#endif
        return;
    }

    // the file is stored as Run Length Encoding (RLE)
    // each sequence is stored as 0x80  NumRepeats ByteToRepeat

    // get the file offset from StartTable and SEEK
    // then read the data

    file->seek((long) rgb.StartTable[y+z * rgb.Header.Ysize]);
    file->read(rgb.tmp, rgb.LengthTable[y+z * rgb.Header.Ysize]);

    // rgb.tmp has the data

    u16 pixel;
    u16 *tempShort;
    u8* iPtr = rgb.tmp;
    u8* oPtr = buf;
    while (true)
    {
        // if BPC = 1, then one byte per pixel
        if (rgb.Header.BPC == 1)
        {
            pixel = *iPtr++;
        }
        else
        {
            // BPC = 2, so two bytes per pixel
            tempShort = (u16 *)  iPtr;
            pixel = *tempShort;
            tempShort++;
            iPtr = (u8 *) tempShort;
        }

#ifndef __BIG_ENDIAN__
        if (rgb.Header.BPC != 1)
            pixel = os::Byteswap::byteswap(pixel);
#endif

        s32 count = (s32)(pixel & 0x7F);

        // limit the count value to the remaining row size
        if (oPtr + count*rgb.Header.BPC > buf + rgb.Header.Xsize * rgb.Header.BPC)
        {
            count = ( (buf + rgb.Header.Xsize * rgb.Header.BPC) - oPtr ) / rgb.Header.BPC;
        }

        if (count<=0)
            break;
        else if (pixel & 0x80)
        {
            // repeat the byte pointed to by iPtr, count times
            while (count--)
            {
                if(rgb.Header.BPC == 1)
                {
                    *oPtr++ = *iPtr++;
                }
                else
                {
                    // write pixel from iPtr to oPtr, move both two bytes ahead
                    tempShort = (u16 *) (iPtr);
                    pixel = *tempShort;
                    tempShort++;
                    iPtr = (u8 *) (tempShort);
#ifndef __BIG_ENDIAN__
                    pixel = os::Byteswap::byteswap(pixel);
#endif
                    tempShort = (u16 *) (oPtr);
                    *tempShort = pixel;
                    tempShort++;
                    oPtr = (u8 *) (tempShort);
                }
            }
        }
        else
        {
            if (rgb.Header.BPC == 1)
            {
                pixel = *iPtr++;
            }
            else
            {
                tempShort = (u16 *) (iPtr);
                pixel = *tempShort;
                tempShort++;
                iPtr = (u8 *) (tempShort);
            }

#ifndef __BIG_ENDIAN__
            if (rgb.Header.BPC != 1)
                pixel = os::Byteswap::byteswap(pixel);
#endif

            while (count--)
            {
                if(rgb.Header.BPC == 1)
                {
                    *oPtr++ = (u8) pixel;
                }
                else
                {
                    tempShort  = (u16 *) (oPtr);
                    *tempShort = pixel;
                    tempShort++;
                    oPtr = (u8 *) (tempShort);
                }
            }
        } // else if (pixel & 0x80)
    } // while (true)
}


// we have 1 byte per COLOR VALUE, eg 24bpp and 1 alpha channel
// color values are stored as RGBA, convert to ARGB
// todo: replace with CColorConverter method
void CImageLoaderRGB::converttoARGB(u32* in, const u32 size) const
{
    for (u32 x=0; x < size; ++x)
    {
        *in=(*in>>8)|(*in<<24);
        ++in;
    }
}


//! creates a loader which is able to load SGI RGB images
IImageLoader* createImageLoaderRGB()
{
    return new CImageLoaderRGB;
}


} // end namespace video
} // end namespace irr

#endif


#endif
