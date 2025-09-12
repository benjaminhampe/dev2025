#pragma once
#include <de/Binary.hpp>
#include <de/ImageReader.hpp>
#include <de/ImageWriter.hpp>

#if defined(DE_IMAGE_READER_TGA_ENABLED) || defined(DE_IMAGE_WRITER_TGA_ENABLED)

namespace de {
namespace image {
namespace tga {

#pragma pack( push )
#pragma pack( 1 )

// TGA ImageType
#define TGA_NULL		0	//! no image data included
#define TGA_CMAP		1	//! uncompressed, color-mapped image
#define TGA_RGB      2	//! uncompressed, true-color image
#define TGA_MONO		3	//! uncompressed, black-and-white image
#define TGA_RLECMAP  9	//! run-length encoded, color-mapped image
#define TGA_RLERGB   10	//! run-length encoded, true-color image
#define TGA_RLEMONO  11	//! run-length encoded, black-and-white image
#define TGA_CMPCMAP  32	//! compressed (Huffman/Delta/RLE) color-mapped image (e.g., VDA/D) - Obsolete
#define TGA_CMPCMAP4	33	//! compressed (Huffman/Delta/RLE) color-mapped four pass image (e.g., VDA/D) - Obsolete

enum class TGAType
{
   Null = 0,
   ColorMap = 1,
   RGB = 2,
   Mono = 3,
   RLE_ColorMap = 9,
   RLE_RGB = 10,
   RLE_Mono = 11,
   CMP_ColorMap = 32,
   CMP_ColorMap4Pass = 33
};

struct TGAHeader
{
   uint8_t idLength;       //! BYTE id_length; length of the image ID field
   uint8_t cm_type;        //! BYTE color_map_type; whether a color map is included
   uint8_t imageType;      //! BYTE image_type;	compression and color types
   uint16_t cm_tableOffset;//! WORD cm_first_entry - first entry index (offset into the color map table)
   uint16_t cm_entryCount; //! WORD cm_length - color map length (number of entries)
   uint8_t cm_entrySize;//! BYTE cm_size - color map entry size, in bits (number of bits per pixel)
   uint16_t xOrigin;       //! WORD X-origin of image (abs lower left for displays where origin is at the lower left)
   uint16_t yOrigin;    	//! WORD Y-origin of image (as for X-origin)
   uint16_t w;             //! WORD image width
   uint16_t h;             //! WORD image height
   uint8_t bitsPerPixel;	//! BYTE bits per pixel
   uint8_t imageDescriptor;//! BYTE image descriptor, bits 3-0 give the alpha channel depth, bits 5-4 give direction
//   DE_DEBUG( "TGA.sample_fmt = ",sample_fmt )
//   DE_DEBUG( "TGA.sample_fmt == SAMPLEFORMAT_IEEEFP = ", SAMPLEFORMAT_IEEEFP == sample_fmt )
//   DE_DEBUG( "TGA.compression = ", compression )
//   DE_DEBUG( "TGA.planar_config = ", planar_config )

   uint32_t getWidth() const { return int32_t( w ); }
   uint32_t getHeight() const { return int32_t( h ); }
   size_t getBytesPerPixel() const { return size_t( bitsPerPixel ) / 8; }
//   size_t getPixelCount() const { return size_t( w ) * size_t( h ); }
//   size_t getByteCount() const { return getPixelCount() * getBytesPerPixel(); }
//   size_t getBitsPerPixel() const { return size_t( bitsPerPixel ); }

//   constexpr static const uint8_t DeCompressed[12] =
//      {0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
//   constexpr static const uint8_t IsCompressed[12] =
//      {0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

//   bool isRaw = (std::memcmp( DeCompressed, &header, sizeof(DeCompressed) ) == 0);
//   bool isCompressed = (std::memcmp( IsCompressed, &header, sizeof(IsCompressed) ) == 0);


   DE_CREATE_LOGGER("de.image.TGA.Header")
   void dump()
   {
      DE_DEBUG( "TGA.idLength = ",int(idLength) )
      DE_DEBUG( "TGA.cm_type = ",int(cm_type) )
      DE_DEBUG( "TGA.imageType = ",int(imageType) )
      DE_DEBUG( "TGA.cm_tableOffset = ",int(cm_tableOffset) )
      DE_DEBUG( "TGA.cm_entryCount = ",int(cm_entryCount) )
      DE_DEBUG( "TGA.cm_entrySize = ",int(cm_entrySize) )
      DE_DEBUG( "TGA.xOrigin = ",int(xOrigin) )
      DE_DEBUG( "TGA.yOrigin = ",int(yOrigin) )

      DE_DEBUG( "TGA.w = ",int(w) )
      DE_DEBUG( "TGA.h = ",int(h) )
      DE_DEBUG( "TGA.bitsPerPixel = ",int(bitsPerPixel) )
      DE_DEBUG( "TGA.imageDescriptor = ",int(imageDescriptor) )
   }

   static bool
   isTARGA20( de::Binary & file )
   {
      unsigned const n = 18;
      uint8_t signature[n];
      // tga_signature = "TRUEVISION-XFILE." (TGA 2.0 only)
      uint8_t tga_signature[n] = { 84, 82, 85, 69, 86, 73, 83, 73, 79, 78, 45, 88, 70, 73, 76, 69, 46, 0 };

      // store old file offset
      size_t const old_offset = file.tell();

      // read the signature at end of file
      file.seek( n, SEEK_END );
      file.read( signature, n );

      // restore old file offset
      file.seek( old_offset );

      return (memcmp(tga_signature, signature, n) == 0);
   }


};

struct TGAExtensionHeader
{
   uint16_t extension_size;   //! Size in bytes of the extension area, always 495
   char author_name[41];		//! Name of the author. If not used, bytes should be set to NULL (\0) or spaces
   char author_comments[324];	//! A comment, organized as four lines, each consisting of 80 characters plus a NULL
   uint16_t datetime_stamp[6];//! Date and time at which the image was created
   char job_name[41];			//! Job ID
   uint16_t job_time[3];      //! Hours, minutes and seconds spent creating the file (for billing, etc.)
   char software_id[41];		//! The application that created the file
   uint8_t software_version[3];
   uint32_t key_color;
   uint16_t pixel_aspect_ratio[2];
   uint16_t gamma_value[2];
   uint32_t color_correction_offset;	// Number of bytes from the beginning of the file to the color correction table if present
   uint32_t postage_stamp_offset;		// Number of bytes from the beginning of the file to the postage stamp image if present
   uint32_t scan_line_offset;			// Number of bytes from the beginning of the file to the scan lines table if present
   uint8_t attributes_type;			// Specifies the alpha channel
};

struct TGAFooter
{
   uint32_t ExtensionOffset; // DWORD extension_offset; extension area offset : offset in bytes from the beginning of the file
   uint32_t DeveloperOffset; // DWORD developer_offset;	developer directory offset : offset in bytes from the beginning of the file
   char Signature[18];       // char signature[18];		// signature string : contains "TRUEVISION-XFILE.\0"
   // tga_signature = "TRUEVISION-XFILE." (TGA 2.0 only)
   // BYTE tga_signature[sizeofSig] = { 84, 82, 85, 69, 86, 73, 83, 73, 79, 78, 45, 88, 70, 73, 76, 69, 46, 0 };
};

typedef union PixelInfo
{
    uint32_t ABGR;
    struct { uint8_t R, G, B, A; };
} *PPixelInfo;

#pragma pack( pop )

} // end namespace tga
} // end namespace image.
} // end namespace de.

/*
   #pragma pack( push )
   #pragma pack( 1 )

   // these structs are also used in the TGA writer
   struct TGAHeader
   {
      uint8_t IdLength;
      uint8_t ColorMapType;
      uint8_t ImageType;
      uint8_t FirstEntryIndex[2];
      uint16_t ColorMapLength;
      uint8_t ColorMapEntrySize;
      uint8_t XOrigin[2];
      uint8_t YOrigin[2];
      uint16_t Width;
      uint16_t Height;
      uint8_t BitsPerPixel;
      uint8_t ImageDescriptor;

      size_t getPixelCount() const {
         return size_t( Height ) * size_t( Width );
      }

      size_t getByteCount() const {
         return getPixelCount() * size_t( BitsPerPixel ) / 8;
      }

      size_t getBitsPerPixel() const {
         return size_t( BitsPerPixel );
      }

      size_t getBytesPerPixel() const {
         return size_t( BitsPerPixel ) / 8;
      }

      int32_t getWidth() const { return int32_t( Width ); }
      int32_t getHeight() const { return int32_t( Height ); }
   };

   struct TGAFooter
   {
      uint32_t ExtensionOffset;
      uint32_t DeveloperOffset;
      char  Signature[18];
   };

   typedef union PixelInfo
   {
       uint32_t ABGR;
       struct { uint8_t R, G, B, A; };
   } *PPixelInfo;


   #pragma pack( pop )
*/

#endif // DE_IMAGE_CODEC_TGA_ENABLED
