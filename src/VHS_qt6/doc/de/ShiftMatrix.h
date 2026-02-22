#pragma once
#include <de/ShiftBuffer.h>
#include <functional>

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
   de::ShiftBuffer< T > m_shiftBuffer;
   // Count matrix cols = m_shiftBuffer.size()
   uint32_t m_colCount;
   // Count matrix rows
   uint32_t m_rowCount;
   // Count m_shiftBuffer fill status to control shift tigger.
   uint32_t m_frameIndex;
   // Debug collection of new samples
   uint32_t m_pushCount;
   // The 3d matrix size in 3d world space.
   //typedef glm::tvec3< T > V3;
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

   void destroy()
   {
        for ( size_t i = 0; i < m_orig.size(); ++i )
        {
            auto p = m_orig[ i ];
            if (p) { delete p; }
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



};
