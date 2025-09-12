#pragma once
#include <DarkImage.h>
#include <de/gpu/smesh/SMesh.h>
#include <de/gpu/VideoDriver.h>
#include <de/audio/dsp/ShiftBuffer.hpp>
#include <functional>

//using namespace de::gpu;

// =======================================================================
struct ShiftMesh_LineStrips
// =======================================================================
{
   DE_CREATE_LOGGER("ShiftMesh_LineStrips")
   std::vector< de::gpu::SMeshBuffer* > m_data; // Owner ( deletes rows in dtr )
   std::vector< de::gpu::SMeshBuffer* > m_copy; // Not owner ( does not delete anything )

   // Collects mono channel data until m_shiftBuffer.size()
   // and uses the data to build a 3d geometry out of it.
   // The input can be small, e.g. 256 samples per channel for realtime audio.
   // So with 1024 shift buffer size we collect 4 audio callbacks
   // then we trigger a new mesh from collection and reset m_frameIndex.
   de::audio::ShiftBuffer m_shiftBuffer;

   bool m_isUploaded;
   uint32_t m_frameIndex;
   uint32_t m_pushCount;
   float m_sizeX;
   float m_sizeY;
   float m_sizeZ;

   de::LinearColorGradient m_ColorGradient;

   ShiftMesh_LineStrips()
      : m_isUploaded( false )
      , m_frameIndex( 0 )
      , m_pushCount( 0 )
      , m_sizeX( 5000.0f)
      , m_sizeY( 500.0f)
      , m_sizeZ( 20000.0f)
   {
      m_data.resize( 64 );
      m_copy.resize( 64 );
      for ( size_t i = 0; i < m_data.size(); ++i )
      {
         auto row = new de::gpu::SMeshBuffer();
         m_data[ i ] = row;
         m_copy[ i ] = row;
      }

      m_ColorGradient.addStop( 0.0f, 0xFF000000 );
      m_ColorGradient.addStop( 0.01f, 0xFFFFFFFF );
      m_ColorGradient.addStop( 0.1f, 0xFF00FFFF );
      m_ColorGradient.addStop( 0.2f, 0xFF00FF00 );
      m_ColorGradient.addStop( 0.4f, 0xFF002000 );
      m_ColorGradient.addStop( 0.6f, 0xFF00FFFF );
      m_ColorGradient.addStop( 1.0f, 0xFF0000FF );
      m_ColorGradient.addStop( 2.0f, 0xFFFF00FF );

      setShiftBufferSize( 256 );
   }

   ~ShiftMesh_LineStrips()
   {
      for ( size_t i = 0; i < m_data.size(); ++i )
      {
          auto p = m_data[ i ];
          if (p)
          {
              delete p;
          }
      }
      m_data.clear();
      m_copy.clear();
   }

   void render( de::gpu::VideoDriver* driver, de::TRSd::M4 const & mvp = de::TRSd::M4(1.0) )
   {
      // Draw Lines
      int n = m_data.size();
      float fx = 4.0f / float( n );
      float dx = m_sizeX;
      float dy = m_sizeY;
      float dz = m_sizeZ / float( n );
      float tz = 0.0f;
      float sx = 5.0f;
      for ( int i = n-1; i >= 0; i-- )
      {
         auto lineStrip = m_data[ i ];
         if ( lineStrip )
         {
            de::TRSd trs;
            tz = (dz * i);
            trs.pos.x = 0.0;
            trs.pos.z = tz;
            trs.scale.x = -sx;
            trs.update();
            sx -= fx;
            //sz += 0.075f;
            driver->setModelMatrix( mvp * trs.modelMat );
            driver->getSMaterialRenderer()->draw3D( *lineStrip );
         }
      }
   }

   void setShiftBufferSize( uint32_t dstFrames )
   {
      if ( m_shiftBuffer.size() != dstFrames )
      {
         m_shiftBuffer.resize( dstFrames );
         m_frameIndex = 0;
      }
   }

   // Impl for LineStrip: Overwrites first row with new data
   virtual void createLineStrip()
   {
      de::gpu::SMeshBuffer & o = *m_data[ 0 ];

      size_t n = m_shiftBuffer.size();

      o.setLighting( false );
      o.setPrimitiveType( de::gpu::PrimitiveType::LineStrip );
      o.vertices.clear();
      o.vertices.reserve( n );
      float dx = m_sizeX / float( n );
      float dy = m_sizeY * 0.5f;
      //float dz = m_sizeZ / float ( m_data.size() );

      for ( size_t i = 0; i < n; ++i )
      {
         float s = m_shiftBuffer[ i ];
         float t = std::abs( s ); // de::Math::clampf( s, -1.f, 1.f);
         float x = dx * i;
         float y = dy * s;
         float z = 0.0f;
         uint32_t color = m_ColorGradient.getColor32( t ); // de::RainbowColor::computeColor32( t );
         o.addVertex( de::gpu::S3DVertex(x,y,z,0,0,0,color,0,0) );
      }


      o.setDirty();
      o.upload();
   }

   // Only mono channel data is allowed.
   void push( float const* src, uint32_t srcFrames )
   {
      m_shiftBuffer.push( "ShiftMesh_LineStrips", src, srcFrames );
      m_pushCount++;
      m_frameIndex += srcFrames;

      if ( m_frameIndex >= m_shiftBuffer.size() )
      {
         auto n = m_data.size();
         for ( size_t i = 0; i < n-1; ++i )
         {
            m_data[ i+1 ] = m_copy[ i ];
         }
         m_data[ 0 ] = m_copy[ n-1 ];

         createLineStrip();

         m_frameIndex = 0;

         //DE_DEBUG("Fill after ",m_pushCount," pushes, m_shiftBuffer.size(", m_shiftBuffer.size(), "), srcCount(", srcCount,")")
//         m_shiftBuffer.zeroMemory();
         m_pushCount = 0;

         m_copy.swap( m_data );

//      }
      }
   }
};


// Manages a rows that are automaticly shifted when new data arrives.
//
// 3D meshes are created out of this data to render with OpenGLES.
//
// Can hold AudioWaveform ( 1 float per sample )
//       or AudioWaveformAmp in dB ( 1 float ), 1 dB = 20 * log10( amp*amp );
//       or AudioSpektrumAmplitude in dB ( 1 float )
//       or AudioSpektrumPhase in radians? or cents? ( 1 float )
//
// does only swap pointers and does not move memory!
// push() manages matrix shift and always refills the 0-th row
// if m_shiftBuffer collected enough samples to fill a new row.
// A ShiftBuffer is used to decouple different audio callback size and matrix column size
// But only meaningful if m_dacFrames <= m_shiftBuffer.size()
//
// To have a more quadratic matrix it uses m_dacFrames = m_shiftBuffer.size()
// so the shiftbuffer probably collects only once untils its already full.
// With colCount == m_dacFrames each row represents exactly one audio callback.
//
// Since push() is called constantly we delay expensive postfx like log10f() until render().
// render() collects data from the matrix using m_data, not m_orig or m_copy.
template < typename T >
// =======================================================================
struct ShiftMatrix
// =======================================================================
{
   DE_CREATE_LOGGER("ShiftMatrix")
   typedef std::vector< T > TRow;
   std::vector< TRow* > m_orig; // Owner ( deletes all )
   std::vector< TRow* > m_data; // Not owner ( can forget something at bug )
   std::vector< TRow* > m_copy; // Not owner ( does not delete anything )
   // Collects mono channel data until m_shiftBuffer.size()
   // and uses the data to build a 3d geometry out of it.
   // The input can be small, e.g. 256 samples per channel for realtime audio.
   // So with 1024 shift buffer size we collect 4 audio callbacks
   // then we trigger a new mesh from collection and reset m_frameIndex.
   de::audio::ShiftBuffer m_shiftBuffer;
   // Count matrix cols = m_shiftBuffer.size()
   uint32_t m_colCount;
   // Count matrix rows
   uint32_t m_rowCount;
   // Count m_shiftBuffer fill status to control shift tigger.
   uint32_t m_frameIndex;
   // Debug collection of new samples
   uint32_t m_pushCount;
   // The 3d matrix size in 3d world space.
   typedef glm::tvec3< T > V3;
   //V3 m_worldSize;

   ShiftMatrix()
      : m_colCount( 1024 )
      , m_rowCount( 64 )
      , m_frameIndex( 0 )
      , m_pushCount( 0 )
      //, m_worldSize( 1500.0f, 300.0f, 9000.0f )
   {
      m_orig.resize( m_rowCount );
      m_data.resize( m_rowCount );
      m_copy.resize( m_rowCount );
      for ( size_t i = 0; i < m_orig.size(); ++i )
      {
         auto row = new TRow( m_colCount, T(0) );
         m_orig[ i ] = row;
         m_data[ i ] = row;
         m_copy[ i ] = row;
      }
      m_shiftBuffer.resize( m_colCount );
   }

   ~ShiftMatrix()
   {
      destroy();
//      m_orig.clear();
//      m_data.clear();
//      m_copy.clear();
   }

   void destroy()
   {
      for ( size_t i = 0; i < m_orig.size(); ++i )
      {
         //SAFE_DELETE( m_orig[ i ] );
         auto p = m_orig[ i ];
         if (p)
         {
             delete p;
         }
      }
      m_orig.clear();
   }

   void resize( uint32_t colCount, uint32_t rowCount )
   {
      if ( rowCount != m_rowCount )
      {
         destroy();
         m_colCount = colCount;
         m_rowCount = rowCount;
         m_orig.resize( m_rowCount );
         m_data.resize( m_rowCount );
         m_copy.resize( m_rowCount );
         for ( size_t i = 0; i < m_rowCount; ++i )
         {
            auto row = new TRow( m_colCount, T(0) );
            m_orig[ i ] = row;
            m_data[ i ] = row;
            m_copy[ i ] = row;
         }

         m_shiftBuffer.resize( m_colCount );
         m_frameIndex = 0;
      }
      else if ( m_colCount != colCount )
      {
         m_colCount = colCount;
         for ( size_t i = 0; i < m_orig.size(); ++i )
         {
            auto row = m_orig[ i ];
            if ( row->size() != m_colCount )
            {
               row->resize( m_colCount );
            }
         }

         m_shiftBuffer.resize( m_colCount );
         m_frameIndex = 0;
      }
   }

   uint32_t rowCount() const { return m_rowCount; }
   uint32_t columnCount() const { return m_colCount; }

//   void setColumnCount( uint32_t colCount )
//   {
//      if ( m_colCount != colCount )
//      {
//         DE_DEBUG("cols = ",colCount)
//         m_colCount = colCount;

//         for ( size_t i = 0; i < m_orig.size(); ++i )
//         {
//            auto row = m_orig[ i ];
//            if ( row->size() != m_colCount )
//            {
//               row->resize( m_colCount );
//            }
//         }

//         m_shiftBuffer.resize( m_colCount );
//         m_frameIndex = 0;
//      }
//   }

//   void setRowCount( uint32_t rowCount )
//   {
//   }


   // Only mono channel data is allowed.
   void push( T const* src, uint32_t srcFrames,
      std::function< void() > const & onNewRow = [] () {} )
   {
      m_shiftBuffer.push( "ShiftMatrix", src, srcFrames );
      m_pushCount++;
      m_frameIndex += srcFrames;

      if ( m_frameIndex >= m_colCount )
      {
         m_data[ 0 ] = m_copy[ m_rowCount-1 ];
         for ( size_t i = 1; i < m_rowCount; ++i )
         {
            m_data[ i ] = m_copy[ i-1 ];
         }

         // Get new front row to overwrite

         TRow & row = *m_data[ 0 ];
//         row.clear();
//         row.reserve( m_colCount );

         // Overwrite new front row
         memcpy( row.data(), m_shiftBuffer.data(), m_colCount * sizeof( float ));
//         for ( size_t i = 0; i < m_colCount; ++i )
//         {
//            row[ i ] = m_shiftBuffer[ i ];
//         }

         onNewRow();

         m_frameIndex = 0;

         //DE_DEBUG("Fill after ",m_pushCount," pushes, m_shiftBuffer.size(", m_shiftBuffer.size(), "), srcCount(", srcCount,")")
//         m_shiftBuffer.zeroMemory();
         m_pushCount = 0;

         m_copy.swap( m_data );

//      }
      }
   }
};

inline void
createMatrixFrontTriangleMesh(
   int ch,
   de::gpu::SMeshBuffer & o,
   float const* row, uint32_t colCount,
   float m_sizeX, float m_sizeY, float m_sizeZ,
   de::LinearColorGradient & gradient, int mode = 1 )
{
   auto vCount = 2*colCount;
   auto iCount = 4*(colCount-1);

   float dx = m_sizeX / float( colCount - 1 );
   float dy = m_sizeY * 0.5f;

   o.setPrimitiveType( de::gpu::PrimitiveType::Triangles );
   o.setLighting( 0 );
   o.setCulling( true );

   o.vertices.clear();
   o.vertices.reserve( vCount );

   if ( mode == 0 ) // +,- raw waveform
   {
      if ( ch == 0 ) // translate left channel in x-dir
      {
         float const* pSrc = row + colCount - 1;
         for ( size_t i = 0; i < colCount; ++i )
         {
            float s = *pSrc;
            float x = -m_sizeX + dx * float( i );
            float y = dy * s;
            uint32_t color = gradient.getColor32( std::abs( s ) ); // de::RainbowColor::computeColor32( t );
            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,1,0,color,0,0) );
            o.addVertex( de::gpu::S3DVertex(x,y,0,0,1,0,color,0,0) );
            pSrc--;
         }
      }
      else if ( ch == 1 ) // translate left channel in x-dir
      {
         float const* pSrc = row;
         for ( size_t i = 0; i < colCount; ++i )
         {
            float s = *pSrc;
            float x = dx * float( i );
            float y = dy * s;
            uint32_t color = gradient.getColor32( std::abs( s ) ); // de::RainbowColor::computeColor32( t );
            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,1,0,color,0,0) );
            o.addVertex( de::gpu::S3DVertex(x,y,0,0,1,0,color,0,0) );
            pSrc++;
         }
      }
   }
   else if ( mode == 1 ) // dB
   {
      if ( ch == 0 ) // translate left channel in x-dir
      {
         float const* pSrc = row + colCount - 1;
         for ( size_t i = 0; i < colCount; ++i )
         {
            float s = std::abs( *pSrc );
            float db = -320.0f;
            if ( s >= 1e-16f ) // log(0) is -inf
            {
               db = 20.0f * log10f( s );
            }
            float x = -m_sizeX + dx * float( i );
            float y = db;
            uint32_t color = gradient.getColor32( s ); // de::RainbowColor::computeColor32( t );
            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,1,0,color,0,0) );
            o.addVertex( de::gpu::S3DVertex(x,y,0,0,1,0,color,0,0) );
            pSrc--;
         }
      }
      else if ( ch == 1 ) // translate left channel in x-dir
      {
         float const* pSrc = row;
         for ( size_t i = 0; i < colCount; ++i )
         {
            float s = std::abs( *pSrc );
            float db = -320.0f;
            if ( s >= 1e-16f ) // log(0) is -inf
            {
               db = 20.0f * log10f( s );
            }
            float x = dx * float( i );
            float y = db;
            uint32_t color = gradient.getColor32( std::abs( s ) ); // de::RainbowColor::computeColor32( t );
            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,1,0,color,0,0) );
            o.addVertex( de::gpu::S3DVertex(x,y,0,0,1,0,color,0,0) );
            pSrc++;
         }
      }
   }

   o.indices.clear();
   o.indices.reserve( iCount );
   for ( size_t i = 0; i < colCount-1; ++i )
   {
      size_t iA = 0 + 2*i;
      size_t iB = 1 + 2*i;
      size_t iC = 3 + 2*i;
      size_t iD = 2 + 2*i;
      o.addIndexedQuad( iA,iB,iC,iD );
   }

   o.setDirty();
   o.upload();
   //de::gpu::SMeshBufferTool::computeNormals( o );
}



inline void
createMatrixTriangleMesh(
   int ch,
   de::gpu::SMeshBuffer & o,
   std::vector< std::vector< float >* > const & shiftMatrixData,
   float m_sizeX, float m_sizeY, float m_sizeZ,
   de::LinearColorGradient & gradient, uint32_t mode,
   float sampleRate_over_fftSize,
   uint32_t scaleXmode )
{
    o.vertices.clear();
    o.indices.clear();

    if (shiftMatrixData.empty())
    {
        DE_ERROR("shiftMatrixData.empty()")
        return;
    }

    if (!shiftMatrixData.front())
    {
        DE_ERROR("!shiftMatrixData.front()")
        return;
    }

    if (shiftMatrixData.front()->empty())
    {
        DE_ERROR("shiftMatrixData.front()->empty()")
        return;
    }

    if (!shiftMatrixData.front()->data())
    {
        DE_ERROR("!shiftMatrixData.front()->data()")
        return;
    }

    auto rowCount = shiftMatrixData.size();

   auto colCount = shiftMatrixData[0]->size();
   auto vCount = rowCount * colCount
               + 2*colCount;
   auto iCount = 4*(rowCount-1) * (colCount-1)
               + 4*(colCount-1);

   //float fx = 15.0f / float ( rowCount );

   o.setPrimitiveType( de::gpu::PrimitiveType::Triangles );
   o.setLighting( 0 );
   o.setCulling( false );
   o.vertices.reserve( vCount );

   float fx = 19.0f / float ( rowCount - 1 );

   if ( mode == 0 ) // PCM mode
   {
      float dx = m_sizeX / float( colCount );
      float dy = m_sizeY * 0.5f;
      float dz = m_sizeZ / float ( rowCount );

      // Matrix Top
      for ( size_t j = 0; j < rowCount; ++j )
      {
         float kx = dx; // * (1.0f + fx * float( j ));
         if ( ch == 0 ) // translate left channel in x-dir
         {
            kx = -kx;
         }

         std::vector< float >* row = shiftMatrixData[ j ];
         if ( row )
         {
            float const* pSrc = row->data();
            for ( size_t i = 0; i < colCount; ++i )
            {
               float s = *pSrc;
               float x = kx * float( i );
               float y = dy * s;
               float z = dz * j;
               uint32_t color = gradient.getColor32( std::abs( s ) );
               //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * s ) );
               o.addVertex( de::gpu::S3DVertex(x,y,z,0,-1,0,color,0,0) );
               pSrc++;
            }
         }
      }
   }
   else if ( mode == 1 ) // Decibel mode, logarithmic dB scale
   {
      float dBmin = -90;
      float dBmax = 60;
      float dBrange = dBmax - dBmin;
      if ( dBrange < 1.0f ) dBrange = 1.0f;
      float dBrangeInv = 1.0f / dBrange;
      float dx = m_sizeX / 2.0f; // / (sampleRate_over_fftSize * colCount);
      float dy = m_sizeY;
      float dz = m_sizeZ / float ( rowCount - 1 );
      if ( scaleXmode == 0 ) // X-axis is scaled logarithmicly.
      {
         dx = m_sizeX / float( colCount - 1 );
         //dy = m_sizeY * 0.5f;
         //dz = m_sizeZ / float ( rowCount - 1 );

      }
      // Matrix Top
      for ( size_t j = 0; j < rowCount; ++j )
      {
         float z = dz * j; // z is const for a same row.
         float kx = dx; // * (1.0f + fx * float( j )); // kx is const for same row.
         if ( ch == 0 ) // left channel goes from 0 to -size
         {
            kx = -kx;
         }

         std::vector< float >* row = shiftMatrixData[ j ];  // The row vector
         if ( row )
         {
            float const* pSrc = row->data();                // The row data

            if ( scaleXmode > 0 )                           // X-axis is scaled logarithmicly.
            {
               for ( size_t i = 0; i < colCount; ++i )
               {
                  float x = 0.0f;
                  float freq = sampleRate_over_fftSize * i;
                  if ( freq > 1e-16f ) // log(0) is -inf
                  {
                     x = kx * (log10f( freq ) - 1.5f); // -1 = shift by 10^-1
                  }
                  float dB = std::clamp( *pSrc++, dBmin, dBmax );
                  float r = (dB-dBmin) * dBrangeInv;
                  float y = dy * r;
                  uint32_t color = gradient.getColor32( r );
                  //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );
                  o.addVertex( de::gpu::S3DVertex(x,y,z,0,1,0,color,0,0) );
               }
            }
            else // if ( scaleXmode == 0 ) // X-axis is scaled linear frequency.
            {
               float x = 0.0f;
               for ( size_t i = 0; i < colCount; ++i )
               {
                  //float dB = std::clamp( *pSrc++, dBmin, dBmax );
                  float dB = *pSrc++;
                  float r = (dB-dBmin) * dBrangeInv;
                  float y = dy * r;
                  uint32_t color = gradient.getColor32( 1.0f - 0.5f * r ); // de::RainbowColor::computeColor32( t );
                  //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );
                  o.addVertex( de::gpu::S3DVertex(x,y,z,0,1,0,color,0,0) );
                  x += kx;
               }
            }
         }
      }
   }

   o.indices.reserve( iCount );
   for ( size_t j = 1; j < rowCount; ++j )
   {
      for ( size_t i = 1; i < colCount; ++i )
      {
         size_t iA = (j-1)*colCount + (i-1);
         size_t iB = (j  )*colCount + (i-1);
         size_t iC = (j  )*colCount + (i  );
         size_t iD = (j-1)*colCount + (i  );
         o.addIndexedQuad( iA,iB,iC,iD );
      }
   }

   // Front
   uint32_t c0 = gradient.getColor32( 0.0f ); // de::RainbowColor::computeColor32( t );

   auto pSrc = shiftMatrixData[ 0 ]->data();
   uint32_t v = o.getVertexCount();
   for ( size_t i = 0; i < colCount; ++i )
   {
      float x = o.getVertex( i ).pos.x;
      o.addVertex( de::gpu::S3DVertex(x,-90,0,0,1,0,c0,0,0) );
      pSrc++;
   }

   for ( size_t i = 0; i < colCount-1; ++i )
   {
      size_t iA = v + i;
      size_t iB = i;
      size_t iC = i+1;
      size_t iD = v + i+1;
      o.addIndexedQuad( iA,iB,iC,iD );
   }


   o.setDirty();
   //de::gpu::SMeshBufferTool::computeNormals( o );
}


/*

inline void
createMatrixTriangleMesh(
   int ch,
   de::gpu::SMeshBuffer & o,
   std::vector< std::vector< float >* > const & shiftMatrixData,
   float m_sizeX, float m_sizeY, float m_sizeZ,
   de::LinearColorGradient & gradient, int mode )
{
   auto rowCount = shiftMatrixData.size();
   if ( rowCount < 1 )
   {
      //DE_ERROR("No rows")
      return;
   }

   auto colCount = shiftMatrixData[0]->size();
   auto vCount = rowCount * colCount
               + 2*colCount;
   auto iCount = 4*(rowCount-1) * (colCount-1)
               + 4*(colCount-1);

   float dx = m_sizeX / float( colCount - 1 );
   float dy = m_sizeY * 0.5f;
   float dz = m_sizeZ / float ( rowCount - 1 );

   //float fx = 15.0f / float ( rowCount );

   o.setPrimitiveType( de::gpu::PrimitiveType::Triangles );
   o.setLighting( 0 );
   o.setCulling( false );

   o.Vertices.clear();
   o.Vertices.reserve( vCount );

   float fx = 19.0f / float ( rowCount - 1 );

   if ( mode == 0 ) // raw
   {
      if ( ch == 0 ) // translate left channel in x-dir
      {
         // Matrix Top
         for ( size_t j = 0; j < rowCount; ++j )
         {
            float kx = -dx * (1.0f + fx * float( j ));
            std::vector< float >* row = shiftMatrixData[ j ];
            if ( row )
            {
               float const* pSrc = row->data(); //  + colCount - 1
               for ( size_t i = 0; i < colCount; ++i )
               {
                  float s = *pSrc;
                  float x = kx * float( i );
                  float y = dy * s;
                  float z = dz * j;
                  uint32_t color = gradient.getColor32( std::abs( s ) ); // de::RainbowColor::computeColor32( t );
                  o.addVertex( de::gpu::S3DVertex(x,y,z,0,-1,0,color,0,0) );
                  pSrc++;
               }
            }
         }
         // Front
//         uint32_t color = gradient.getColor32( 0.0f ); // de::RainbowColor::computeColor32( t );
//         float const* pSrc = shiftMatrixData[ 0 ]->data() + colCount - 1;
//         for ( size_t i = 0; i < colCount; ++i )
//         {
//            float x = -m_sizeX + dx * float( i );
//            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,1,0,color,0,0) );
//            pSrc--;
//         }
//         for ( size_t i = 0; i < colCount-1; ++i )
//         {
//            size_t iA = colCount * rowCount + i;
//            size_t iB = i;
//            size_t iC = i+1;
//            size_t iD = colCount * rowCount + i+1;
//            o.addIndexedQuad( iA,iB,iC,iD );
//         }
      }
      else if ( ch == 1 )
      {
         // Matrix Top
         for ( size_t j = 0; j < rowCount; ++j )
         {
            float kx = dx * (1.0f + fx * float( j ));
            std::vector< float >* row = shiftMatrixData[ j ];
            if ( row )
            {
               float const* pSrc = row->data();
               for ( size_t i = 0; i < colCount; ++i )
               {
                  float s = *pSrc;
                  float x = kx * float( i );
                  float y = dy * s;
                  float z = dz * j;
                  uint32_t color = gradient.getColor32( std::abs( s ) ); // de::RainbowColor::computeColor32( t );
                  o.addVertex( de::gpu::S3DVertex(x,y,z,0,1,0,color,0,0) );
                  pSrc++;
               }
            }
         }
         // Front
//         uint32_t color = gradient.getColor32( 0.0f ); // de::RainbowColor::computeColor32( t );
//         float const* pSrc = shiftMatrixData[ 0 ]->data();
//         for ( size_t i = 0; i < colCount; ++i )
//         {
//            float x = dx * float( i );
//            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,1,0,color,0,0) );
//            pSrc++;
//         }
//         for ( size_t i = 0; i < colCount-1; ++i )
//         {
//            size_t iA = colCount * rowCount + i;
//            size_t iB = i;
//            size_t iC = i+1;
//            size_t iD = colCount * rowCount + i+1;
//            o.addIndexedQuad( iA,iB,iC,iD );
//         }
      }
   }
   else if ( mode == 1 ) // logarithmic dB scale
   {
      if ( ch == 0 ) // translate left channel in x-dir
      {
         // Matrix Top
         for ( size_t j = 0; j < rowCount; ++j )
         {
            std::vector< float >* row = shiftMatrixData[ j ];
            if ( row )
            {
               float const* pSrc = row->data() + colCount - 1;
               for ( size_t i = 0; i < colCount; ++i )
               {
                  float s = std::abs( *pSrc );
                  float db = -320.0f;
                  if ( s >= 1e-16f ) // log(0) is -inf
                  {
                     db = 20.0f * log10f( s );
                  }
                  float x = -m_sizeX + dx * float( i );
                  float y = db;
                  float z = dz * j;
                  uint32_t color = gradient.getColor32( s ); // de::RainbowColor::computeColor32( t );
                  o.addVertex( de::gpu::S3DVertex(x,y,z,0,1,0,color,0,0) );
                  pSrc--;
               }
            }
         }
         // Front
//         uint32_t color = gradient.getColor32( 0.0f ); // de::RainbowColor::computeColor32( t );
//         float const* pSrc = shiftMatrixData[ 0 ]->data() + colCount - 1;
//         for ( size_t i = 0; i < colCount; ++i )
//         {
//            float x = -m_sizeX + dx * float( i );
//            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,1,0,color,0,0) );
//            pSrc--;
//         }
//         for ( size_t i = 0; i < colCount-1; ++i )
//         {
//            size_t iA = colCount * rowCount + i;
//            size_t iB = i;
//            size_t iC = i+1;
//            size_t iD = colCount * rowCount + i+1;
//            o.addIndexedQuad( iA,iB,iC,iD );
//         }
      }
      else if ( ch == 1 )
      {
         // Matrix Top
         for ( size_t j = 0; j < rowCount; ++j )
         {
            std::vector< float >* row = shiftMatrixData[ j ];
            if ( row )
            {
               float const* pSrc = row->data();
               for ( size_t i = 0; i < colCount; ++i )
               {
                  float s = std::abs( *pSrc );
                  float db = -320.0f;
                  if ( s >= 1e-16f ) // log(0) is -inf
                  {
                     db = 20.0f * log10f( s );
                  }
                  float x = dx * float( i );
                  float y = db;
                  float z = dz * j;
                  uint32_t color = gradient.getColor32( s ); // de::RainbowColor::computeColor32( t );
                  o.addVertex( de::gpu::S3DVertex(x,y,z,0,0,-1,color,0,0) );
                  pSrc++;
               }
            }
         }
         // Front
//         uint32_t color = gradient.getColor32( 0.0f ); // de::RainbowColor::computeColor32( t );
//         float const* pSrc = shiftMatrixData[ 0 ]->data();
//         for ( size_t i = 0; i < colCount; ++i )
//         {
//            float x = dx * float( i );
//            o.addVertex( de::gpu::S3DVertex(x,-320,0,0,0,-1,color,0,0) );
//            pSrc++;
//         }
//         for ( size_t i = 0; i < colCount-1; ++i )
//         {
//            size_t iA = colCount * rowCount + i;
//            size_t iB = i;
//            size_t iC = i+1;
//            size_t iD = colCount * rowCount + i+1;
//            o.addIndexedQuad( iA,iB,iC,iD );
//         }
      }
   }

   o.Indices.clear();
   o.Indices.reserve( iCount );
   for ( size_t j = 1; j < rowCount; ++j )
   {
      for ( size_t i = 1; i < colCount; ++i )
      {
         size_t iA = (j-1)*colCount + (i-1);
         size_t iB = (j  )*colCount + (i-1);
         size_t iC = (j  )*colCount + (i  );
         size_t iD = (j-1)*colCount + (i  );
         o.addIndexedQuad( iA,iB,iC,iD );
      }
   }

   //de::gpu::SMeshBufferTool::computeNormals( o );
}


*/

