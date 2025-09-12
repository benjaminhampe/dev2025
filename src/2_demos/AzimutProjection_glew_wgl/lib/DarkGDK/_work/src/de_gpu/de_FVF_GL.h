#pragma once
#include <de_gpu/de_FVF.h>
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

// ===========================================================================
struct GL_PrimitiveType
// ===========================================================================
{
   static PrimitiveType
   fromOpenGL( uint32_t const primitiveType )
   {
      switch ( primitiveType )
      {
         case GL_LINES: return PrimitiveType::Lines;
         case GL_LINE_STRIP: return PrimitiveType::LineStrip;
         case GL_LINE_LOOP: return PrimitiveType::LineLoop;
         case GL_TRIANGLES: return PrimitiveType::Triangles;
         case GL_TRIANGLE_STRIP: return PrimitiveType::TriangleStrip;
         case GL_TRIANGLE_FAN: return PrimitiveType::TriangleFan;
         case GL_QUADS: return PrimitiveType::Quads;
         default: return PrimitiveType::Points;
      }
   }

   static uint32_t
   toOpenGL( PrimitiveType const primitiveType )
   {
      switch ( primitiveType )
      {
         case PrimitiveType::Lines: return GL_LINES;
         case PrimitiveType::LineStrip: return GL_LINE_STRIP;
         case PrimitiveType::LineLoop: return GL_LINE_LOOP;
         case PrimitiveType::Triangles: return GL_TRIANGLES;
         case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
         case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
         case PrimitiveType::Quads: return GL_QUADS;
         default: return GL_POINTS;
      }
   }
};


}// end namespace de
