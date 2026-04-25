#include <de/Font5x8.h>

namespace de
{

// ===========================================================================
Font5x8::Font5x8()
// ===========================================================================
    : quadWidth( 1 )
    , quadHeight( 1 )
    , quadSpacingX( 0 )
    , quadSpacingY( 0 )
    , glyphSpacingX( 1 )
    , glyphSpacingY( 1 )
{}

Font5x8::Font5x8( int quadWidth_, int quadHeight_,
                  int quadSpacingX_, int quadSpacingY_,
                  int glyphSpacingX_, int glyphSpacingY_)
    : quadWidth(quadWidth_ < 1 ? 1 : quadWidth_)
    , quadHeight(quadHeight_ < 1 ? 1 : quadHeight_)
    , quadSpacingX(quadSpacingX_)
    , quadSpacingY(quadSpacingY_)
    , glyphSpacingX(glyphSpacingX_)
    , glyphSpacingY(glyphSpacingY_)
{}

uint64_t
Font5x8::operator< ( Font5x8 const & o ) const
{
    if ( quadWidth < o.quadWidth ) return true;
    if ( quadHeight < o.quadHeight ) return true;
    if ( quadSpacingX < o.quadSpacingX ) return true;
    if ( quadSpacingY < o.quadSpacingY ) return true;
    if ( glyphSpacingX < o.glyphSpacingX ) return true;
    if ( glyphSpacingY < o.glyphSpacingY ) return true;
    return false;
}

uint64_t
Font5x8::hash() const
{
    return ( uint64_t( quadWidth & 0x0F ) )
        |  ( uint64_t( quadHeight & 0x0F ) << 4 )
        |  ( uint64_t( quadSpacingX & 0x0F ) << 8 )
        |  ( uint64_t( quadSpacingY & 0x0F ) << 12 )
        |  ( uint64_t( glyphSpacingX & 0x0F ) << 16 )
        |  ( uint64_t( glyphSpacingY & 0x0F ) << 20 );
}

std::string
Font5x8::toString() const
{
    std::stringstream o; o <<
        quadWidth << ","
      << quadHeight << ","
      << quadSpacingX << ","
      << quadSpacingY << ","
      << glyphSpacingX << ","
      << glyphSpacingY;
    return o.str();
}

Font5x8::TextSize
Font5x8::getTextSize( std::string const & msg ) const
{
    // return getTextSize( msg, *this );

    if ( msg.empty() ) return TextSize(0,0);

    int32_t lineCount = 1;
    int32_t lineWidth = 0;
    int32_t lineWidthMax = 0;

    for ( size_t i = 0; i < msg.size(); ++i )
    {
        uint32_t ch = msg[ i ];
        if ( ch == '\r' )  // Mac or Windows line breaks.
        {
            lineCount++;
            lineWidthMax = std::max( lineWidthMax, lineWidth );
            lineWidth = 0;
            continue;
        }
        if ( ch == '\n' )	// Mac or Windows line breaks.
        {
            lineCount++;
            lineWidthMax = std::max( lineWidthMax, lineWidth );
            lineWidth = 0;
            continue;
        }

        lineWidth++;
    }

    lineWidthMax = std::max( lineWidthMax, lineWidth );

    int tx = 5 * (quadWidth + quadSpacingX) * lineWidthMax
             + (lineWidthMax-1) * glyphSpacingX;
    int ty = 8 * (quadHeight + quadSpacingY) * lineCount
             + (lineCount-1) * glyphSpacingY;

    return TextSize( tx, ty );
}

void Font5x8::renderText(int x, int y, const std::string & msg, uint32_t color,
                            Align align, const FN_SET_PIXEL & setPixel ) const
{
    const int cx = quadWidth + quadSpacingX;
    const int cy = quadHeight + quadSpacingY;
    const int gw = 5 * cx + glyphSpacingX;
    const int gh = 8 * cy + glyphSpacingY;

    auto face = getFontFace5x8();

    // Align: default is TopLeft
    glm::ivec2 aligned_pos(x,y);

    if ( align != Align::TopLeft )
    {
        // Optimize here for single lines, default: multiline but slower.
        const auto ts = getTextSize( msg );

             if ( align & Align::Center ){ aligned_pos.x -= ts.width/2; }
        else if ( align & Align::Right ) { aligned_pos.x -= ts.width;   }
        else {}
             if ( align & Align::Middle ){ aligned_pos.y -= ts.height/2; }
        else if ( align & Align::Bottom ){ aligned_pos.y -= ts.height;   }
        else {}
    }

    auto g = aligned_pos;

    //const int w = o.w();
    //const int h = o.h();

    for ( size_t u = 0; u < msg.size(); ++u )
    {
        uint32_t ch = msg[ u ];

        if ( ch == '\r' )  // Mac or Windows line breaks.
        {
            g.x = aligned_pos.x;
            g.y += gh;
            continue;
        }
        if ( ch == '\n' )	// Mac or Windows line breaks.
        {
            g.x = aligned_pos.x;
            g.y += gh;
            continue;
        }

        //char ch = char( std::clamp( unicode, uint32_t(30), uint32_t(127) ) );
        Font5x8Glyph & m = face->getGlyph( ch );

        // Add dot matrix 5mal8 as max 49 quads
        for ( int j = 0; j < 8; ++j ) {
            for ( int i = 0; i < 5; ++i ) {

                const int x1 = g.x + i * cx;
                const int y1 = g.y + j * cy;

                if ( m.get( i,j ) )
                {
                    for ( int ky = 0; ky < quadHeight; ++ky ) {
                        for ( int kx = 0; kx < quadWidth; ++kx )  {

                            const int x2 = x1 + kx;
                            // if ( x2 < 0 || x2 >= w )
                            // {
                            //     //DE_DEBUG("Invalid x(",x2,") of w(",w,")")
                            //     continue;
                            // }

                            const int y2 = y1 + ky;
                            // if ( y2 < 0 || y2 >= h )
                            // {
                            //     //DE_DEBUG("Invalid y(",y2,") of h(",h,")")
                            //     continue;
                            // }

                            setPixel( x2,y2, color );
                        }
                    }
                }
            }
        }
        g.x += gw;
    }

}

void
Font5x8::drawText( Image & o, int x, int y, std::string const & msg, uint32_t color, Align align ) const
{
    auto my_setPixel = [&] (int x, int y, uint32_t color )
    {
        o.setPixel(x,y,color, false );
    };
    renderText( x,y, msg, color, align, my_setPixel);
}

/*
// static
Font5x8::TextSize
// ===========================================================================
Font5x8::getTextSize( std::string const & msg, Font5x8 const & font )
// ===========================================================================
{
    if ( msg.empty() ) return TextSize(0,0);

    int32_t lineCount = 1;
    int32_t lineWidth = 0;
    int32_t lineWidthMax = 0;

    for ( size_t i = 0; i < msg.size(); ++i )
    {
        uint32_t ch = msg[ i ];
        if ( ch == '\r' )  // Mac or Windows line breaks.
        {
            lineCount++;
            lineWidthMax = std::max( lineWidthMax, lineWidth );
            lineWidth = 0;
            continue;
        }
        if ( ch == '\n' )	// Mac or Windows line breaks.
        {
            lineCount++;
            lineWidthMax = std::max( lineWidthMax, lineWidth );
            lineWidth = 0;
            continue;
        }

        lineWidth++;
    }

    lineWidthMax = std::max( lineWidthMax, lineWidth );

    int tx = 5 * (font.quadWidth + font.quadSpacingX) * lineWidthMax
            + (lineWidthMax-1) * font.glyphSpacingX;
    int ty = 8 * (font.quadHeight + font.quadSpacingY) * lineCount
            + (lineCount-1) * font.glyphSpacingY;

    return TextSize( tx, ty );
}
*/

// ===========================================================================
Font5x8Glyph::Font5x8Glyph()
// ===========================================================================
    : m_symbol( 0 )
{}

bool
Font5x8Glyph::isPixel( int x, int y ) const
{
    if ( (x < 0) || (x > (5-1)) ) return false;
    if ( (y < 0) || (y > (8-1)) ) return false;
    return true;
}

void
Font5x8Glyph::clear()
{
    for ( bool & dot : dots ) { dot = false; }
}

void
Font5x8Glyph::set( int x, int y, bool state )
{
    size_t i = size_t(x) + (size_t(y) * 5);
    if ( i >= dots.size() ) { return; }
    dots[ i ] = state;
}

void
Font5x8Glyph::off( int x, int y ) { set( x,y,false ); }

void
Font5x8Glyph::on( int x, int y ) { set( x,y,true ); }

void
Font5x8Glyph::clearRow( int y )
{
    for ( size_t i = 0; i < 8; ++i ) { set( i,y, false ); }
}

void
Font5x8Glyph::setRow( int y, std::string cc )
{
    clearRow( y );
    for ( size_t i = 0; i < std::min( cc.size(), size_t(5) ); ++i )
    {
        if ( cc[ i ] != ' ' ) { set( i,y, true ); }
    }
}

bool
Font5x8Glyph::get( int x, int y ) const
{
    size_t i = size_t(x) + 5*size_t(y);
    if ( i >= dots.size() ) { return false; }
    return dots[ i ];
}

// ===========================================================================
Font5x8Face::Font5x8Face()
// ===========================================================================
{
   //std::cout << "Font5x8Face()" << std::endl;
   createGlyphCache( m_glyphCache );
}

Font5x8Face::~Font5x8Face()
{
   //std::cout << "~Font5x8Face()" << std::endl;
}

// static
void
Font5x8Face::createGlyphCache( Font5x8Face::GlyphCache & cache )
{
    auto addGlyph = [&cache] ( uint32_t unicode,
        std::string const & row0 = "", std::string const & row1 = "",
        std::string const & row2 = "", std::string const & row3 = "",
        std::string const & row4 = "", std::string const & row5 = "",
        std::string const & row6 = "", std::string const & row7 = "" )
    {
        //if ( cache.find( unicode ) != cache.end() ) { return; }
        Font5x8Glyph glyph;
        glyph.setRow(0,row0);
        glyph.setRow(1,row1);
        glyph.setRow(2,row2);
        glyph.setRow(3,row3);
        glyph.setRow(4,row4);
        glyph.setRow(5,row5);
        glyph.setRow(6,row6);
        glyph.setRow(7,row7);
        cache[ unicode ] = glyph;
    };

    addGlyph(' ', "","","","","","","" );
    addGlyph(',', "","","","","","  ++","   +","  +" );
    addGlyph(';', "","","  ++","  ++","","  ++","   +","  +" );
    addGlyph('.', "","","","","","  ++","  ++" );
    addGlyph(':', "","  ++","  ++","","  ++","  ++","" );
    addGlyph('*', "","  +  ","+ + +"," +++ ","+ + +","  +  ","" );
    addGlyph('~', ""," +   ","+ + +","   + ","","","" );
    addGlyph(L'^',"  +"," + +","+   +","","","","" );
    //addGlyph(L"°","  ++"," +  +","  ++","","","","" );
    addGlyph('"'," + +"," + +","","","","","" );
    addGlyph('\'',"  +","  +","","","","","" );
    addGlyph('\\',"","+"," +","  +","   +","","" );
    addGlyph('/',"","   +","  +"," +","+","","" );
    addGlyph('#'," + +"," + +","+++++"," + +","+++++"," + +"," + +" );
    addGlyph('!',"  +","  +","  +","  +","","  +","  +" );
    addGlyph('?'," +++","+   +","    +","   +","  +","","  +" );
    addGlyph('_',"","","","","","","+++++" );
    addGlyph('$',"  +  "," ++++","+ +  "," +++ ","  + +","++++ ","  +  " );
    addGlyph('%', "++   ","++  +","   + ","  +  "," +   ","+  ++","   ++" );
    addGlyph('&', " +   ","+ +  ","+ +  "," +   ","+ + +","+  +"," ++ +" );
    addGlyph('@', " +++ ","+   +","+ + +","+ +++","+ +  ","+    "," ++++" );
    addGlyph('<', "   +","  +"," +","+"," +","  +","   +" );
    addGlyph('>', " +","  +","   +","    +","   +","  +"," +" );
    addGlyph('|', "  +","  +","  +","  +","  +","  +","  +" );
    addGlyph('=', "","","+++++","","+++++","","" );
    addGlyph('+', "","  +","  +","+++++","  +","  +","" );
    addGlyph('-', "","","","+++++","","","" );
    addGlyph('[', "  ++","  +","  +","  +","  +","  +","  ++" );
    addGlyph(']', " ++","  +","  +","  +","  +","  +"," ++" );
    addGlyph('(', "   +","  +"," +"," +"," +","  +","   +" );
    addGlyph(')', " +","  +","   +","   +","   +","  +"," +" );
    addGlyph('{', "   ++","  +","  +"," +","  +","  +","   ++" );
    addGlyph('}', "++","  +","  +","   +","  +","  +","++" );
    addGlyph('0', " +++ ","+   +","+   +","+ + +","+   +","+   +"," +++ " );
    addGlyph('1', "  +  "," ++  ","  +  ","  +  ","  +  ","  +  "," +++ " );
    addGlyph('2', "  ++ "," +  +","    +","   + ","  +  "," +   ","+++++" );
    addGlyph('3', "+++++","   + ","  +  ","   + ","    +","+   +"," +++ " );
    addGlyph('4', "   + ","  ++ "," + + ","+  + ","+++++","   + ","   + " );
    addGlyph('5', "+++++","+    ","++++ ","    +","    +","+   +"," +++ " );
    addGlyph('6', "  ++ "," +   ","+    ","++++ ","+   +","+   +"," +++ " );
    addGlyph('7', "+++++","    +","   + ","  +  "," +   "," +   "," +   " );
    addGlyph('8', " +++ ","+   +","+   +"," +++ ","+   +","+   +"," +++ " );
    addGlyph('9', " +++ ","+   +","+   +"," ++++","    +","   + "," ++  " );
    addGlyph('A', " +++ ","+   +","+   +","+++++","+   +","+   +","+   +" );
    addGlyph('B', "++++ ","+   +","+   +","++++ ","+   +","+   +","++++ " );
    addGlyph('C', " +++ ","+   +","+    ","+    ","+    ","+   +"," +++ " );
    addGlyph('D', "++++ ","+   +","+   +","+   +","+   +","+   +","++++ " );
    addGlyph('E', "+++++","+    ","+    ","++++ ","+    ","+    ","+++++" );
    addGlyph('F', "+++++","+    ","+    ","++++ ","+    ","+    ","+    " );
    addGlyph('G', " +++ ","+   +","+    ","+ +++","+   +","+   +"," +++ " );
    addGlyph('H', "+   +","+   +","+   +","+++++","+   +","+   +","+   +" );
    addGlyph('I', " +++ ","  +  ","  +  ","  +  ","  +  ","  +  "," +++ " );
    addGlyph('J', "  +++","   +" ,"   +" ,"   +" ,"   +" ,"+  +" ," ++ " );
    addGlyph('K', "+   +","+  + ","+ +  ","++   ","+ +  ","+  + ","+   +" );
    addGlyph('L', "+    ","+    ","+    ","+    ","+    ","+    ","+++++" );
    addGlyph('M', "+   +","++ ++","+ + +","+   +","+   +","+   +","+   +" );
    addGlyph('N', "+   +","++  +","+ + +","+  ++","+   +","+   +","+   +" );
    addGlyph('O', " +++ ","+   +","+   +","+   +","+   +","+   +"," +++ " );
    addGlyph('P', "++++","+   +","+   +","++++","+","+","+" );
    addGlyph('Q', " +++ ","+   +","+   +","+   +","+ + +","+  + "," ++ +" );
    addGlyph('R', "++++","+   +","+   +","++++","+ +","+  +","+   +" );
    addGlyph('S', " ++++","+    ","+    "," +++ ","    +","    +","++++" );
    addGlyph('T', "+++++","  +","  +","  +","  +","  +","  +" );
    addGlyph('U', "+   +","+   +","+   +","+   +","+   +","+   +"," +++ " );
    addGlyph('V', "+   +","+   +","+   +","+   +","+   +"," + +","  + " );
    addGlyph('W', "+   +","+   +","+   +","+ + +","+ + +","+ + +"," + + " );
    addGlyph('X', "+   +","+   +"," + + ","  +  "," + + ","+   +","+   +" );
    addGlyph('Y', "+   +","+   +","+   +"," + +","  +","  +","  +" );
    addGlyph('Z', "+++++","    +","   +","  +"," +","+","+++++" );
    addGlyph('a', "",""," +++ ","    +"," ++++","+   +"," ++++" );
    addGlyph('b', "+    ","+    ","+ ++ ","++  +","+   +","+   +","++++ " );
    addGlyph('c', "",""," +++ ","+    ","+    ","+   +"," +++ " );
    addGlyph('d', "    +","    +"," ++ +","+  ++","+   +","+   +"," ++++" );
    addGlyph('e', "",""," +++ ","+   +","+++++","+    "," +++ " );
    addGlyph('f', "  ++ "," +  +"," +   ","+++  "," +   "," +   "," +   " );
    addGlyph('g', "",""," +++ ","+   +","+   +"," ++++","    +"," +++ " );
    addGlyph('h', "+    ","+    ","+ ++ ","++  +","+   +","+   +","+   +" );
    addGlyph('i', "  +  ","     "," ++  ","  +  ","  +  ","  +  "," +++ " );
    addGlyph('j', "    +","     ","   ++","    +","    +","    +","+  +"," ++" );
    addGlyph('k', " +   "," +   "," +  +"," + + "," ++  "," + + "," +  +" );
    addGlyph('l', " ++","  +","  +","  +","  +","  +"," +++" );
    addGlyph('m', "","","++ + ","+ + +","+ + +","+   +","+   +" );
    addGlyph('n', "","","+ ++ ","++  +","+   +","+   +","+   +" );
    addGlyph('o', "",""," +++ ","+   +","+   +","+   +"," +++" );
    addGlyph('p', "","","++++ ","+   +","+   +","++++","+","+" );
    addGlyph('q', "",""," ++ +","+  ++","+   +"," ++++","    +","    +" );
    addGlyph('r', "","","+ ++ ","++  +","+    ","+    ","+    " );
    addGlyph('s', "     ","     "," +++ ","+    "," +++ ","    +","++++ " );
    addGlyph('t', " +   "," +   ","+++  "," +   "," +   "," +  +","  ++ " );
    addGlyph('u', "","","+   +","+   +","+   +","+  ++"," ++ +" );
    addGlyph('v', "","","+   +","+   +","+   +"," + +","  +" );
    addGlyph('w', "","","+   +","+   +","+ + +","+ + +"," + +" );
    addGlyph('x', "","","+   +"," + + ","  +  "," + + ","+   +" );
    addGlyph('y', "","","+   +","+   +","+   +"," ++++","    +"," +++" );
    addGlyph('z', "","","+++++","   +","  +"," +","+++++" );
}

/*
Font5x8::TextSize
Font5x8Face::getTextSize( std::string const & msg ) const
{
    return Font5x8::getTextSize( msg, font );
}
*/

bool
Font5x8Face::hasGlyph( uint32_t unicode ) const
{
    auto it = m_glyphCache.find( unicode );
    return it != m_glyphCache.end();
}

Font5x8Glyph &
Font5x8Face::getGlyph( uint32_t unicode )
{
    auto it = m_glyphCache.find( unicode );
    if ( it == m_glyphCache.end() )
    {
        return m_glyphCache[ uint32_t('?') ];
    }
    else
    {
        return it->second;
    }
}

void
Font5x8Face::cacheString( std::string const & msg )
{
//   for ( size_t i = 0; i < msg.size(); ++i )
//   {
//      char ch = msg[ i ];
//      auto it = m_glyphCache.find( ch );
//      if ( it == m_glyphCache.end() )
//      {
//         m_glyphCache[ ch ] = Font5x8::getGlyph( ch );
//      }
//   }
}

/*
void
Font5x8Face::add2DText( SMeshBuffer & o, int x, int y, std::string const & msg, uint32_t color, Align align )
{
    cacheString( msg );

    o.setPrimitiveType( PrimitiveType::Triangles );
    o.setLighting( 0 );
    //o.setBlend( Blend::disabled() );
    //o.setCulling( false );
    o.moreVertices( 4*msg.size() );
    o.moreIndices( 6*msg.size() );

    int cx = font.quadWidth + font.quadSpacingX;
    int cy = font.quadHeight + font.quadSpacingY;
    int gw = 5 * cx + font.glyphSpacingX;
    int gh = 8 * cy + font.glyphSpacingY;

    // Align: default is TopLeft
    glm::ivec2 aligned_pos(x,y);

    if ( align != Align::TopLeft )
    {
    glm::ivec2 ts = getTextSize( msg ); // Optimize here for single lines, default: multiline but slower.
    float tw = ts.x;
    float th = ts.y;

    if ( align & Align::Center ){ aligned_pos.x -= tw/2; }
    else if ( align & Align::Right ) { aligned_pos.x -= tw;   }
    else {}
    if ( align & Align::Middle ){ aligned_pos.y -= th/2; }
    else if ( align & Align::Bottom ){ aligned_pos.y -= th;   }
    else {}
    }

    auto g = aligned_pos;

    for ( size_t u = 0; u < msg.size(); ++u )
    {
    char ch = msg[ u ];

    if ( ch == '\r' )  // Mac or Windows line breaks.
    {
    g.x = aligned_pos.x;
    g.y += gh;
    continue;
    }
    if ( ch == '\n' )	// Mac or Windows line breaks.
    {
    g.x = aligned_pos.x;
    g.y += gh;
    continue;
    }

    //char ch = char( std::clamp( unicode, uint32_t(30), uint32_t(127) ) );
    Font5x8Glyph & m = m_glyphCache[ ch ];

    // Add dot matrix 5mal8 as max 49 quads
    for ( int j = 0; j < 8; ++j ) {
    for ( int i = 0; i < 5; ++i ) {

    int x1 = g.x + i * cx;
    int y1 = g.y + j * cy;

    int x2 = x1 + font.quadWidth;
    int y2 = y1 + font.quadHeight;

    if ( m.get( i,j ) )
    {
    S3DVertex A( x1, y1, -1, 0,0,-1, color, 0,1 );
    S3DVertex B( x1, y2, -1, 0,0,-1, color, 0,0 );
    S3DVertex C( x2, y2, -1, 0,0,-1, color, 1,0 );
    S3DVertex D( x2, y1, -1, 0,0,-1, color, 1,1 );
    o.addQuad( A,B,C,D, true );
    }
    }
    }

    g.x += gw;
    }
}
*/

// ===========================================================================
struct Font5x8Manager
// ===========================================================================
{
    Font5x8Manager();
    ~Font5x8Manager();
    static Font5x8Manager& getInstance();

    Font5x8Face::SharedPtr getFontFace(); // Font5x8 const & font

    //std::map< Font5x8, Font5x8Face::SharedPtr > m_FontCache;
    Font5x8Face::SharedPtr m_fontFace;
};

//static
Font5x8Manager &
Font5x8Manager::getInstance()
{
    static Font5x8Manager s_Instance;
    return s_Instance;
}

// ===========================================================================
Font5x8Manager::Font5x8Manager()
// ===========================================================================
{
   //std::cout << "Font5x8Manager()" << std::endl;
}

Font5x8Manager::~Font5x8Manager()
{
   //std::cout << "~Font5x8Manager()" << std::endl;
}

Font5x8Face::SharedPtr
Font5x8Manager::getFontFace() //  Font5x8 const & font
{
    if ( !m_fontFace )
    {
        m_fontFace = std::make_shared<Font5x8Face>();
    }
    /*
    auto it = m_FontCache.find( font );
    if ( it != m_FontCache.end() )
    {
        return it->second; // Cache hit!
    }

    // Cache miss!
    Font5x8Face::SharedPtr fontImpl = std::make_shared< Font5x8Face >();
    fontImpl->font = font;
    m_FontCache[ font ] = fontImpl;
    return m_FontCache[ font ];
    */
    return m_fontFace;
}

Font5x8Face::SharedPtr
getFontFace5x8() //  Font5x8 const & font
{
    return Font5x8Manager::getInstance().getFontFace(); //  font
}

} // end namespace de.
