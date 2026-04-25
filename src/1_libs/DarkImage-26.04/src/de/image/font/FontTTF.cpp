#include <de/image/font/FontTTF.h>

#ifdef HAVE_FREETYPE2

namespace de {

/*
  ----------------------------
  file: fterrdef.h
  ----------------------------
  FT_NOERRORDEF_( Ok                   0x00, "no error" )
  FT_ERRORDEF_( Cannot_Open_Resource,  0x01, "cannot open resource" )
  FT_ERRORDEF_( Unknown_File_Format,   0x02, "unknown file format" )
  FT_ERRORDEF_( Invalid_File_Format,   0x03, "broken file" )
  FT_ERRORDEF_( Invalid_Version,       0x04, "invalid FreeType version" )
  FT_ERRORDEF_( Lower_Module_Version,  0x05, "module version is too low" )
  FT_ERRORDEF_( Invalid_Argument,      0x06, "invalid argument" )
  FT_ERRORDEF_( Unimplemented_Feature, 0x07, "unimplemented feature" )
  FT_ERRORDEF_( Invalid_Table,         0x08, "broken table" )
  FT_ERRORDEF_( Invalid_Offset,        0x09, "broken offset within table" )
  FT_ERRORDEF_( Array_Too_Large,       0x0A, "array allocation size too large" )
  FT_ERRORDEF_( Missing_Module,        0x0B, "missing module" )
  FT_ERRORDEF_( Missing_Property,      0x0C, "missing property" )

  // glyph/character errors
  FT_ERRORDEF_( Invalid_Glyph_Index,   0x10, "invalid glyph index" )
  FT_ERRORDEF_( Invalid_Character_Code,0x11, "invalid character code" )
  FT_ERRORDEF_( Invalid_Glyph_Format,  0x12, "unsupported glyph image format" )
  FT_ERRORDEF_( Cannot_Render_Glyph,   0x13, "cannot render this glyph format" )
  FT_ERRORDEF_( Invalid_Outline,       0x14, "invalid outline" )
  FT_ERRORDEF_( Invalid_Composite,     0x15, "invalid composite glyph" )
  FT_ERRORDEF_( Too_Many_Hints,        0x16, "too many hints" )
  FT_ERRORDEF_( Invalid_Pixel_Size,    0x17, "invalid pixel size" )

  // handle errors
  FT_ERRORDEF_( Invalid_Handle,        0x20, "invalid object handle" )
  FT_ERRORDEF_( Invalid_Library_Handle,0x21, "invalid library handle" )
  FT_ERRORDEF_( Invalid_Driver_Handle, 0x22, "invalid module handle" )
  FT_ERRORDEF_( Invalid_Face_Handle,   0x23, "invalid face handle" )
  FT_ERRORDEF_( Invalid_Size_Handle,   0x24, "invalid size handle" )
  FT_ERRORDEF_( Invalid_Slot_Handle,   0x25, "invalid glyph slot handle" )
  FT_ERRORDEF_( Invalid_CharMap_Handle,0x26, "invalid charmap handle" )
  FT_ERRORDEF_( Invalid_Cache_Handle,  0x27, "invalid cache manager handle" )
  FT_ERRORDEF_( Invalid_Stream_Handle, 0x28, "invalid stream handle" )

  // driver errors
  FT_ERRORDEF_( Too_Many_Drivers,      0x30, "too many modules" )
  FT_ERRORDEF_( Too_Many_Extensions,   0x31, "too many extensions" )

  // memory errors
  FT_ERRORDEF_( Out_Of_Memory,         0x40, "out of memory" )
  FT_ERRORDEF_( Unlisted_Object,       0x41, "unlisted object" )

  // stream errors
  FT_ERRORDEF_( Cannot_Open_Stream,    0x51, "cannot open stream" )
  FT_ERRORDEF_( Invalid_Stream_Seek,   0x52, "invalid stream seek" )
  FT_ERRORDEF_( Invalid_Stream_Skip,   0x53, "invalid stream skip" )
  FT_ERRORDEF_( Invalid_Stream_Read,   0x54, "invalid stream read" )
  FT_ERRORDEF_( Invalid_Stream_Operation,0x55,"invalid stream operation" )
  FT_ERRORDEF_( Invalid_Frame_Operation,0x56,"invalid frame operation" )
  FT_ERRORDEF_( Nested_Frame_Access,   0x57, "nested frame access" )
  FT_ERRORDEF_( Invalid_Frame_Read,    0x58, "invalid frame read" )

  // raster errors
  FT_ERRORDEF_( Raster_Uninitialized,  0x60, "raster uninitialized" )
  FT_ERRORDEF_( Raster_Corrupted,      0x61, "raster corrupted" )
  FT_ERRORDEF_( Raster_Overflow,       0x62, "raster overflow" )
  FT_ERRORDEF_( Raster_Negative_Height,0x63, "negative height while rastering" )

  // cache errors
  FT_ERRORDEF_( Too_Many_Caches,       0x70, "too many registered caches" )

  // TrueType and SFNT errors
  FT_ERRORDEF_( Invalid_Opcode,        0x80, "invalid opcode" )
  FT_ERRORDEF_( Too_Few_Arguments,     0x81, "too few arguments" )
  FT_ERRORDEF_( Stack_Overflow,        0x82, "stack overflow" )
  FT_ERRORDEF_( Code_Overflow,         0x83, "code overflow" )
  FT_ERRORDEF_( Bad_Argument,          0x84, "bad argument" )
  FT_ERRORDEF_( Divide_By_Zero,        0x85, "division by zero" )
  FT_ERRORDEF_( Invalid_Reference,     0x86, "invalid reference" )
  FT_ERRORDEF_( Debug_OpCode,          0x87, "found debug opcode" )
  FT_ERRORDEF_( ENDF_In_Exec_Stream,   0x88, "found ENDF opcode in execution stream" )
  FT_ERRORDEF_( Nested_DEFS,           0x89, "nested DEFS" )
  FT_ERRORDEF_( Invalid_CodeRange,     0x8A, "invalid code range" )
  FT_ERRORDEF_( Execution_Too_Long,    0x8B, "execution context too long" )
  FT_ERRORDEF_( Too_Many_Function_Defs,0x8C, "too many function definitions" )
  FT_ERRORDEF_( Too_Many_Instruction_Defs, 0x8D, "too many instruction definitions" )
  FT_ERRORDEF_( Table_Missing,         0x8E, "SFNT font table missing" )
  FT_ERRORDEF_( Horiz_Header_Missing,  0x8F, "horizontal header (hhea) table missing" )
  FT_ERRORDEF_( Locations_Missing,     0x90, "locations (loca) table missing" )
  FT_ERRORDEF_( Name_Table_Missing,    0x91, "name table missing" )
  FT_ERRORDEF_( CMap_Table_Missing,    0x92, "character map (cmap) table missing" )
  FT_ERRORDEF_( Hmtx_Table_Missing,    0x93, "horizontal metrics (hmtx) table missing" )
  FT_ERRORDEF_( Post_Table_Missing,    0x94, "PostScript (post) table missing" )
  FT_ERRORDEF_( Invalid_Horiz_Metrics, 0x95, "invalid horizontal metrics" )
  FT_ERRORDEF_( Invalid_CharMap_Format,0x96, "invalid character map (cmap) format" )
  FT_ERRORDEF_( Invalid_PPem,          0x97, "invalid ppem value" )
  FT_ERRORDEF_( Invalid_Vert_Metrics,  0x98, "invalid vertical metrics" )
  FT_ERRORDEF_( Could_Not_Find_Context,0x99, "could not find context" )
  FT_ERRORDEF_( Invalid_Post_Table_Format,0x9A, "invalid PostScript (post) table format" )
  FT_ERRORDEF_( Invalid_Post_Table,    0x9B, "invalid PostScript (post) table" )
  FT_ERRORDEF_( DEF_In_Glyf_Bytecode,  0x9C, "found FDEF or IDEF opcode in glyf bytecode" )
  FT_ERRORDEF_( Missing_Bitmap,        0x9D, "missing bitmap in strike" )

  // CFF, CID, and Type 1 errors
  FT_ERRORDEF_( Syntax_Error,          0xA0, "opcode syntax error" )
  FT_ERRORDEF_( Stack_Underflow,       0xA1, "argument stack underflow" )
  FT_ERRORDEF_( Ignore,                0xA2, "ignore" )
  FT_ERRORDEF_( No_Unicode_Glyph_Name, 0xA3, "no Unicode glyph name found" )
  FT_ERRORDEF_( Glyph_Too_Big,         0xA4, "glyph too big for hinting" )

  // BDF errors
  FT_ERRORDEF_( Missing_Startfont_Field,0xB0, "`STARTFONT' field missing" )
  FT_ERRORDEF_( Missing_Font_Field,    0xB1, "`FONT' field missing" )
  FT_ERRORDEF_( Missing_Size_Field,    0xB2, "`SIZE' field missing" )
  FT_ERRORDEF_( Missing_Fontboundingbox_Field,0xB3,"`FONTBOUNDINGBOX' field missing" )
  FT_ERRORDEF_( Missing_Chars_Field,   0xB4, "`CHARS' field missing" )
  FT_ERRORDEF_( Missing_Startchar_Field,0xB5, "`STARTCHAR' field missing" )
  FT_ERRORDEF_( Missing_Encoding_Field,0xB6, "`ENCODING' field missing" )
  FT_ERRORDEF_( Missing_Bbx_Field,     0xB7, "`BBX' field missing" )
  FT_ERRORDEF_( Bbx_Too_Big,           0xB8, "`BBX' too big" )
  FT_ERRORDEF_( Corrupted_Font_Header, 0xB9, "Font header corrupted or missing fields" )
  FT_ERRORDEF_( Corrupted_Font_Glyphs, 0xBA, "Font glyphs corrupted or missing fields" )
*/

// ===========================================================================
FontTTF::FontTTF( Font font )
// ===========================================================================
   : m_Font( font )
   , m_ftLib( nullptr )
   , m_ftFace( nullptr )
   , m_IsOpen( false )
   , m_isDirty( true )
   , m_AtlasX( 1 )
   , m_AtlasY( 1 )
   , m_AtlasCLH( 1 ) // Current line height
   , m_AtlasMLH( 1 ) // Max line height
   , m_AtlasPage0()
{
   if ( !m_ftLib )
   {
      FT_Error e = FT_Init_FreeType( &m_ftLib );
      if ( e != 0 )
      {
         DE_ERROR("FT_Init_FreeType() failed.")
         m_ftLib = nullptr;
      }
   }

   //open();
   //DE_DEBUG("Destructor")

}

FontTTF::~FontTTF()
{
   //DE_DEBUG("Destructor for Font(", m_Font.toString(),")")

   //saveAtlas();

   close();

   if ( m_ftLib )
   {
      FT_Error e = FT_Done_FreeType( m_ftLib );
      if ( e != 0 )
      {
         // DE_ERROR("FT_Done_FreeType() failed.")
      }
      m_ftLib = nullptr;
   }

   //std::cout << std::flush;
}


bool
FontTTF::setPixelSize( int pixelSize )
{
    return FontTTF_Utils::setPixelSize( m_ftFace, pixelSize );
}

bool
FontTTF::is_open() const
{
    return m_IsOpen;
}

void
FontTTF::openMemory( const uint8_t* pBytes, size_t nBytes )
{
    if ( !m_ftLib )
    {
        DE_ERROR("No library.")
        return;
    }

    if ( m_IsOpen )
    {
        DE_ERROR("Already open")
        return;
    }

    //if ( m_ftFace ) { return; } // Already open

    //if ( m_Uri.empty() ) { return; } // No URI

    FT_Error e = FT_New_Memory_Face(m_ftLib, pBytes, nBytes, 0, &m_ftFace );

    if ( e == FT_Err_Unknown_File_Format )
    {
        DE_ERROR("FT_New_Face() got FT_Err_Unknown_File_Format") return;
    }
    else if ( e )
    {
        DE_ERROR("FT_New_Face() failed with error(",e,")")
        //return;
    }

    if ( !m_ftFace )
    {
        DE_ERROR("Cant open memory font")
        return;
    }

    DE_DEBUG("Font Family Name: ", m_ftFace->family_name)
    DE_DEBUG("Font Style Name: ", m_ftFace->style_name)

    m_Uri = m_ftFace->family_name;

    setPixelSize( m_Font.pixelSize );

    m_AtlasPage0 = de::Image( 1024, 1024 );
    m_AtlasPage0.fill( 0x00000000 );
    m_AtlasPage0.setUri( "Atlas_" + m_Font.toFontString() );

    m_IsOpen = true;

    std::wstring preStr = StringUtil::joinW( L"0123456789"
                                            L"abcdefghijklmnopqrstuvwxyz"
                                            L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            L"ÄäÖöÜüß"
                                            L"*-:;.,<>[](){}=?!\"§$%&/\\'_^°|@€",
                                            wchar_t(fa::amazon),
                                            wchar_t(fa::adjust),
                                            wchar_t(fa::aligncenter) );
    cacheString( preStr );
    //saveAtlas();
}

void
FontTTF::open( std::string uri )
{
    if ( m_IsOpen )
    {
        DE_ERROR("")
        return; // Already open
    }

    m_Uri = uri;

    //if ( m_ftFace ) { return; } // Already open
    if ( !m_ftLib ) { return; } // No library
    if ( m_Uri.empty() ) { return; } // No URI

    FT_Error e = FT_New_Face( m_ftLib, m_Uri.c_str(), 0, &m_ftFace );

    if ( e == FT_Err_Unknown_File_Format )
    {
        DE_ERROR("FT_New_Face() got FT_Err_Unknown_File_Format") return;
    }
    else if ( e )
    {
        DE_ERROR("FT_New_Face() failed with error(",e,")")
        //return;
    }

    if ( !m_ftFace )
    {
        DE_ERROR("Cant open font uri(", m_Uri ,")") return;
    }

    DE_DEBUG("Font Family Name: ", m_ftFace->family_name)
    DE_DEBUG("Font Style Name: ", m_ftFace->style_name)
    DE_DEBUG("Font FileName: ", m_Uri)

    setPixelSize( m_Font.pixelSize );

    m_AtlasPage0 = de::Image( 1024, 1024 );
    m_AtlasPage0.fill( 0x00000000 );
    m_AtlasPage0.setUri( "Atlas_" + m_Font.toFontString() );

    m_IsOpen = true;

    std::wstring preStr = StringUtil::joinW( L"0123456789"
                                            L"abcdefghijklmnopqrstuvwxyz"
                                            L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            L"ÄäÖöÜüß"
                                            L"*-:;.,<>[](){}=?!\"§$%&/\\'_^°|@€",
                                            wchar_t(fa::amazon),
                                            wchar_t(fa::adjust),
                                            wchar_t(fa::aligncenter) );
    cacheString( preStr );
    //saveAtlas();
}

void
FontTTF::close()
{
    if ( !m_IsOpen ) return;
    if ( !m_ftLib ) return; // Already closed.
    if ( !m_ftFace ) return; // Already closed.

    if ( !m_ftFace->driver )
    {
        //DE_ERROR("No face driver, face(", face, ")" )
        return;
    }

    if ( !m_ftFace->internal )
    {
        //DE_ERROR("No face internal, face(", face, ")" )
        return;
    }

    if ( m_ftFace->internal->refcount < 1 )
    {
        //DE_ERROR("FontTTF refcount already zero while closing face(", face, ")" )
        return;
    }

    FT_Error e = FT_Done_Face( m_ftFace );
    if ( e != 0 )
    {
        // DE_ERROR("Got error while closing face(", face, ")" )
    }

    // DE_DEBUG("Closed face.")
    m_ftFace = nullptr;
    m_ftLib = nullptr;

    m_AtlasPage0.clear();

    m_IsOpen = false;
}

void
FontTTF::saveAtlas()
{
    //std::string uri = "atlas_" + m_Font.toFontString() + ".png";
    std::string uri = "fontface_" + m_Font.toFontString() + ".webp";
    //DE_DEBUG("uri = ", uri)
    dbSaveImage( m_AtlasPage0, uri );
    //dbSaveImage( m_AtlasPage0, fileName + ".jpg" );
}

bool
FontTTF::hasGlyph( uint32_t unicode ) const
{
   auto it = m_glyphCache.find( unicode );
   if ( it == m_glyphCache.end() )
   {
      return false;
   }
   else
   {
      return true;
   }
}

Glyph &
FontTTF::getGlyph( uint32_t unicode )
{
   auto it = m_glyphCache.find( unicode );
   if ( it == m_glyphCache.end() )
   {
      m_glyphCache[ unicode ] = createGlyph( unicode );
      return m_glyphCache[ unicode ];
   }
   else
   {
      return it->second;
   }
}

Glyph
FontTTF::createGlyph( uint32_t unicode )
{
   //DE_DEBUG("unicode(",unicode,"), font(",m_Font.toString(),")")
   Glyph glyph;
   glyph.unicode = unicode;

   uint32_t index = FontTTF_Utils::getGlyphIndex( m_ftFace, unicode );
   if ( index == 0 )
   {
      // its not a bug for white spaces and unprintable characters like margins, paddings
      // it would spam the logs printing errors for white spaces.
      // DE_ERROR("Invalid index. unicode(",unicode,")")
      return glyph;
   }

   //DE_DEBUG("unicode(",unicode,"), index(",index,")")

   // This is because we cache faces and the face may have been set to a different size.
   FontTTF_Utils::setPixelSize( m_ftFace, m_Font.pixelSize );

   uint32_t loadFlags = FT_LOAD_RENDER;
   loadFlags |= FT_LOAD_COMPUTE_METRICS;
   loadFlags |= FT_LOAD_FORCE_AUTOHINT;

   if ( m_Font.lcdFit )
   {
      loadFlags |= FT_LOAD_TARGET_LCD;
   }

   if ( !m_Font.antiAlias )
   {
      loadFlags |= FT_LOAD_MONOCHROME;
   }

   // FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP

   FT_Error e = FT_Load_Glyph( m_ftFace, index, loadFlags );
   if ( e )
   {
      DE_ERROR("Failed FT_Load_Glyph(",unicode,")")
      return glyph;
   }

   FT_GlyphSlot ftGlyph = m_ftFace->glyph;
   if ( !ftGlyph )
   {
      DE_ERROR("No glyph to access unicode(",unicode,")")
      return glyph;
   }

   //FT_Glyph_Metrics const & ftGlyphMetrics = ftGlyph->metrics;

   glyph.glyph_index = index;
   glyph.advance = ftGlyph->advance.x;

   if ( m_Font.lcdFit )
   {
      glyph.advance = 3 * ftGlyph->advance.x;
   }

   //glyph.advance_y = ftGlyph->advance.y;
   if ( FT_IS_SCALABLE( m_ftFace ) )
   {
      glyph.advance /= 64;
      //glyph.advance_y /= 64;
//    glyph.hBearing /= 64;
//    glyph.vBearing /= 64;
//    glyph.bbox /= 64;
   }

//    glyph.hBearing.x = ftGlyphMetrics.horiBearingX;
//    glyph.hBearing.y = ftGlyphMetrics.horiBearingY;
//    glyph.vBearing.x = ftGlyphMetrics.vertBearingX;
//    glyph.vBearing.y = ftGlyphMetrics.vertBearingY;
//    glyph.bbox = computeOutlineBBox();

   int32_t bmp_x = ftGlyph->bitmap_left;
   int32_t bmp_y = ftGlyph->bitmap_top;
   int32_t bmp_w = ftGlyph->bitmap.width;
   int32_t bmp_h = ftGlyph->bitmap.rows;
   // Now raster glyph to image and convert image to the dark side.
   glyph.bmp = Recti( bmp_x, bmp_y, bmp_w, bmp_h );

   // int w_total = m_AtlasPage0.getWidth();
   // int h_total = m_AtlasPage0.getHeight();

   if ( m_AtlasX + glyph.bmp.w + 4 >= m_AtlasPage0.w() )
   {
      m_AtlasY += m_AtlasCLH;
      m_AtlasX = 1;
      m_AtlasCLH = 0;
   }

   // drawBorder
   //Recti r_border( m_AtlasX - 1, m_AtlasY - 1, bmp_w + 2, bmp_h + 2 );
   //ImagePainter::drawOutlineRect( m_AtlasPage0, r_border, 0xFF0000FF );

   // drawGlyph
   glyph.ref.m_img = &m_AtlasPage0;
   glyph.ref.m_rect = Recti( m_AtlasX, m_AtlasY, bmp_w, bmp_h );
   FontTTF_Utils::drawFtBitmap( m_AtlasPage0, m_AtlasX, m_AtlasY, ftGlyph->bitmap );

   // advance atlas cursor
   m_AtlasX += bmp_w + 2;
   m_AtlasCLH = std::max( m_AtlasCLH, bmp_h + 2 );

   // debugImage
//   auto dbg = glyph.ref.m_rect;
//   glyph.debugImg = Image( dbg.w()+2, dbg.h()+2 );
//   glyph.debugImg.fill(0);
//   FontTTF_Utils::drawFtBitmap( glyph.debugImg, 0, 0, ftGlyph->bitmap );

   //DE_DEBUG("unicode(",unicode,"), index(",index,")")
   //std::cout << ANSIConsole_drawImage( glyph.debugImg ) << std::endl;
   // dbSaveImage( glyph.copyImage(), glyph.getGlyphString() + ".jpg" );

   // DE_DEBUG( "Glyph[", char( glyph.unicode & 0xFF ), "] ", glyph.toString() )
   return glyph;
}

bool
FontTTF::cacheString( std::wstring const & msg )
{
   if (!m_IsOpen)
   {
       DE_ERROR("Not open.")
      return false;
   }

   //DE_DEBUG("msg(",dbStr(msg),")")

   bool needUpload = false;
   for ( size_t i = 0; i < msg.size(); ++i )
   {
      uint32_t unicode = uint32_t( msg[ i ] );
      if ( !hasGlyph( unicode ) )
      {
         m_glyphCache[ unicode ] = createGlyph( unicode );
          needUpload = true;
      }
   }

   if (needUpload)
   {
       m_isDirty = true;
   }
   return needUpload;
}

//bool
//FontTTF::cacheString( std::wstring const & msg, std::vector< Glyph > & out )
//{
//   if (!m_IsOpen) return false;

//   bool needUpdate = false;

//   out.clear();
//   out.reserve( msg.size() );

//   for ( size_t i = 0; i < msg.size(); ++i )
//   {
//      uint32_t unicode = uint32_t( msg[ i ] );
//      bool cacheHit;
//      out.emplace_back( getGlyph( unicode, &cacheHit ) );
//      if ( !cacheHit )
//      {
//         needUpdate = true;
//      }
//   }

//   if ( needUpdate )
//   {
//      // DE_DEBUG("Need update after caching (", dbStr( msg ), ")" )
//   }

//   return needUpdate;
//}



glm::ivec2
FontTTF::getKerning( uint32_t prev, uint32_t curr ) const
{
   if ( !m_ftFace ) return {};
   if ( !prev ) return {};
   if ( !curr ) return {};
   return FontTTF_Utils::getKerning( m_ftFace, prev, curr );
}

Image
FontTTF::createGlyphImage( uint32_t unicode, uint32_t textColor, uint32_t fillColor )
{
   TextSize textSize = getGlyphSize( unicode );
   Image img( textSize.width, textSize.height );
   img.fill( fillColor );
   drawGlyph( img, 0, 0, unicode, textColor );
   return img;
}

TextSize
FontTTF::getGlyphSize( uint32_t unicode )
{
   Glyph const & glyph = getGlyph( unicode );
   int32_t m = glyph.bmp.y;
   int32_t n = glyph.bmp.h - glyph.bmp.y;
   int32_t lineWidth = glyph.advance;
   int32_t lineHeight = n + m + 2;
   int32_t baselineY = m+1;
   return { lineWidth, lineHeight, baselineY, lineHeight, 1 };
}

void
FontTTF::drawGlyph( Image & img, int x, int y, uint32_t unicode, uint32_t color )
{
   if (img.empty() || img.w() < 2 || img.h() < 2 ) return;

   Glyph glyph = getGlyph( unicode );

   if ( !glyph.glyph_index )
   {
      DE_ERROR("No glyph_index for unicode(", unicode, ")" )
      return;
   }

   int w = glyph.w();
   int h = glyph.h();
   if ( w < 1 || h < 1 )
   {
      return;
   }

   #if 1
   int32_t x1 = x;
   int32_t y1 = y;
   int32_t x2 = x + w - 1;
   int32_t y2 = y + h - 1;
   ImagePainter::drawRectBorder( img, Recti::fromPoints(x1, y1, x2, y2), 0xFF505050 );
   #endif

   // DE_DEBUG("Glyph[", char(unicode),"] = bmp_w(", bmp_w, "), bmp_h(", bmp_h, ")" )

   int32_t r = dbRGBA_R( color );
   int32_t g = dbRGBA_G( color );
   int32_t b = dbRGBA_B( color );

   for ( int32_t j = 0; j < h; ++j )
   {
      for ( int32_t i = 0; i < w; ++i )
      {
         int32_t a = dbRGBA_R( glyph.getPixel( i,j ) );

         img.setPixel( x + i, y + j, dbRGBA( r,g,b,a ), true );
      }
   }
}

Image
FontTTF::createTextImage( std::wstring const & txt, uint32_t textColor, uint32_t fillColor )
{
   TextSize ts = getTextSize( txt );
   Image img( ts.width, ts.height );
   img.fill( fillColor );
   drawText( img, 0, 0, txt, textColor );
   return img;
}


TextSize
FontTTF::getTextSize( std::wstring const & msg )
{
//   int32_t m1 = getGlyph( uint32_t('g') ).bmp_y;
//   int32_t m2 = getGlyph( uint32_t('j') ).bmp_y;
//   int32_t m3 = getGlyph( uint32_t('_') ).bmp_y;
//   int32_t m4 = getGlyph( uint32_t('W') ).bmp_y;
//   int32_t m5 = getGlyph( uint32_t('q') ).bmp_y;
//   int32_t m = std::max( m1, std::max( m2, std::max( m3, std::max( m4, m5 ))));
//   int32_t n1 = getGlyph( uint32_t('g') ).bmp_h - getGlyph( uint32_t('g') ).bmp_y;
//   int32_t n2 = getGlyph( uint32_t('j') ).bmp_h - getGlyph( uint32_t('j') ).bmp_y;
//   int32_t n3 = getGlyph( uint32_t('_') ).bmp_h - getGlyph( uint32_t('_') ).bmp_y;
//   int32_t n4 = getGlyph( uint32_t('W') ).bmp_h - getGlyph( uint32_t('W') ).bmp_y;
//   int32_t n5 = getGlyph( uint32_t('q') ).bmp_h - getGlyph( uint32_t('q') ).bmp_y;
//   int32_t n = std::max( n1, std::max( n2, std::max( n3, std::max( n4, n5 ))));
   int32_t textWidth = 0;
   int32_t textHeight = 0;
   int32_t baselineMax = 0;
   int32_t lineHeightMax = 0;

   int32_t lineWidth = 0;
   int32_t lineHeight = 0;

   int32_t m = 0;
   int32_t n = 0;

   int32_t lineCount = 1;

   for ( size_t i = 0; i < msg.size(); ++i )
   {
      uint32_t unicode = msg[ i ];

      if ( unicode == '\r' )  // Mac or Windows line breaks.
      {
         textHeight += lineHeight;
         lineWidth = 0;
         lineHeight = 0;
         lineCount++;
         continue;
      }
      if ( unicode == '\n' )	// Mac or Windows line breaks.
      {
         textHeight += lineHeight;
         lineWidth = 0;
         lineHeight = 0;
         lineCount++;
         continue;
      }

      Glyph & glyph = getGlyph( unicode );

      int32_t curr_m = glyph.bmp.y;
      int32_t curr_n = glyph.bmp.h - glyph.bmp.y;

      m = std::max( m, curr_m );
      n = std::max( n, curr_n );

      lineWidth += glyph.advance;
      lineHeight = n + m + 2;
      int32_t baselineY = m+1;

      textWidth = std::max( textWidth, lineWidth );
      baselineMax = std::max( baselineMax, baselineY );
      lineHeightMax = std::max( lineHeightMax, lineHeight );
   }
   if ( textHeight < 1 )
   {
      textHeight = std::max( textHeight, lineHeightMax );
   }

   TextSize ts{ textWidth, textHeight, baselineMax, lineHeightMax, lineCount };
   // DE_DEBUG( "TextSize(", ts.toString(), ")" )
   return ts;
}

void
FontTTF::drawText( Image & img, int x, int y, std::wstring const & msg,
                    uint32_t color, Align align )
{
   if (img.empty() || img.w() < 2 || img.h() < 2 ) return;

   cacheString( msg );
   TextSize ts = getTextSize( msg );

   //DE_DEBUG("TextSize = ", ts.toString() )
   // glm::ivec2 img_size = img.getSize();
   // glm::ivec2 txt_size = getTextSize( txt );
   // Recti r_patch = Align::applyTextAlign( Recti( x, y ), align, txt_size );

   // ALIGN POS
   int32_t w = ts.width;
   int32_t h = ts.height;

   glm::ivec2 pos(0,0);

   // horizontal align to entire screen
   if ( align & Align::Left )
   {
      pos.x = x;
   }
   else if ( align & Align::Center )
   {
      pos.x = x - w/2;
   }
   else if ( align & Align::Right )
   {
      pos.x = x - w;
   }

   // vertical align to entire screen
   if ( align & Align::Top )
   {
      pos.y = y;
   }
   else if ( align & Align::Middle )
   {
      pos.y = y - h/2;
   }
   else if ( align & Align::Bottom )
   {
      pos.y = y - h;
   }

#if 0
   int32_t x1 = pos.x;
   int32_t y1 = pos.y;
   int32_t x2 = pos.x + w;
   int32_t y2 = pos.y + h;
   ImagePainter::drawOutlineRect( img, x1, y1, x2, y2, 0xFFFF6FFF );
#endif

   int cx = pos.x;
   int cy = pos.y + ts.baseline;

   uint32_t last_unicode = 0;

   for ( size_t i = 0; i < msg.size(); ++i )
   {
      uint32_t unicode = msg[ i ];

      //drawGlyph( img, px + kerning.x, py + kerning.y, unicode, color );
      if ( unicode == '\n' || unicode == '\r' )
      {
         cx = pos.x;
         cy += ts.lineHeight;
         continue;
      }

      glm::ivec2 kerning(0,0);
      if ( i > 0 )
      {
         kerning = getKerning( last_unicode, unicode );
      }

      last_unicode = unicode;

      // Draw glyph, if there is something to draw.
      Glyph & glyph = getGlyph( unicode );
      if ( glyph.glyph_index )
      {
         int bmp_w = glyph.w();
         int bmp_h = glyph.h();
         if ( bmp_w > 0 && bmp_h > 0 )
         {
            int bmp_x = cx + glyph.bmp.x + kerning.x;
            int bmp_y = cy - glyph.bmp.y;

            // DE_DEBUG("Glyph[", char(unicode),"] = bmp_w(", bmp_w, "), bmp_h(", bmp_h, ")" )
         #if 0
            int32_t x1 = x;
            int32_t y1 = y;
            int32_t x2 = x + bmp_w;
            int32_t y2 = y - bmp_h;
            ImagePainter::drawOutlineRect( img, x1, y1, x2, y2, 0xFFFF6FFF );
         #endif

            int32_t r = dbRGBA_R( color );
            int32_t g = dbRGBA_G( color );
            int32_t b = dbRGBA_B( color );

            for ( int32_t j = 0; j < bmp_h; ++j )
            {
               for ( int32_t i = 0; i < bmp_w; ++i )
               {
                  int32_t a = dbRGBA_R( glyph.getPixel( i,j ) );

                  img.setPixel( bmp_x + i, bmp_y + j, dbRGBA( r,g,b,a ), true );
               }
            }
         }
         else
         {
            if ( glyph.advance == 0 )
            {
               DE_WARN("Got empty bitmap with advance = 0 for glyph ",glyph.toString())
            }

         }
      }
      else
      {
         DE_DEBUG("No glyph_index for unicode(", unicode, "), glyph(",glyph.toString(),")")
      }

      cx += glyph.advance;
   }
}


void
FontTTF_Utils::drawFtBitmap( Image & dst, int dst_x, int dst_y, FT_Bitmap const & bmp )
{
   if ( !bmp.buffer )
   {
      // DE_ERROR("No bitmap was rendered")
      return;
   }

   int32_t const w = int32_t( bmp.width );
   int32_t const h = int32_t( bmp.rows );
   uint8_t const* data = bmp.buffer; // A source ptr to play with.
   //uint8_t const * src = bmp.buffer;
   // DE_DEBUG("FT_PIXEL_MODE_GRAY, gray_count = ",gray_count)

   // uint8_t r = rand() % 256;
   // uint8_t g = rand() % 256;
   // uint8_t b = rand() % 256;

   uint32_t bgColor = 0x00000000; // dbRGBA(r,g,b); // 0x00000000; // for debug tex-coords into atlas i used 0xFF404040

   switch ( bmp.pixel_mode )
   {
   case FT_PIXEL_MODE_GRAY: // Load grayscale data in.
      {
         //DE_DEBUG("FT_PIXEL_MODE_GRAY")
         int32_t const n = int32_t( bmp.num_grays );
         float f = 255.0f / float( n );

         for ( int32_t y = 0; y < h; ++y )
         {
            uint8_t const* row = data; // curr data ptr is given by pitch aligned 'p'.
            for ( int32_t x = 0; x < w; ++x )
            {
               uint8_t byte = *row;
               int a = std::clamp( int( f * float( byte ) ), 0, 255 );
               if ( a > 0 )
               {
                  dst.setPixel( dst_x + x, dst_y + y, dbRGBA( a,a,a,a ) );
               }
               else
               {
                  dst.setPixel( dst_x + x, dst_y + y, bgColor );
               }

               ++row;
            }
            data += bmp.pitch;
         }
      }
      break;
   case FT_PIXEL_MODE_MONO:
      {
         //DE_DEBUG("FT_PIXEL_MODE_MONO")

         for ( int32_t y = 0; y < h; ++y )
         {
            uint8_t const* bytePtr = data; // curr data ptr is given by pitch aligned 'p'.

            for ( int32_t x = 0; x < bmp.pitch; ++x )
            {
               uint8_t byteRef = *bytePtr;
               for ( int32_t bit = 0; bit < 8; ++bit )
               {
                  uint8_t bit_mask = 0x80 >> bit;
                  if ( byteRef & bit_mask )
                  {
                     dst.setPixel( dst_x + 8*x + bit, dst_y + y, 0xFFFFFFFF );
                  }
                  else
                  {
                     dst.setPixel( dst_x + 8*x + bit, dst_y + y, bgColor );
                  }
               }

               ++bytePtr;
            }

            data += bmp.pitch;
         }
      }
      break;
   case FT_PIXEL_MODE_LCD:
      // throw std::runtime_error("Unsupported FT_PIXEL_MODE_LCD");
      {
         //DE_DEBUG("FT_PIXEL_MODE_GRAY")
         int32_t const n = int32_t( bmp.num_grays );
         float f = 255.0f / float( n );

         for ( int32_t y = 0; y < h; ++y )
         {
            uint8_t const* row = data; // curr data ptr is given by pitch aligned 'p'.
            for ( int32_t x = 0; x < w; ++x )
            {
               uint8_t byte = *row;
               int a = std::clamp( int( f * float( byte ) ), 0, 255 );
               if ( a > 0 )
               {
                  dst.setPixel( dst_x + x, dst_y + y, dbRGBA( a,a,a,a ) );
               }
               else
               {
                  dst.setPixel( dst_x + x, dst_y + y, bgColor );
               }

               ++row;
            }
            data += bmp.pitch;
         }
      }
      break;
   default:
      throw std::runtime_error("Unsupported FT_PIXEL_MODE?");
      break;
   }
}

bool
FontTTF_Utils::hasOutline( FT_Face face )
{
   if ( !face ) return false;
   FT_GlyphSlot glyph = face->glyph;
   if ( !glyph ) return false; // Should never happen.
   if ( glyph->format != FT_GLYPH_FORMAT_OUTLINE ) return false; // Should never happen.  Just an extra check.
   FT_Outline & outline = glyph->outline;
   if ( outline.n_contours <= 0 || outline.n_points <= 0 ) return false; // Can happen for some font files.
   FT_Error const e = FT_Outline_Check( &outline );
   return (e == 0);
}

bool
FontTTF_Utils::flipOutline( FT_Face face )
{
   if ( !face ) return false;
   FT_Fixed const multiplier = 65536L;
   FT_Matrix matrix;
   matrix.xx = 1L * multiplier;
   matrix.xy = 0L * multiplier;
   matrix.yx = 0L * multiplier;
   matrix.yy = -1L * multiplier;
   FT_GlyphSlot glyph = face->glyph;
   if ( !glyph ) return false;
   FT_Outline & outline = glyph->outline;
   FT_Outline_Transform( &outline, &matrix );
   return true;
}

Recti
FontTTF_Utils::computeOutlineBBox( FT_Face face )
{
   if ( !face ) return {};
   FT_GlyphSlot glyph = face->glyph;
   if ( !glyph ) return {};
   FT_Outline & outline = glyph->outline;
   FT_BBox bbox;
   FT_Error e = FT_Outline_Get_BBox( &outline, &bbox );
   if (e != 0)
   {
      DE_ERROR("Failed FT_Outline_Get_BBox")
      return {};
   }

   // DE_DEBUG("bbox( xMin:",bbox.xMin,", yMin:",bbox.yMin,", xMax:",bbox.xMax,", yMax:",bbox.yMax," )")
   return Recti::fromPoints( int32_t( bbox.xMin ), int32_t( bbox.yMin ),
                             int32_t( bbox.xMax ), int32_t( bbox.yMax ) );
}

bool
FontTTF_Utils::setPixelSize( FT_Face face, int pixelSize )
{
   if ( !face ) return false;
   FT_Error e = FT_Set_Pixel_Sizes( face, 0, pixelSize );
   if ( e )
   {
      DE_ERROR("Cant set pixelSize(", pixelSize, ")")
      return false;
   }
   //DE_DEBUG("pixelHeight(", pixelHeight, ")")
   return true;
}

uint32_t
FontTTF_Utils::getGlyphIndex( FT_Face face, uint32_t unicode )
{
   if ( !face )
   {
      DE_ERROR("No ftFace. unicode(",unicode,")")
      return 0;
   }

   uint32_t index = FT_Get_Char_Index( face, FT_ULong( unicode ) );
/*
   if ( index == 0 )
   {
      DE_DEBUG("No glyph_index. unicode(",unicode,")")
   }
   else
   {
      // DE_DEBUG("glyph_index(", index,"), unicode(",unicode,")" )
   }
*/
   return index;
}

glm::ivec2
FontTTF_Utils::getKerning( FT_Face face, uint32_t prev, uint32_t curr )
{
   if ( !face || !FT_HAS_KERNING( face ) ) return {};
   if ( prev < 1 || curr == 0 ){ return {}; }

   uint32_t iPrev = getGlyphIndex( face, prev );
   uint32_t iCurr = getGlyphIndex( face, curr );

   FT_Vector ftKerning;
   FT_Get_Kerning( face, iPrev, iCurr, FT_KERNING_DEFAULT, &ftKerning );
   glm::ivec2 kerning( ftKerning.x, ftKerning.y );

//   FT_Vector ftUnfitted;
//   FT_Get_Kerning( face, iPrev, iCurr, FT_KERNING_UNFITTED, &ftUnfitted );
//   glm::ivec2 unfitted( ftUnfitted.x, ftUnfitted.y );

//   FT_Vector ftUnscaled;
//   FT_Get_Kerning( face, iPrev, iCurr, FT_KERNING_UNSCALED, &ftUnscaled );
//   glm::ivec2 unscaled( ftUnscaled.x, ftUnscaled.y );

//   DE_DEBUG("getKerning(", (prev>=32 ? char( prev ) : '?'), ",",
//                           (curr>=32 ? char( curr ) : '?'), ") -> "
//      "kerning(",kerning,"), "
//      "unfitted(",unfitted,"), "
//      "unscaled(",unscaled,")" )

   if ( FT_IS_SCALABLE( face ) )
   {
      kerning.x /= 64;
      kerning.y /= 64;
   }


//   if ( kerning.x != 0 || kerning.y != 0 )
//   {
//      DE_DEBUG("kerning(",kerning,")")
//   }

   return kerning;
}



} // end namespace de.




// ---------------------------------------------------------------------------
/*
void
FontTTF_Utils::createLibInstance( FT_Library & lib )
{
   if ( lib ) { return; }
   FT_Error e = FT_Init_FreeType( &lib );
   if ( e != 0 )
   {
      DE_ERROR("FT_Init_FreeType() failed.")
      lib = nullptr;
   }
}
void
FontTTF_Utils::destroyLibInstance( FT_Library & lib )
{
   if ( !lib ) { return; }
   FT_Error e = FT_Done_FreeType( lib );
   if ( e != 0 )
   {
      // DE_ERROR("FT_Done_FreeType() failed.")
   }
   lib = nullptr;
}

bool
FontTTF_Utils::isFace( FT_Face face )
{
   return( face != nullptr );
}

void
FontTTF_Utils::closeFace( FT_Face & face )
{
   if ( !face ) { return; } // Already closed.

   // DE_DEBUG("Close face (", face, ")")
   FT_Error e = FT_Done_Face( face );
   if ( e != 0 )
   {
      // DE_ERROR("Got error while closing face(", face, ")" )
   }
   face = nullptr;
}

FT_Face
FontTTF_Utils::openFace( FT_Library lib, std::string const & uri )
{
   if ( !lib ) { DE_ERROR("FT_Library is nullptr") return nullptr; }
   if ( uri.empty() ) { DE_ERROR("FileName is empty") return nullptr; }

   FT_Face face;
   FT_Error e = FT_New_Face( lib, uri.c_str(), 0, &face );
   if ( e == FT_Err_Unknown_File_Format )
   { DE_ERROR("FT_New_Face() got unknown format") return nullptr; }
   else if ( e )
   { DE_ERROR("FT_New_Face() failed") return nullptr; }

   if ( !face ) { DE_ERROR("Cant open font uri(",uri,")") return nullptr; }

   FontTTF_Utils::setPixelSize( face, 16 );
   DE_DEBUG( "Font.Scalable = ", FT_IS_SCALABLE( face ) ? "1" : "0" )
   DE_DEBUG( "Font.Kerning = ", FT_HAS_KERNING( face ) ? "1" : "0" )
   DE_DEBUG( "Font.Outline = ", FontTTF_Utils::hasOutline( face ) ? "1" : "0" )

   int32_t ascender = 0;
   int32_t descender = 0;

   if ( face->size )
   {
      ascender = face->size->metrics.ascender;
      descender = face->size->metrics.descender;
      if ( FT_IS_SCALABLE( face ) )
      {
         ascender /= 64;
         descender /= 64;
      }
   }

   DE_DEBUG("Font.Ascender = ", ascender )
   DE_DEBUG("Font.Descender = ", descender )

   return face;
}
*/
// ---------------------------------------------------------------------------



/*
bool
FontTTF_Utils::setPointSize( FT_Face face, int pointHeight, int dpi )
{
   if ( !face ) return false;
   FT_Error e = FT_Set_Char_Size( face, // handle to face object
                  0,       // char_width in 1/64th of points, A point is 1/72th of an inch.
                  pointHeight * 64,   // char_height in 1/64th of points
                  dpi,  // horizontal device resolution in [dpi] dots-per-inch.
                  dpi   // vertical device resolution in [dpi]
   );
   if ( e )
   {
      DE_ERROR("Cant set pointHeight(", pointHeight, ")")
      return false;
   }
   else
   {
      // DE_DEBUG("pointHeight(", pointHeight, ")")
      return true;
   }
}


Glyph
FontTTF_Utils::drawFtBitmapOnImage( Image & img, int x, int y, uint32_t unicode )
{
image::Image
FontTTF_Utils::createGlyphImage( FT_Face face, uint32_t unicode, uint32_t pixelSize )
{
   image::Image img;
   //DE_DEBUG("unicode(",unicode,").")
   // For simplicity, use the charmap FreeType provides by default in most cases this means Unicode.
   FT_UInt index = getGlyphIndex( face,unicode );
   if ( index == 0 )
   {
      // its not a bug for white spaces and unprintable characters like margins, paddings
      // it would spam the logs printing errors for white spaces.
      // DE_ERROR("Invalid index. unicode(",unicode,")")
      return img;
   }

   // This is because we cache faces and the face may have been set to a different size.
   setPixelSize( pixelSize );

   patch.unicode = unicode;

   // FT_LOAD_DEFAULT
   FT_Error e = FT_Load_Glyph( face, index, FT_LOAD_RENDER ); // FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP
   if ( e )
   {
      DE_ERROR("FT_Load_Glyph error. unicode(",unicode,")")
      return patch;
   }

   FT_GlyphSlot ftGlyph = face->glyph;
   if ( !ftGlyph )
   {
      DE_ERROR("Glyph access error. unicode(",unicode,")")
      return patch;
   }

   // Finally we store a valid index.


   FT_Glyph_Metrics const & ftGlyphMetrics = ftGlyph->metrics;
   glyph.hBearing.x = ftGlyphMetrics.horiBearingX;
   glyph.hBearing.y = ftGlyphMetrics.horiBearingY;
   glyph.vBearing.x = ftGlyphMetrics.vertBearingX;
   glyph.vBearing.y = ftGlyphMetrics.vertBearingY;

   FT_Vector advance = ftGlyph->advance;
   glyph.advance.x = advance.x;
   glyph.advance.y = advance.y;
   glyph.bbox = computeOutlineBBox();

   glyph.offset.x = ftGlyph->bitmap_left;
   glyph.offset.y = ftGlyph->bitmap_top;

   // Now raster glyph to image and convert image to the dark side.
   FT_Bitmap const & bmp = ftGlyph->bitmap;
   FT_Pos const w = bmp.width;
   FT_Pos const h = bmp.rows;

   // Fill (my) glyph atlas struct and create (my) glyph image from ftBitmap.

   if ( !bmp.buffer )
   {
      DE_ERROR("No bitmap was rendered")
   }
   else
   {
      Image img( w, h );

      bool ok = true;
      switch ( bmp.pixel_mode )
      {
      // Load the grayscale data in.
      case FT_PIXEL_MODE_GRAY:
         {
            float const gray_count = float( bmp.num_grays );
            // DE_DEBUG("FT_PIXEL_MODE_GRAY, gray_count = ",gray_count)

            uint8_t const * src = bmp.buffer;
            for ( int32_t y = 0; y < int32_t( bmp.rows ); ++y )
            {
               uint8_t const * row = src;
               for ( int32_t x = 0; x < int32_t( bmp.width ); ++x )
               {
                  float const t = float( *row ) * ( 255.0f / float( gray_count ) );
                  uint8_t const gray = uint8_t( Math::clampT< int >( int(t+0.5f), 0, 255 ) );
                  img.setPixel( x,y, RGBA( 255,255,255,gray ) );
                  ++row;
               }
               src += bmp.pitch;
            }
         }
         break;
      case FT_PIXEL_MODE_MONO:
         {
         DE_ERROR("FT_PIXEL_MODE_MONO")
         //            // Create a blank image and fill it with transparent pixels.
         //            texture_size = d.getOptimalSize(true, true);
         //            image = driver->createImage(ECF_A1R5G5B5, texture_size);
         //            image->fill(SColor(0, 255, 255, 255));

         //            // Load the monochrome data in.
         //            const u32 image_pitch = image->getPitch() / sizeof(u16);
         //            u16* image_data = (u16*)image->lock();
         //            u8* glyph_data = bmp.buffer;
         //            for (u32 y = 0; y < bmp.rows; ++y)
         //            {
         //                u16* row = image_data;
         //                for (u32 x = 0; x < bmp.width; ++x)
         //                {
         //                    // Monochrome bitmaps store 8 pixels per byte.  The left-most pixel is the bit 0x80.
         //                    // So, we go through the data each bit at a time.
         //                    if ((glyph_data[y * bmp.pitch + (x / 8)] & (0x80 >> (x % 8))) != 0)
         //                        *row = 0xFFFF;
         //                    ++row;
         //                }
         //                image_data += image_pitch;
         //            }
         //            image->unlock();
         }
         break;
      default:
         {
         DE_ERROR("FT_PIXEL_MODE_UNKNOWN")
         ok = false;
         }
         break;
      }

      if ( ok )
      {
         glyph.img = std::move( img );
      }
      else
      {
         DE_ERROR("OMG")
      }
   }

   if ( FT_IS_SCALABLE( face ) )
   {
      glyph.hBearing /= 64;
      glyph.vBearing /= 64;
      glyph.advance /= 64;
      glyph.bbox /= 64;
      //glyph.offset /= 64;
      // DE_DEBUG( "New scalable Glyph: ", glyph.toString() )
   }
   else
   {
      // DE_DEBUG( "New fixed Glyph: ", glyph.toString() )
   }

   // return up_glyph;

   if ( patch.ok() )
   {
      m_Glyphs.emplace_back( patch );
      patch = m_Glyphs.back();
   }
   else
   {
      DE_ERROR("Cannot create glyph unicode(", unicode,")")
   }

   return glyph;
}

glm::ivec2
FontTTF_Utils::getGlyphSizePx( FT_Face face, uint32_t unicode )
{
   if ( !face ) return false;
   FT_GlyphSlot glyph = face->glyph;
   if ( !glyph ) return false; // Should never happen.

   if ( !glyph ) return glm::ivec2(0,0);
   int32_t w = glyph->advance.x;
   int32_t h = m_ftMetrics.ascender;   // Grab the true height of the character, taking into account underhanging glyphs.
   if ( FT_IS_SCALABLE( face ) )
   {
      h /= 64;
   }
   h += -glyph->offset.y + glyph->getImageSize().y;

   return glm::ivec2( w, h );
}


uint32_t
FontTTF_Utils::getCharacterWidthPx( uint32_t unicode )
{
   Glyph glyph = getGlyph( unicode );
   return glyph.advance.x;
//   if ( unicode >= 0x2000 )
//   {
//   return (m_ftMetrics.ascender / 64);
//   }
//   else
//   {
//   return (m_ftMetrics.ascender / 64) / 2;
//   }
}

uint32_t
FontTTF_Utils::getCharacterHeightPx( uint32_t unicode )
{
   // Grab the true height of the character, taking into account underhanging glyphs.
   Glyph glyph = getGlyph( unicode );
   int32_t h = m_ftMetrics.ascender;
   if ( FT_IS_SCALABLE( face ) )
   {
      h /= 64;
   }
   h += -glyph.offset.y + glyph.getSize().y;
   return h;
}



// ===========================================================================
// Lib
// ===========================================================================

// Reference:
// https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html
// FT_Library object across threads is possible also,
// as long as a mutex lock is used around FT_New_Face and FT_Done_Face.

struct FreeTypeLib
{
   DE_CREATE_LOGGER("de.freetype3.Lib")
   FT_Library lib;
   FreeTypeLib() : lib( nullptr )
   {
      init();
   }

   ~FreeTypeLib()
   {
      deinit();
   }

   static FreeTypeLib &
   getInstance()
   {
      static FreeTypeLib s_Instance;
      return s_Instance;
   }

   FT_Library &
   getLibrary()
   {
      return lib;
   }

   void
   deinit()
   {
      if ( !lib )
      {
         return; // nothing todo
      }
      FT_Error e = FT_Done_FreeType( lib );
      if ( e != 0 )
      {
         std::cout << "[Error] while shutting down FT_Library\n";
      }
      lib = nullptr;
   }

   void
   init()
   {
      if ( lib )
      {
         // std::cout << "[Error] FT_Library already initialized.\n";
         return;
      }
      FT_Error e = FT_Init_FreeType( &lib );
      if ( e != 0 )
      {
         std::cout << "[Error] initializing FT_Library\n";
         lib = nullptr;
      }
   }

   FT_Library getLib()
   {
      init();
      return lib;
   }
};

TextSize
FontTTF::getTextSizeMultiLine( std::wstring msg )
{
   //dbStrReplace( msg, L"\\", L"/");
   dbStrReplace( msg, L"\n\r", L"\n");
   dbStrReplace( msg, L"\r\n", L"\n");
   std::vector< std::wstring > lines = dbStrSplit( msg, L'\n');

   TextSize retVal;

   for ( size_t i = 0; i < lines.size(); ++i )
   {
      auto ts = getTextSize( lines[ i ] );

      // LeftToRight, TopToBottom -> German reading
      retVal.width = std::max( retVal.width, ts.width );
      retVal.height += ts.height;
   }
}

void
FontTTF::drawTextMultiLine(
   Image & img, int x, int y, std::wstring msg,
   uint32_t color, Align align )
{
   //dbStrReplace( msg, L"\\", L"/");
   dbStrReplace( msg, L"\n\r", L"\n");
   dbStrReplace( msg, L"\r\n", L"\n");
   std::vector< std::wstring > lines = dbStrSplit( msg, L'\n');

   for ( size_t i = 0; i < lines.size(); ++i )
   {
      drawText( img, x, y, lines[ i ], color, align );

      auto ts = getTextSize( lines[ i ] );

      y += ts.height;
   }
}

TextSize
FontTTF::getTextSize( std::wstring const & msg )
{
//   int32_t m1 = getGlyph( uint32_t('g') ).bmp_y;
//   int32_t m2 = getGlyph( uint32_t('j') ).bmp_y;
//   int32_t m3 = getGlyph( uint32_t('_') ).bmp_y;
//   int32_t m4 = getGlyph( uint32_t('W') ).bmp_y;
//   int32_t m5 = getGlyph( uint32_t('q') ).bmp_y;
//   int32_t m = std::max( m1, std::max( m2, std::max( m3, std::max( m4, m5 ))));
//   int32_t n1 = getGlyph( uint32_t('g') ).bmp_h - getGlyph( uint32_t('g') ).bmp_y;
//   int32_t n2 = getGlyph( uint32_t('j') ).bmp_h - getGlyph( uint32_t('j') ).bmp_y;
//   int32_t n3 = getGlyph( uint32_t('_') ).bmp_h - getGlyph( uint32_t('_') ).bmp_y;
//   int32_t n4 = getGlyph( uint32_t('W') ).bmp_h - getGlyph( uint32_t('W') ).bmp_y;
//   int32_t n5 = getGlyph( uint32_t('q') ).bmp_h - getGlyph( uint32_t('q') ).bmp_y;
//   int32_t n = std::max( n1, std::max( n2, std::max( n3, std::max( n4, n5 ))));

   int32_t lineWidth = 0;
   int32_t m = 0;
   int32_t n = 0;

   for ( size_t i = 0; i < msg.size(); ++i )
   {
      Glyph const & glyph = getGlyph( msg[ i ] );

      //   if ( unicode1 == '\r' )	// Mac or Windows line breaks.
      //   {
      //      continue;
      //   }
      //   if ( unicode1 == '\n' )	// Mac or Windows line breaks.
      //   {
      //      continue;
      //   }

      int32_t curr_m = glyph.bmp.getY();
      int32_t curr_n = glyph.bmp.getHeight() - glyph.bmp.getY();

      m = std::max( m, curr_m );
      n = std::max( n, curr_n );

      lineWidth += glyph.advance;
   }

   int32_t lineHeight = n + m + 2;
   int32_t baselineY = m+1;

   // DE_DEBUG( "msg(",dbStr( msg ),") has pixels ", lineWidth, " x ", lineHeight, ", baselineY = ", baselineY )
   return { lineWidth, lineHeight, baselineY };
}

void
FontTTF::drawText( Image & img, int x, int y, std::wstring const & msg,
                    uint32_t color, Align align )
{
   if (img.empty()) return;
   int img_w = img.getWidth();
   int img_h = img.getHeight();
   if ( img_w < 2 || img_h < 2 ) return;

   std::vector< Glyph > glyphs;
   cacheString( msg, glyphs );

   TextSize textSize = getTextSize( msg );

   // glm::ivec2 img_size = img.getSize();
   // glm::ivec2 txt_size = getTextSize( txt );
   // Recti r_patch = Align::applyTextAlign( Recti( x, y ), align, txt_size );

#if 1 // ALIGN POS
   int32_t w = textSize.width;
   int32_t h = textSize.height;

   glm::ivec2 aligned_pos;

   // horizontal align to entire screen
   if ( align & Align::Left )
   {
      aligned_pos.x = x;
   }
   else if ( align & Align::Center )
   {
      aligned_pos.x = x - w/2;
   }
   else if ( align & Align::Right )
   {
      aligned_pos.x = x - w;
   }

   // vertical align to entire screen
   if ( align & Align::Top )
   {
      aligned_pos.y = y;
   }
   else if ( align & Align::Middle )
   {
      aligned_pos.y = y - h/2;
   }
   else if ( align & Align::Bottom )
   {
      aligned_pos.y = y - h;
   }

   int px = aligned_pos.x;
   int py = aligned_pos.y;
#endif

#if 1
   int32_t x1 = px;
   int32_t y1 = py;
   int32_t x2 = px + w;
   int32_t y2 = py + h;
   ImagePainter::drawOutlineRect( img, x1, y1, x2, y2, 0xFFFF6FFF );
#endif

   for ( size_t i = 0; i < glyphs.size(); ++i )
   {
      Glyph const & glyph = glyphs[ i ];
      uint32_t unicode = glyph.unicode;

      //drawGlyph( img, px + kerning.x, py + kerning.y, unicode, color );

      glm::ivec2 kerning(0,0);
      if ( i > 0 )
      {
         Glyph const & prev = glyphs[ i - 1 ];
         kerning = getKerning( prev.unicode, unicode );
      }

      if ( !glyph.glyph_index )
      {
         DE_ERROR("No glyph_index for unicode(", unicode, ")" )
         continue;
      }

      int bmp_y = glyph.bmp.getY();
      int bmp_w = glyph.bmp.getWidth();
      int bmp_h = glyph.bmp.getHeight();
      if ( bmp_w > 0 && bmp_h > 0 )
      {
         py = aligned_pos.y + textSize.baseline - bmp_y;

         // DE_DEBUG("Glyph[", char(unicode),"] = bmp_w(", bmp_w, "), bmp_h(", bmp_h, ")" )
      #if 0
         int32_t x1 = x;
         int32_t y1 = y;
         int32_t x2 = x + bmp_w;
         int32_t y2 = y - bmp_h;
         ImagePainter::drawOutlineRect( img, x1, y1, x2, y2, 0xFFFF6FFF );
      #endif

         int32_t r = RGBA_R( color );
         int32_t g = RGBA_G( color );
         int32_t b = RGBA_B( color );

         for ( int32_t j = 0; j < bmp_h; ++j )
         {
            for ( int32_t i = 0; i < bmp_w; ++i )
            {
               int32_t a = RGBA_R( glyph.getPixel( i,j ) );

               img.setPixel( px + i, py + j, RGBA( r,g,b,a ), true );
            }
         }
      }

      px += glyph.advance;
      px += kerning.x;
   }
}


// ===========================================================================
// Lib
// ===========================================================================
struct FontLib
{
   DE_CREATE_LOGGER("de.FontLib")

   FontLib()
      : m_ftLib( nullptr )
   {
      if ( !m_ftLib )
      {
         FT_Error e = FT_Init_FreeType( &m_ftLib );
         if ( e != 0 )
         {
            DE_ERROR("FT_Init_FreeType() failed.")
            // throw std::runtime_error("FT_Init_FreeType() failed in FontTTF.");
            m_ftLib = nullptr;
         }
      }
   }

   ~FontLib()
   {
      if ( m_ftLib )
      {
         FT_Error e = FT_Done_FreeType( m_ftLib );
         if ( e != 0 )
         {
            DE_ERROR("FT_Done_FreeType() failed.")
         }
         m_ftLib = nullptr;
      }
   }

   static std::shared_ptr< FontLib >
   getInstance()
   {
      static std::shared_ptr< FontLib > s_Instance = std::make_shared< FontLib >();
      return s_Instance;
   }

   void
   closeFace( FT_Face & face )
   {
      //FontTTF_Utils::closeFace( face );
      if ( !face ) { return; } // Already closed.

      if ( !face->driver )
      {
         DE_ERROR("No face driver, face(", face, ")" )
         return;
      }

      if ( !face->internal )
      {
         DE_ERROR("No face internal, face(", face, ")" )
         return;
      }

      if ( face->internal->refcount < 1 )
      {
         DE_ERROR("FontTTF refcount already zero while closing face(", face, ")" )
         return;
      }

      // DE_DEBUG("Close face (", face, ")")
      FT_Error e = FT_Done_Face( face );
      if ( e != 0 )
      {
         // DE_ERROR("Got error while closing face(", face, ")" )
      }
      face = nullptr;
   }


   FT_Face
   openFace( std::string const & uri )
   {
      //return FontTTF_Utils::openFace( m_ftLib, uri );
      if ( !m_ftLib ) { return nullptr; }
      if ( uri.empty() ) { return nullptr; }

      FT_Face face;
      FT_Error e = FT_New_Face( m_ftLib, uri.c_str(), 0, &face );
      if ( e == FT_Err_Unknown_File_Format )
      { DE_ERROR("FT_New_Face() got unknown format") return nullptr; }
      else if ( e )
      { DE_ERROR("FT_New_Face() failed") return nullptr; }

      if ( !face ) { DE_ERROR("Cant open font uri(",uri,")") return nullptr; }


      // FontTTF_Utils::setPixelSize( face, 16 );
      // DE_DEBUG( "Font.Scalable = ", FT_IS_SCALABLE( face ) ? "1" : "0" )
      // DE_DEBUG( "Font.Kerning = ", FT_HAS_KERNING( face ) ? "1" : "0" )
      // DE_DEBUG( "Font.Outline = ", FontTTF_Utils::hasOutline( face ) ? "1" : "0" )

      // int32_t ascender = 0;
      // int32_t descender = 0;

      // if ( face->size )
      // {
         // ascender = face->size->metrics.ascender;
         // descender = face->size->metrics.descender;
         // if ( FT_IS_SCALABLE( face ) )
         // {
            // ascender /= 64;
            // descender /= 64;
         // }
      // }

      // DE_DEBUG("Font.Ascender = ", ascender )
      // DE_DEBUG("Font.Descender = ", descender )

      return face;
   }

   FT_Library m_ftLib;
};





Glyph
FontTTF::drawFtBitmapOnImage( Image & img, int x, int y, uint32_t unicode )
{
   // Now raster glyph to image and convert image to the dark side.
   FT_Bitmap const & bmp = ftGlyph->bitmap;
   FT_Pos const w = bmp.width;
   FT_Pos const h = bmp.rows;

   // Fill (my) glyph atlas struct and create (my) glyph image from ftBitmap.

   if ( !bmp.buffer )
   {
      DE_ERROR("No bitmap was rendered")
   }
   else
   {
      Image img( w, h );

      bool ok = true;
      switch ( bmp.pixel_mode )
      {
      // Load the grayscale data in.
      case FT_PIXEL_MODE_GRAY:
         {
            float const gray_count = float( bmp.num_grays );
            // DE_DEBUG("FT_PIXEL_MODE_GRAY, gray_count = ",gray_count)

            uint8_t const * src = bmp.buffer;
            for ( int32_t y = 0; y < int32_t( bmp.rows ); ++y )
            {
               uint8_t const * row = src;
               for ( int32_t x = 0; x < int32_t( bmp.width ); ++x )
               {
                  float const t = float( *row ) * ( 255.0f / float( gray_count ) );
                  uint8_t const gray = uint8_t( Math::clampT< int >( int(t+0.5f), 0, 255 ) );
                  img.setPixel( x,y, RGBA( 255,255,255,gray ) );
                  ++row;
               }
               src += bmp.pitch;
            }
         }
         break;
      case FT_PIXEL_MODE_MONO:
         {
         DE_ERROR("FT_PIXEL_MODE_MONO")
         //            // Create a blank image and fill it with transparent pixels.
         //            texture_size = d.getOptimalSize(true, true);
         //            image = driver->createImage(ECF_A1R5G5B5, texture_size);
         //            image->fill(SColor(0, 255, 255, 255));

         //            // Load the monochrome data in.
         //            const u32 image_pitch = image->getPitch() / sizeof(u16);
         //            u16* image_data = (u16*)image->lock();
         //            u8* glyph_data = bmp.buffer;
         //            for (u32 y = 0; y < bmp.rows; ++y)
         //            {
         //                u16* row = image_data;
         //                for (u32 x = 0; x < bmp.width; ++x)
         //                {
         //                    // Monochrome bitmaps store 8 pixels per byte.  The left-most pixel is the bit 0x80.
         //                    // So, we go through the data each bit at a time.
         //                    if ((glyph_data[y * bmp.pitch + (x / 8)] & (0x80 >> (x % 8))) != 0)
         //                        *row = 0xFFFF;
         //                    ++row;
         //                }
         //                image_data += image_pitch;
         //            }
         //            image->unlock();
         }
         break;
      default:
         {
         DE_ERROR("FT_PIXEL_MODE_UNKNOWN")
         ok = false;
         }
         break;
      }

      if ( ok )
      {
         glyph.img = std::move( img );
      }
      else
      {
         DE_ERROR("OMG")
      }
   }

   if ( FT_IS_SCALABLE( m_ftFace ) )
   {
      glyph.hBearing /= 64;
      glyph.vBearing /= 64;
      glyph.advance /= 64;
      glyph.bbox /= 64;
      //glyph.offset /= 64;
      // DE_DEBUG( "New scalable Glyph: ", glyph.toString() )
   }
   else {
      // DE_DEBUG( "New fixed Glyph: ", glyph.toString() )
   }

   return glyph;
}

Glyph
FontTTF::createGlyph( uint32_t unicode )
{
   FT_UInt index = FontTTF_Utils::getGlyphIndex( m_ftFace, unicode );
   if ( index == 0 )
   {
      // its not a bug for white spaces and unprintable characters like margins, paddings
      // it would spam the logs printing errors for white spaces.
      // DE_ERROR("Invalid index. unicode(",unicode,")")
      return nullptr;
   }

   // This is because we cache faces and the face may have been set to a different size.
   FontTTF_Utils::setPixelSize( m_ftFace, m_Font.pixelSize );

   Glyph glyph;
   glyph.unicode = unicode;

   // FT_LOAD_DEFAULT
   FT_Error e = FT_Load_Glyph( m_ftFace, index, FT_LOAD_RENDER ); // FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP
   if ( e )
   {
      DE_ERROR("FT_Load_Glyph error. unicode(",unicode,")")
      return glyph;
   }

   FT_GlyphSlot ftGlyph = m_ftFace->glyph;
   if ( !ftGlyph )
   {
      DE_ERROR("Glyph access error. unicode(",unicode,")")
      return glyph;
   }

   // Finally we store a valid glyph_index.
   glyph.glyph_index = index;

   FT_Glyph_Metrics const & ftGlyphMetrics = ftGlyph->metrics;
   glyph.hBearing.x = ftGlyphMetrics.horiBearingX;
   glyph.hBearing.y = ftGlyphMetrics.horiBearingY;
   glyph.vBearing.x = ftGlyphMetrics.vertBearingX;
   glyph.vBearing.y = ftGlyphMetrics.vertBearingY;

   FT_Vector advance = ftGlyph->advance;
   glyph.advance.x = advance.x;
   glyph.advance.y = advance.y;
   glyph.bbox = computeOutlineBBox();

   glyph.offset.x = ftGlyph->bmp_left;
   glyph.offset.y = ftGlyph->bmp_top;


   // Now raster glyph to image and convert image to the dark side.
   FT_Bitmap const & bmp = ftGlyph->bitmap;
   FT_Pos const w = bmp.width;
   FT_Pos const h = bmp.rows;

   // Fill (my) glyph atlas struct and create (my) glyph image from ftBitmap.

   if ( !bmp.buffer )
   {
      DE_ERROR("No bitmap was rendered")
   }
   else
   {
      Image img( w, h );

      bool ok = true;
      switch ( bmp.pixel_mode )
      {
      // Load the grayscale data in.
      case FT_PIXEL_MODE_GRAY:
         {
            float const gray_count = float( bmp.num_grays );
            // DE_DEBUG("FT_PIXEL_MODE_GRAY, gray_count = ",gray_count)

            uint8_t const * src = bmp.buffer;
            for ( int32_t y = 0; y < int32_t( bmp.rows ); ++y )
            {
               uint8_t const * row = src;
               for ( int32_t x = 0; x < int32_t( bmp.width ); ++x )
               {
                  float const t = float( *row ) * ( 255.0f / float( gray_count ) );
                  uint8_t const gray = uint8_t( Math::clampT< int >( int(t+0.5f), 0, 255 ) );
                  img.setPixel( x,y, RGBA( 255,255,255,gray ) );
                  ++row;
               }
               src += bmp.pitch;
            }
         }
         break;
      case FT_PIXEL_MODE_MONO:
         {
         DE_ERROR("FT_PIXEL_MODE_MONO")
         //            // Create a blank image and fill it with transparent pixels.
         //            texture_size = d.getOptimalSize(true, true);
         //            image = driver->createImage(ECF_A1R5G5B5, texture_size);
         //            image->fill(SColor(0, 255, 255, 255));

         //            // Load the monochrome data in.
         //            const u32 image_pitch = image->getPitch() / sizeof(u16);
         //            u16* image_data = (u16*)image->lock();
         //            u8* glyph_data = bmp.buffer;
         //            for (u32 y = 0; y < bmp.rows; ++y)
         //            {
         //                u16* row = image_data;
         //                for (u32 x = 0; x < bmp.width; ++x)
         //                {
         //                    // Monochrome bitmaps store 8 pixels per byte.  The left-most pixel is the bit 0x80.
         //                    // So, we go through the data each bit at a time.
         //                    if ((glyph_data[y * bmp.pitch + (x / 8)] & (0x80 >> (x % 8))) != 0)
         //                        *row = 0xFFFF;
         //                    ++row;
         //                }
         //                image_data += image_pitch;
         //            }
         //            image->unlock();
         }
         break;
      default:
         {
         DE_ERROR("FT_PIXEL_MODE_UNKNOWN")
         ok = false;
         }
         break;
      }

      if ( ok )
      {
         glyph.img = std::move( img );
      }
      else
      {
         DE_ERROR("OMG")
      }
   }

   if ( FT_IS_SCALABLE( m_ftFace ) )
   {
      glyph.hBearing /= 64;
      glyph.vBearing /= 64;
      glyph.advance /= 64;
      glyph.bbox /= 64;
      //glyph.offset /= 64;
      // DE_DEBUG( "New scalable Glyph: ", glyph.toString() )
   }
   else {
      // DE_DEBUG( "New fixed Glyph: ", glyph.toString() )
   }

   return glyph;
}

glm::ivec2
FontTTF::getGlyphSize( uint32_t unicode )
{
   std::shared_ptr< Glyph > glyph = getGlyph( unicode );
   if ( !glyph ) return glm::ivec2(0,0);
   return glyph->getImageSize();
}


glm::ivec2
FontTTF::getCharacterSizePx( uint32_t unicode )
{
   std::shared_ptr< Glyph > glyph = getGlyph( unicode );
   if ( !glyph ) return glm::ivec2(0,0);
   int32_t w = glyph->advance.x;
   int32_t h = m_ftMetrics.ascender;   // Grab the true height of the character, taking into account underhanging glyphs.
   if ( FT_IS_SCALABLE( m_ftFace ) )
   {
      h /= 64;
   }
   h += -glyph->offset.y + glyph->getImageSize().y;

   return glm::ivec2( w, h );
}


FT_UInt
FontTTF::getGlyphIndex( uint32_t unicode ) const
{
   FT_UInt glyph_index = 0;
   if ( m_ftFace )
   {
      glyph_index = FT_Get_Char_Index( m_ftFace, FT_ULong( unicode ) );
   }
   else
   {
      DE_ERROR("No ftFace. unicode(",unicode,")")
   }

   if ( glyph_index == 0 )
   {
      DE_ERROR("No glyph_index. unicode(",unicode,")")
   }
   // DE_DEBUG("getGlyphIndex(",unicode,") = ", glyph_index )
   return glyph_index;
}

glm::ivec2
FontTTF::getKerning( uint32_t prevLetter, uint32_t thisLetter )
{
   if ( !m_ftFace || !FT_HAS_KERNING( m_ftFace ) ) return glm::ivec2( 0,0 );
   if ( prevLetter == 0 || thisLetter == 0 ){ return glm::ivec2( 0,0 ); }
   FT_Vector ftKerning;
   FT_Get_Kerning( m_ftFace, getGlyphIndex( prevLetter ),
               getGlyphIndex( thisLetter ), FT_KERNING_DEFAULT, &ftKerning );
   glm::ivec2 kerning( ftKerning.x, ftKerning.y );
   if ( FT_IS_SCALABLE( m_ftFace ) )
   {
      kerning.x /= 64;
      kerning.y /= 64;
   }
   return kerning;
}


uint32_t
FontTTF::getCharacterWidthPx( uint32_t unicode )
{
   Glyph glyph = getGlyph( unicode );
   return glyph.advance.x;
//   if ( unicode >= 0x2000 )
//   {
//   return (m_ftMetrics.ascender / 64);
//   }
//   else
//   {
//   return (m_ftMetrics.ascender / 64) / 2;
//   }
}

uint32_t
FontTTF::getCharacterHeightPx( uint32_t unicode )
{
   // Grab the true height of the character, taking into account underhanging glyphs.
   Glyph glyph = getGlyph( unicode );
   int32_t h = m_ftMetrics.ascender;
   if ( FT_IS_SCALABLE( m_ftFace ) )
   {
      h /= 64;
   }
   h += -glyph.offset.y + glyph.getSize().y;
   return h;
}


void
FontTTF::drawGlyph( Image & img, int x, int y, uint32_t unicode, uint32_t color )
{
   std::shared_ptr< Glyph > glyph = getGlyph( unicode );
   if ( !glyph )
   {
      DE_ERROR("No glyph for unicode(", unicode, ")" )
      return;
   }
   drawGlyphImage( img, x, y, glyph->img, color );
}

void
FontTTF::drawGlyphImage( Image & img, int x, int y, Image const & glyph, uint32_t color )
{
   int32_t const w = glyph.getWidth();
   int32_t const h = glyph.getHeight();
   for ( int32_t j = 0; j < h; ++j )
   {
      for ( int32_t i = 0; i < w; ++i )
      {
         uint32_t src_color = glyph.getPixel( i,j );
         int32_t r = RGBA_R( color );
         int32_t g = RGBA_G( color );
         int32_t b = RGBA_B( color );
         int32_t a = RGBA_A( src_color );
         int32_t px = x + i;
         int32_t py = y + j;
         img.setPixel( px, py, RGBA( r,g,b,a ), true );
      }
   }
}

void
FontTTF::drawText( Image & img, int x, int y, std::wstring const & txt, uint32_t brushColor, Align align )
{
   glm::ivec2 text_size = getTextSize( txt );
   glm::ivec2 pos_start = applyTextAlign( glm::ivec2( x, y ), align, text_size );

#if 1
   int32_t x1 = pos_start.x;
   int32_t y1 = pos_start.y;

   ImagePainter::drawEllipse( img, Recti( x1-1, y1-1, 3,3 ), 0xFFE80020 );
   ImagePainter::drawLineRect( img, Recti( x1, y1, text_size.x, text_size.y ), 0xFF0000FF );
#endif
   //DE_DEBUG("brushColor(", brushColor, ")" )
   //DE_DEBUG("TextSize(", text_size, ")" )

   for ( size_t i = 0; i < txt.size(); ++i )
   {
      uint32_t unicode = uint32_t( txt[ i ] );

      std::shared_ptr< Glyph > glyph = getGlyph( unicode );
      if ( !glyph )
      {
         DE_ERROR("No glyph for unicode(", unicode, ")" )
         continue;
      }

      // glyph exists:
      glm::ivec2 img_size = glyph->getImageSize();
      //DE_DEBUG("Glyph[",i,"] unicode(", unicode, "), img_size(", img_size, ")" )

      // Draw
      glm::ivec2 kerning(0,0);
      // glm::ivec2 glyph_size = getCharacterSizePx( currUnic );
      if ( i > 0 )
      {
         uint32_t prev = uint32_t( txt[ i-1 ] );
         kerning = getKerning( prev, unicode );
      }

      int32_t bx1 = pos_start.x;
      int32_t by1 = pos_start.y;
      int32_t bx2 = pos_start.x + img_size.x;
      int32_t by2 = pos_start.y - img_size.y;
      ImagePainter::drawLineRect( img, bx1, by1, bx2, by2, 0xFF2090FF );
      ImagePainter::drawEllipse( img, Recti( bx1-1, by1-1, 3,3 ), 0xFF209020 );

      if ( img_size.x > 0 && img_size.y > 0 )
      {
         int32_t gx = pos_start.x + kerning.x;
         int32_t gy = pos_start.y - kerning.y - glyph->offset.y;
         drawGlyphImage( img, gx, gy, glyph->img, brushColor );
      }

      //pos_start.x += getCharacterWidthPx( currUnic );
      pos_start.x += glyph->advance.x;
   }
}

Image
FontTTF::createImage( std::wstring const & txt, uint32_t textColor, uint32_t fillColor )
{
   glm::ivec2 s = getTextSize( txt );
   Image img( s.x, s.y );
   img.fill( fillColor );
   drawText( img, 0, 0, txt, textColor );
   return img;
}

*/

#endif
