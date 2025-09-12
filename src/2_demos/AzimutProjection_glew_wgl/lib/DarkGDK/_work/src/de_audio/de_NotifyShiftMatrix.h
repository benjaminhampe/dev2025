#pragma once
#include "de_NotifyBuffer.h"

namespace de {

// Does only swap pointers and makes no expensive memory copying!

// To have a more quadratic matrix it uses m_dacFrames = m_shiftBuffer.size()
// so the shiftbuffer probably collects only once untils its already full.
// With colCount == m_dacFrames each row represents exactly one audio callback.
//
// Since push() is called constantly we delay expensive postfx like log10f() until render().
// render() collects data from the matrix using m_data, not m_orig or m_copy.

/////////////////////////////////////////////////////////////////////////////
template < typename T >
struct NotifyShiftMatrix
/////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("NotifyShiftMatrix")

   typedef std::vector< T > TRow;

   typedef std::function< void( NotifyShiftMatrix* nsm ) > FN_OnPushRow;

   FN_OnPushRow m_callbackOnPushRow;
   std::vector< TRow* > m_orig; // Owner ( deletes all )
   std::vector< TRow* > m_data; // Not owner ( can forget something at bug )
   std::vector< TRow* > m_copy; // Not owner ( does not delete anything )
   // Collects mono channel data until m_shiftBuffer.size()
   // and uses the data to build a 3d geometry out of it.
   // The input can be small, e.g. 256 samples per channel for realtime audio.
   // So with 1024 shift buffer size we collect 4 audio callbacks
   // then we trigger a new mesh from collection and reset m_frameIndex.
   //ShiftBuffer< T > m_shiftBuffer;
   //uint32_t m_frameIndex; ///< Count m_shiftBuffer fill status to control shift tigger.
   uint32_t m_colCount;    ///< Count matrix cols
   uint32_t m_rowCount;    ///< Count matrix rows
   uint32_t m_pushCount;   ///< Debug collection of new samples

   NotifyShiftMatrix()
      : m_callbackOnPushRow( [] ( NotifyShiftMatrix* nsm ) {} )
      , m_colCount( 1024 )
      , m_rowCount( 64 )
      , m_pushCount( 0 )
      //, m_frameIndex( 0 )
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
      //m_shiftBuffer.resize( m_colCount );
   }

   ~NotifyShiftMatrix()
   {
      destroy();
   }

   void setCallback( FN_OnPushRow const & onPushRow )
   {
      m_callbackOnPushRow = onPushRow;
   }

   //T const* src, uint32_t srcFrames,
   void notifyPushedRow()
   {
      if ( m_callbackOnPushRow )
      {
         m_callbackOnPushRow( this );
      }
   }

   void destroy()
   {
      for ( size_t i = 0; i < m_orig.size(); ++i )
      {
         if ( m_orig[ i ] )
         {
            delete m_orig[ i ];
            m_orig[ i ] = nullptr;
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
         //m_shiftBuffer.resize( m_colCount );
         //m_frameIndex = 0;
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
         //m_shiftBuffer.resize( m_colCount );
         //m_frameIndex = 0;
      }
   }

   uint32_t rowCount() const { return m_rowCount; }
   uint32_t colCount() const { return m_colCount; }

      // Only mono channel data is allowed.
   void pushRow( T const* src, uint32_t srcFrames )
   {
      if ( srcFrames != m_colCount )
      {
         resize( srcFrames, rowCount() );
      }

      //m_shiftBuffer.push( "ShiftMatrix", src, srcFrames );
      //m_frameIndex += srcFrames;
      m_pushCount++;

      // Shift row
      m_data[ 0 ] = m_copy[ m_rowCount-1 ];
      for ( size_t i = 1; i < m_rowCount; ++i )
      {
         m_data[ i ] = m_copy[ i-1 ];
      }

      // Get new front row to overwrite
      TRow & row = *m_data[ 0 ];
      memcpy( row.data(), src, srcFrames * sizeof( T ) );

      notifyPushedRow();

      //m_frameIndex = 0;

         //DE_DEBUG("Fill after ",m_pushCount," pushes, m_shiftBuffer.size(", m_shiftBuffer.size(), "), srcCount(", srcCount,")")
//         m_shiftBuffer.zeroMemory();
      //m_pushCount = 0;

      m_copy.swap( m_data );
   }

};

} // end namespace de
