#include <de/image/Image_GIF.h>

#if defined(DE_IMAGE_READER_GIF_ENABLED) || defined(DE_IMAGE_WRITER_GIF_ENABLED)
    #include <gif5/gif_lib.h>
#endif

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_GIF_ENABLED) || defined(DE_IMAGE_WRITER_GIF_ENABLED)

namespace gif {

// ===========================================================================
// de.image.gif.Decoder
// ===========================================================================
struct RAII_Decoder
{
    GifFileType* m_File;
    std::string m_Uri;

    RAII_Decoder() : m_File( nullptr )
    {}

    ~RAII_Decoder()
    {
        close(); // most important call of this class -> is now RAII = auto cleanup on leaving scope.
    }

    bool is_open() const { return( m_File != nullptr ); }

    bool open( std::string const & uri )
    {
        if ( m_Uri == uri ) return is_open();

        close();

        int error = 0;
        m_Uri = uri;
        m_File = DGifOpenFileName( m_Uri.c_str(), &error );
        if ( !m_File )
        {
            DE_ERROR("Cant open gif uri(", uri, ")")
        }

        return is_open();
    }

    void close()
    {
        if ( m_File )
        {
            DE_ERROR("Closing gif uri(", m_Uri, ")" )
            // DumpScreen2RGB(OutFileName, OneFileFlag, colorMap, rows, file->SWidth, file->SHeight);
            //(void)free(rows);
            int error;
            if ( !DGifCloseFile( m_File, &error ) )
            {
                DE_ERROR("")
            }

            m_File = nullptr;
        }
    }
};

} // end namespace gif

#endif

// ===========================================================================
// ===========================================================================
// ===========================================================================

#if defined(DE_IMAGE_READER_GIF_ENABLED)

/*
#include <gif_lib.h>
#include <vector>
#include <cstring>
#include <iostream>

// Custom memory reader struct
struct GifMemoryReader {
    const uint8_t* data;
    size_t size;
    size_t pos;
};

// Function to read data from memory
int GifReadFromMemory(GifFileType* gif, GifByteType* buf, int size) {
    GifMemoryReader* reader = (GifMemoryReader*)gif->UserData;
    if (reader->pos + size > reader->size)
        size = reader->size - reader->pos; // Prevent overflow
    std::memcpy(buf, reader->data + reader->pos, size);
    reader->pos += size;
    return size;
}

void decodeGifFromMemory(std::vector<uint8_t>& gifData) {
    GifMemoryReader reader = { gifData.data(), gifData.size(), 0 };

    // Open GIF from memory
    GifFileType* gif = DGifOpen(&reader, GifReadFromMemory, nullptr);
    if (!gif) {
        std::cerr << "Failed to open GIF from memory" << std::endl;
        return;
    }

    // Read and decode GIF
    if (DGifSlurp(gif) != GIF_OK) {
        std::cerr << "Failed to decode GIF" << std::endl;
        DGifCloseFile(gif, nullptr);
        return;
    }

    // Process frames
    std::cout << "GIF Dimensions: " << gif->SWidth << "x" << gif->SHeight << std::endl;
    std::cout << "Number of frames: " << gif->ImageCount << std::endl;

    // Close GIF
    DGifCloseFile(gif, nullptr);
}

int main() {
    std::vector<uint8_t> gifData = ...; // Load GIF binary data here
    decodeGifFromMemory(gifData);
    return 0;
}
*/
// New: loadMemory resource.
bool
ImageReaderGIF::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
{
    return false;
}

bool
ImageReaderGIF::load( Image & img, std::string const & uri )
{
    int iOffsets[] = { 0, 4, 2, 1 }; // The way Interlaced image should.
    int iJumps[] = { 8, 8, 4, 2 };   // be read - offsets and jumps...

    gif::RAII_Decoder decoder; // wrapper auto closes the file on func exit/return.
    if ( !decoder.open( uri ) )
    {
        DE_ERROR("")
        //PrintGifError( error );
        //exit(EXIT_FAILURE);
        return false;
    }

    GifFileType* file = decoder.m_File;
    if ( file->SHeight == 0 || file->SWidth == 0 )
    {
        DE_ERROR("")
        //printf( "Image of width or height 0\n" );
        //exit(EXIT_FAILURE);
        return false;
    }

    uint32_t w = file->SWidth;
    uint32_t h = file->SHeight;
    size_t bytesPerRow = w * sizeof( GifPixelType ); // Size in bytes one row.
    //size_t byteCount = bytesPerRow * h;

    GifRowType scanline = (GifRowType)malloc( bytesPerRow );
    if ( !scanline )
    {
        DE_ERROR("")
        //GIF_EXIT("Failed to allocate memory required, aborted.");
        return false;
    }

    #if STRANGER_THINGS
    for ( uint32_t x = 0; x < w; ++x )
    {
        // Set its color to BackGround.
        scanline[ x ] = file->SBackGroundColor;
    }
    #endif

    /* Lets dump it - set the global variables required and do it: */
    ColorMapObject* colorMap = file->Image.ColorMap ? file->Image.ColorMap : file->SColorMap;
    if ( !colorMap )
    {
        DE_ERROR("No color map")
        //printf( "Gif Image does not have a colormap\n" );
        //exit(EXIT_FAILURE);
        return false;
    }

    /* check that the background color isn't garbage (SF bug #87) */
    if ( file->SBackGroundColor < 0 || file->SBackGroundColor >= colorMap->ColorCount )
    {
        DE_ERROR("")
        //printf( "Background color out of range for colormap\n");
        //exit(EXIT_FAILURE);
        return false;
    }

    img = de::Image( w, h );
    img.setUri( uri );
    img.fill( 0xFFFFFFFF );
    // Scan the content of the GIF file and load the image(s) in:
    GifRecordType chunk;
    do
    {
        if ( !DGifGetRecordType( file, &chunk ) )
        {
            DE_ERROR("")
            //PrintGifError(file->Error);
            //exit(EXIT_FAILURE);
            continue;
        }

        switch ( chunk )
        {
        case IMAGE_DESC_RECORD_TYPE:
        {
            if ( !DGifGetImageDesc( file ) )
            {
                DE_ERROR("")
                // PrintGifError(file->Error);
                // exit(EXIT_FAILURE);
                return false;
            }

            de::Recti rect( file->Image.Left, file->Image.Top,
            file->Image.Width, file->Image.Height );

            //printf( "Image %d at (%d, %d) [%dx%d]: ",++ImageNum, Col, Row, Width, Height );

            if ( rect.x2() >= int32_t( w ) || rect.y2() >= int32_t( h ) )
            {
                DE_ERROR("rect.getX2() >= w || rect.getY2() >= h")
                // printf( "Image %d is not confined to screen dimension, aborted.\n",ImageNum);
                // exit(EXIT_FAILURE);
                return false;
            }

            if ( !file->Image.Interlace )
            {
                DE_WARN("GIF non-interlaced, direct decode.")
                for ( int32_t y = 0; y < rect.h; ++y )
                {
                    //GifQprintf("\b\b\b\b%-4d", i);
                    if ( !DGifGetLine( file, scanline, rect.w ) )
                    {
                        DE_ERROR("ScanLine Error ")
                        //PrintGifError(file->Error);
                        //exit(EXIT_FAILURE);
                        return false;
                    }

                    for ( int32_t x = 0; x < rect.w; ++x )
                    {
                        int index = scanline[ x ];
                        GifColorType gifColor = colorMap->Colors[ index ];
                        uint32_t color = dbRGBA( gifColor.Red, gifColor.Green, gifColor.Blue );
                        if ( index == file->SBackGroundColor )
                        {
                            color = 0x00000000;
                        }

                        img.setPixel( x,y,color );
                    }
                }
            }
            else
            {
                DE_WARN("GIF interlaced, need 4 passes to decode")

                // Need to perform 4 passes on the images:
                for ( int pass = 0; pass < 4; ++pass )
                {
                    for ( int j = rect.y1() + iOffsets[ pass ]; j <= rect.y2(); j += iJumps[ pass ] )
                    {
                        // GifQprintf("\b\b\b\b%-4d", Count++);
                        if ( !DGifGetLine( file, scanline, rect.w ) )
                        {
                            DE_ERROR("")
                            // PrintGifError(file->Error);
                            // exit(EXIT_FAILURE);
                            return false;
                        }
                    }
                }
            }
        } break;

        case EXTENSION_RECORD_TYPE:
        {
            GifByteType* ext;
            int extCode;

            // Skip any extension blocks in file:
            if ( !DGifGetExtension( file, &extCode, &ext ) )
            {
                DE_ERROR("")
                // PrintGifError(file->Error);
                // exit(EXIT_FAILURE);
                return false;
            }
            while ( !ext )
            {
                if ( !DGifGetExtensionNext( file, &ext ) )
                {
                    DE_ERROR("")
                    // PrintGifError(file->Error);
                    // exit(EXIT_FAILURE);
                    return false;
                }
            }
        } break;

        case TERMINATE_RECORD_TYPE: break;

        default: break;// Should be trapped by DGifGetRecordType.
        }
    } while ( chunk != TERMINATE_RECORD_TYPE );

    // DumpScreen2RGB(OutFileName, OneFileFlag, colorMap, rows, file->SWidth, file->SHeight);
    return true;
}



#endif // DE_IMAGE_READER_GIF_ENABLED

// ===========================================================================
// ===========================================================================
// ===========================================================================

#if defined(DE_IMAGE_WRITER_GIF_ENABLED)

bool
ImageWriterGIF::save( Image const & img, std::string const & uri, uint32_t quality )
{
   static_cast< void >( quality ); // UNUSED

   if ( img.empty() )
   {
      DE_ERROR("Image is empty.")
      return false;
   }

//   std::ofstream file( uri.c_str() );

//   if ( !file.is_open() )
//   {
//      DE_ERROR("Cant open uri(",uri,") for text write\n")
//      return false;
//   }

//   int32_t w = img.getWidth();
//   int32_t h = img.getHeight();


//   file.close();

   return false;
}

#endif // DE_IMAGE_WRITER_GIF_ENABLED

} // end namespace image.
} // end namespace de.


























































































#if 0


bool
ImageReaderICO::load( Image & img, std::string const & uri, uint32_t frame )
{
   Binary file( uri );
   if ( !file.is_open() )
   {
      DE_ERROR( "Cant open file stream ",uri )
      return false;
   }

   return load( img, file, frame );
}

bool
ImageReaderICO::load( Image & img, Binary & file, uint32_t frame )
{
   std::string const & uri = file.getUri();
   if ( !file.is_open() )
   {
      DE_ERROR( "Cant open binary file(",uri,").")
      return false;
   }

   img.setUri( file.getUri() );

   // GifBegin( &writer, filename, width, height, 2, 8, true );
   //fputs("GIF89a", writer->f);
   char header[ 7 ];
   if ( file.read( header, 7 ) != 7 ) { DE_ERROR("No 7 bytes header.") return false; }
   if ( header[ 0 ] != 'G' ) { DE_ERROR("No header 'G'") return false; }
   if ( header[ 1 ] != 'I' ) { DE_ERROR("No header 'I'") return false; }
   if ( header[ 2 ] != 'F' ) { DE_ERROR("No header 'F'") return false; }
   if ( header[ 3 ] != '8' ) { DE_ERROR("No header '8'") return false; }
   if ( header[ 4 ] != '9' ) { DE_ERROR("No header '9'") return false; }
   if ( header[ 5 ] != 'a' ) { DE_ERROR("No header 'a'") return false; }
   if ( header[ 6 ] != '\0' ) { DE_ERROR("No header '\0'") return false; }

   // screen descriptor
   // fputc(width & 0xff, writer->f);
   // fputc((width >> 8) & 0xff, writer->f);
   // fputc(height & 0xff, writer->f);
   // fputc((height >> 8) & 0xff, writer->f);

   uint8_t w1 = 0;
   uint8_t w2 = 0;
   if ( file.read( &w1, 1 ) != 1 ) { DE_ERROR("No w1") return false; }
   if ( file.read( &w2, 1 ) != 1 ) { DE_ERROR("No w2") return false; }
   int w = int( size_t(w1) | (size_t(w2) << 8) );
   DE_DEBUG("Read w = ", w)

   uint8_t h1 = 0;
   uint8_t h2 = 0;
   if ( file.read( &h1, 1 ) != 1 ) { DE_ERROR("No h1") return false; }
   if ( file.read( &h2, 1 ) != 1 ) { DE_ERROR("No h2") return false; }
   int h = int( size_t(h1) | (size_t(h2) << 8) );
   DE_DEBUG("Read h = ", h)

   // fputc(0xf0, writer->f);  // there is an unsorted global color table of 2 entries
   // fputc(0, writer->f);     // background color
   // fputc(0, writer->f);     // pixels are square (we need to specify this because it's 1989)
   uint8_t dummy = 0;
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No dummy1") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No dummy2") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No dummy3") return false; }

   // now the "global" palette (really just a dummy palette)
   // color 0: black
   //fputc(0, writer->f);
   //fputc(0, writer->f);
   //fputc(0, writer->f);
   // color 1: also black
   //fputc(0, writer->f);
   //fputc(0, writer->f);
   //fputc(0, writer->f);
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No black1") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No black2") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No black3") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No black4") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No black5") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No black6") return false; }

   // Read animation header
   //   fputc(0x21, writer->f); // extension
   //   fputc(0xff, writer->f); // application specific
   //   fputc(11, writer->f); // length 11
   //   fputs("NETSCAPE2.0", writer->f); // yes, really
   //   fputc(3, writer->f); // 3 bytes of NETSCAPE2.0 data

   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No anim header byte 1 (extension)") return false; }
   if ( dummy != 0x21 ) { DE_ERROR("No extension 0x21") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No anim header byte 2 (app-specific)") return false; }
   if ( dummy != 0xFF ) { DE_ERROR("No app specific 0xFF") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No anim header byte 3 (NETSCAPE2.0)") return false; }
   if ( dummy != 11 ) { DE_ERROR("No length 11") return false; }

   char netscape[11];
   if ( file.read( &netscape, 11 ) != 11 ) { DE_ERROR("No NETSCAPE2.0") return false; }
   if ( netscape[ 0 ] != 'N' ) { DE_ERROR("No netscape 'N'") return false; }
   if ( netscape[ 1 ] != 'E' ) { DE_ERROR("No netscape 'E'") return false; }
   if ( netscape[ 2 ] != 'T' ) { DE_ERROR("No netscape 'T'") return false; }
   if ( netscape[ 3 ] != 'S' ) { DE_ERROR("No netscape 'S'") return false; }
   if ( netscape[ 4 ] != 'C' ) { DE_ERROR("No netscape 'C'") return false; }
   if ( netscape[ 5 ] != 'A' ) { DE_ERROR("No netscape 'A'") return false; }
   if ( netscape[ 6 ] != 'P' ) { DE_ERROR("No netscape 'P'") return false; }
   if ( netscape[ 7 ] != 'E' ) { DE_ERROR("No netscape 'E'") return false; }
   if ( netscape[ 8 ] != '2' ) { DE_ERROR("No netscape '2'") return false; }
   if ( netscape[ 9 ] != '.' ) { DE_ERROR("No netscape '.'") return false; }
   if ( netscape[ 10 ] != '0' ) { DE_ERROR("No netscape '0'") return false; }

   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No netscape data byte 1") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No netscape data byte 2") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No netscape data byte 3") return false; }

   //   fputc(1, writer->f); // JUST BECAUSE
   //   fputc(0, writer->f); // loop infinitely (byte 0)
   //   fputc(0, writer->f); // loop infinitely (byte 1)
   //   fputc(0, writer->f); // block terminator
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No JUST BECAUSE byte") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No loop infinitely byte 0") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No loop infinitely byte 1") return false; }
   if ( file.read( &dummy, 1 ) != 1 ) { DE_ERROR("No block terminator") return false; }


   // Writes out a new frame to a GIF in progress.
   // The GIFWriter should have been created by GIFBegin.
   // AFAIK, it is legal to use different bit depths for different frames of an image -
   // this may be handy to save bits in animations that don't change much.
//   bool GifReadFrame( GifWriter* writer, const uint8_t* image, uint32_t width, uint32_t height, uint32_t delay, int bitDepth = 8, bool dither = false )
//   {
//      if(!writer->f) return false;

//      const uint8_t* oldImage = writer->firstFrame? NULL : writer->oldImage;
//      writer->firstFrame = false;

//      GifPalette pal;
//      GifMakePalette((dither? NULL : oldImage), image, width, height, bitDepth, dither, &pal);

//      if(dither)
//        GifDitherImage(oldImage, image, writer->oldImage, width, height, &pal);
//      else
//        GifThresholdImage(oldImage, image, writer->oldImage, width, height, &pal);

//      GifWriteLzwImage(writer->f, writer->oldImage, 0, 0, width, height, delay, &pal);

//      return true;
//   }

//    for( int frame=0; frame<256; ++frame )
//    {

//        // Make an image, somehow
//        // this is the default shadertoy - credit to shadertoy.com
//        float tt = frame * 3.14159f * 2 / 255.0f;
//        for( int yy=0; yy<height; ++yy )
//        {
//            for( int xx=0; xx<width; ++xx )
//            {
//                float fx = xx / (float)width;
//                float fy = yy / (float)height;

//                float red = 0.5f + 0.5f * cosf(tt+fx);
//                float grn = 0.5f + 0.5f * cosf(tt+fy+2.f);
//                float blu = 0.5f + 0.5f * cosf(tt+fx+4.f);

//                SetPixelFloat( xx, yy, red, grn, blu );
//            }
//        }


//        // Write the frame to the gif
//        printf( "Writing frame %d...\n", frame );
//        GifWriteFrame( &writer, image, width, height, 2, 8, true );
//    }

//    // Write EOF
//    GifEnd( &writer );

   ColorFormat::EColorFormat srcType = ColorFormat::RGBA8888;
   ColorFormat::EColorFormat dstType = ColorFormat::RGBA8888;

   // Create the image structure to be filled by png data
/*
   if ( srcFmt == PNG_COLOR_TYPE_RGB_ALPHA )
   {
      srcType = ColorFormat::RGBA8888;
   }
   else
   {
      srcType = ColorFormat::RGB888;
   }

   ColorConverter::Converter_t converter =
   ColorConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_ERROR("No color converter found from (", srcType," to (", dstType, ")" )
      return false;
   }
*/


   Image tmp( w, h, srcType );
   img.clear();
   img.setFormat( dstType );
   img.resize( w, h );

   // Create array of pointers to rows in image data
//   std::vector< uint8_t* > rows;
//   rows.reserve( h );
//   for ( uint32_t i = 0; i < h; ++i )
//   {
//      rows.emplace_back( tmp.getRow( i ) );
//   }

   file.close();

//   for ( int y = 0; y < tmp.getHeight(); ++y )
//   {
//      converter( tmp.getRow( y ), img.getRow( y ), img.getWidth() );
//   }

   return true;
}

/*
const int width = 256;
const int height = 256;
uint8_t image[ width * height * 4 ];

void SetPixel( int xx, int yy, uint8_t red, uint8_t grn, uint8_t blu )
{
    uint8_t* pixel = &image[(yy*width+xx)*4];
    pixel[0] = red;
    pixel[1] = blu;
    pixel[2] = grn;
    pixel[3] = 255;  // no alpha for this demo
}

void SetPixelFloat( int xx, int yy, float fred, float fgrn, float fblu )
{
    // convert float to unorm
    uint8_t red = (uint8_t)roundf( 255.0f * fred );
    uint8_t grn = (uint8_t)roundf( 255.0f * fgrn );
    uint8_t blu = (uint8_t)roundf( 255.0f * fblu );

    SetPixel( xx, yy, red, grn, blu );
}

int main(int argc, const char * argv[])
{
    const char* filename = "./MyGif.gif";
    if( argc > 1 )
    {
        filename = argv[1];
    }

    // Create a gif
    GifWriter writer = {};
    GifBegin( &writer, filename, width, height, 2, 8, true );

    for( int frame=0; frame<256; ++frame )
    {

        // Make an image, somehow
        // this is the default shadertoy - credit to shadertoy.com
        float tt = frame * 3.14159f * 2 / 255.0f;
        for( int yy=0; yy<height; ++yy )
        {
            for( int xx=0; xx<width; ++xx )
            {
                float fx = xx / (float)width;
                float fy = yy / (float)height;

                float red = 0.5f + 0.5f * cosf(tt+fx);
                float grn = 0.5f + 0.5f * cosf(tt+fy+2.f);
                float blu = 0.5f + 0.5f * cosf(tt+fx+4.f);

                SetPixelFloat( xx, yy, red, grn, blu );
            }
        }


        // Write the frame to the gif
        printf( "Writing frame %d...\n", frame );
        GifWriteFrame( &writer, image, width, height, 2, 8, true );
    }

    // Write EOF
    GifEnd( &writer );

    return 0;
}


// Creates a gif file.
// The input GIFWriter is assumed to be uninitialized.
// The delay value is the time between frames in hundredths of a second - note that not all viewers pay much attention to this value.
bool GifBegin( GifWriter* writer, const char* filename, uint32_t width, uint32_t height, uint32_t delay, int32_t bitDepth = 8, bool dither = false )
{
    (void)bitDepth; (void)dither; // Mute "Unused argument" warnings
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   writer->f = 0;
    fopen_s(&writer->f, filename, "wb");
#else
    writer->f = fopen(filename, "wb");
#endif
    if(!writer->f) return false;

    writer->firstFrame = true;

    // allocate
    writer->oldImage = (uint8_t*)GIF_MALLOC(width*height*4);

    fputs("GIF89a", writer->f);

    // screen descriptor
    fputc(width & 0xff, writer->f);
    fputc((width >> 8) & 0xff, writer->f);
    fputc(height & 0xff, writer->f);
    fputc((height >> 8) & 0xff, writer->f);

    fputc(0xf0, writer->f);  // there is an unsorted global color table of 2 entries
    fputc(0, writer->f);     // background color
    fputc(0, writer->f);     // pixels are square (we need to specify this because it's 1989)

    // now the "global" palette (really just a dummy palette)
    // color 0: black
    fputc(0, writer->f);
    fputc(0, writer->f);
    fputc(0, writer->f);
    // color 1: also black
    fputc(0, writer->f);
    fputc(0, writer->f);
    fputc(0, writer->f);

    if( delay != 0 )
    {
        // animation header
        fputc(0x21, writer->f); // extension
        fputc(0xff, writer->f); // application specific
        fputc(11, writer->f); // length 11
        fputs("NETSCAPE2.0", writer->f); // yes, really
        fputc(3, writer->f); // 3 bytes of NETSCAPE2.0 data

        fputc(1, writer->f); // JUST BECAUSE
        fputc(0, writer->f); // loop infinitely (byte 0)
        fputc(0, writer->f); // loop infinitely (byte 1)

        fputc(0, writer->f); // block terminator
    }

    return true;
}

// Writes out a new frame to a GIF in progress.
// The GIFWriter should have been created by GIFBegin.
// AFAIK, it is legal to use different bit depths for different frames of an image -
// this may be handy to save bits in animations that don't change much.
bool GifWriteFrame( GifWriter* writer, const uint8_t* image, uint32_t width, uint32_t height, uint32_t delay, int bitDepth = 8, bool dither = false )
{
    if(!writer->f) return false;

    const uint8_t* oldImage = writer->firstFrame? NULL : writer->oldImage;
    writer->firstFrame = false;

    GifPalette pal;
    GifMakePalette((dither? NULL : oldImage), image, width, height, bitDepth, dither, &pal);

    if(dither)
        GifDitherImage(oldImage, image, writer->oldImage, width, height, &pal);
    else
        GifThresholdImage(oldImage, image, writer->oldImage, width, height, &pal);

    GifWriteLzwImage(writer->f, writer->oldImage, 0, 0, width, height, delay, &pal);

    return true;
}

// Writes the EOF code, closes the file handle, and frees temp memory used by a GIF.
// Many if not most viewers will still display a GIF properly if the EOF code is missing,
// but it's still a good idea to write it out.
bool GifEnd( GifWriter* writer )
{
    if(!writer->f) return false;

    fputc(0x3b, writer->f); // end of file
    fclose(writer->f);
    GIF_FREE(writer->oldImage);

    writer->f = NULL;
    writer->oldImage = NULL;

    return true;
}
*/

/******************************************************************************
 The real screen dumping routine.
******************************************************************************/
static void DumpScreen2RGB(char *FileName, int OneFileFlag,
            ColorMapObject *ColorMap,
            GifRowType *ScreenBuffer,
            int ScreenWidth, int ScreenHeight)
{
    int i, j;
    GifRowType GifRow;
    GifColorType *ColorMapEntry;
    FILE *rgbfp[3];

    if (FileName != NULL)
    {
        if (OneFileFlag)
        {
            if ((rgbfp[0] = fopen(FileName, "wb")) == NULL)
            GIF_EXIT("Can't open input file name.");
        }
        else
        {
            static char *Postfixes[] = { ".R", ".G", ".B" };
            char OneFileName[80];

            for (i = 0; i < 3; i++)
            {
                strncpy(OneFileName, FileName, sizeof(OneFileName)-1);
                strncat(OneFileName, Postfixes[i], sizeof(OneFileName) - 1 - strlen(OneFileName) );

                if ((rgbfp[i] = fopen(OneFileName, "wb")) == NULL)
                {
                    GIF_EXIT("Can't open input file name.");
                }
            }
        }
    }
    else
    {
        OneFileFlag = true;

#ifdef _WIN32
   _setmode(1, O_BINARY);
#endif /* _WIN32 */

        rgbfp[0] = stdout;
    }

   // ================================

   if (ColorMap == NULL)
   {
      fprintf(stderr, "Color map pointer is NULL.\n");
      exit(EXIT_FAILURE);
   }

    if (OneFileFlag)
    {
        unsigned char *BufferP;
        unsigned char *Buffer = (unsigned char *) malloc(ScreenWidth * 3)
        if ( !Buffer )
        {
            GIF_EXIT("Failed to allocate memory required, aborted.");
         }
        for (i = 0; i < ScreenHeight; i++)
        {
            GifRow = ScreenBuffer[i];
            GifQprintf("\b\b\b\b%-4d", ScreenHeight - i);
            for (j = 0, BufferP = Buffer; j < ScreenWidth; j++)
            {
                ColorMapEntry = &ColorMap->Colors[GifRow[j]];
                *BufferP++ = ColorMapEntry->Red;
                *BufferP++ = ColorMapEntry->Green;
                *BufferP++ = ColorMapEntry->Blue;
            }
            if (fwrite(Buffer, ScreenWidth * 3, 1, rgbfp[0]) != 1)
                GIF_EXIT("Write to file(s) failed.");
        }

        free((char *) Buffer);
        fclose(rgbfp[0]);
    }
    else
    {
        unsigned char *Buffers[3];

        if ((Buffers[0] = (unsigned char *) malloc(ScreenWidth)) == NULL ||
            (Buffers[1] = (unsigned char *) malloc(ScreenWidth)) == NULL ||
            (Buffers[2] = (unsigned char *) malloc(ScreenWidth)) == NULL)
            GIF_EXIT("Failed to allocate memory required, aborted.");

        for (i = 0; i < ScreenHeight; i++) {
            GifRow = ScreenBuffer[i];
            GifQprintf("\b\b\b\b%-4d", ScreenHeight - i);
            for (j = 0; j < ScreenWidth; j++) {
                ColorMapEntry = &ColorMap->Colors[GifRow[j]];
                Buffers[0][j] = ColorMapEntry->Red;
                Buffers[1][j] = ColorMapEntry->Green;
                Buffers[2][j] = ColorMapEntry->Blue;
            }
            if (fwrite(Buffers[0], ScreenWidth, 1, rgbfp[0]) != 1 ||
                fwrite(Buffers[1], ScreenWidth, 1, rgbfp[1]) != 1 ||
                fwrite(Buffers[2], ScreenWidth, 1, rgbfp[2]) != 1)
                GIF_EXIT("Write to file(s) failed.");
        }

        free((char *) Buffers[0]);
        free((char *) Buffers[1]);
        free((char *) Buffers[2]);
        fclose(rgbfp[0]);
        fclose(rgbfp[1]);
        fclose(rgbfp[2]);
    }
}

#endif
