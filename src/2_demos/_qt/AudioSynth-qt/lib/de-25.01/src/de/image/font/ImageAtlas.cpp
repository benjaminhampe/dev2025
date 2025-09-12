#include <de/image/font/ImageAtlas.h>
#include <de/image/ImagePainter.h>
//#include <DarkImage.h>
#include <tinyxml2/tinyxml2.h>

namespace de {


ImageRef::ImageRef()
   : m_img( nullptr ), m_rect( 0,0,0,0 )
{
}

ImageRef::ImageRef( Image const * src )
   : m_img( const_cast< Image* >( src ) ), m_rect( 0,0,0,0 )
{
   if ( m_img )
   {
      m_rect = Recti( 0, 0, m_img->w(), m_img->h() );
   }
}

ImageRef::ImageRef( Image const * src, Recti const & region )
   : m_img( const_cast< Image* >( src ) ), m_rect( region )
{}

Image
ImageRef::copyImage() const
{
   if ( !m_img ) return {};

   int32_t w = m_rect.w;
   int32_t h = m_rect.h;
   int32_t x = m_rect.x;
   int32_t y = m_rect.y;

   Image img( w,h );

   for ( int32_t j = 0; j < h; ++j )
   {
      for ( int32_t i = 0; i < w; ++i )
      {
         img.setPixel( i,j, m_img->getPixel( x+i, y+j ) );
      }
   }
   return img;
}

uint32_t
ImageRef::pixel( int32_t i, int32_t j ) const
{
   if ( !m_img ) return 0x00000000;
   return m_img->getPixel( i + x(), j + y() );
}

Rectf
ImageRef::texCoords( bool useOffset ) const
{
   if ( !m_img ) return Rectf();
   return Rectf::fromRecti( m_rect, m_img->w(), m_img->h(), useOffset );
}

std::string
ImageRef::toString() const
{
   std::stringstream s; s <<
   "X" << x() << "_"
   "Y" << y() << "_"
   "W" << w() << "_"
   "H" << h() << "_"
   "TexCoords" << texCoords().toString() << "_"
   "S" << image();
   return s.str();
}


std::string
ImageAtlasRef::toString() const
{
   std::ostringstream s; s <<
   "name:" << name << ", "
   "id:" << id << ", "
   "page:" << page << ", "
   "rect:" << rect.str() << ", "
   "wrap:" << wrapMode << ", "
   "mipmaps:" << mipmaps << ", "
   "img:";
   s << ( img ? img->str() : "nullptr");
   return s.str();
}




ImageAtlasPage::ImageAtlasPage()
   : pageId(-1)
   , img()
{}

ImageAtlasPage::~ImageAtlasPage()
{}

std::string
ImageAtlasPage::toString() const
{
   return "NotImpl";
}

//Image
//getImage( std::string const & name ) const
//{
//   Image img(
//}

//Image
//ImageAtlasPage::getRefImage( int id ) const
//{
//   return
//}

void
ImageAtlasPage::clear()
{
   img.clear();
}

void
ImageAtlasPage::setup( std::string pageName, int w, int h )
{
   name = pageName;
   img.setUri( pageName + ".png" );
   img.resize( w,h );
   img.fillZero();
}

bool
ImageAtlasPage::addFullPageRef( int id, std::string name, bool mipmaps, int wrapMode )
{
   if ( refs.size() > 0 )
   {
      std::ostringstream s; s << __func__ << " Full ref already exist";
      throw std::runtime_error( s.str() );
   }

   this->wrapMode = wrapMode;
   ImageAtlasRef item;
   item.id = id;
   item.name = name;
   item.rect = img.rect(); // ref covers entire full image
   item.wrapMode = wrapMode;
   item.mipmaps = mipmaps;
   refs.emplace_back( std::move( item ) );
   return true;
}

bool
ImageAtlasPage::addRef( int id, std::string name, Image const & chunk, int x, int y, bool mipmaps  )
{
   if ( findRef( id ) > -1 )
   {
      std::ostringstream s;
      s << __func__ << " Id already exist " << id << ".";
      throw std::runtime_error( s.str() );
      //return false;
   }

   if ( chunk.empty() )
   {
      DE_ERROR("Empty image chunk ", id)
      return false;
   }
   ImageAtlasRef item;
   item.id = id;
   item.name = name;
   item.page = pageId;
   item.img = &img;
   item.rect = Recti( x,y, chunk.w(), chunk.h() );
   item.mipmaps = mipmaps;
   ImagePainter::drawImage( img, chunk, x, y );
   refs.emplace_back( std::move( item ) );
   return true;
}

bool
ImageAtlasPage::addRef( int id, std::string name, std::string uri, int x, int y, bool mipmaps  )
{
   std::string mediaUri;
   if ( mediaDir.empty() )
   {
      mediaUri = uri;
   }
   else
   {
      mediaUri = mediaDir + "/" + uri;
   }

   Image img;
   if ( !dbLoadImage( img, mediaUri ) )
   {
      DE_ERROR("Cant add ", mediaUri)
      return false;
   }

   img.setUri( mediaUri );
   return addRef( id, name, img, x, y, mipmaps );
}

int
ImageAtlasPage::findRef( int id ) const
{
   for ( size_t i = 0; i < refs.size(); ++i )
   {
      if ( refs[ i ].id == id )
      {
         return int( i );
      }
   }

   return -1;
}

ImageAtlasRef const &
ImageAtlasPage::getRef( int id ) const
{
   int foundRef = findRef( id );
   if ( foundRef < 0 )
   {
      std::ostringstream s;
      s << __func__ << " Invalid ref id " << id;
      throw std::runtime_error( s.str() );
   }

   return refs[ foundRef ];
}

bool
ImageAtlasPage::loadXml( std::string const & uri )
{
   DE_DEBUG("Load atlas image ", uri)

   return dbLoadImage( img, uri );
}

bool
ImageAtlasPage::saveXml( std::string const & uri )
{
   DE_DEBUG("=========================================" )
   DE_DEBUG("saveXml(",uri,")" )
   DE_DEBUG("=========================================" )
   std::string baseName = FileSystem::fileBase( uri );
   std::string dirName = FileSystem::fileDir( uri );
   DE_DEBUG("URI baseName = ", baseName )
   DE_DEBUG("URI dirName = ", dirName )
   std::string png_file = baseName + ".png";
   std::string xml_file = baseName + ".xml";
   DE_DEBUG("PNG fileName = ", png_file )
   std::string png_uri = dirName + "/" + png_file;
   DE_DEBUG("PNG uri = ", png_uri )
   DE_DEBUG("XML fileName = ", xml_file )
   DE_DEBUG("XML uri = ", uri )

   if ( !dbSaveImage( img, png_uri ) )
   {
      DE_DEBUG("No image ", png_uri)
      return false;
   }

   DE_DEBUG("Save atlas image ", png_uri )

   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* page = doc.NewElement( "image-atlas-page" );
   page->SetAttribute( "id", pageId );
   page->SetAttribute( "w", w() );
   page->SetAttribute( "h", h() );
   page->SetAttribute( "src", png_file.c_str() );
   page->SetAttribute( "refs", int( refs.size() ) );

   // Write Refs:
   for ( size_t i = 0; i < refs.size(); ++i )
   {
      auto const & ref = refs[ i ];
      tinyxml2::XMLElement* refNode = doc.NewElement( "img" );
      refNode->SetAttribute("id", ref.id );
      refNode->SetAttribute("x", ref.rect.x );
      refNode->SetAttribute("y", ref.rect.y );
      refNode->SetAttribute("w", ref.rect.w );
      refNode->SetAttribute("h", ref.rect.h );
      refNode->SetAttribute( "mipmaps", ref.mipmaps ? 1: 0 );
      refNode->SetAttribute("name", ref.name.c_str() );
      page->InsertEndChild( refNode );
   }

   // limb->InsertEndChild( ilem );

   doc.InsertFirstChild( page );

   tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant save xml ", uri)
      return false;
   }

   DE_ERROR("Saved atlas xml ", uri)
   return true;
}











int
ImageAtlas::freePageId() const
{
   static int idCounter = 0;
   idCounter++;
   return idCounter;
}

Image
ImageAtlas::copyImage( int id ) const
{
   ImageAtlasRef const & ref = getRef( id );
   uint32_t w = ref.rect.w;
   uint32_t h = ref.rect.h;
   uint32_t x = ref.rect.x;
   uint32_t y = ref.rect.y;
   DE_DEBUG("id:",id, ", w:",w, ", h:",h, ", x:",x, ", y:",y, ", img:", ref.img->str())

   Image img( w,h );
   for ( uint32_t j = 0; j < h; ++j )
   {
   for ( uint32_t i = 0; i < w; ++i )
   {
      img.setPixel( i,j, ref.img->getPixel(x+i,y+j) );
   }
   }

   return img;
}


ImageAtlasRef const &
ImageAtlas::getRef( int id ) const
{
   int foundPage = -1;
   int foundRef = -1;

   for ( size_t i = 0; i < pages.size(); ++i )
   {
      ImageAtlasPage const & page = pages[ i ];
      int index = page.findRef( id );
      if (index > -1)
      {
         foundPage = int( i );
         foundRef = index;
         break;
      }
   }

   if ( foundRef < 0 || foundPage < 0 )
   {
      std::ostringstream s;
      s << __func__ << " Invalid ref id " << id;
      throw std::runtime_error( s.str() );
   }

   return pages[ foundPage ].refs[ foundRef ];
}

int
ImageAtlas::findPage( int pageId ) const
{
   for ( size_t i = 0; i < pages.size(); ++i )
   {
      ImageAtlasPage const & page = pages[ i ];
      if ( page.pageId == pageId )
      {
         return int( i );
      }
   }
   return -1;
}

ImageAtlasPage &
ImageAtlas::getPage( int pageId )
{
   int foundPage = findPage( pageId );
   if (foundPage < 0)
   {
      throw std::runtime_error( std::string("getPage(): Invalid pageId ") + std::to_string( pageId ) );
   }

   return pages[ foundPage ];
}

ImageAtlasPage const &
ImageAtlas::getPage( int pageId ) const
{
   int foundPage = findPage( pageId );
   if (foundPage < 0)
   {
      std::ostringstream s;
      s << __func__ << " Invalid pageId " << pageId;
      throw std::runtime_error( s.str() );
   }

   return pages[ foundPage ];
}

void
ImageAtlas::delPage( int pageId )
{
   int pageIndex = findPage( pageId );
   if (pageIndex < 0) { DE_ERROR("No pageIndex = ", pageIndex ) return; }
   size_t nBefore = pages.size();
   pages.erase( pages.begin() + pageIndex );
   size_t nAfter = pages.size();
   if (nAfter > nBefore)
   {
      DE_DEBUG("Deleted pageId(",pageId,") pageIndex(", pageIndex,")" )
   }
   else
   {
      DE_ERROR("Did not delete pageId(",pageId,") pageIndex(", pageIndex,")" )
   }
}

void
ImageAtlas::addPageToPage( int srcPageId, int dstPageId, int dstX, int dstY )
{
   int srcPageIndex = findPage( srcPageId );
   int dstPageIndex = findPage( dstPageId );

   if ( srcPageIndex < 0 ) { DE_ERROR("No srcPageId = ", srcPageId ) return; }
   if ( dstPageIndex < 0 ) { DE_ERROR("No dstPageId = ", dstPageId ) return; }

   ImageAtlasPage const & srcPage = getPage( srcPageId );
   ImageAtlasPage       & dstPage = getPage( dstPageId );

   DE_DEBUG("DstPage.RefCount = ", dstPage.refs.size() )
   DE_DEBUG("SrcPage.RefCount = ", srcPage.refs.size() )

   ImagePainter::drawImage( dstPage.img, srcPage.img, dstX, dstY );

   for ( int32_t i = 0; i < srcPage.refs.size(); ++i )
   {
      ImageAtlasRef const & ref = srcPage.refs[ i ];
      int32_t w = ref.rect.w;
      int32_t h = ref.rect.h;
      int32_t x = ref.rect.x;
      int32_t y = ref.rect.y;

      DE_DEBUG("SrcPage.Ref[",i,"] ", srcPage.refs[ i ].toString() )

      if ( !ref.img )
      {
         DE_ERROR("No ref ", ref.toString())
         continue;
      }

      ImageAtlasRef dst;
      dst.id = ref.id;
      dst.name = ref.name;
      dst.img = &dstPage.img;
      dst.page = dstPage.pageId;
      dst.rect = Recti( dstX + x, dstY + y, w, h );
      dstPage.refs.emplace_back( std::move( dst ) );
      DE_DEBUG("Copied src(", ref.toString(), ") to dst(", dst.toString(),")")
   }

   // Now delete src page
   delPage( srcPageId );
}

int
ImageAtlas::addPage( std::string pageName, Image const & img )
{
   pages.emplace_back();
   ImageAtlasPage & page = pages.back();
   int pageId = freePageId();
   page.pageId = pageId;
   page.name = pageName;
   page.img = img;
   page.img.setUri( pageName + ".png" );
   DE_DEBUG("AddPage [",pageId,"] ",pageName," from image ", page.img.str() )
   return pageId;
}

int
ImageAtlas::addPage( std::string pageName, int w, int h, std::string mediaDir )
{
   pages.emplace_back();
   ImageAtlasPage & page = pages.back();

   int pageId = freePageId();
   page.pageId = pageId;
   page.setup( pageName, w, h );
   page.setMediaDir( mediaDir );
   DE_DEBUG("AddPage [",pageId,"] ",pageName," from w,h, ", page.img.str() )
   return pageId;
}

void
ImageAtlas::addRef( int id, std::string name, Image const & img, int x, int y, bool mipmaps )
{
   ImageAtlasPage & page = pages.back();
   page.addRef( id, name, img, x, y, mipmaps );
}

void
ImageAtlas::addRef( int id, std::string name, std::string const & uri, int x, int y, bool mipmaps )
{
   ImageAtlasPage & page = pages.back();
   page.addRef( id, name, uri, x, y, mipmaps );
}

void
ImageAtlas::addFullPageRef( int id, std::string name, bool mipmaps, int wrapMode )
{
   ImageAtlasPage & page = pages.back();
   page.addFullPageRef( id, name, mipmaps, wrapMode );
}

bool
ImageAtlas::saveXml( std::string const & uri ) const
{
   DE_DEBUG("=========================================" )
   DE_DEBUG("saveXml(",uri,")" )
   DE_DEBUG("=========================================" )
   std::string baseName = FileSystem::fileBase( uri );
   std::string dirName = FileSystem::fileDir( uri );
   DE_DEBUG("URI baseName = ", baseName )
   DE_DEBUG("URI dirName = ", dirName )

//   std::string png_file = baseName + ".png";
//   std::string xml_file = baseName + ".xml";
//   DE_DEBUG("PNG fileName = ", png_file )
//   std::string png_uri = dirName + "/" + png_file;
//   DE_DEBUG("PNG uri = ", png_uri )
//   DE_DEBUG("XML fileName = ", xml_file )
//   DE_DEBUG("XML uri = ", uri )

//   if ( !dbSaveImage( img, png_uri ) )
//   {
//      DE_DEBUG("No image ", png_uri)
//      return false;
//   }

//   DE_DEBUG("Save atlas image ", png_uri )

   tinyxml2::XMLDocument doc;

   tinyxml2::XMLElement* atlasNode = doc.NewElement( "image-atlas" );
   atlasNode->SetAttribute( "name", uri.c_str() );
   atlasNode->SetAttribute( "pages", int( pages.size() ) );

   // Write Pages:
   for ( size_t pageIndex = 0; pageIndex < pages.size(); ++pageIndex )
   {
      ImageAtlasPage const & page = pages[ pageIndex ];
      std::string png_file = page.name + ".png";
      DE_DEBUG("PNG fileName = ", png_file )
      std::string png_uri = dirName + "/" + png_file;
      DE_DEBUG("PNG uri = ", png_uri )

      if ( !dbSaveImage( page.img, png_uri ) )
      {
         DE_DEBUG("No image ", png_uri)
         //return false;
      }

      tinyxml2::XMLElement* pageNode = doc.NewElement( "page" );
      pageNode->SetAttribute("id", page.pageId );
      pageNode->SetAttribute("src", png_file.c_str() );
      pageNode->SetAttribute("w", page.img.w() );
      pageNode->SetAttribute("h", page.img.h() );
      pageNode->SetAttribute("wrap", page.wrapMode );
      pageNode->SetAttribute("refs", int(page.refs.size()) );

      // Write Refs:
      for ( size_t i = 0; i < page.refs.size(); ++i )
      {
         ImageAtlasRef const & ref = page.refs[ i ];
         tinyxml2::XMLElement* refNode = doc.NewElement( "img" );
         refNode->SetAttribute("id", ref.id );
         refNode->SetAttribute("x", ref.rect.x );
         refNode->SetAttribute("y", ref.rect.y );
         refNode->SetAttribute("w", ref.rect.w );
         refNode->SetAttribute("h", ref.rect.h );
         refNode->SetAttribute("wrap", ref.wrapMode );
         refNode->SetAttribute("mipmaps", ref.mipmaps ? 1:0 );
         refNode->SetAttribute("name", ref.name.c_str() );
         pageNode->InsertEndChild( refNode );
      }

      atlasNode->InsertEndChild( pageNode );
   }

   doc.InsertFirstChild( atlasNode );

   tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant save xml ", uri)
      return false;
   }

   DE_DEBUG("Saved atlas xml ", uri)
   return true;
}


bool
ImageAtlas::loadXml( std::string const & uri )
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLError err = doc.LoadFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant load xml ", uri)
      return false;
   }

   std::string xmlDir = FileSystem::fileDir( uri );

   DE_DEBUG("=========================================" )
   DE_DEBUG("loadXml(",uri,")" )
   DE_DEBUG("=========================================" )
   //std::string baseName = dbGetFileBaseName( uri );
   //std::string dirName = dbGetFileDir( uri );
   //DE_DEBUG("URI baseName = ", baseName )
   //DE_DEBUG("URI dirName = ", dirName )

   tinyxml2::XMLElement* atlasNode = doc.FirstChildElement( "image-atlas" );
   if ( !atlasNode )
   {
      DE_ERROR("No image atlas node in xml ", uri)
      return false;
   }

   if ( atlasNode->Attribute( "name" ) )
   {
      name = atlasNode->Attribute( "name" );
   }
   else
   {
      DE_ERROR("No atlas name found ", uri)
   }

   //int pageCount = atlasNode->IntAttribute( "pages" );

   tinyxml2::XMLElement* pageNode = atlasNode->FirstChildElement( "page" );
   if ( !pageNode )
   {
      DE_ERROR("No image atlas pages found in xml ", uri)
      return false;
   }

   // Load Pages:
   while ( pageNode )
   {
      if ( !pageNode->Attribute("src") )
      {
         DE_ERROR("No page src" )
         continue;
      }

      int pageW = pageNode->IntAttribute("w");
      int pageH = pageNode->IntAttribute("h");
      int wrapMode = pageNode->IntAttribute("wrap");

      pages.emplace_back();
      ImageAtlasPage & page = pages.back();
      page.pageId = int( pages.size() );
      page.wrapMode = wrapMode;
      page.name = pageNode->Attribute("src");

      std::string pageUri = xmlDir + "/" + page.name;
      if ( !dbLoadImage( page.img, pageUri ) )
      {
         DE_ERROR("No page ", page.pageId," src image uri ", pageUri)
         page.img.clear();
      }

      if ( pageW != page.img.w() )
      {
         DE_ERROR("Differing src image width ", page.name)
      }
      if ( pageH != page.img.h() )
      {
         DE_ERROR("Differing src image height ", page.name)
      }

      // Load Refs:
      //int refCount = pageNode->IntAttribute("refs");

      // Load Refs:
      tinyxml2::XMLElement* refNode = pageNode->FirstChildElement( "img" );
      while ( refNode )
      {
         page.refs.emplace_back();
         ImageAtlasRef & ref = page.refs.back();

         int refId = refNode->IntAttribute("id");
         int refX = refNode->IntAttribute("x");
         int refY = refNode->IntAttribute("y");
         int refW = refNode->IntAttribute("w");
         int refH = refNode->IntAttribute("h");
         int wrapMode = refNode->IntAttribute("wrap");
         bool wantMipmaps = refNode->IntAttribute("mipmaps") != 0 ? true:false;
         std::string name = refNode->Attribute("name" );
         ref.id = refId;
         ref.name = name;
         ref.img = &page.img;
         ref.page = page.pageId;
         ref.wrapMode = wrapMode;
         ref.mipmaps = wantMipmaps;
         ref.rect = Recti( refX, refY, refW, refH );

         //DE_DEBUG("Add ref ", ref.toString() )

         refNode = refNode->NextSiblingElement( "img" );
      }

      pageNode = pageNode->NextSiblingElement( "page" );
   }

   DE_DEBUG("Loaded atlas xml ", uri)

//   DE_DEBUG("PageCount = ", pages.size())
//   for ( int i = 0; i < pages.size(); ++i )
//   {
//      ImageAtlasPage & page = pages[ i ];
//      DE_DEBUG("Page[",i,"].RefCount = ", page.refs.size())
//      for ( int r = 0; r < page.refs.size(); ++r )
//      {
//         ImageAtlasRef & ref = page.refs[ r ];
//         DE_DEBUG("Page[",i,"].Ref[",r,"].Rect = ", ref.rect.toString() )
//      }
//   }

   return true;
}

} // end namespace de
