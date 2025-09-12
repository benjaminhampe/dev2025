#pragma once
// #include <cstdint>
// #include <sstream>
// #include <vector>
//#include <de/Core.h>
//#include <de/Image.h>
#include <de/gpu/GPU.h>
#include <de/gpu/State.h>
//#include <de/Math3D_Quaternion.h>
#include <tinyxml2/tinyxml2.h>
// #include <de/GL_debug_layer.h>

namespace de {
namespace gpu {

// ===========================================================================
struct AnimTimer
// ===========================================================================
{
    double start = 0.0;
    double last = 0.0;
    double curr = 0.0;
    bool running = true;
    bool paused = false;
};

// ===========================================================================
struct SMaterialLight
// ===========================================================================
{
   int typ;
   glm::vec3 siz;   // world size of light source
   glm::vec3 pos;
   glm::vec3 color;
   glm::vec3 dir;
   float maxdist;    // distance
   //float power;      // distance
   //float m_Radius = 500.0f;
   glm::vec3 clipPos; // for Shader, computed by CPU

   //SMeshBuffer mesh;
};

// ===========================================================================
struct SMaterial
// ===========================================================================
{
   State state;
   int Lighting;   // 2 Highlight on
   bool Wireframe;
   bool CloudTransparent;
   bool FogEnable;
   uint32_t FogColor;
   uint32_t FogMode;
   float FogLinearStart;
   float FogLinearEnd;
   float FogExpDensity;
   glm::vec4 Kd; // diffuse_color; // Kd
   glm::vec4 Ka; // ambient_color; // Ka
   glm::vec4 Ks; // specular_color; // Ks
   glm::vec4 Ke; // emissive_color; // Ke
   glm::vec4 Kt; // TransparentColor; // Kt, Tf
   glm::vec4 Kr; // ReflectiveColor = Emissive?
   //uint32_t TransparentColor; // Kt, Tf
   //uint32_t ReflectiveColor; // Kr
   float Opacity; // d in MTL?
   float Reflectivity; // Tr
   float Shininess; // Ns
   float Shininess_strength; // Ni
   // 3ds
   uint32_t MaterialType; // EMT_SOLID, EMT_TRANSPARENT_VERTEX_ALPHA
   bool GouraudShading;
   TexRef DiffuseMap;
   TexRef BumpMap;
   float BumpScale;
   TexRef NormalMap;
   float NormalScale;
   TexRef SpecularMap;
   TexRef DetailMap;
   std::string Name;

   SMaterial();

   std::string toString() const;

   uint32_t getTextureCount() const;
   bool hasTexture() const;
   TexRef const & getTexture( int stage ) const;
   TexRef & getTexture( int stage );
   void setTexture( int stage, TexRef const & ref );

   std::vector< TexRef > getTextures() const
   {
      std::vector< TexRef > textures;
      if ( !DiffuseMap.empty() ) textures.emplace_back( DiffuseMap );
      if ( !BumpMap.empty() ) textures.emplace_back( BumpMap );
      if ( !NormalMap.empty() ) textures.emplace_back( NormalMap );
      if ( !SpecularMap.empty() ) textures.emplace_back( SpecularMap );
      if ( !DetailMap.empty() ) textures.emplace_back( DetailMap );
      return textures;
   }


   void setFog( bool enabled ) { FogEnable = enabled; }
   void setLighting( int illum ) { Lighting = illum; }
   void setWireframe( bool enabled ) { Wireframe = enabled; }
   void setCulling( bool enable ) { state.culling.setEnabled( enable ); }
   void setDepth( bool enable ) { state.depth.setEnabled( enable ); }
   void setStencil( bool enable ) { state.stencil.setEnabled( enable ); }
   void setBlend( bool enable ) { state.blend.setEnabled( enable ); }
   void setCulling( Culling const & cull ) { state.culling = cull; }
   void setDepth( Depth const & depth ) { state.depth = depth; }
   void setStencil( Stencil const & stencil ) { state.stencil = stencil; }
   void setBlend( Blend const & blend ) { state.blend = blend; }
   void setBumpScale( float scale ) { BumpScale = scale; }
   void setDiffuseMap( TexRef ref ) { DiffuseMap = ref; }
   void setBumpMap( TexRef ref ) { BumpMap = ref; }
   void setNormalMap( TexRef ref ) { NormalMap = ref; }
   void setSpecularMap( TexRef ref ) { SpecularMap = ref; }
/*
   void setTexture( int stage, ITexture* tex ) { setTexture( stage, TexRef(tex) ); }
   void setDiffuseMap( ITexture* tex ) { DiffuseMap = TexRef( tex ); }
   void setBumpMap( ITexture* tex ) { BumpMap = TexRef( tex ); }
   void setNormalMap( ITexture* tex ) { NormalMap = TexRef( tex ); }
   void setSpecularMap( ITexture* tex ) { SpecularMap = TexRef( tex ); }
*/
   bool hasDiffuseMap() const { return !DiffuseMap.empty(); }
   bool hasBumpMap() const { return !BumpMap.empty(); }
   bool hasNormalMap() const { return !NormalMap.empty(); }
   bool hasSpecularMap() const { return !SpecularMap.empty(); }
   bool hasDetailMap() const { return !DetailMap.empty(); }
   TexRef const & getDiffuseMap() const { return DiffuseMap; }
   TexRef const & getBumpMap() const { return BumpMap; }
   TexRef const & getNormalMap() const { return NormalMap; }
   TexRef const & getSpecularMap() const { return SpecularMap; }
   TexRef const & getDetailMap() const { return DetailMap; }
   TexRef & getDiffuseMap() { return DiffuseMap; }
   TexRef & getBumpMap() { return BumpMap; }
   TexRef & getNormalMap() { return NormalMap; }
   TexRef & getSpecularMap() { return SpecularMap; }
   TexRef & getDetailMap() { return DetailMap; }
//   bool
//   operator==( Material const & o ) const;
//   bool
//   operator!= ( Material const & o ) const { return !( o == *this ); }
//   bool
//   loadMTL( std::string uri, VideoDriver* driver );
//   bool
//   saveMTL( std::string uri ) const;
   bool
   writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* matNode ) const;
   bool
   saveXML( std::string uri ) const;
   bool
   loadXML( std::string uri );

};


#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct S3DVertex // FVF_POSITION_XYZ | FVF_NORMAL_XYZ | FVF_RGBA | FVF_TEXCOORD0
// ===========================================================================
{
   glm::vec3 pos;
   glm::vec3 normal;
   uint32_t color;
   glm::vec2 tex;    // 12 + 12 + 4 + 8 = 36 Bytes if packed
public:
   S3DVertex()
      : pos(), normal( 0, 0, 1 ), color( 0xFFFF00FF ), tex()
   {}
   S3DVertex( float x, float y, float z, float nx, float ny, float nz, uint32_t crgba, float u, float v )
      : pos( x,y,z ), normal( nx,ny,nz ), color( crgba ), tex( u,v )
   {}
   S3DVertex( glm::vec3 const & p, glm::vec3 const & nrm, uint32_t crgba, glm::vec2 const & tex0 )
      : pos( p ), normal( nrm ), color( crgba ), tex( tex0 )
   {}

   void
   flipV()
   {
      tex.y = glm::clamp( 1.0f - tex.y, 0.f, 1.0f ); // Invert v for OpenGL
   }

   /*
   // STANDARD_FVF = POS_XYZ | NORMAL_XYZ | COLOR_RGBA | TEX0_UV = 36 Bytes.
   static FVF const &
   getFVF()
   {
      static FVF const fvf( sizeof( S3DVertex ),
         {  VertexAttrib( VertexAttribType::F32, 3, VertexAttribSemantic::Pos ),
            VertexAttrib( VertexAttribType::F32, 3, VertexAttribSemantic::Normal ),
            VertexAttrib( VertexAttribType::U8,  4, VertexAttribSemantic::Color ),
            VertexAttrib( VertexAttribType::F32, 2, VertexAttribSemantic::Tex )
         }
      );
      return fvf;
   }
   glm::vec3 const & getPos() const { return pos; }
   glm::vec3 & getPos() { return pos; }
   void setPos( glm::vec3 const & position ) { pos = position; }
   glm::vec3 const & getNormal() const { return normal; }
   uint32_t const & getColor() const { return color; }
   glm::vec2 const & getTexCoord( int stage = 0 ) const { return tex; }
   glm::vec3 getTangent( glm::vec3 const & ) { return {}; }
   glm::vec3 getBiTangent( glm::vec3 const & ) { return {}; }
   void setNormal( glm::vec3 const & nrm ) { this->normal = nrm; }
   void setTangent( glm::vec3 const & ) {}
   void setBiTangent( glm::vec3 const & ) {}
   void setColor( uint32_t const & colorRGBA ) { this->color = colorRGBA; }
   void setTexCoord( glm::vec2 const & texCoord, int stage = 0 ) { this->tex = texCoord; }
   */
};
#pragma pack( pop )



// =======================================================================
template < typename T > struct MeshIntersectionResult
// =======================================================================
{
   glm::tvec3< T > position;
   T distance = T(0);
   Triangle3< T > triangle;
   int32_t triangleIndex = -1;
   int32_t bufferIndex = -1;
   int32_t meshIndex = -1;
   int32_t nodeIndex = -1;
};

// ===========================================================================
struct SMeshBuffer
// =======================================================================
{
    typedef S3DVertex TVertex;   /// @brief Vertices
    typedef glm::vec3 V3;

    PrimitiveType primitiveType;   /// @brief GLenum PrimitiveType;
    bool bVisible;   /// @brief Used by modeler, viewer and renderer
    bool bShouldUpload;
    //FVF m_FVF;   /// @brief Flexible vertex format, init TVertex::getFVF(), then can be read and changed at runtime.
    uint32_t vao;   /// @brief Vertex array object handle, if driver supports it.
    uint32_t vbo;   /// @brief Vertex buffer object handle, if driver supports it.
    uint32_t ibo;   /// @brief Index buffer object handle, if driver supports it.
    Box3f boundingBox;   /// @brief 3d bounding box.
    //glm::dmat4 ModelMat;   /// @brief Absolute transformation of the geometry.
    SMaterial material;   /// @brief Standard material.
    typedef std::vector< TVertex > TVertexVector;   /// @brief Vertices
    TVertexVector vertices;   /// @brief Vertices
    typedef std::vector< uint32_t > TIndexVector;   /// @brief Indices
    TIndexVector indices;   /// @brief Indices
    std::string name;   /// @brief Able to find specific MeshBuffer in Mesh by (category/material) name. ( e.g. "Lines", "Triangles" )

    /// @brief Constructor
    explicit SMeshBuffer( PrimitiveType primType = PrimitiveType::Triangles )
        : primitiveType( primType )
        , bVisible( true )
        , bShouldUpload( false )
        //, m_FVF( TVertex::getFVF() )
        , vao( 0 )
        , vbo( 0 )
        , ibo( 0 )

        , boundingBox()
        //, ModelMat( 1.0 )
        //, name("")
    {
    // DE_DEBUG(getName())
    }

   ~SMeshBuffer()
    {
        if ( vao )
        {
            DE_ERROR("Forgot to call SMeshBuffer.destroy()")
        }
    }

    void destroy();
    void upload( bool bForceUpload = false );
    void draw() const;

    TVertexVector const &    getVertexVector() const { return vertices; }
    TVertexVector &          getVertexVector()        { return vertices; }
    S3DVertex const &        getVertex( size_t i ) const { return vertices[ i ]; }
    S3DVertex &              getVertex( size_t i )       { return vertices[ i ]; }
    TIndexVector const &     getIndexVector() const  { return indices; }
    TIndexVector &           getIndexVector()        { return indices; }

   /// @brief Generic GPU link, Is zero on OpenGLES 2.0 ( not used )
   /// even with ES3.1 the VBO/VAO does not work on Win10 + QtSDK (GLESv2.dll + libEGL [ANGLE] )
   /// what always works is simple glEnableVertexAttribArray() + glVertexAttribPointer()
   /// No glGenBuffers/glGenVertexArrayObject(),
   /// just always use cpu memory pointer while rendering ( immediate mode )
   /// see GLES::drawPrimitiveList().
   uint32_t &           getVBO() { return vbo; }
   uint32_t &           getIBO() { return ibo; }
   uint32_t &           getVAO() { return vao; }
   void                 setDirty() { bShouldUpload = true; }
   // [IsVisible]
   bool                 isVisible() const { return bVisible; }
   void                 setVisible( bool visible ) { bVisible = visible; }

   //glm::dmat4 const &   getModelMatrix() const { return ModelMat; }
   //void                 setModelMatrix( glm::dmat4 const & modelMat ) { ModelMat = modelMat; }
   //std::string const &  getName() const { return Name; }
   //void                 setName( std::string const & name ) { Name = name; }
   void                 clear() { clearVertices(); clearIndices(); boundingBox.reset(); }
   void                 clearVertices() { vertices.clear(); }
   void                 clearIndices() { indices.clear(); }
   void                 moreVertices( size_t n ) { vertices.reserve( vertices.size() + n ); }
   void                 moreIndices( size_t n ) { indices.reserve( indices.size() + n ); }

   // [MemorySizeInBytes]
   size_t               computeByteConsumption() const;

   std::string          str( bool withMaterial = false ) const;

   SMaterial const &    getMaterial() const { return material; }
   SMaterial &          getMaterial() { return material; }
   void                 setMaterial( SMaterial const & m ) { material = m; }
   void                 setLighting( int illum ) { getMaterial().setLighting( illum ); }
   void                 setFogEnable( bool enable ) { getMaterial().setFog( enable ); }
   void                 setWireframe( bool enable ) { getMaterial().setWireframe( enable ); }
   void                 setCulling( bool enable ) { getMaterial().setCulling( enable ); }
   void                 setDepth( bool enable ) { getMaterial().setDepth( enable ); }
   void                 setStencil( bool enable ) { getMaterial().setStencil( enable ); }
   void                 setBlend( bool enable ) { getMaterial().setBlend( enable ); }
   void                 setCulling( Culling cull ) { getMaterial().setCulling( cull ); }
   void                 setDepth( Depth depth ) { getMaterial().setDepth( depth ); }
   void                 setStencil( Stencil blend ) { getMaterial().setStencil( blend ); }
   void                 setBlend( Blend blend ) { getMaterial().setBlend( blend ); }
   void                 setPointSize( float ps ) { getMaterial().state.pointSize.m_Now = ps; }
   void                 setLineWidth( float lw ) { getMaterial().state.lineWidth.m_Now = lw; }
   void                 setTexture( int stage, Texture* tex ) { getMaterial().setTexture( stage, TexRef( tex ) ); }
   void                 setTexture( int stage, TexRef const & ref ) { getMaterial().setTexture( stage, ref ); }

   Box3f const &        getBoundingBox() const { return boundingBox; }
   Box3f &              getBoundingBox() { return boundingBox; }
   void                 setBoundingBox( Box3f const & bbox ) { boundingBox = bbox; }
   PrimitiveType        getPrimitiveType() const { return primitiveType; }
   void                 setPrimitiveType( PrimitiveType type ) { primitiveType = type; }
   //FVF const &        getFVF() const { return m_FVF; }
   //FVF &              getFVF() { return m_FVF; }
   //void               setFVF( FVF const & fvf ) { m_FVF = fvf; }
   size_t               getVertexCount() const  { return vertices.size(); }
   size_t               getVertexSize() const   { return sizeof( S3DVertex ); }
   size_t               getVertexCapacity() const {return vertices.capacity(); }
   void const *         getVertices() const     { return reinterpret_cast< void const* >( vertices.data() ); }
   void *               getVertices()            { return reinterpret_cast< void* >( vertices.data() ); }
   //IndexType          getIndexType() const   { return IndexType::U32; }
   uint32_t             getIndexSize() const    { return 4; }
   uint32_t             getIndexCount() const   { return indices.size(); }
   uint32_t             getIndexCapacity() const{ return indices.capacity(); }
   void const *         getIndices() const      { return reinterpret_cast< void const* >( indices.data() ); }
   void*                getIndices()            { return reinterpret_cast< void* >( indices.data() ); }
   uint32_t             getIndex( uint32_t i ) const  { return indices[ i ]; }
   uint32_t &           getIndex( uint32_t i )        { return indices[ i ]; }

   glm::vec3 const &    getPos( uint32_t i ) const { return vertices[ i ].pos; }
   glm::vec3 &          getPos( uint32_t i ) { return vertices[ i ].pos; }
   void                 setPos( uint32_t i, glm::vec3 const & v ) { vertices[ i ].pos = v; }
   //glm::vec3          getNormal( size_t i ) const { return {0,0,-1}; }
   //glm::vec3          getTangent( size_t i ) const { return {1,0,0}; }
   //glm::vec3          getBitangent( size_t i ) const { return {0,1,0}; }
   //uint32_t           getColor( size_t i ) const { return 0xFFFFFFFF; }
   //glm::vec2          getTexCoords( size_t i, int stage = 0 ) const { return {0,0}; }

   void                 recalculateBoundingBox()
   {
       if ( vertices.empty() )
       {
           boundingBox.reset();
       }
       else
       {
           boundingBox.reset( vertices[ 0 ].pos );

           for ( size_t i = 1; i < vertices.size(); ++i )
           {
               boundingBox.addInternalPoint( vertices[ i ].pos );
           }
       }
   }


   std::vector< V3 > getPoints() const
   {
       std::vector< V3 > pts;
       pts.reserve( getVertexCount() );
       for ( const auto & v : vertices )
       {
           pts.emplace_back( v.pos );
       }
       return pts;
   }

   void addVertex( S3DVertex const & vertex )
   {
        vertices.push_back( vertex );
   }

   void addLine( S3DVertex const & a, S3DVertex const & b )
   {
      auto v = getVertexCount();
      addVertex( a );
      addVertex( b );
      addIndex( v );
      addIndex( v+1 );
   }

   void
   addLineTriangle( S3DVertex const & a, S3DVertex const & b, S3DVertex const & c )
   {
      auto v = getVertexCount();
      addVertex( a );
      addVertex( b );
      addVertex( c );
      addIndexedLine( v, v+1 );
      addIndexedLine( v+1, v+2 );
      addIndexedLine( v+2, v );
   }

   void
   addLineQuad( S3DVertex const & a, S3DVertex const & b, S3DVertex const & c, S3DVertex const & d )
   {
      auto v = getVertexCount();
      addVertex( a );
      addVertex( b );
      addVertex( c );
      addVertex( d );
      addIndexedLine( v, v+1 );
      addIndexedLine( v+1, v+2 );
      addIndexedLine( v+2, v+3 );
      addIndexedLine( v+3, v );
   }

   void
   addIndex()
   {
      //assert( size_t( index ) <= size_t( std::numeric_limits< TIndex >::max() ) );
      if ( vertices.empty() )
      {
         //DE_ERROR("No Vertices")
         return;
      }
      indices.push_back( vertices.size() - 1 );
   }

   void
   addIndex( uint32_t index )
   {
      //assert( size_t( index ) <= size_t( std::numeric_limits< TIndex >::max() ) );
      indices.push_back( index );
   }

   void
   addTriangle( S3DVertex const & a, S3DVertex const & b, S3DVertex const & c, bool flip = false )
   {
      size_t const v = getVertexCount();
      if ( flip )
      {
         addVertex( a );
         addVertex( c );
         addVertex( b );
      }
      else
      {
         addVertex( a );
         addVertex( b );
         addVertex( c );
      }

      addIndex( v );
      addIndex( v+1 );
      addIndex( v+2 );
   }

   void
   addQuad( S3DVertex const & a, S3DVertex const & b, S3DVertex const & c, S3DVertex const & d, bool flip = false )
   {
      addVertex( a );
      addVertex( b );
      addVertex( c );
      addVertex( d );
      addIndexedQuad( flip );
   }

   void
   addVertexQuad( S3DVertex const & a, S3DVertex const & b, S3DVertex const & c, S3DVertex const & d, bool flip = false )
   {
      if ( flip )
      {
         addVertex( a );
         addVertex( c );
         addVertex( b );
         addVertex( a );
         addVertex( d );
         addVertex( c );
      }
      else
      {
         addVertex( a );
         addVertex( b );
         addVertex( c );
         addVertex( a );
         addVertex( c );
         addVertex( d );
      }
   }

   void
   addIndexedTriangle( uint32_t a, uint32_t b, uint32_t c, bool flip = false )
   {
      if ( flip )
      {
         addIndex( a );
         addIndex( c );
         addIndex( b );
      }
      else
      {
         addIndex( a );
         addIndex( b );
         addIndex( c );
      }
   }

   void
   addIndexedTriangle( bool flip = false )
   {
      uint32_t const v = getVertexCount();
      if ( v < 3 ) return;
      addIndexedTriangle( v-3, v-2, v-1, flip );
   }

   // Quad ABCD: (ccw) ACB + ADC, (cw) ABC + ACD
   void
   addIndexedQuad( uint32_t a, uint32_t b, uint32_t c, uint32_t d, bool flip = false )
   {
      addIndexedTriangle( a,b,c, flip );
      addIndexedTriangle( a,c,d, flip );
      //addIndex( a ); addIndex( c ); addIndex( b );
      //addIndex( a ); addIndex( d ); addIndex( c );
   }

   void
   addIndexedQuad( bool flip = false )
   {
      uint32_t const v = getVertexCount();
      if ( v < 4 ) return;
      addIndexedQuad( v-4, v-3, v-2, v-1, flip );
   }

   void
   addIndexedLine( uint32_t a, uint32_t b )
   {
      addIndex( a );
      addIndex( b );
   }

   void
   addIndexedLine()
   {
      uint32_t const v = getVertexCount();
      if ( v < 2 ) return;
      addIndex( v-2 );
      addIndex( v-1 );
   }

   void
   addIndexedLineTriangle( uint32_t a, uint32_t b, uint32_t c )
   {
      addIndex( a ); addIndex( b );
      addIndex( b ); addIndex( c );
      addIndex( c ); addIndex( a );
   }

   void
   addIndexedLineQuad( uint32_t a, uint32_t b, uint32_t c, uint32_t d )
   {
      addIndex( a ); addIndex( b );
      addIndex( b ); addIndex( c );
      addIndex( c ); addIndex( d );
      addIndex( d ); addIndex( a );
   }

   void
   addIndexedLineBox( uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                    uint32_t e, uint32_t f, uint32_t g, uint32_t h )
   {
      addIndexedLineQuad( a, b, c, d );
      addIndexedLineQuad( e, f, g, h );
      addIndexedLine( a, e );
      addIndexedLine( b, f );
      addIndexedLine( c, g );
      addIndexedLine( d, h );
   }

   void
   addIndexedLineTriangle()
   {
      uint32_t const v = getVertexCount();
      if ( v < 3 ) return;
      addIndexedLineTriangle( v-3, v-2, v-1 );
   }

   void
   addIndexedLineQuad()
   {
      uint32_t const v = getVertexCount();
      if ( v < 4 ) return;
      addIndexedLineQuad( v-4, v-3, v-2, v-1 );
   }

   void
   rotate( glm::vec3 new_nrm, glm::vec3 old_nrm = glm::vec3(0.0f,0.0f,-1.0f) )
   {
       if ( old_nrm != new_nrm )
       {
           recalculateBoundingBox();
           Box3f bb = getBoundingBox();
           auto cc = bb.getCenter();
           Quatf qq = Quatf::from2Vectors( old_nrm, new_nrm );
           for ( size_t i = 0; i < vertices.size(); ++i )
           {
               auto v1 = vertices[ i ].pos - cc; // dir from center to pos
               auto l1 = glm::length( v1 ); // length
               auto v2 = qq.rotate( v1 ); // rotated dir with length one.
               auto p2 = cc + v2 * l1; // pos = center + rotated dir with length len1.
               setPos( i, p2 );
           }
       }
   }

   std::vector< Triangle3f >
   createCollisionTriangles() const
   {
       if ( getPrimitiveType() != PrimitiveType::Triangles ) return {};

       std::vector< Triangle3f > tris;

       if ( getIndexCount() > 0 )
       {
           tris.reserve( tris.size() + getIndexCount()/3 );
           for ( size_t i = 0; i < getIndexCount()/3; ++i )
           {
               tris.emplace_back( getPos( getIndex( 3*i   ) ),
                                 getPos( getIndex( 3*i+1 ) ),
                                 getPos( getIndex( 3*i+2 ) ));
           }
       }
       else
       {
           tris.reserve( tris.size() + getVertexCount()/3 );
           for ( size_t i = 0; i < getVertexCount()/3; ++i )
           {
               tris.emplace_back( getPos( 3*i ),
                                 getPos( 3*i+1 ),
                                 getPos( 3*i+2 ) );
           }
       }

       return tris;
   }

   void
   forAllTriangles( std::function< void(uint32_t, Triangle3f const &)> const & func ) const
   {
       // [Triangles]
       if ( getPrimitiveType() == PrimitiveType::Triangles )
       {
           if ( getVertexCount() < 3 )
           {
               return; // no triangles
           }

           if ( getIndexCount() > 0 )
           {
               if ( getIndexCount() < 3 ) { return; }
               for ( size_t i = 0; i < getIndexCount()/3; i++ )
               {
                   auto const & a = getPos( getIndex( 3*i ) );
                   auto const & b = getPos( getIndex( 3*i+1 ) );
                   auto const & c = getPos( getIndex( 3*i+2 ) );
                   func( i, Triangle3f(a,b,c) );
               }
           }
           else
           {
               for ( size_t i = 0; i < getVertexCount()/3; i++ )
               {
                   auto const & a = getPos( 3*i );
                   auto const & b = getPos( 3*i+1 );
                   auto const & c = getPos( 3*i+2 );
                   func( i, Triangle3f(a,b,c) );
               }
           }
       }

       // [TriangleStrip]
       else if ( getPrimitiveType() == PrimitiveType::TriangleStrip )
       {
           if ( getVertexCount() < 3 )
           {
               return; // no triangle strip
           }

           if ( getIndexCount() > 0 )
           {
               if ( getVertexCount() < 3 ) { return; }
               if ( getIndexCount() < 3 ) { return; }

               for ( size_t i = 2; i < getIndexCount(); i++ )
               {
                   auto const & a = getPos( getIndex( i-2 ) );
                   auto const & b = getPos( getIndex( i-1 ) );
                   auto const & c = getPos( getIndex( i ) );
                   func( i-2, Triangle3f(a,b,c) );
               }
           }
           else
           {
               if ( getVertexCount() < 3 ) { return; }

               for ( size_t i = 2; i < getVertexCount(); i++ )
               {
                   auto const & a = getPos( i-2 );
                   auto const & b = getPos( i-1 );
                   auto const & c = getPos( i );
                   func( i-2, Triangle3f(a,b,c) );
               }
           }
       }

       // [BenniQuads]
       else if ( getPrimitiveType() == PrimitiveType::Quads )
       {
           if ( getVertexCount() < 4 )
           {
               return; // no quads
           }

           if ( getIndexCount() > 0 )
           {
               for ( size_t i = 0; i < getIndexCount()/4; i++ )
               {
                   auto const & a = getPos( getIndex( 4*i ) );
                   auto const & b = getPos( getIndex( 4*i+1 ) );
                   auto const & c = getPos( getIndex( 4*i+2 ) );
                   auto const & d = getPos( getIndex( 4*i+3 ) );
                   // process quad, aka 2 triangles from 4 indexed vertices
                   func( 2*i, Triangle3f(a,b,c) );
                   func( 2*i+1, Triangle3f(a,c,d) );
               }
           }
           else
           {
               for ( size_t i = 0; i < getVertexCount()/4; i++ )
               {
                   auto const & a = getPos( 4*i );
                   auto const & b = getPos( 4*i+1 );
                   auto const & c = getPos( 4*i+2 );
                   auto const & d = getPos( 4*i+3 );
                   // process quad, aka 2 triangles from 4 indexed vertices
                   func( 2*i, Triangle3f(a,b,c) );
                   func( 2*i+1, Triangle3f(a,c,d) );
               }
           }

       }
       else
       {
           DE_ERROR("Unsupported PrimType = ", primitiveType.str())
       }
   }

   template < typename T >
   bool
   intersectRayWithBoundingBox( Ray3< T > const & ray, MeshIntersectionResult< T > & result ) const
   {
       Box3< T >         bbox( boundingBox.getMin(), boundingBox.getMax() );
       T                 beginDist = std::numeric_limits< T >::max();
       T                 globalHitDist = beginDist;
       glm::tvec3< T >   globalHitPos;
       Triangle3< T >    globalTriangle;
       int32_t           globalTriangleIndex = -1;
       bool              doesIntersect = false;

       for ( size_t i = 0; i < 12; ++i ) // Loop 12 bbox triangles
       {
           glm::tvec3< T > localHitPos; // local

           Triangle3< T > const & localTriangle = bbox.getTriangle( i );

           if ( localTriangle.intersectRay( ray.getPos(), ray.getDir(), localHitPos ) )
           {
               auto localHitDist = glm::length( ray.getPos() - localHitPos );
               if ( localHitDist == beginDist || globalHitDist > localHitDist )
               {
                   doesIntersect = true;
                   globalHitDist = localHitDist;
                   globalHitPos = localHitPos;
                   globalTriangle = localTriangle;
                   globalTriangleIndex = i;
               }
           }
           //DE_DEBUG("Mesh[",i,"].Triangle[", t, "] = ", tri.toString() )
       }

       if ( doesIntersect )
       {
           result.meshIndex = -1;     // indicates bbox
           result.bufferIndex = -1;   // indicates bbox
           result.triangle = globalTriangle;
           result.triangleIndex = globalTriangleIndex; // triangle of bbox 0..11
           result.distance = globalHitDist;
           result.position = globalHitPos;
       }
       return doesIntersect;
   }

   template < typename T >
   bool
   intersectRay( Ray3< T > const & ray, MeshIntersectionResult< T > & result ) const
   {
       if ( !intersectRayWithBoundingBox( ray, result ) )
       {
           return false;
       }

       T const distMax = std::numeric_limits< T >::max();
       T globalHitDistance = distMax;
       glm::tvec3< T > globalHitPosition;
       uint32_t globalTriangleIndex = 0;
       Triangle3< T > globalTriangle;
       bool doesIntersect = false;

       forAllTriangles(
           [&] ( uint32_t triIndex, const Triangle3< T > & triangle )
           {
               glm::tvec3< T > tmpPos;
               if ( triangle.intersectRay( ray.getPos(), ray.getDir(), tmpPos ) )
               {
                   doesIntersect = true;
                   auto localDistance = glm::length( ray.getPos() - tmpPos );
                   if ( globalHitDistance == distMax || globalHitDistance >= localDistance )
                   {
                       globalTriangle = triangle;
                       globalHitDistance = localDistance;
                       globalHitPosition = tmpPos;
                       globalTriangleIndex = triIndex;
                   }
               }
           }
           );

       if ( doesIntersect )
       {
           result.meshIndex = -1;     // indicates bbox
           result.bufferIndex = -1;   // indicates bbox
           result.triangle = globalTriangle;
           result.triangleIndex = globalTriangleIndex; // triangle of bbox 0..11
           result.distance = globalHitDistance;
           result.position = globalHitPosition;
       }
       return doesIntersect;
   }

};


// ===========================================================================
struct SMeshBufferTool
// ===========================================================================
{
    DE_CREATE_LOGGER("de.gpu.SMeshBufferTool")

    static void
    transformPoints( std::vector< glm::vec3 > & points, glm::dmat4 const & modelMat );
    static void
    rotatePoints( std::vector< glm::vec3 > & points, float a, float b, float c );
    static void
    rotateTexCoords90_onlyQuads( SMeshBuffer & mesh );
    static SMeshBuffer
    tesselate( SMeshBuffer const & src );
    static void
    fuse( SMeshBuffer & dst, SMeshBuffer const & src, glm::vec3 const & offset = glm::vec3() );
    static void
    fuse( SMeshBuffer & dst, std::vector< SMeshBuffer > const & meshes );
    static uint32_t
    countVertices( std::vector< SMeshBuffer > const & liste );
    static uint32_t
    countIndices( std::vector< SMeshBuffer > const & liste );
    static std::vector< Triangle3f >
    createCollisionTriangles( SMeshBuffer const & b );
    static void
    computeBoundingBox( SMeshBuffer & o );
    static Box3f
    computeBoundingBox( std::vector< S3DVertex > const & vertices );
    static void
    computeNormals( SMeshBuffer & o );
    static void
    computeNormals( std::vector< S3DVertex > & vertices, std::vector< uint32_t > const & indices );
    static void
    bumpVertices( SMeshBuffer & o, Image* bumpMap, float bumpScale = 1.0f );

    static void
    flipX( SMeshBuffer & o );
    static void
    flipY( SMeshBuffer & o );
    static void
    flipZ( SMeshBuffer & o );
    static void
    flipXY( SMeshBuffer & o );
    static void
    flipYZ( SMeshBuffer & o );
    static void
    flipXZ( SMeshBuffer & o );
    static void
    flipNormals( SMeshBuffer & o );
    static void
    flipVertexPosYZ( SMeshBuffer & o );
    static void
    flipWinding( SMeshBuffer & o );

    static void
    translateVertices( SMeshBuffer & o, glm::vec3 const & offset );
    static void
    translateVertices( SMeshBuffer & o, glm::vec3 const & offset, uint32_t vStart );
    static void
    translateVertices( SMeshBuffer & o, float x, float y, float z );
    static void
    transformVertices( SMeshBuffer & o, glm::dmat4 const & modelMat = glm::dmat4(1.0) );

    static void
    randomRadialDisplace( SMeshBuffer & o, float h );

    static void
    rotateVertices( SMeshBuffer & o, float a, float b, float c );
    static void
    scaleVertices( SMeshBuffer & o, float scale = 1.01f );
    static void
    scaleVertices( SMeshBuffer & o, float sx, float sy, float sz );

    static void
    translateNormals( SMeshBuffer & o, glm::vec3 const & offset );
    static void
    translateNormals( SMeshBuffer & o, float nx, float ny, float nz );
    static void
    setNormals( SMeshBuffer & o, glm::vec3 const & normal );
    static void
    setNormals( SMeshBuffer & o, float nx, float ny, float nz );
    static void
    setNormalZ( SMeshBuffer & o, float nz );

    static void
    scaleTexture( SMeshBuffer & o, float u, float v );
    static void
    transformTexCoords( SMeshBuffer & o, Recti const & r_atlas, int atlas_w, int atlas_h );
    static void
    colorVertices( SMeshBuffer & o, uint32_t color );
    static void
    centerVertices( SMeshBuffer & o );
    /*
   static SMeshBuffer
   createLines();

   static SMeshBuffer
   createTriangles();
*/
    static void
    addVertex( SMeshBuffer & o, S3DVertex const & vertex );
    static void
    addVertex( SMeshBuffer & o, glm::vec3 const & pos, glm::vec3 const & nrm, uint32_t color, glm::vec2 const & tex );
    static void
    addVertex( SMeshBuffer & o, glm::vec3 const & pos, uint32_t color );
    static void
    addLine( SMeshBuffer & o, S3DVertex const & a, S3DVertex const & b );
    static void
    addTriangle( SMeshBuffer & o, S3DVertex const & a, S3DVertex const & b, S3DVertex const & c );
    static void
    addLine( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, uint32_t color = 0xFFFFFFFF );
    static void
    addLine( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, uint32_t colorA, uint32_t colorB );
    static void
    addLine( SMeshBuffer & o, float x1, float y1, float x2, float y2, float z = 0.0f, uint32_t color = 0xFFFFFFFF );
    static void
    addLine( SMeshBuffer & o, float x1, float y1, float x2, float y2, float z, uint32_t colorA, uint32_t colorB );
    static void
    addLineTriangle( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, uint32_t color = 0xFFFFFFFF );
    static void
    addLineTriangle( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, uint32_t colorA, uint32_t colorB, uint32_t colorC );
    static void
    addLineQuad( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, glm::vec3 const & d, uint32_t color = 0xFFFFFFFF );
    static void
    addLineQuad( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, glm::vec3 const & d,
                uint32_t colorA, uint32_t colorB, uint32_t colorC = 0xFFFFFFFF, uint32_t colorD = 0xFFFFFFFF );
    static void
    addLineBox( SMeshBuffer & o, Box3f const & box, uint32_t color );

    // =======================================================================
    // DebugGeometry:
    // =======================================================================
    static void
    addLineNormals( SMeshBuffer & o, SMeshBuffer const & src, float n_len, uint32_t color );
    static void
    addWireframe( SMeshBuffer & o, SMeshBuffer const & src, float outScale, uint32_t color );
    static SMeshBuffer
    createWireframe( SMeshBuffer const & src, float scale = 1.0f, uint32_t color = 0 );
    static SMeshBuffer
    createScaled( SMeshBuffer const & src, float scale = 1.01f );
    static SMeshBuffer
    createTranslated( SMeshBuffer const & src, glm::vec3 pos );
    static SMeshBuffer
    createLineBox( Box3f const & box, uint32_t color );
    static SMeshBuffer
    createLineNormals( SMeshBuffer const & src, float normalLen, uint32_t color );
};

// ===========================================================================
struct SMesh
// ===========================================================================
{
   typedef double T;
   typedef std::shared_ptr< SMesh > SharedPtr;
   typedef std::unique_ptr< SMesh > UniquePtr;
   typedef std::weak_ptr< SMesh > WeakPtr;

   static SharedPtr
   createShared( std::string const & name = "" ) { return std::make_shared< SMesh >(); }

   static SharedPtr
   createShared( SMeshBuffer const & buf ) { return std::make_shared< SMesh >( buf ); }

   SMesh();
   //SMesh( char const* name );
   //SMesh( std::string const & name );
   SMesh( SMeshBuffer const & buf );

   std::vector<TexRef> getUniqueTextures() const;
   std::string       getPrimTypesStr() const;

   void              upload();
   void              clear();

   //glm::dmat4 const& getModelMatrix() const { return ModelMat; }
   //void              setModelMatrix( glm::dmat4 const & modelMat ) { ModelMat = modelMat; }

   //std::string       getName() const { return Name; }
   //void              setName( std::string const & name ) { Name = name; }

   bool              isVisible() const { return bVisible; }
   void              setVisible( bool visible ) { bVisible = visible; }

   Box3f const &     getBoundingBox() const { return boundingBox; }
   void              setBoundingBox( Box3f const & bbox ) { boundingBox = bbox; }

   int               getMaterialCount() const;
   SMaterial const & getMaterial( int i ) const;
   SMaterial &       getMaterial( int i );

   void              addEmptyBuffer();
   void              addMeshBuffer( SMeshBuffer const & buf );
   void              addMesh( SMesh const & other );

   std::vector< SMeshBuffer > const &  getMeshBuffers() const { return buffers; }
   std::vector< SMeshBuffer > &        getMeshBuffers() { return buffers; }

   int               getMeshBufferCount() const;
   const SMeshBuffer&getMeshBuffer( int i ) const;
   SMeshBuffer&      getMeshBuffer( int i );

   void              removeMeshBuffer( int i );
   //void            shiftMeshBuffer( int shifts );
   void              recalculateNormals();
   void              recalculateBoundingBox( bool fully = false );
   bool              isIndex( int i ) const;

   std::string       toString( bool printDetails = false ) const;
   size_t            getVertexCount() const;
   size_t            getIndexCount() const;
   size_t            computeByteConsumption() const;

   // Material:
   int               getLighting() const;
   int               getFogEnable() const;
   int               getWireframe() const;

   void setLighting( int illum );
   void setWireframe( bool enable );
   void setFogEnable( bool enable );
   void setCulling( Culling cull );
   void setCulling( bool enable );
   void setDepth( Depth depth );
   void setStencil( Stencil stencil );
   void setBlend( Blend blend );
   //void setTexture( int stage, ITexture* tex );
   void setTexture( int stage, TexRef ref );

   //std::vector< Triangle3f > createCollisionTriangles() const;
   void flipVertexPosYZ();
   void flipNormals();

   void transformVertices( glm::dmat4 const & m );
   void translateVertices( glm::vec3 const & offset );
   void translateVertices( float x, float y, float z );
   void rotateVertices( float a, float b, float c );
   void colorVertices( uint32_t color );
   void centerVertices();
   void scaleTexture( float u, float v );
   void scaleVertices( float scale = 1.01f );
   void scaleVertices( float sx, float sy, float sz );

//   bool
//   intersectRayWithBoundingBox( Ray3< T > const & ray, MeshIntersectionResult< T > & result,
//                                glm::tmat4x4<T> const & trs ) const;

   bool
   intersectRayWithBoundingBox( Ray3< T > const & ray,
                                MeshIntersectionResult< T > & result ) const;

   bool
   intersectRay( Ray3< T > const & ray,
                 MeshIntersectionResult< T > & result ) const;

//   bool
//   intersectRay( Ray3< T > const & ray,
//                 MeshIntersectionResult< T > & result,
//                 glm::tmat4x4<T> const & modelMat = glm::tmat4x4<T>( T(1) ) ) const;
   /*
   template < typename T >
   bool intersectRay(
         Ray3< T > const & ray,
         glm::tvec3< T >* hitPosition = nullptr,
         uint32_t* bufferIndex = nullptr,
         uint32_t* triangleIndex = nullptr ) const
   {
      auto hitDist = std::numeric_limits< T >::max();
      auto hitPos = glm::tvec3< T >( T(0), T(0), T(0) );

      uint32_t bufIndex = 0;
      uint32_t triIndex = 0;
      bool doesIntersect = false;

      for ( int32_t i = 0; i < getMeshBufferCount(); ++i )
      {
         auto const & p = getMeshBuffer( i );
         if ( !p.isVisible() ) continue; // nothing todo

         glm::tvec3< T > tmpPos;
         uint32_t tmpIndex = 0;
         if ( p.intersectRay( ray, &tmpPos, &tmpIndex ) )
         {
            doesIntersect = true;
            auto dist = glm::length( hitPos - ray.getPos() );
            if ( hitDist > dist )
            {
               hitDist = dist;
               hitPos = hitPos;
               bufIndex = uint32_t( i );
               triIndex = tmpIndex;
            }
         }
      }

      if ( doesIntersect )
      {
         if ( hitPosition ) { *hitPosition = hitPos; }
         if ( bufferIndex ) { *bufferIndex = bufIndex; }
         if ( triangleIndex ) { *triangleIndex = triIndex; }
      }

      return doesIntersect;
   }
*/
public:
    std::vector< SMeshBuffer > buffers;   //! Geometry buffers for this mesh
    //glm::dmat4 ModelMat;
    Box3f boundingBox;//! Bounding box of this meshbuffer.
    std::string name;
    bool bVisible;
};


// ===========================================================================
struct SMeshTool
// ===========================================================================
{
    DE_CREATE_LOGGER("de.gpu.SMeshTool")
    static void
    transformVertices( SMesh & o, glm::dmat4 const & modelMat = glm::dmat4(1.0) );

    static void
    flipNormals( SMeshBuffer & o );
    static void
    flipWinding( SMeshBuffer & o );

    static void
    flipX(SMesh & o);
    static void
    flipY(SMesh & o);
    static void
    flipZ(SMesh & o);
    static void
    flipXY(SMesh & o);
    static void
    flipYZ(SMesh & o);
    static void
    flipXZ(SMesh & o);
    static void
    flipNormals( SMesh & o );
    static void
    flipWinding( SMesh & o );
    static void
    colorVertices( SMesh & o, uint32_t color );
    static void
    translateVertices( SMesh & o, glm::vec3 const & offset );
    static void
    translateVertices( SMesh & o, float x, float y, float z );
    static void
    rotateVertices( SMesh & o, float a, float b, float c );
    static void
    rotateVertices( SMesh & o, glm::vec3 const & degrees );
    static void
    scaleVertices( SMesh & o, glm::vec3 const & scale );
    static void
    scaleVertices( SMesh & o, float x, float y, float z );
    static void
    centerVertices( SMesh & o );
    static void
    recalculateNormals( SMesh & o );
    static void
    fitVertices( SMesh & o, float maxSize = 1000.0f );
    static void
    removeDoubleVertices( SMesh & mesh );
    static void
    removeDoubleMaterials( SMesh & mesh );
    static void
    removeEmptyMeshBuffer( SMesh & mesh );
};


} // end namespace gpu.
} // end namespace de.
