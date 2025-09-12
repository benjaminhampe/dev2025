#include "Material.hpp"
#include <de_opengles32.hpp>
#include <de/GL_Validate.hpp>
#include <de/RuntimeError.hpp>

namespace de {
namespace gpu {

// =======================================================================
State::State( int dummy )
// =======================================================================
{

}

State::State( State const & o )
   : culling( o.culling )
   , depth( o.depth )
   , stencil( o.stencil )
   , blend( o.blend )
   , rasterizerDiscard( o.rasterizerDiscard )
   , lineWidth( o.lineWidth )
   , pointSize( o.pointSize )
   , polygonOffset( o.polygonOffset )
{}

State&
State::operator=( State const & o )
{
// clear = o.clear;
// viewport = o.viewport;
// scissor = o.scissor;
// depthRange = o.depthRange;
   culling = o.culling;
   depth = o.depth;
   stencil = o.stencil;
   blend = o.blend;
   rasterizerDiscard = o.rasterizerDiscard;
   lineWidth = o.lineWidth;
   pointSize = o.pointSize;
   polygonOffset = o.polygonOffset;
// m_Texturing = o.m_Texturing;
   return *this;
}

bool
State::operator==( State const & o ) const
{
// if ( clear != o.clear ) return false;
// if ( viewport != o.viewport ) return false;
// if ( scissor != o.scissor ) return false;
// if ( depthRange != o.depthRange ) return false;
   if ( culling != o.culling ) return false;
   if ( depth != o.depth ) return false;
   if ( stencil != o.stencil ) return false;
   if ( blend != o.blend ) return false;
   if ( rasterizerDiscard != o.rasterizerDiscard ) return false;
   if ( lineWidth != o.lineWidth ) return false;
   if ( pointSize != o.pointSize ) return false;
   if ( polygonOffset != o.polygonOffset ) return false;
// if ( m_Texturing != o.m_Texturing ) return false;
   return true;
}

bool
State::operator!= ( State const & rhs ) const { return !( *this == rhs ); }

std::string
State::toString() const
{
   std::stringstream s; s <<
   "cull(" << (culling.isBack() ? 0 : 1) << "," << (culling.isBack() ? 0 : 1) << "), "
   "depth(" << (depth.isEnabled() ? 1 : 0) << "), "
   "stencil(" << (stencil.isEnabled() ? 1 : 0) << "), "
   "blend(" << (blend.isEnabled() ? 1 : 0) << "), "
   "rastDiscard(" << rasterizerDiscard.toString() << "), "
   "lineWidth(" << lineWidth.toString() << "), "
   "pointSize(" << pointSize.toString() << "), "
   "polyOffset(" << polygonOffset.toString() << ")";
   return s.str();
}
/*
std::string
State::toString() const
{
   std::stringstream s; s <<
   //"clear:" << clear.toString() << "|"
   //"viewport:" << viewport.toString() << "|"
   //"scissor:" << scissor.toString() << "|"
   //"depthRange:" << depthRange.toString() << "|"
   "culling:" << culling.toString() << "|"
   "depth:" << depth.toString() << "|"
   "stencil:" << stencil.toString() << "|"
   "blend:" << blend.toString() << "|"
   "rasterizerDiscard:" << rasterizerDiscard.toString() << "|"
   "lineWidth:" << lineWidth.toString() << "|"
   "pointSize:" << pointSize.toString() << "|"
   "polygonOffset:" << polygonOffset.toString()
   ;
   return s.str();
}
*/

// ===========================================================================
SurfaceFormat::SurfaceFormat()
// ===========================================================================
   : r( 8 ), g( 8 ), b( 8 ), a( 8 ), d( 24 ), s( 8 ), offscreen( false ), dummy( false )
{}
SurfaceFormat::SurfaceFormat( int r, int g, int b, int a, int d, int s, bool isRendBuf, bool offscreen )
   : r( r ), g( g ), b( b ), a( a ), d( d ), s( s ), offscreen( offscreen ), dummy( isRendBuf )
{}

bool SurfaceFormat::isRenderBuffer() const { return dummy; }
int SurfaceFormat::getColorBits() const { return r + g + b + a; }
bool SurfaceFormat::hasTransparency() const { return a > 0; }
bool SurfaceFormat::hasColor() const { return (r > 0 || g > 0 || g > 0); }
bool SurfaceFormat::hasDepth() const { return d > 0; }
bool SurfaceFormat::hasStencil() const { return s > 0; }
bool SurfaceFormat::isOnlyColor() const { return (hasColor() || hasTransparency()) && d == 0 && s == 0; }

std::string
SurfaceFormat::toString() const
{
   std::ostringstream txt;
   if ( a > 0 ) { txt << "A"; }
   if ( r > 0 ) { txt << "R"; }
   if ( g > 0 ) { txt << "G"; }
   if ( b > 0 ) { txt << "B"; }
   if ( a > 0 ) { txt << a; }
   if ( r > 0 ) { txt << r; }
   if ( g > 0 ) { txt << g; }
   if ( b > 0 ) { txt << b; }
   if ( d > 0 ) { txt << "_D" << d; }
   if ( s > 0 ) { txt << "_S" << s; }
   if ( offscreen ) { txt << "_Offscreen"; }
   return txt.str();
}


// ===========================================================================
// SamplerOptions
// ===========================================================================

// static
SO
SO::skybox()
{
   SO so( MagNearest, MinNearest, ClampToBorder, ClampToBorder );
   return so;
}

SO::SO()
   : mag( MagNearest )
   , min( MinNearest )
   , wrapS( ClampToEdge )
   , wrapT( ClampToEdge )
   , af( 4 )
{

}

//SO::SO()
//   : mag( MagLinear )
//   , min( MinMipmapLinearLinear )
//   , wrapS( RepeatMirrored )
//   , wrapT( RepeatMirrored )
//   , af( 4 )
//{

//}

SO::SO( EMagFilter u_mag, EMinFilter u_min, EWrap s_wrap, EWrap t_wrap )
   : mag( u_mag )
   , min( u_min )
   , wrapS( s_wrap )
   , wrapT( t_wrap )
   , af( 4 )
{

}

SO::SO( SO const & o )
   : mag( o.mag )
   , min( o.min )
   , wrapS( o.wrapS )
   , wrapT( o.wrapT )
   , af( o.af )
{

}

bool
SO::hasMipmaps() const
{
   if ( min <= MinLinear ) { return false; } else { return true; }
}

std::string
SO::toString() const
{
   std::ostringstream s;
   s << int(min) << "," << int(mag) << "," << int(wrapS) << "," << int(wrapT) << "," << af;
   return s.str();
}






// ===========================================================================
TexRef::TexRef()
// ===========================================================================
   : m_tex( nullptr )
   , m_w(0)
   , m_h(0)
   , m_id(-1)
   , m_loadCounter(0)
   , m_rect( 0,0,0,0 )
   , m_coords( 0,0,1,1 )
   , m_repeat( 1,1 )
{}

TexRef::TexRef( Tex* ptr, bool useOffset )
   : m_tex( ptr )
   , m_w(0)
   , m_h(0)
   , m_id(-1)
   , m_loadCounter(0)
   , m_rect( 0,0,0,0 )
   , m_coords( 0,0,1,1 )
   , m_repeat( 1,1 )
{
   if ( m_tex )
   {
      m_w = m_tex->getWidth();
      m_h = m_tex->getHeight();
      m_rect = Recti( 0, 0, m_w, m_h );
      m_coords = Rectf::fromRecti( m_rect, m_w, m_h, useOffset );
   }
}

TexRef::TexRef( Tex* ptr, Recti const & pos, bool useOffset )
   : m_tex( ptr )
   , m_w(0)
   , m_h(0)
   , m_id(-1)
   , m_loadCounter(0)
   , m_rect( pos )
   , m_coords( 0,0,1,1 )
   , m_repeat( 1,1 )
{
   if ( m_tex )
   {
      m_w = m_tex->getWidth();
      m_h = m_tex->getHeight();
      m_coords = Rectf::fromRecti( m_rect, m_w, m_h, useOffset );
   }
}

//bool
//TexRef::operator==( TexRef const & other ) const
//{
//   if ( other.m_tex != m_tex ) return false;
//   if ( other.m_rect != m_rect ) return false;
//   if ( other.m_repeat != m_repeat )
//   {
//      DE_WARN("Differs in m_repeat tex scaling")
//      return false;
//   }
//   return true;
//}

Image*         TexRef::img() { return m_img; }
Image const*   TexRef::img() const { return m_img; }
Rectf const &  TexRef::texCoords() const { return m_coords; }
float          TexRef::u1() const { return m_coords.u1(); }
float          TexRef::v1() const { return m_coords.v1(); }
float          TexRef::u2() const { return m_coords.u2(); }
float          TexRef::v2() const { return m_coords.v2(); }
float          TexRef::du() const { return m_coords.du(); }
float          TexRef::dv() const { return m_coords.dv(); }
Rectf const &  TexRef::getTexCoords() const { return texCoords(); }
float          TexRef::getU1() const { return u1(); }
float          TexRef::getV1() const { return v1(); }
float          TexRef::getU2() const { return u2(); }
float          TexRef::getV2() const { return v2(); }
float          TexRef::getDU() const { return du(); }
float          TexRef::getDV() const { return dv(); }
Recti const &  TexRef::rect() const { return m_rect; }
int            TexRef::x() const { return m_rect.x(); }
int            TexRef::y() const { return m_rect.y(); }
int            TexRef::w() const { return m_rect.w(); }
int            TexRef::h() const { return m_rect.h(); }
Recti const &  TexRef::getRect() const { return rect(); }
int            TexRef::getX() const { return x(); }
int            TexRef::getY() const { return y(); }
int            TexRef::getWidth() const { return w(); }
int            TexRef::getHeight() const { return h(); }
int            TexRef::texWidth() const { return m_w; }
int            TexRef::texHeight() const { return m_h; }
glm::ivec2     TexRef::texSize() const { return glm::ivec2(m_w,m_h); }
glm::vec2      TexRef::texSizef() const { return glm::vec2(m_w,m_h); }
int            TexRef::getTexWidth() const { return texWidth(); }
int            TexRef::getTexHeight() const { return texHeight(); }
glm::ivec2     TexRef::getTexSize() const { return texSize(); }
glm::vec2      TexRef::getTexSizef() const { return texSizef(); }
std::string    TexRef::toString() const
{
   std::ostringstream s;
   if (m_tex) { s << dbGetFileName(m_tex->getName()); }
   else       { s << "0"; }
   s << ", rect:" << getRect() << ", semantic:" << int(m_tex->getTexSemantic());
   return s.str();
}

bool           TexRef::empty() const
{
   if ( !m_tex || w() < 1 || h() < 1 )
      return true;
   else
      return false;
}

void           TexRef::setMatrix( float tx, float ty, float sx, float sy )
{
   m_coords = Rectf( tx,ty,sx,sy );
   m_rect = m_coords.toRecti( m_w, m_h );
}

glm::vec4      TexRef::getTexTransform() const
{
   return glm::vec4( m_coords.x(), m_coords.y(), m_coords.w(), m_coords.h() );
}

/*
Image
TexRef::copyImage() const
{
   uint32_t w = ref.rect.getWidth();
   uint32_t h = ref.rect.getHeight();
   uint32_t x = ref.rect.getX();
   uint32_t y = ref.rect.getY();
   DE_DEBUG("id:",id, ", w:",w, ", h:",h, ", x:",x, ", y:",y, ", img:", ref.m_img->toString())

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
*/



//static
std::string
ShaderUtil::getShaderTypeStr( ShaderType const shaderType )
{
   switch ( shaderType )
   {
      case ShaderType::vs: return "vs";
      case ShaderType::fs: return "fs";
      case ShaderType::gs: return "gs";
      default: assert( false ); return "";
   }
}

// static
uint32_t
ShaderUtil::getShaderTypeEnum( ShaderType const shaderType )
{
   switch ( shaderType )
   {
      case ShaderType::vs: return GL_VERTEX_SHADER;
      case ShaderType::fs: return GL_FRAGMENT_SHADER;
      case ShaderType::gs: return 0x8DD9; // GL_GEOMETRY_SHADER; // 0x8DD9; // ;
      default: assert( false ); return 0;
   }
}

// static
std::string
ShaderUtil::getShaderError( uint32_t shaderId )
{
   GLint n = 0;
   ::glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &n ); GL_VALIDATE
   if ( n < 1 ) { return std::string(); }
   std::vector< char > s;
   s.reserve( n );
   ::glGetShaderInfoLog( shaderId, s.capacity(), NULL, s.data() ); GL_VALIDATE
   return s.data();
}

// static
std::string
ShaderUtil::getProgramError( uint32_t programId )
{
   GLint n = 0;
   ::glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &n ); GL_VALIDATE
   if ( n < 1 ) { return std::string(); }
   std::vector< char > s;
   s.resize( 4096 );
   memset( s.data(), 0, s.size() );
   ::glGetProgramInfoLog( programId, s.size(), nullptr, s.data() ); GL_VALIDATE
   std::string t = s.data();
   dbStrTrim( t );
   return t;
}

// static
std::string
ShaderUtil::printTextWithLineNumbers( std::string const & src )
{
   std::stringstream s;
   std::vector< std::string > lines = dbStrSplit( src, '\n' );
   for ( size_t i = 0; i < lines.size(); ++i )
   {
      if ( i < 100 ) s << "_";
      if ( i < 10 ) s << "_";
      s << i << " " << lines[ i ] << '\n';
   }
   return s.str();
}


// static
std::vector< std::string >
ShaderUtil::enumerateAttributes( uint32_t programId, bool debug )
{
   if ( !programId )
   {
      DE_ERROR("No programId(",programId,")")
      return {};
   }

   GLint nAttributes = 0;
   ::glGetProgramiv( programId, GL_ACTIVE_ATTRIBUTES, &nAttributes ); GL_VALIDATE
   GLint maxLen = 0;
   ::glGetProgramiv( programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLen ); GL_VALIDATE

   if ( debug )
   {
      DE_DEBUG( "Shader[",programId,"] has attributes(",nAttributes,") , maxLen(",maxLen,")")
   }

   // Enumerate attributes
   std::vector< std::string > attribs;
   std::vector< char > name;
   name.resize( maxLen );
   for ( GLint i = 0; i < nAttributes; ++i )
   {
      ::memset( name.data(), 0, name.size() );
      GLint attrSize = 0;
      GLenum attrType = 0;
      ::glGetActiveAttrib( programId, i, name.size(), NULL, &attrSize, &attrType, name.data() );

      std::string typeStr;
      switch ( attrType )
      {
         case GL_FLOAT:       typeStr = "float"; break;
         case GL_FLOAT_VEC2:  typeStr = "vec2"; break;
         case GL_FLOAT_VEC3:  typeStr = "vec3"; break;
         case GL_FLOAT_VEC4:  typeStr = "vec4"; break;
         case GL_INT:         typeStr = "int"; break;
         default:             typeStr = "unknown"; break;
      }

      std::ostringstream s;
      s << name.data() << " = " << typeStr << ", size = " << attrSize;
      attribs.emplace_back( std::move( s.str() ) );

      if ( debug )
      {
         DE_DEBUG("Attribute[",i,"] name:", name.data(), ", type:",typeStr,", size:",attrSize)
      }
   }
   return attribs;
}

// static
std::vector< std::string >
ShaderUtil::enumerateUniforms( uint32_t programId, bool debug )
{
   if ( !programId )
   {
      DE_ERROR("No programId(",programId,")")
      return {};
   }

   // query:
   GLint activeUniforms = 0;
   GLint maxLen = 0;
   ::glGetProgramiv( programId, GL_ACTIVE_UNIFORMS, &activeUniforms ); GL_VALIDATE
   ::glGetProgramiv( programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen ); GL_VALIDATE

   // debug:
   if ( debug )
   {
      DE_DEBUG( "Shader[",programId,"] has ",activeUniforms," active Uniforms, maxLen(",maxLen,")")
   }

   // Enumerate uniforms
   std::vector< std::string > uniforms;
   std::vector< char > name;
   name.resize( maxLen );

   for ( GLint i = 0; i < activeUniforms; ++i )
   {
      ::memset( name.data(), 0, name.size() );
      GLint uniSize; // u.m_InstanceCount
      GLenum uniType;
      ::glGetActiveUniform( programId, i, name.size(), NULL, &uniSize, &uniType, name.data() );
      GLint uniLoc = ::glGetUniformLocation( programId, name.data() );
      // typeStr = ShaderUtil::toEUniformType( uniType );
      std::ostringstream s;
      s << "name=" << name.data() << ", type=" << uniType << ", size=" << uniSize << ", location=" << uniLoc;
      uniforms.emplace_back( std::move( s.str() ) );
   }

   if ( debug )
   {
      for ( size_t i = 0; i < uniforms.size(); ++i )
      {
         DE_DEBUG("Uniform[",i,"] ", uniforms[ i ] )
      }
   }

   return uniforms;
}

// static
void
ShaderUtil::deleteProgram( uint32_t & programId )
{
   if ( !programId ) { return; }
   ::glDeleteProgram( programId );
   programId = 0;
}

// static
uint32_t
ShaderUtil::compileShader( std::string const & name, ShaderType const & shaderType, std::string const & source, bool debug )
{
   auto typeStr = getShaderTypeStr( shaderType );
   //debug = true;
   if ( debug )
   {
      DE_DEBUG(name," :: Compile ",typeStr," shader with len(", source.size(), ")" )
   }

   uint32_t shader = ::glCreateShader( getShaderTypeEnum( shaderType ) );
   GL_VALIDATE
   if ( shader == 0 )
   {
      DE_ERROR(name," :: No ",typeStr," shader id.\n", source.size() )
      return 0;
   }

   char const * const pcszText = source.c_str();
   ::glShaderSource( shader, 1, &pcszText, nullptr ); GL_VALIDATE
   ::glCompileShader( shader ); GL_VALIDATE
   GLint ok; ::glGetShaderiv( shader, GL_COMPILE_STATUS, &ok ); GL_VALIDATE
   if ( ok == GL_FALSE )
   {
      DE_ERROR(name," :: Cant compile ",typeStr," shader program(", getShaderError( shader ), " )" )
      DE_ERROR( printTextWithLineNumbers( source ) )
      ::glDeleteShader( shader ); GL_VALIDATE
      return 0;
   }
   else
   {
      //DE_DEBUG(name,"_",typeStr," :: Compile Log(", getShaderError( shader ), " )" )
      if ( debug )
      {
         DE_DEBUG( printTextWithLineNumbers( source ) )
      }
   }

   return shader;
}

// static
uint32_t
ShaderUtil::linkProgram( std::string const & name,
                            std::string const & VS,
                            std::string const & FS, bool debug )
{
   uint32_t vs = compileShader( name, ShaderType::vs, VS.c_str(), debug );
   if ( !vs )
   {
      DE_ERROR( "program[",name,"] :: No vertex shader created" )
      return 0;
   }
   uint32_t fs = compileShader( name, ShaderType::fs, FS.c_str(), debug );
   if ( !fs )
   {
      DE_ERROR( "program[",name,"] :: No fragment shader created" )
      return 0;
   }
   uint32_t programId = ::glCreateProgram(); GL_VALIDATE
   if ( !programId )
   {
      DE_ERROR( "program[",name,"|",programId,"] :: No program created" )
      return 0;
   }
   ::glAttachShader( programId, vs ); GL_VALIDATE
   ::glAttachShader( programId, fs ); GL_VALIDATE
   ::glLinkProgram( programId ); GL_VALIDATE
   GLint ok; ::glGetProgramiv( programId, GL_LINK_STATUS, &ok ); GL_VALIDATE
   if ( ok == GL_FALSE || !programId )
   {
      DE_ERROR( "program[",name,"|",programId,"] :: Linker error(",ok,") ", getProgramError(programId) )
      ::glDeleteProgram ( programId ); GL_VALIDATE
      return 0;
   }

   //DE_DEBUG("program[",name,"|",programId,"] :: Linker Log(", getProgramError( programId ), " )" )
   if ( debug )
   {
      DE_DEBUG( "program[",name,"|",programId,"] :: OK" )
   }

   enumerateAttributes( programId, debug );
   enumerateUniforms( programId, debug );
   return programId;
}

// static
void
ShaderUtil::setAF( float af )
{
//   glTexParameterf( , GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
//   GLfloat maxAF = 0.0f;
//   glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
}

// static
float
ShaderUtil::getAF()
{
   // GL_EXT_texture_filter_anisotropic
   GLfloat maxAF = 0.0f;
   glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
   GL_VALIDATE
   return maxAF;
}

// static
void
ShaderUtil::setSamplerOptions2D( SO const & so )
{
   // [Magnify Filter]
   GLint magMode = GL_NEAREST; // Low quality is default.
   if ( so.mag == SO::MagLinear ) { magMode = GL_LINEAR; }// High quality on demand.
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode );
   GL_VALIDATE

   // [Minify Filter]
   GLint minMode = GL_NEAREST;// Low quality is default.
   switch ( so.min )
   {
      // Higher qualities on demand.
      case SO::MinLinear: minMode = GL_LINEAR; break;
      case SO::MinMipmapNearestNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
      case SO::MinMipmapNearestLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
      case SO::MinMipmapLinearNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
      case SO::MinMipmapLinearLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
      default: break;
   }
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode );
   GL_VALIDATE

   // [WrapS]
   GLint wrapMode = GL_CLAMP_TO_EDGE; // GL_REPEAT;
   switch ( so.wrapS )
   {
      case SO::Repeat: wrapMode = GL_REPEAT; break;
      case SO::RepeatMirrored: wrapMode = GL_MIRRORED_REPEAT; break;
      //case SO::ClampToEdge: wrapMode = GL_CLAMP_TO_EDGE; break;
      #ifdef GL_ES_VERSION_3_2
      case SO::ClampToBorder: wrapMode = GL_CLAMP_TO_BORDER; break;
      #endif
      default: break;
   }
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode );
   GL_VALIDATE

   // [WrapT]
   wrapMode = GL_CLAMP_TO_EDGE; // GL_REPEAT
   switch ( so.wrapT )
   {
      case SO::Repeat: wrapMode = GL_REPEAT; break;
      case SO::RepeatMirrored: wrapMode = GL_MIRRORED_REPEAT; break;
      //case SO::ClampToEdge: wrapMode = GL_CLAMP_TO_EDGE; break;
      #ifdef GL_ES_VERSION_3_2
      case SO::ClampToBorder: wrapMode = GL_CLAMP_TO_BORDER; break;
      #endif
      default: break;
   }
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
   GL_VALIDATE

   // [WrapR] Not used so far.

   // [Anisotropic Filtering] AF | needs GL_EXT_texture_filter_anisotropic ( >= 3.0 ) so its there.
   if ( so.af > 0 )
   {
      ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, float( so.af ) );
      GL_VALIDATE
   }
}


// =======================================================================
// =======================================================================
// =======================================================================

// static
Shader*
Shader::create( std::string const & name, std::string const & vs, std::string const & fs, bool debug )
{
   uint32_t programId = ShaderUtil::linkProgram( name, vs, fs, debug );
   if ( !programId )
   {
      DE_ERROR( "Compile or linker error. Shader (", name, ")" )
      return nullptr;
   }

   Shader* shader = new Shader( name );
   shader->m_programId = programId;
   shader->m_VS = vs;
   shader->m_GS = "";
   shader->m_FS = fs;

   return shader;
}

Shader::Shader( std::string name )
   : m_programId( 0 )
   , m_bound( false )
   , m_name( name )
{
   //DE_DEBUG(getName())
}

Shader::~Shader()
{
   //DE_DEBUG(getName())
   ShaderUtil::deleteProgram( m_programId );
}

// Used for (MVP) matrices.
bool
Shader::setUniformM4f( std::string const & uniformName, glm::mat4 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniformMatrix4fv( loc, 1, GL_FALSE, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for (TextureUnit) indices and (light mode) enums.
bool
Shader::setUniformi( std::string const & uniformName, int32_t uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform1i( loc, uniformValue );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for (time) values in seconds and positions
bool
Shader::setUniformf( std::string const & uniformName, float uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform1f( loc, uniformValue );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for positions, colors, normals and texcoords
bool
Shader::setUniform2f( std::string const & uniformName, glm::vec2 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform2fv( loc, 1, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}
// Used for positions, colors, normals and texcoords
bool
Shader::setUniform3f( std::string const & uniformName, glm::vec3 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform3fv( loc, 1, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for positions, colors, normals and texcoords
bool
Shader::setUniform4f( std::string const & uniformName, glm::vec4 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }
   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform4fv( loc, 1, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}



ShaderManager::ShaderManager()
   : m_versionMajor( 0 )
   , m_versionMinor( 0 )
   , m_shader( nullptr )
   , m_programId( 0 )
{
}

ShaderManager::~ShaderManager()
{
}

void
ShaderManager::initGL(int logLevel)
{
   m_versionMajor = glGetIntegerDE( GL_MAJOR_VERSION );
   m_versionMinor = glGetIntegerDE( GL_MINOR_VERSION );
   m_shader = nullptr;
   m_programId = 0;

   // DEBUG
   if ( logLevel > 0 )
   {
      DE_DEBUG("GL_VERSION_HEADER = ", m_versionMajor, ".", m_versionMinor )
      DE_DEBUG("GL_VERSION = ", (char*)glGetString( GL_VERSION ) )
      DE_DEBUG("GL_VENDOR = ", (char*)glGetString( GL_VENDOR ) )
      DE_DEBUG("GL_RENDERER = ", (char*)glGetString( GL_RENDERER ) )
      DE_DEBUG("GL_SHADING_LANGUAGE_VERSION = ", (char*)glGetString( GL_SHADING_LANGUAGE_VERSION ) )

      std::string extString = (char*)glGetString( GL_EXTENSIONS );
      auto extVector = dbStrSplit( extString, ' ' );
      DE_DEBUG("GL_EXTENSIONS = ", extVector.size() )

      if ( logLevel > 1 )
      {
         for ( size_t i = 0; i < extVector.size(); ++i )
         {
            DE_DEBUG("GL_EXTENSION[",i,"] = ", extVector[ i ] )
         }
      }
   }
}

void
ShaderManager::destroyGL(int logLevel)
{

}

int32_t
ShaderManager::getShaderVersionMajor() const { return m_versionMajor; }
int32_t
ShaderManager::getShaderVersionMinor() const { return m_versionMinor; }

std::string
ShaderManager::getShaderVersionHeader() const // GLES
{
   std::ostringstream s; s << "#version " << m_versionMajor << m_versionMinor << "0 es\n";
   return s.str();
}

void
ShaderManager::clear()
{
   for ( Shader* p : m_shaders ) { if ( p ) delete p; }
   m_shaders.clear();
   m_shader = nullptr;
   m_programId = 0;
}

Shader*
ShaderManager::current()
{
   return m_shader;
}

bool
ShaderManager::use( Shader* new_shader )
{
   uint32_t old_programId = 0;
   Shader* old_shader = m_shader;
   if ( old_shader )
   {
      old_programId = old_shader->getProgramId();
      if ( m_programId != old_programId )
      {
         m_programId = old_programId;
      }
   }

   uint32_t new_programId = 0;
   if ( new_shader )
   {
      new_programId = new_shader->getProgramId();
//      if ( new_programId != old_programId )
//      {
         ::glUseProgram( new_programId );
         bool ok = GL_VALIDATE;
         if ( ok )
         {
            //DE_DEBUG("Use programId = ", new_programId)
            m_programId = new_programId;
         }
         else
         {
            DE_ERROR("Cant use program(",new_shader->getName(),"), programId(", new_programId,")")
         }
//      }
   }

   m_shader = new_shader;
   return true;
}

Shader*
ShaderManager::findProgramId( uint32_t programId ) const
{
   if ( programId < 1 ) return nullptr;

   for ( Shader* shader : m_shaders )
   {
      if ( shader && shader->getProgramId() == programId ) { return shader; }
   }

   return nullptr;
}

int32_t
ShaderManager::findShader( std::string const & name ) const
{
   if ( name.empty() ) return -1;
   for ( size_t i = 0; i < m_shaders.size(); ++i )
   {
      Shader* p = m_shaders[ i ];
      if ( p && p->getName() == name ) { return int32_t( i ); }
   }
   return -1;
}

Shader*
ShaderManager::getShader( std::string const & name )
{
   int32_t found = findShader( name );
   if ( found < 0 ) return nullptr;
   return m_shaders[ found ];
}

Shader*
ShaderManager::createShader( std::string const & name,
                       std::string const & vsText, std::string const & fsText, bool debug )
{
   int32_t found = findShader( name );
   if ( found > -1 ) { DE_ERROR( "Shader already exist at index. (",name,")" ) return nullptr; }
   std::ostringstream vs;
   vs << getShaderVersionHeader()
        << "// ShaderName: "  << name << "_vs\n"
        << vsText;
   std::ostringstream fs;
   fs << getShaderVersionHeader()
        << "// ShaderName: "  << name << "_fs\n"
        << fsText;

   Shader* shader = Shader::create( name, vs.str(), fs.str(), debug );
   if ( !shader ) { DE_ERROR( "No shader created. (",name,")" ) return nullptr; }
   if ( debug )
   {
      DE_DEBUG("Created shader(", name, "), programId(", shader->getProgramId(), ")")
   }
   m_shaders.push_back( shader );
   //setActiveShader( shader );
   return shader;
}

/*
Shader*
ShaderManager::createShader( std::string const & name,
                       std::string const & vs,
                       std::string const & gs, std::string const & fs, bool debug = false )
{
   int32_t found = findShader( name );
   if ( found > -1 ) { DE_ERROR( "Shader already exist at index. (",name,")" ) return nullptr; }

   DE_DEBUG("Add shaderGS")

   std::ostringstream v; v << getShaderVersionHeader() << vs;
   std::ostringstream g; g << getShaderVersionHeader() << gs;
   std::ostringstream f; f << getShaderVersionHeader() << fs;

   //bool oldDebug = m_isDebug;
   //m_isDebug = true;
   Shader* shader = Shader::create( name, v.str(), g.str(), f.str(), debug );
   //m_isDebug = oldDebug;
   if ( !shader ) { DE_ERROR( "No shader created. (",name,")" ) return nullptr; }
   if ( debug )
   {
      DE_DEBUG("Created shader(", name, "), programId(", shader->getProgramId(), ")")
   }
   m_shaders.push_back( shader );
   //setActiveShader( shader );
   return shader;
}
*/

void
ShaderManager::addShader( Shader* shader )
{
   if ( !shader ) { DE_ERROR("Got nullptr") return; }
   std::string const & name = shader->getName();
   int32_t found = findShader( name );
   if ( found >= 0 ) { DE_ERROR("Shader already cached(", name,").") return; }
   m_shaders.push_back( shader );
   DE_DEBUG("[+] New managed Shader(", shader->getProgramId(), "), name(",name,")")
}



Tex2D::Tex2D( TexUnitManager* unitManager )
   : m_TexManager( unitManager )
   , m_Type( TexType::Tex2D )
   , m_Format( TexFormat::R8G8B8A8 )
   , m_semantic( TexSemantic::None )
   , m_Width( 0 )
   , m_Height( 0 )
   , m_Depth( 0 )
   , m_MaxMipLevel( 4 )
   , m_SamplerOptions()
   , m_TexUnit( -1 )
   , m_TextureId( 0 )
   , m_IsRenderBuffer( false )
   , m_KeepImage( true )
   , m_debugLevel(0)
   , m_Name()
   , m_Uri()
   , m_Image()
{
}

Tex2D::~Tex2D()
{
   destroyTexture();

//   if ( m_TexManager )
//   {
//      m_TexManager->removeTexture( getName() );
//   }
}

std::string
Tex2D::toString() const
{
   std::ostringstream s;
   s  << "w:" << getWidth() << ", "
      << "h:" << getHeight()   << ", "
      << "id:" << m_TextureId << ", "
      << "u:" << m_TexUnit << ", "
      << "s:" << int(m_semantic) << ", "
      << dbGetFileName(m_Name);
   return s.str();
}

void
Tex2D::destroyTexture()
{
   if ( m_TextureId > 0 )
   {
      GLuint id = m_TextureId;
      ::glDeleteTextures( 1, &id ); GL_VALIDATE
      m_TextureId = 0;

      if ( m_debugLevel > 0 )
      {
         DE_DEBUG( "Delete Texture2D texId[", id, "] ", toString() )
      }
   }

//   if ( m_TexManager )
//   {
//      m_TexManager->removeTexture( getName() );
//   }
}

// static
Tex2D*
Tex2D::create( TexUnitManager* unitmanager,
               std::string const & name,
               Image const & img,
               SO  const & so, bool keepImage, uint8_t debugLevel )
{
   if ( name.empty() )
   {
      DE_ERROR("Got empty name")
      return nullptr;
   }

   GLuint texId = 0;
   ::glGenTextures( 1, &texId );
   bool ok = GL_VALIDATE;
   if ( !ok || texId < 1 )
   {
      DE_ERROR("Cant gen tex(",name,")")
      return nullptr;
   }

   auto tex = new Tex2D( unitmanager );
   tex->m_Name = name;
   tex->m_Type = TexType::Tex2D;
   tex->m_SamplerOptions = so;
   tex->m_TextureId = texId;
   tex->m_KeepImage = keepImage;
   if ( keepImage )
   {
      tex->m_Image = img;
   }

   ok = tex->upload( img );
   if ( !ok )
   {
      DE_ERROR("Cant upload tex(",name,") image ", tex->toString() )
      delete tex;
      return nullptr;
   }

   tex->m_debugLevel = debugLevel;

   if ( debugLevel > 0 )
   {
      DE_DEBUG("Created Texture2D ", tex->toString() )
   }

   return tex;
}


bool
Tex2D::upload( Image const & img )
{
   if ( img.empty() ) { DE_ERROR("No image") return false; }
   if ( img.getFormat() != ColorFormat::RGBA8888 &&
        img.getFormat() != ColorFormat::RGB888 )
   {
      DE_ERROR("Unsupported image format ", img.toString() )
      return false;
   }

   if ( !m_TextureId ) { DE_ERROR("No texture id") return false; }
   if ( !m_TexManager ) { DE_ERROR("No texunit manager id") return false; }

   int stage = m_TexManager->bindTexture( this );
   if (stage < 0)
   {
      DE_ERROR("No free stage, tex ", toString() )
      return false;
   }
   //glActiveTexture( GL_TEXTURE0 + tex->getTexUnit() ); GL_VALIDATE
   //glBindTexture( GL_TEXTURE_2D, texId ); GL_VALIDATE

   TexFormat texFormat = TexFormat::R8G8B8A8;
   GLenum internal = GL_RGBA;
   GLenum internalFormat = GL_RGBA;
   GLenum internalType = GL_UNSIGNED_BYTE;
   if ( img.getFormat() == ColorFormat::RGB888 )
   {
      texFormat = TexFormat::R8G8B8;
      internal = GL_RGB;
      internalFormat = GL_RGB;
      internalType = GL_UNSIGNED_BYTE;
   }

   ShaderUtil::setSamplerOptions2D( m_SamplerOptions );

   GLsizei w = img.getWidth();
   GLsizei h = img.getHeight();
   ::glTexImage2D( GL_TEXTURE_2D, 0, // GLenum target + GLint level
                  internal, w, h, 0, // GLint border
                  internalFormat, internalType, img.data() );
   bool ok = GL_VALIDATE;
   if ( !ok ) { DE_ERROR("Cant upload" ) return false; }

   if ( m_SamplerOptions.hasMipmaps() )
   {
      ::glGenerateMipmap( GL_TEXTURE_2D ); GL_VALIDATE
   }

   // ::glBindTexture( GL_TEXTURE_2D, 0 ); GL_VALIDATE

   m_Width = w;
   m_Height = h;
   m_Format = texFormat;
   m_TexUnit = stage;
   m_InternalFormat.internal = internal;
   m_InternalFormat.format = internalFormat;
   m_InternalFormat.type = internalType;
   return true;
}

bool
Tex2D::resize( int w, int h )
{
   if ( !m_TextureId ) { DE_ERROR("No texture id") return false; }
   if ( !m_TexManager ) { DE_ERROR("No texunit manager id") return false; }
   int stage = m_TexManager->bindTexture( this );
   if (stage < 0)
   {
      DE_ERROR("No free stage, tex ", toString() )
      return false;
   }

   ::glTexImage2D( GL_TEXTURE_2D, 0, // GLenum target + GLint level
                  m_InternalFormat.internal, w, h, 0, // GLint border
                  m_InternalFormat.format,
                  m_InternalFormat.type, nullptr );
   bool ok = GL_VALIDATE;
   if ( !ok ) { DE_ERROR("Cant resize" ) return false; }

   if ( m_SamplerOptions.hasMipmaps() )
   {
      ::glGenerateMipmap( GL_TEXTURE_2D ); GL_VALIDATE
   }
   // ::glBindTexture( GL_TEXTURE_2D, 0 ); GL_VALIDATE
   m_Width = w;
   m_Height = h;
   return true;
}

void
Tex2D::setTexUnitManager( TexUnitManager* driver ) { m_TexManager = driver; }
void
Tex2D::setTexUnit( int32_t unit ) { m_TexUnit = unit; }
void
Tex2D::setName( std::string const & name ) { m_Name = name; }
void
Tex2D::setSamplerOptions( SO const & so ) { m_SamplerOptions = so; }

TexFormat const &
Tex2D::getFormat() const { return m_Format; }
TexType
Tex2D::getType() const { return m_Type; }
std::string const &
Tex2D::getName() const { return m_Name; }
int32_t
Tex2D::getWidth() const { return m_Width; }
int32_t
Tex2D::getHeight() const { return m_Height; }
int32_t
Tex2D::getDepth() const { return m_Depth; }
uint32_t
Tex2D::getTextureId() const { return m_TextureId; }
int32_t
Tex2D::getTexUnit() const { return m_TexUnit; }
uint32_t
Tex2D::getMaxMipLevel() const { return m_MaxMipLevel; }
SO const &
Tex2D::getSamplerOptions() const { return m_SamplerOptions; }
bool
Tex2D::hasTransparency() const { return m_Format == TexFormat::R8G8B8A8; }
bool
Tex2D::isRenderBuffer() const { return m_IsRenderBuffer; }

void
Tex2D::clearCPUSide() { m_Image.clear( true ); }
Image const &
Tex2D::getImage() const { return m_Image; }
Image &
Tex2D::getImage() { return m_Image; }



TexMipMapper::TexMipMapper()
   : m_ReadFBO( 0 )
   , m_DrawFBO( 0 )
{}

TexMipMapper::~TexMipMapper()
{
   close();
}

void
TexMipMapper::init()
{
   if ( !m_ReadFBO )
   {
      glGenFramebuffers( 1, &m_ReadFBO ); GL_VALIDATE
   }
   if ( !m_DrawFBO )
   {
      glGenFramebuffers( 1, &m_DrawFBO ); GL_VALIDATE
   }
}

void
TexMipMapper::close()
{
   if ( m_ReadFBO )
   {
      glDeleteFramebuffers( 1, &m_ReadFBO ); GL_VALIDATE
      m_ReadFBO = 0;
   }
   if ( m_DrawFBO )
   {
      glDeleteFramebuffers( 1, &m_DrawFBO ); GL_VALIDATE
      m_DrawFBO = 0;
   }
}

/// @brief Creates all mipmaps ( resized texture ) for all levels determined by Tex->getMaxMipmapLevel() if they are not up-to-date (already generated and the data was not changed)
///        and the texture is bound to a texture unit.
/// @param[in] textureBase The texture, for which the mipmaps should be generated.
/// @param[in] singleLayer Layer for which mipmaps need to be generated, or -1 to generate for all array texture layers

bool
TexMipMapper::createMipmaps( Tex2D* tex, int specificDepthLayer )
{
   if ( !tex ) return true; // Nothing was done very successfully.

   int texId = tex->getTextureId();
   if ( !texId )
   {
      DE_ERROR("No texId ", tex->toString() )
      return false;
   }

   init();
   GLint oldReadFBO = 0;
   GLint oldDrawFBO = 0;
   glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &oldReadFBO );
   glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &oldDrawFBO );
   glBindFramebuffer( GL_READ_FRAMEBUFFER, m_ReadFBO ); GL_VALIDATE
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_DrawFBO ); GL_VALIDATE

   int w = tex->getWidth();
   int h = tex->getHeight();
   int w2 = w >> 1; // Maybe your mipmaps do look wrong because your tex is not a multiple of 2.
   int h2 = h >> 1; // Maybe your mipmaps do look wrong because your tex is not a multiple of 2.

   // For Tex3D, Tex2DArray, TexCube?
   if ( tex->getDepth() > 0 )
   {
      int depthS = 0;
      int depthE = tex->getDepth();
      if ( specificDepthLayer > -1 )
      {
         depthS = depthE = specificDepthLayer;
      }

      for ( int i = 1; i <= int(tex->getMaxMipLevel()); ++i )
      {
         for ( int d = depthS; d <= depthE; ++d )
         {
            glFramebufferTextureLayer( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, i - 1, d );
            glFramebufferTextureLayer( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, i, d );
            glBlitFramebuffer( 0, 0, w, h, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR );
         }
         w = w2;
         h = h2;
         w2 >>= 1;
         h2 >>= 1;
      }
      return true;
   }
   // For Tex2D
   else
   {
      if ( tex->getType() != TexType::Tex2D )
      {
         for ( int i = 1; i <= int(tex->getMaxMipLevel()); ++i )
         {
            glFramebufferTexture2D( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, i - 1 );
            glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, i );
            glBlitFramebuffer( 0, 0, w, h, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR );
            w = w2;
            h = h2;
            w2 >>= 1;
            h2 >>= 1;
         }

         return true;
      }
      else
      {
         DE_ERROR("Unsupported texType ", tex->toString() )
         return false;
      }
   }
}

// ===========================================================================
TexUnitManager::TexUnitManager() {}
TexUnitManager::~TexUnitManager() {}

void
TexUnitManager::dump()
{
   DE_DEBUG( "TexUnit.Count = ", m_TexUnits.size() )
   for ( size_t i = 0; i < m_TexUnits.size(); ++i )
   {
      DE_DEBUG( "TexUnit[",i,"] = ", m_TexUnits[ i ] )
   }
}

void
TexUnitManager::initGL( int logLevel )
{
   //::glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); GL_VALIDATE   // Affects glTexImage2D()
   //::glPixelStorei( GL_PACK_ALIGNMENT, 1 ); GL_VALIDATE     // Affects glReadPixels()
   int nTexUnits = glGetIntegerDE( GL_MAX_TEXTURE_IMAGE_UNITS );
   m_TexUnits.clear();
   m_TexUnits.resize( nTexUnits, 0 );
   if ( logLevel > 0 )
   {
      DE_DEBUG( "HardwareTexUnitCount = ", nTexUnits )
   }

}

void
TexUnitManager::destroyGL( int logLevel )
{}

uint32_t
TexUnitManager::getUnitCount() const
{
   return m_TexUnits.size();
}

int32_t
TexUnitManager::findUnit( uint32_t texId ) const
{
   if ( texId < 1 ) return -1; // Not an id.
   for ( size_t i = 0; i < m_TexUnits.size(); ++i )
   {
      if ( m_TexUnits[ i ] == texId )
      {
         return int( i );
      }
   }
   return -1;
}

bool
TexUnitManager::bindTextureId( int stage, uint32_t texId )
{
   if ( texId < 1 )
   {
      DE_WARN("More an unbind with texId = ", texId )
   }

   if ( stage < 0 || stage >= int(m_TexUnits.size()) )
   {
      DE_ERROR("No valid stage ",stage,"/", int(m_TexUnits.size()) )
      return false;
   }

   if ( m_TexUnits[ stage ] == texId )
   {
      return true; // Cache hit, nothing todo.
   }

   m_TexUnits[ stage ] = texId; // Update unit
   ::glActiveTexture( GL_TEXTURE0 + stage ); GL_VALIDATE
   ::glBindTexture( GL_TEXTURE_2D, texId );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      throw std::runtime_error( dbStrJoin( "No stage(",stage,") and texId(",texId,")" ) );
   }
   // ShaderUtil::setSamplerOptions2D( tex->getSamplerOptions() );
   return true;
}

bool
TexUnitManager::unbindTextureId( uint32_t texId )
{
   if ( texId < 1 ) return true;  // Nothing to unbind.

   // Search unit with this tex...
   int found = findUnit( texId );
   if ( found < 0 )
   {
      return true; // Nothing to unbind.
   }

   m_TexUnits[ found ] = 0;
   ::glActiveTexture( GL_TEXTURE0 + found ); GL_VALIDATE
   ::glBindTexture( GL_TEXTURE_2D, 0 ); GL_VALIDATE
   // DE_DEBUG( "Unbound texture ", texId, " from stage ", found, " and target=", target )
   return true;
}


// More intelligent GL: stage/unit is auto select.
// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int
TexUnitManager::bindTextureId( uint32_t texId )
{
   if ( texId < 1 ) { return -1; }

   int unit = findUnit( texId );
   if ( unit > -1 )
   {
      return unit; // Cache hit, activated and bound.
   }

   // Find free unit, if any, and activate it
   for ( size_t i = 0; i < m_TexUnits.size(); ++i )
   {
      if ( !m_TexUnits[ i ] )
      {
         if ( bindTextureId( i, texId ) )
         {
            //DE_DEBUG("Bound tex ", texId, " at unit ", i)
            return int( i );
         }
         else
         {
            DE_ERROR("Not bound unit(", i, ") with texId(",texId,")")
         }
      }
   }

   static int unitCounter = 0;
   unitCounter = (unitCounter + 1) % int(m_TexUnits.size());
   unit = unitCounter;
   if ( !bindTextureId( unit, texId ) )
   {
      DE_ERROR("No force bound unit(", unit, ") with texId(",texId,")")
      return -1;
   }

   return unit;
}

// Benni's HighLevelCalls (class Tex) to native bindTexture2D

bool
TexUnitManager::bindTexture( int stage, Tex* tex )
{
   if ( !tex ) { DE_ERROR("No tex") return false; }
   if ( tex->getType() == TexType::Tex2D )
   {
      if ( bindTextureId( stage, tex->getTextureId() ) )
      {
         tex->setTexUnit( stage );
         return true;
      }
   }
//   else if ( tex->isRenderBuffer() )
//   {
//      ::glBindRenderbuffer( GL_RENDERBUFFER, tex->getTextureId() ); GL_VALIDATE
////      ::glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h ); GL_VALIDATE
////      ::glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
////            GL_RENDERBUFFER, m_depthTex ); GL_VALIDATE
//   }
   return false;
}


// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int
TexUnitManager::bindTexture( Tex* tex )
{
   if ( !tex ) { DE_ERROR("No tex") return -1; }
   if ( tex->getType() == TexType::Tex2D )
   {
      return bindTextureId( tex->getTextureId() );
   }
   else
   {
      // throw std::runtime_error("Invalid texture type");
   }
   return -1;
}



bool
TexUnitManager::unbindTexture( Tex* tex )
{
   if ( !tex ) { return true; }
   if ( tex->getType() == TexType::Tex2D )
   {
      if ( !unbindTextureId( tex->getTextureId() ) )
      {
         DE_ERROR("Cant unbind tex ", tex->toString())
         return false;
      }
      else
      {
         return true;
      }
   }
   else
   {
      DE_ERROR("Unsupported tex type ", tex->toString())
      return false;
   }
}

// ===========================================================================
// ===========================================================================

TexManager::TexManager()
   : m_maxTex2DSize(1024)
{
   //init();
}
TexManager::~TexManager()
{
   //unbindAll();
}

TexRef
TexManager::getTexture( std::string const & name, SO const & so, bool keepImage )
{
   // We abort on empty string, nothing todo, return invalid texref.
   if ( name.empty() ) { return TexRef(); }

   // Ask cache if ever heard of 'name' before...
   auto it = m_refs.find( name );

   // (cachemiss)...
   // If cache has no string 'name' then it always tries to load
   // the given filename 'name'. This 'always loading' will be bad
   // behaviour if the ressource file does not exist.
   if (it != m_refs.end())
   {
      return it->second; // Cache Hit
   }

   de::Image img;
   if ( !dbLoadImage( img, name ) )
   {
      DE_ERROR("No image loaded for texture ", name )
      return TexRef();
   }

   Tex2D* tex = Tex2D::create( m_unitManager, name, img, so, keepImage );
   if ( !tex )
   {
      //DE_RUNTIME_ERROR(name, "Got tex nullptr")
      DE_ERROR( "No tex(",name,") created")
      return TexRef();
   }

   m_Textures.emplace_back( tex );
   m_refs[ name ] = TexRef( tex );
   it = m_refs.find( name );

   if ( it == m_refs.end() )
   {
      //DE_RUNTIME_ERROR(name, "Got m_Textures.end() iterator")
      DE_ERROR( "No tex(",name,") added to cache")
      return TexRef();
   }
   else
   {
      return it->second; // Cache Hit
   }
}

TexRef
TexManager::createTexture(
   std::string const & name,
   Image const & img,
   SO so,
   bool keepImage )
{
   if ( name.empty() ) { return TexRef(); }
   if ( img.empty() ) { return TexRef(); }
   if ( img.getFormat() != ColorFormat::RGBA8888 &&
        img.getFormat() != ColorFormat::RGB888 )
   {
      DE_ERROR("Unsupported image format ", img.toString() )
      return TexRef();
   }

   auto it = m_refs.find( name ); // Find 'name' in cache
   if (it != m_refs.end()) { return it->second; } // Cache hit

   auto tex = Tex2D::create( m_unitManager, name, img, so, keepImage );
   if ( !tex )
   {
      return TexRef();
   }

   m_Textures.emplace_back( tex );
   m_refs[ name ] = TexRef(tex);
   return m_refs[ name ];

}

void
TexManager::initGL( TexUnitManager* unitManager, int logLevel )
{
   m_unitManager = unitManager;

   //::glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); GL_VALIDATE   // Affects glTexImage2D()
   //::glPixelStorei( GL_PACK_ALIGNMENT, 1 ); GL_VALIDATE     // Affects glReadPixels()
   m_maxTex2DSize = glGetIntegerDE( GL_MAX_TEXTURE_SIZE );
   if ( logLevel > 0 )
   {
      DE_DEBUG( "Tex2D.MaxSize = ", m_maxTex2DSize )
      if ( logLevel > 1 )
      {
         dump();
      }
   }
}

void
TexManager::destroyGL( int logLevel )
{
   for ( auto cached : m_Textures )
   {
      if ( !cached ) continue;
      delete cached;
   }

   m_Textures.clear();

   for ( auto cached : m_TexturesToRemove )
   {
      if ( !cached ) continue;
      cached->destroyTexture();
      delete cached;
   }

   m_TexturesToRemove.clear();
}


void
TexManager::dump()
{
   DE_DEBUG( "TexCache.Count = ", m_Textures.size() )
   for ( size_t i = 0; i < m_Textures.size(); ++i )
   {
      Tex* tex = m_Textures[ i ];
      if ( tex )
      {
         DE_DEBUG( "TexCache[",i,"] = ", tex->toString() )
      }
      else
      {
         DE_ERROR( "TexCache[",i,"] = nullptr" )
      }
   }
}

bool
TexManager::upload( Tex* tex, Image const & img )
{
   if ( !tex )
   {
      return false;
   }

   return tex->upload( img );
}

void
TexManager::updateTextures()
{
   for ( size_t i = 0; i < m_TexturesToRemove.size(); ++i )
   {
      Tex* markedAsTrash = m_TexturesToRemove[ i ];
      if ( markedAsTrash )
      {
         markedAsTrash->destroyTexture();
         delete markedAsTrash;
      }
   }
   m_TexturesToRemove.clear();
}

void
TexManager::clearTextures()
{
   for ( size_t i = 0; i < m_Textures.size(); ++i )
   {
      Tex* cached = m_Textures[ i ];
      if ( cached )
      {
         DE_DEBUG( "MarkAsObsolete Tex[",i,"] ", cached->toString() )
         m_TexturesToRemove.emplace_back( cached );
      }
   }
   m_Textures.clear();
}

void
TexManager::removeTexture( std::string const & name )
{
   auto ref = getTexture( name );
   removeTexture( ref.m_tex );
}

void
TexManager::removeTexture( Tex* tex )
{
   if ( !tex ) { DE_DEBUG("No tex") return; }
   if ( !m_unitManager ) { DE_DEBUG("No m_unitManager") return; }

   // Delete cache entry
   auto cacheIt = std::find_if(
         m_Textures.begin(),
         m_Textures.end(),
         [&] ( Tex const * const p ) { return p == tex; } );
   if ( cacheIt == m_Textures.end() )
   {
      return;
   }

   Tex* cached = *cacheIt;
   if ( cached )
   {
      DE_DEBUG("[TexCache] DeleteEntry ", cached->toString() )
   }
   m_unitManager->unbindTexture( tex );
   m_Textures.erase( cacheIt );
   m_TexturesToRemove.emplace_back( tex );
}



// int32_t findTex( Tex* tex ) const;
// int32_t findTex( std::string const & name ) const;
// bool hasTex( std::string const & name ) const;
// TexRef getTex( std::string const & name ) const;
// TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, SO so = SO() );
// TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, SO so = SO() );
uint32_t
TexManager::getMaxTex2DSize() const
{
   return m_maxTex2DSize;
}

Tex*
TexManager::findTexture( std::string const & name ) const
{
   return nullptr;
}

bool
TexManager::hasTexture( std::string const & name ) const
{
   auto it = m_refs.find( name ); // Find 'name' in cache
   if (it == m_refs.end()) // Cache miss => try load ( every time, yet )
   {
      return false;
   }
   else
   {
      return true;
   }
}




Material::Material()
   : state()
   , Lighting( 0 )
   , Wireframe( false ) // ? Not used, wireframes are overlays of debug geom
   , CloudTransparent( false ) // Benni special. uses only tex.r channel as color(r,r,r,r)
   , FogEnable( false )
   , FogColor( 0xFFFFFFFF )
   , FogMode( 0 )
   , FogLinearStart( 0.0f )
   , FogLinearEnd( 1.0f )
   , FogExpDensity( 0.001f )
   , Kd( 1,1,1,1 )
   , Ka( 0,0,0,0 )
   , Ks( 1,1,1,1 )
   , Ke( 0,0,0,0 )
   , Kt( 1,1,1,1 ) // ?
   , Kr( 1,1,1,1 ) // ?
   , Opacity( 1.0f ) // ?
   , Reflectivity( 0.1f ) // ?
   , Shininess( 30.0f ) // ?
   , Shininess_strength( 1.5f ) // ?
   , MaterialType( 0 ) // ?
   , GouraudShading( false ) // ?
   , DiffuseMap()
   , BumpMap()
   , BumpScale( 1.0f )
   , NormalMap()
   , NormalScale( 1.0f )
   , SpecularMap()
   , DetailMap()
   , Name("")
{} // Default ctr constructed is also always the default material.

std::string
Material::toString() const
{
   std::ostringstream s;

   s << "Tex" << getTextureCount() << ", ";
   if ( state.culling.isEnabled() ) { s << state.culling.toString(); }
   if ( Wireframe ){ s << ", W"; }
   if ( FogEnable ){ s << ", F"; }
   //	illum 2
   if ( Lighting > 0) { s << ", illum"<<Lighting; }
   s << ", Ns:" << Shininess;   //	Ns 30.0000
   s << ", Ni:" << Shininess_strength;  //	Ni 1.5000
   s << ", Tr:" << Reflectivity;   //	Tr 0.0000
   if ( hasDiffuseMap() ){ s << ", Td:" << getDiffuseMap().toString(); }
   if ( hasBumpMap() ){ s << ", Tb:" << getBumpMap().toString(); }
   if ( hasNormalMap() ){ s << ", Tn:" << getNormalMap().toString(); }
   if ( hasSpecularMap() ){ s << ", Ts:" << getSpecularMap().toString(); }
   if ( hasDetailMap() ){ s << ", Tdet:" << getDetailMap().toString(); }
   //   { s << ", Ka:" << Ka; }   //	Ka 0.5882 0.5882 0.5882
   //   { s << ", Kd:" << Kd; }   //	Kd 1.0000 1.0000 1.0000
   //   { s << ", Ks:" << Ks; }   //	Ks 0.3600 0.3600 0.3600
   //   { s << ", Ke:" << Ke; }   //	Ke 0.0000 0.0000 0.0000
   //   { s << ", Kt:" << Kt; }
   //	Tf 1.0000 1.0000 1.0000
   //	d 1.0000
   return s.str();
}

//void setFog( bool enabled ) { FogEnable = enabled; }
//void setLighting( int illum ) { Lighting = illum; }
//void setWireframe( bool enabled ) { Wireframe = enabled; }
//void setCulling( bool enable ) { state.culling.setEnabled( enable ); }
//void setDepth( bool enable ) { state.depth.setEnabled( enable ); }
//void setStencil( bool enable ) { state.stencil.setEnabled( enable ); }
//void setBlend( bool enable ) { state.blend.setEnabled( enable ); }
//void setCulling( Culling const & cull ) { state.culling = cull; }
//void setDepth( Depth const & depth ) { state.depth = depth; }
//void setStencil( Stencil const & stencil ) { state.stencil = stencil; }
//void setBlend( Blend const & blend ) { state.blend = blend; }

uint32_t
Material::getTextureCount() const
{
   uint32_t n = 0;
   n += DiffuseMap.empty() ? 0 : 1;
   n += BumpMap.empty() ? 0 : 1;
   n += NormalMap.empty() ? 0 : 1;
   n += SpecularMap.empty() ? 0 : 1;
   n += DetailMap.empty() ? 0 : 1;
   return n;
}

bool
Material::hasTexture() const { return getTextureCount() > 0; }

TexRef const &
Material::getTexture( int stage ) const
{
        if ( stage == 0 ) { return DiffuseMap; }
   else if ( stage == 1 ) { return BumpMap; }
   else if ( stage == 2 ) { return NormalMap; }
   else if ( stage == 3 ) { return SpecularMap; }
   else if ( stage == 4 ) { return DetailMap; }
   else
   {
      //DE_RUNTIME_ERROR( std::to_string( stage ), "Invalid tex stage" )
      DE_ERROR( "Material(",Name,") :: Bad stage index " + std::to_string( stage ) )
      return DiffuseMap;
   }
}

TexRef &
Material::getTexture( int stage )
{
        if ( stage == 0 ) { return DiffuseMap; }
   else if ( stage == 1 ) { return BumpMap; }
   else if ( stage == 2 ) { return NormalMap; }
   else if ( stage == 3 ) { return SpecularMap; }
   else if ( stage == 4 ) { return DetailMap; }
   else
   {
      //DE_RUNTIME_ERROR( std::to_string( stage ), "Invalid tex stage" )
      DE_ERROR( "Material(",Name,") :: Bad stage index " + std::to_string( stage ) )
      return DiffuseMap;
   }
}

void Material::setTexture( int stage, TexRef const & ref )
{
        if ( stage == 0 ) { DiffuseMap = ref; }
   else if ( stage == 1 ) { BumpMap = ref; }
   else if ( stage == 2 ) { NormalMap = ref; }
   else if ( stage == 3 ) { SpecularMap = ref; }
   else if ( stage == 4 ) { DetailMap = ref; }
   else
   {
      //throw std::runtime_error( "Invalid tex stage " + std::to_string( stage ) );
      DE_ERROR( "Material(",Name,") :: Bad stage index " + std::to_string( stage ) )
   }
}

//void setTexture( int stage, Tex* tex ) { setTexture( stage, TexRef(tex) ); }
//void setBumpScale( float scale ) { BumpScale = scale; }

//void setDiffuseMap( TexRef ref ) { DiffuseMap = ref; }
//void setBumpMap( TexRef ref ) { BumpMap = ref; }
//void setNormalMap( TexRef ref ) { NormalMap = ref; }
//void setSpecularMap( TexRef ref ) { SpecularMap = ref; }

//void setDiffuseMap( Tex* tex ) { DiffuseMap = TexRef( tex ); }
//void setBumpMap( Tex* tex ) { BumpMap = TexRef( tex ); }
//void setNormalMap( Tex* tex ) { NormalMap = TexRef( tex ); }
//void setSpecularMap( Tex* tex ) { SpecularMap = TexRef( tex ); }

//bool hasDiffuseMap() const { return !DiffuseMap.empty(); }
//bool hasBumpMap() const { return !BumpMap.empty(); }
//bool hasNormalMap() const { return !NormalMap.empty(); }
//bool hasSpecularMap() const { return !SpecularMap.empty(); }
//bool hasDetailMap() const { return !DetailMap.empty(); }

//TexRef const & getDiffuseMap() const { return DiffuseMap; }
//TexRef const & getBumpMap() const { return BumpMap; }
//TexRef const & getNormalMap() const { return NormalMap; }
//TexRef const & getSpecularMap() const { return SpecularMap; }
//TexRef const & getDetailMap() const { return DetailMap; }

//TexRef & getDiffuseMap() { return DiffuseMap; }
//TexRef & getBumpMap() { return BumpMap; }
//TexRef & getNormalMap() { return NormalMap; }
//TexRef & getSpecularMap() { return SpecularMap; }
//TexRef & getDetailMap() { return DetailMap; }

//   bool
//   operator==( Material const & o ) const
//   {
//      if ( state != o.state ) return false;
//      if ( illum != o.illum ) return false;
//      if ( fog != o.fog ) return false;
//      if ( wireframe != o.wireframe ) return false;
//      if ( state != o.state ) return false;
//      if ( cloud_transparent != o.cloud_transparent ) return false;
//      if ( m_DiffuseMap.tex != o.m_DiffuseMap.tex ) return false;
//      if ( m_BumpMap.tex != o.m_BumpMap.tex ) return false;
//      if ( m_NormalMap.tex != o.m_NormalMap.tex ) return false;
//      if ( m_SpecularMap.tex != o.m_SpecularMap.tex ) return false;
//      if ( m_DetailMap.tex != o.m_DetailMap.tex ) return false;
//      return true;
//   }

//   bool
//   operator!= ( Material const & o ) const { return !( o == *this ); }

//# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware
//# File Created: 10.01.2012 12:09:34

//newmtl 12993_diffuse
//	Ns 30.0000
//	Ni 1.5000
//	d 1.0000
//	Tr 0.0000
//	illum 2
//	Ka 1.0000 1.0000 1.0000    //	Ka 0.5882 0.5882 0.5882
//	Kd 1.0000 1.0000 1.0000    //	Kd 0.5882 0.5882 0.5882
//	Ks 0.3600 0.3600 0.3600    //	Ks 0.0000 0.0000 0.0000
//	Ke 0.0000 0.0000 0.0000    //	Ke 0.0000 0.0000 0.0000
//	Tf 1.0000 1.0000 1.0000
//	map_Ka fish.jpg            //	map_Ka moon_diffuse.jpg
//	map_Kd fish.jpg            //	map_Kd moon_diffuse.jpg
//	map_bump moon_normal.jpg
//	bump moon_normal.jpg
//
//      Illumination Properties that are turned on in the
//      model Property Editor
//      0 Color on and Ambient off
//      1 Color on and Ambient on
//      2 Highlight on
//      3 Reflection on and Ray trace on
//      4 Transparency: Glass on //      Reflection: Ray trace on
//      5 Reflection: Fresnel on and Ray trace on
//      6 Transparency: Refraction on  //      Reflection: Fresnel off and Ray trace on
//      7 Transparency: Refraction on //      Reflection: Fresnel on and Ray trace on
//      8 Reflection on and Ray trace off
//      9 Transparency: Glass on //      Reflection: Ray trace off
//      10 Casts shadows onto invisible surfaces

//   bool
//   loadMTL( std::string uri, VideoDriver* driver );

//   bool
//   saveMTL( std::string uri ) const;


bool
Material::writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* matNode ) const
{
   tinyxml2::XMLElement* cullNode = doc.NewElement( "culling" );
   cullNode->SetAttribute("on", state.culling.isEnabled() );
   cullNode->SetAttribute("front", state.culling.isFront() );
   cullNode->SetAttribute("back", state.culling.isFront() );
   cullNode->SetAttribute("cw", state.culling.isCW() );
   matNode->InsertEndChild( cullNode );

   tinyxml2::XMLElement* blendNode = doc.NewElement( "blend" );
   blendNode->SetAttribute("on", state.blend.enabled );
   blendNode->SetAttribute("eq", Blend::getEquationString(state.blend.equation).c_str() );
   blendNode->SetAttribute("src_rgb", Blend::getFunctionString(state.blend.src_rgb).c_str() );
   blendNode->SetAttribute("src_a", Blend::getFunctionString(state.blend.src_a).c_str() );
   blendNode->SetAttribute("dst_rgb", Blend::getFunctionString(state.blend.dst_rgb).c_str() );
   blendNode->SetAttribute("dst_a", Blend::getFunctionString(state.blend.dst_a).c_str() );
   matNode->InsertEndChild( blendNode );

   tinyxml2::XMLElement* depthNode = doc.NewElement( "depth" );
   depthNode->SetAttribute("on", state.depth.isEnabled() );
   depthNode->SetAttribute("zwrite", state.depth.isZWriteEnabled() );
   depthNode->SetAttribute("fn", Depth::getString( state.depth.getFunc() ).c_str() );
   matNode->InsertEndChild( depthNode );

   tinyxml2::XMLElement* stencilNode = doc.NewElement( "stencil" );
   stencilNode->SetAttribute("on", state.stencil.enabled );
   stencilNode->SetAttribute("sfail", Stencil::getActionString( state.stencil.sfail ).c_str() );
   stencilNode->SetAttribute("zfail", Stencil::getActionString( state.stencil.zfail ).c_str() );
   stencilNode->SetAttribute("zpass", Stencil::getActionString( state.stencil.zpass ).c_str() );
   stencilNode->SetAttribute("func", Stencil::getFuncString( state.stencil.testFunc ).c_str() );
   stencilNode->SetAttribute("refValue", state.stencil.testRefValue );
   stencilNode->SetAttribute("testMask", state.stencil.testMask );
   stencilNode->SetAttribute("stencilMask", state.stencil.stencilMask );
   matNode->InsertEndChild( stencilNode );

   tinyxml2::XMLElement* wireNode = doc.NewElement( "wireframe" );
   wireNode->SetAttribute("on", Wireframe );
   matNode->InsertEndChild( wireNode );

   tinyxml2::XMLElement* cloudtNode = doc.NewElement( "cloud" );
   cloudtNode->SetAttribute("on", CloudTransparent );
   matNode->InsertEndChild( cloudtNode );

   tinyxml2::XMLElement* illumNode = doc.NewElement( "illumination" );
   illumNode->SetAttribute("v", Lighting );
   matNode->InsertEndChild( illumNode );

   tinyxml2::XMLElement* fogNode = doc.NewElement( "fog" );
   fogNode->SetAttribute("v", FogEnable );
   matNode->InsertEndChild( fogNode );

   //	Ka 0.5882 0.5882 0.5882
   //	Kd 1.0000 1.0000 1.0000
   //	Ks 0.3600 0.3600 0.3600
   //	Ke 0.0000 0.0000 0.0000

   tinyxml2::XMLElement* ambientNode = doc.NewElement( "Ka" );
   ambientNode->SetAttribute("r", Ka.r );
   ambientNode->SetAttribute("g", Ka.g );
   ambientNode->SetAttribute("b", Ka.b );
   ambientNode->SetAttribute("a", Ka.a );
   matNode->InsertEndChild( ambientNode );

   tinyxml2::XMLElement* diffuseNode = doc.NewElement( "Kd" );
   diffuseNode->SetAttribute("r", Kd.r );
   diffuseNode->SetAttribute("g", Kd.g );
   diffuseNode->SetAttribute("b", Kd.b );
   diffuseNode->SetAttribute("a", Kd.a );
   matNode->InsertEndChild( diffuseNode );

   tinyxml2::XMLElement* specularNode = doc.NewElement( "Ks" );
   specularNode->SetAttribute("r", Ks.r );
   specularNode->SetAttribute("g", Ks.g );
   specularNode->SetAttribute("b", Ks.b );
   specularNode->SetAttribute("a", Ks.a );
   matNode->InsertEndChild( specularNode );

   tinyxml2::XMLElement* emissiveNode = doc.NewElement( "Ke" );
   emissiveNode->SetAttribute("r", Ke.r );
   emissiveNode->SetAttribute("g", Ke.g );
   emissiveNode->SetAttribute("b", Ke.b );
   emissiveNode->SetAttribute("a", Ke.a );
   matNode->InsertEndChild( emissiveNode );

   tinyxml2::XMLElement* reflectNode = doc.NewElement( "Kr" );
   reflectNode->SetAttribute("r", Kr.r );
   reflectNode->SetAttribute("g", Kr.g );
   reflectNode->SetAttribute("b", Kr.b );
   reflectNode->SetAttribute("a", Kr.a );
   matNode->InsertEndChild( reflectNode );

   tinyxml2::XMLElement* transparentNode = doc.NewElement( "Kt" );
   transparentNode->SetAttribute("r", Kt.r );
   transparentNode->SetAttribute("g", Kt.g );
   transparentNode->SetAttribute("b", Kt.b );
   transparentNode->SetAttribute("a", Kt.a );
   matNode->InsertEndChild( transparentNode );

   tinyxml2::XMLElement* shinyNode = doc.NewElement( "Ns" );
   shinyNode->SetAttribute("v", Shininess );
   matNode->InsertEndChild( shinyNode );

   tinyxml2::XMLElement* shinysNode = doc.NewElement( "Ni" );
   shinysNode->SetAttribute("v", Shininess_strength );
   matNode->InsertEndChild( shinysNode );

   tinyxml2::XMLElement* reflectivityNode = doc.NewElement( "Tr" );
   reflectivityNode->SetAttribute("v", Reflectivity );
   matNode->InsertEndChild( reflectivityNode );

   //	d 1.0000

//   if ( hasDiffuseMap() )
//   {
//      TexRef const & ref = getDiffuseMap();
//      tinyxml2::XMLElement* texNode = doc.NewElement( "Td" );
//      texNode->SetAttribute("name", ref.tex->getName().c_str() );
//      texNode->SetAttribute("src", ref.tex->getName().c_str() );
//      matNode->InsertEndChild( texNode );
//   }

//   if ( hasBumpMap() ){ s << ", Tb:" << getBumpMap().tex->getName(); }
//   if ( hasNormalMap() ){ s << ", Tn:" << getNormalMap().tex->getName(); }
//   if ( hasSpecularMap() ){ s << ", Ts:" << getSpecularMap().tex->getName(); }
//   if ( hasDetailMap() ){ s << ", Tdet:" << getDetailMap().tex->getName(); }
   return true;
}

bool
Material::saveXML( std::string uri ) const
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* matNode = doc.NewElement( "Material" );
   matNode->SetAttribute( "name", Name.c_str() );
   matNode->SetAttribute( "tex-count", int( getTextureCount() ) );
   writeXML( doc, matNode );

   doc.InsertEndChild( matNode );
   tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DEM_ERROR("Cant save xml ", uri)
      return false;
   }
   return true;
}

bool
Material::loadXML( std::string uri )
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLError err = doc.LoadFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant load xml ", uri)
      return false;
   }

/*
   std::string xmlDir = dbGetFileDir( uri );

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
      Name = atlasNode->Attribute( "name" );
   }
   else
   {
      DE_ERROR("No atlas name found ", uri)
   }

   int pageCount = atlasNode->IntAttribute( "pages" );

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
      pages.emplace_back();
      ImageAtlasPage & page = pages.back();
      page.pageId = int( pages.size() );
      page.name = pageNode->Attribute("src");

      std::string pageUri = xmlDir + "/" + page.name;
      if ( !dbLoadImage( page.img, pageUri ) )
      {
         DE_ERROR("No page ", page.pageId," src image uri ", pageUri)
         page.img.clear();
      }

      if ( pageW != page.img.getWidth() )
      {
         DE_ERROR("Differing src image width ", page.name)
      }
      if ( pageH != page.img.getHeight() )
      {
         DE_ERROR("Differing src image height ", page.name)
      }

      // Load Refs:
      int refCount = pageNode->IntAttribute("refs");

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
         bool wantMipmaps = refNode->IntAttribute("mipmaps") != 0 ? true:false;
         std::string name = refNode->Attribute("name" );
         ref.id = refId;
         ref.name = name;
         ref.img = &page.img;
         ref.page = page.pageId;
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
*/
   return true;
}




} // end namespace gpu.
} // end namespace de.



#if 0
// static
std::string
GLES::getActiveAttribString( uint32_t program )
{
   std::stringstream s;
   GLint attribCount = getProgramValue( program, GL_ACTIVE_ATTRIBUTES );
   //GLint maxAttribCount = getProgramValue( program, GL_MAX_ATTRIBUTES );
//   GLint maxUniformLen;
//   GLint maxUniforms;
   s << "[GL] Attributes.Count = " << attribCount << "\n";
//   s << "[GL] Uniforms.MaxLen = " << maxUniformLen << "\n";
//   std::vector< char > uniformName;
//   uniformName.reserve( maxUniformLen );
//   for ( GLint i = 0; i < maxUniforms; ++i )
//   {
//      GLint uniformSize;
//      GLenum uniformType;
//      ::glGetActiveUniform( program, i, maxUniformLen, NULL, &uniformSize, &uniformType, uniformName.data() );
//      GLint location = ::glGetUniformLocation ( program, uniformName.data() );

//      std::string uniformTypeStr;
//      switch ( uniformType )
//      {
//         case GL_FLOAT:       uniformTypeStr = "float"; break;
//         case GL_FLOAT_VEC2:  uniformTypeStr = "vec2"; break;
//         case GL_FLOAT_VEC3:  uniformTypeStr = "vec3"; break;
//         case GL_FLOAT_VEC4:  uniformTypeStr = "vec4"; break;
//         case GL_INT:         uniformTypeStr = "int"; break;
//         default: break;
//      }
//      s << "[GL] Uniforms[" << i << "] loc:" << location << ", name:" << uniformName.data() << ", type:" << uniformType << ":" << uniformTypeStr << ", size:" << uniformSize << "\n";
//   }
   return s.str();
}
#endif
