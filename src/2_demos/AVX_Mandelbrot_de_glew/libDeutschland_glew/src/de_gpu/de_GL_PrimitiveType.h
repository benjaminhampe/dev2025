#pragma once
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

struct GL_PrimitiveType
{
   static PrimitiveType
   fromOpenGL( u32 const primitiveType )
   {
      switch ( primitiveType )
      {
         //case GL_POINTS: return Points;
         case GL_LINES: return Lines;
         case GL_LINE_STRIP: return LineStrip;
         case GL_LINE_LOOP: return LineLoop;
         case GL_TRIANGLES: return Triangles;
         case GL_TRIANGLE_STRIP: return TriangleStrip;
         case GL_TRIANGLE_FAN: return TriangleFan;
         case GL_QUADS: return Quads;
         default: return Points;
      }
   }

   static u32
   toOpenGL( PrimitiveType const primitiveType )
   {
      switch ( primitiveType )
      {
         //case Points: return GL_POINTS;
         case Lines: return GL_LINES;
         case LineStrip: return GL_LINE_STRIP;
         case LineLoop: return GL_LINE_LOOP;
         case Triangles: return GL_TRIANGLES;
         case TriangleStrip: return GL_TRIANGLE_STRIP;
         case TriangleFan: return GL_TRIANGLE_FAN;
         case Quads: return GL_QUADS;
         default: return GL_POINTS;
      }
   }   
};


}// end namespace de