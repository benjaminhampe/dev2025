#pragma once
#include "IMeshBuffer.hpp"

namespace de {
namespace gpu {

// ===========================================================================
// TMeshBuffer
// ===========================================================================
template < typename TVertex >
struct TMeshBuffer : public IMeshBuffer
// =======================================================================
{
   PrimitiveType PrimType;   /// @brief GLenum PrimitiveType;
   bool IsDirty;   /// @brief Controls VAO upload CPU side changed, need (re)upload to GPU
   bool IsVisible;   /// @brief Used by modeler, viewer and renderer
   FVF m_FVF;   /// @brief Flexible vertex format, init TVertex::getFVF(), then can be read and changed at runtime.
   uint32_t m_VBO;   /// @brief Vertex buffer object handle, if driver supports it.
   uint32_t m_IBO;   /// @brief Index buffer object handle, if driver supports it.
   uint32_t m_VAO;   /// @brief Vertex array object handle, if driver supports it.
   Box3f BoundingBox;   /// @brief 3d bounding box.
   glm::dmat4 ModelMat;   /// @brief Absolute transformation of the geometry.
   Material m_material;   /// @brief Standard material.
   typedef std::vector< TVertex > TVertexVector;   /// @brief Vertices
   TVertexVector Vertices;   /// @brief Vertices
   typedef std::vector< uint32_t > TIndexVector;   /// @brief Indices
   TIndexVector Indices;   /// @brief Indices
   std::string Name;   /// @brief Able to find specific MeshBuffer in Mesh by (category/material) name. ( e.g. "Lines", "Triangles" )

   /// @brief Constructor
   explicit TMeshBuffer( PrimitiveType primType = PrimitiveType::Triangles )
      : IMeshBuffer()
      , PrimType( primType )
      , IsDirty( true )
      , IsVisible( true )
      , m_FVF( TVertex::getFVF() )
      , m_VBO( 0 )
      , m_IBO( 0 )
      , m_VAO( 0 )
      , BoundingBox()
      , ModelMat( 1.0 )
      , Name("")
   {
      // DE_DEBUG(getName())
   }

//   ~TMeshBuffer() override
//   {
//      //DE_DEBUG(getName())
//      destroy();
//   }

   /// @brief Generic GPU link, Is zero on OpenGLES 2.0 ( not used )
   /// even with ES3.1 the VBO/VAO does not work on Win10 + QtSDK (GLESv2.dll + libEGL [ANGLE] )
   /// what always works is simple glEnableVertexAttribArray() + glVertexAttribPointer()
   /// No glGenBuffers/glGenVertexArrayObject(),
   /// just always use cpu memory pointer while rendering ( immediate mode )
   /// see GLES::drawPrimitiveList().
   uint32_t & getVBO() override { return m_VBO; }
   uint32_t & getIBO() override { return m_IBO; }
   uint32_t & getVAO() override { return m_VAO; }



   // [IsVisible]
   bool isVisible() const override { return IsVisible; }
   void setVisible( bool visible ) override { IsVisible = visible; }

   std::vector< glm::vec3 >
   getPoints() const
   {
      std::vector< glm::vec3 > pts;
      pts.reserve( getVertexCount() );
      for ( TVertex const & v : Vertices )
      {
         pts.emplace_back( v.pos );
      }
      return pts;
   }

   // [MemorySizeInBytes]
   size_t
   getByteCount() const override
   {
      size_t byteCount = sizeof( *this );
      byteCount += Vertices.capacity() * sizeof( TVertex );
      byteCount += Indices.capacity() * sizeof( uint32_t );
      return byteCount;
   }

   // [ModelMatrix]
   glm::dmat4 const & getModelMatrix() const override { return ModelMat; }
   void setModelMatrix( glm::dmat4 const & modelMat ) override { ModelMat = modelMat; }

   // [Name]
   std::string const & getName() const override { return Name; }
   void setName( std::string const & name ) override { Name = name; }

   void
   clear() override
   {
      clearVertices();
      clearIndices();
      BoundingBox.reset();
   }

   void
   clearVertices() override { Vertices.clear(); }

   void
   clearIndices() override { Indices.clear(); }

   void
   moreVertices( size_t n ) override { Vertices.reserve( Vertices.size() + n ); }

   void
   moreIndices( size_t n ) override { Indices.reserve( Vertices.size() + n ); }

   /// @brief String
   std::string
   toString( bool withFVF = false ) const
   {
      std::stringstream s;
      s << "name(" << Name << "), ";
      s << "p(" << getPrimitiveTypeString( PrimType ) << "), ";
      s << "v(" << getVertexCount() << "), ";
      s << "i(" << getIndexCount() << "), ";
      auto dx = int32_t( getBoundingBox().getSize().x );
      auto dy = int32_t( getBoundingBox().getSize().y );
      auto dz = int32_t( getBoundingBox().getSize().z );
      s << "bbox(" << glm::ivec3(dx,dy,dz) << "), ";
      if ( withFVF )
         s << "fvf(" << getFVF().toString() << "), ";
      else
         s << "fvf(" << getFVF().getByteSize() << "), ";

      s << "mat(" << getMaterial().toString() << ")";

//      for ( size_t i = 0; i < getMaterial().getTextureCount(); ++i )
//      {
//         if ( !getMaterial().getTexture( i ).empty() )
//         {
//            s << ", tex" << i << "(" << getMaterial().getTexture( i ).toString() << ")";
//         }
//      }

      return s.str();
   }


   /// @brief Material
   Material const & getMaterial() const { return m_material; }
   Material &       getMaterial() { return m_material; }
   void              setMaterial( Material const & material ) { m_material = material; }

   /// @brief BoundingBox
   Box3f const &           getBoundingBox() const { return BoundingBox; }
   Box3f &                 getBoundingBox() { return BoundingBox; }
   void                    setBoundingBox( Box3f const & bbox ) { BoundingBox = bbox; }

   PrimitiveType           getPrimitiveType() const { return PrimType; }
   void                    setPrimitiveType( PrimitiveType type ) { PrimType = type; }

   /// @brief Flexible VertexFormat Type
   /// Specific attribs can be enabled/disabled for shaders that dont use these attribs.
   FVF const &             getFVF() const { return m_FVF; }
   FVF &                   getFVF() { return m_FVF; }
   void                    setFVF( FVF const & fvf ) { m_FVF = fvf; }

   /// @brief Vertices
   size_t                  getVertexCount() const  { return Vertices.size(); }
   size_t                  getVertexSize() const   { return sizeof( TVertex ); }
   size_t                  getVertexCapacity() const{return Vertices.capacity(); }
   void const *            getVertices() const     { return reinterpret_cast< void const* >( Vertices.data() ); }
   void *                  getVertices()           { return reinterpret_cast< void* >( Vertices.data() ); }
   TVertexVector const &   getVertexVector() const { return Vertices; }
   TVertexVector &         getVertexVector()       { return Vertices; }
   TVertex const &         getVertex( size_t i ) const { return Vertices[ i ]; }
   TVertex &               getVertex( size_t i )       { return Vertices[ i ]; }

   /// @brief Indices
   IndexType               getIndexType() const    { return IndexType::U32; }
   uint32_t                getIndexSize() const    { return 4; }
   uint32_t                getIndexCount() const   { return Indices.size(); }
   uint32_t                getIndexCapacity() const{ return Indices.capacity(); }
   void const *            getIndices() const      { return reinterpret_cast< void const* >( Indices.data() ); }
   void*                   getIndices()            { return reinterpret_cast< void* >( Indices.data() ); }

   uint32_t                getIndex( uint32_t i ) const  { return Indices[ i ]; }
   uint32_t &              getIndex( uint32_t i )        { return Indices[ i ]; }
   TIndexVector const &    getIndexVector() const  { return Indices; }
   TIndexVector &          getIndexVector()        { return Indices; }

   void                    recalculateBoundingBox() override {}


//   void
//   recalculateBoundingBox() override
//   {
//      BoundingBox.reset( 0.f, 0.f, 0.f );
//      size_t const v = getVertexCount();
//      if ( v == 0 ) { return; }

//      BoundingBox.reset( getPos( 0 ) );

//      // this loop only fires if v > 1.
//      for ( size_t i = 1; i < v; ++i )
//      {
//         BoundingBox.addInternalPoint( getPos( i ) );
//      }
//   }

//   virtual glm::vec3       getPos( uint32_t i ) const { return glm::vec3(Vertices[ i ].pos); }
//   virtual void            setPos( uint32_t i, glm::vec3 const & v ) { Vertices[ i ].pos = v; }
//   glm::vec3 const &       getPos( uint32_t i ) const { return Vertices[ i ].getPos(); }
//   glm::vec3 &             getPos( uint32_t i )       { return Vertices[ i ].getPos(); }
//   void                    setPos( uint32_t i, glm::vec3 const & v ) { return Vertices[ i ].setPos( v ); }

//   glm::vec3               getNormal( size_t i ) const { return {0,0,-1}; }
//   glm::vec3               getTangent( size_t i ) const { return {1,0,0}; }
//   glm::vec3               getBitangent( size_t i ) const { return {0,1,0}; }
//   uint32_t                getColor( size_t i ) const { return 0xFFFFFFFF; }
//   glm::vec2               getTexCoords( size_t i, int stage = 0 ) const { return {0,0}; }


   void
   addVertex( TVertex const & vertex )
   {
//      if ( Vertices.empty() )
//      {
//         BoundingBox.reset( glm::vec3(vertex.pos) );
//      }
//      else
//      {
//         BoundingBox.addInternalPoint( glm::vec3(vertex.pos) );
//      }

      Vertices.push_back( vertex );
   }

   void
   addLine( TVertex const & a, TVertex const & b )
   {
      auto v = getVertexCount();
      addVertex( a );
      addVertex( b );
      addIndex( v );
      addIndex( v+1 );
   }

//   void
//   addTriangle( TVertex const & a, TVertex const & b, TVertex const & c )
//   {
//      addVertex( a );
//      addVertex( b );
//      addVertex( c );
//      addIndexedTriangle();
//   }

   void
   addLineTriangle( TVertex const & a, TVertex const & b, TVertex const & c )
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
   addLineQuad( TVertex const & a, TVertex const & b, TVertex const & c, TVertex const & d )
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
      if ( Vertices.empty() )
      {
         //DE_ERROR("No Vertices")
         return;
      }
      Indices.push_back( Vertices.size() - 1 );
   }

   void
   addIndex( uint32_t index )
   {
      //assert( size_t( index ) <= size_t( std::numeric_limits< TIndex >::max() ) );
      Indices.push_back( index );
   }

   void
   addTriangle( TVertex const & a, TVertex const & b, TVertex const & c, bool flip = false )
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
   addQuad( TVertex const & a, TVertex const & b, TVertex const & c, TVertex const & d, bool flip = false )
   {
      addVertex( a );
      addVertex( b );
      addVertex( c );
      addVertex( d );
      addIndexedQuad( flip );
   }

   void
   addVertexQuad( TVertex const & a, TVertex const & b, TVertex const & c, TVertex const & d, bool flip = false )
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



};

} // end namespace gpu.
} // end namespace de.


/*
 *

//   void
//   addVertexTriangle( TVertex const & a, TVertex const & b, TVertex const & c )
//   {
//      addVertex( a );
//      addVertex( b );
//      addVertex( c );
//   }

//   void
//   addVertexTriangle( TVertex const & a, TVertex const & b, TVertex const & c, bool flip )
//   {
//      if ( flip )
//      {
//         addVertex( a );
//         addVertex( c );
//         addVertex( b );
//      }
//      else
//      {
//         addVertex( a );
//         addVertex( b );
//         addVertex( c );
//      }
//   }

//   void
//   addVertexQuad( TVertex const & a, TVertex const & b, TVertex const & c, TVertex const & d )
//   {
//      addVertex( a );
//      addVertex( b );
//      addVertex( c );
//      addVertex( d );
//   }


   /// @brief Render
   // void render() override {}
//   bool upload( int mode = 1 ) override
//   {
//      return BufferTools::upload(
//         #ifdef USE_GPU_BUFFER_CALLER
//         getName(),
//         #endif
//         getVBO(), getIBO(), getVAO(),
//         getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
//         getIndices(), getIndexCount(), getIndexType() );
//   }

//   void destroy() override
//   {
//      BufferTools::destroy(
//         #ifdef USE_GPU_BUFFER_CALLER
//         getName(),
//         #endif
//         getVBO(), getIBO(), getVAO() );
//   }

//   void draw( int mode = 0 ) override
//   {
//      if ( mode == 0 )
//      {
//         BufferTools::drawImmediate(
//            #ifdef USE_GPU_BUFFER_CALLER
//            getName(),
//            #endif
//            getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
//            getIndices(), getIndexCount(), getIndexType() );
//      }
//      else
//      {
//         BufferTools::draw(
//   #ifdef USE_GPU_BUFFER_CALLER
//            getName(),
//   #endif
//            getVBO(),
//            getIBO(),
//            getVAO(),
//            getPrimitiveType(),
//            getVertices(),
//            getVertexCount(),
//            getFVF(),
//            getIndices(),
//            getIndexCount(),
//            getIndexType() );
//      }
//   }



   static std::vector< Triangle3f >
   createCollisionTriangles( SMeshBuffer const & b )
   {
      if ( b.getPrimitiveType() != PrimitiveType::Triangles ) return {};

      std::vector< Triangle3f > tris;

      if ( b.getIndexCount() > 0 )
      {
         tris.reserve( tris.size() + b.getIndexCount()/3 );
         for ( size_t i = 0; i < b.getIndexCount()/3; ++i )
         {
            glm::vec3 const & A = b.Vertices[ b.Indices[ 3*i ] ].pos;
            glm::vec3 const & B = b.Vertices[ b.Indices[ 3*i+1 ] ].pos;
            glm::vec3 const & C = b.Vertices[ b.Indices[ 3*i+2 ] ].pos;
            tris.push_back( { A, B, C } );
         }
      }
      else
      {
         tris.reserve( tris.size() + b.getVertexCount()/3 );
         for ( size_t i = 0; i < b.getVertexCount()/3; ++i )
         {
            glm::vec3 const & A = b.Vertices[ 3*i ].pos;
            glm::vec3 const & B = b.Vertices[ 3*i+1 ].pos;
            glm::vec3 const & C = b.Vertices[ 3*i+2 ].pos;
            tris.push_back( { A, B, C } );
         }
      }

      return tris;
   }

   bool
   intersectRay( glm::vec3 const & rayStart, glm::vec3 const & rayDir,
         glm::vec3 & hitPosition,
         uint32_t* faceIndex = nullptr,
         uint32_t* edgeIndex = nullptr,
         uint32_t* vertIndex = nullptr ) const
   {
      //if ( !meshBuffer ) return nullptr;

      // if ( m_SceneNodes.size() < 2 ) return m_SceneNodes[ 0 ];
      bool doesIntersect = false;
      Triangle3f hitTriangle;
      size_t iTriangle = 0;
      size_t iNode = 0;
      float const distMax = std::numeric_limits< float >::max();
      float dist = distMax;

   //   Plane3f z_plane(0,0,1,0);
   //   glm::vec3 zPosition;
   //   if ( z_plane.intersectRay( rayStart, rayDir, zPosition ) )
   //   {
   //      DE_MAIN_DEBUG("Got intersection with z-plane(",zPosition,"), rayStart(",rayStart,"), rayDir(",rayDir,")")
   //   }
   //   else
   //   {
   //      zPosition = rayStart;
   //   }

      std::vector< Triangle3f > const & triangles = getCollisionTriangles();
      if ( triangles.size() > 0 )
      {
         for ( size_t t = 0; t < triangles.size(); ++t )
         {
            Triangle3f const & tri = triangles[ t ];

            //DE_DEBUG("MeshNode[",i,"].Triangle[", t, "] = ", tri.toString() )
            glm::vec3 tmpPosition;
            if ( tri.intersectRay( rayStart, rayDir, tmpPosition ) )
            {
               float d = glm::length( rayStart - tmpPosition );

               //DE_DEBUG("Node[",i,"](", node->getName(),").Triangle[",t,"].Hit(", tmpPosition,"), "
               //   "d(", d, "), dist(", dist, "), tri(", tri.toString(), ")")

               if ( dist == distMax || dist >= d )
               {
                  dist = d;
                  pick = snode;
                  hitPosition = tmpPosition;
                  hitTriangle = tri;
                  iTriangle = t;
                  iNode = i;
               }
            }
         }
      }


      if ( pick )
      {
         if ( outPosition )
         {
            *outPosition = hitPosition;
         }

         //DE_DEBUG("Picked Node[", iNode ,"].Triangle[", iTriangle, "] "
           //       "hitPos(", hitPosition, "), name(", pick->getName(), "), "
             //     "hitTriangle(", hitTriangle.toString(), ")" )
      }

      return pick;

   }


   void
   translateVertices( glm::vec3 const & offsetPos )
   {
      for ( size_t i = 0; i < getVertexCount(); ++i )
      {
         getVertexPos( i ) += offsetPos;
      }
   }

   /// @brief Normalize all triangle normals.
   void
   normalizeNormals()
   {
      if ( PrimType != PrimitiveType::Triangles )
      {
         return;
      }
      if ( Indices.size() > 0 )
      {
         for ( size_t i = 0; i < Indices.size() / 3; ++i )
         {
            auto a = getVertexPos( Indices[ 3*i + 0 ] );
            auto b = getVertexPos( Indices[ 3*i + 1 ] );
            auto c = getVertexPos( Indices[ 3*i + 2 ] );
            auto n = glm::normalize( glm::cross( c-b, a-b ) );
            Vertices[ Indices[ 3*i + 0 ] ].m_Normal = n;
            Vertices[ Indices[ 3*i + 1 ] ].m_Normal = n;
            Vertices[ Indices[ 3*i + 2 ] ].m_Normal = n;
         }
      }
      else
      {
         for ( size_t i = 0; i < Vertices.size() / 3; ++i )
         {
            auto a = getVertexPos( 3*i + 0 );
            auto b = getVertexPos( 3*i + 1 );
            auto c = getVertexPos( 3*i + 2 );
            auto n = glm::normalize( glm::cross( c-b, a-b ) );
            Vertices[ 3*i + 0 ].m_Normal = n;
            Vertices[ 3*i + 1 ].m_Normal = n;
            Vertices[ 3*i + 2 ].m_Normal = n;
         }
      }
   }
*/
