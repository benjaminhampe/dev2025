//! author: BenjaminHampe@gmx.de
#pragma once
#include <de_core/de_AlignedMalloc.h>
#include <de_core/de_ForceInline.h>
#include <de_core/de_Math.h>
#include <de_core/de_RuntimeError.h>
#include <cstdint>
#include <cstdlib>

// ===============================================================================================
struct AlignedVectorf
// ===============================================================================================
{
   float* m_begin;
   float* m_end;
   float* m_capacity;

   AlignedVectorf( size_t n = 0 )
      : m_begin(nullptr)
      , m_end(nullptr)
      , m_capacity(nullptr)
   {
      resize( n );
   }

   ~AlignedVectorf()
   {
      destroy();
   }

   void destroy()
   {
      de_aligned_free( m_begin );
      m_begin = nullptr;
      m_end = nullptr;
      m_capacity = nullptr;
   }

   void clear()
   {
      m_end = m_begin;
   }

   void shrink_to_fit()
   {

   }

   void reserve( size_t n = 0 )
   {
      if ( n < 1 )
      {
         clear();
         return;
      }

      if ( capacity() == 0 )
      {
         size_t const new_capacity = dbNextPowerOf2( n );
         m_begin = reinterpret_cast<float*>( de_aligned_malloc( new_capacity * sizeof(float), 64 ) );
         //m_end = m_begin + n;
         m_capacity = m_begin + new_capacity;
         return;
      }

      if ( n > capacity() )
      {
         size_t new_capacity = capacity() * 2; // exponential growth
         while ( n > new_capacity )
         {
            new_capacity *= 2; // exponential growth
         }

         m_begin = reinterpret_cast<float*>( de_aligned_realloc( m_begin, new_capacity * sizeof(float), 64 ) );
      }
   }

   void resize( size_t n = 0, float defaultValue = 0.0f )
   {
      size_t old_size = size();
      reserve( n );
      m_end = m_begin + n;
   }

   void push_back( float value )
   {

   }

   void emplace_back( float value )
   {

   }

   size_t size() const
   {
      return m_end - m_begin;
   }

   size_t capacity() const
   {
      return m_capacity - m_begin;
   }

   bool empty() const
   {
      return m_end == m_begin;
   }

   float* begin() const { return m_begin; }
   float* end() const { return m_end; }

   float const* cbegin() const { return m_begin; }
   float const* cend() const { return m_end; }

};
