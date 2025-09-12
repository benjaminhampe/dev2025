#pragma once
#include <cstdint>

namespace de {

// ===========================================================================
struct PrimitiveType
// ===========================================================================
{
   enum EType : uint8_t
   {
      Points = 0,    // 1 GL_POINTS
      Lines,         // 2 GL_LINES
      LineStrip,     // 2 GL_LINE_STRIP
      LineLoop,      // 2 GL_LINE_LOOP
      Triangles,     // 3 GL_TRIANGLES
      TriangleStrip, // 3 GL_TRIANGLE_STRIP
      TriangleFan,   // 3 GL_TRIANGLE_FAN
      Quads,         // 4 GL_QUADS -> not really impl on GPU but used for saving meshes.
      //Polygon,       // 5-N GL_POLYGON = VERTEX_OUTLINE_LIST, FILLED lINE_lOOP
      Max
   };

   EType m_type;
   PrimitiveType() : m_type(Points) {}
   PrimitiveType( EType type ) : m_type(type) {}
   PrimitiveType( PrimitiveType const & other ) : m_type(other.m_type) {}
   operator uint8_t() const { return m_type; }
   std::string toString() const { return getString( *this ); }

   static std::string
   getString( PrimitiveType const primitiveType )
   {
      switch ( primitiveType )
      {
        case Lines: return "Lines";
        case LineStrip: return "LineStrip";
        case LineLoop: return "LineLoop";
        case Triangles: return "Triangles";
        case TriangleStrip: return "TriangleStrip";
        case TriangleFan: return "TriangleFan";
        case Quads: return "Quads";
        default: return "Points";
      }
   }

   static uint32_t
   getPrimitiveCount( EType const primType, uint32_t const vCount, uint32_t const iCount )
   {
      if ( iCount > 0 )
      {
        switch( primType )
        {
          case Points: return iCount;            // #num points
          case Lines: return iCount / 2;         // #num Lines
          case LineLoop: return iCount - 1;
          case Triangles: return iCount / 3;     // #num triangles in Triangles
          case TriangleStrip: return iCount - 2; // #num triangles in Strip
          case TriangleFan: return iCount - 2;   // #num triangles in Fan
          case Quads: return iCount / 4;         // #num quads in Quads
          default: return 0;
        }
      }
      else
      {
        switch( primType )
        {
          case Points: return vCount;            // #num points
          case Lines: return vCount / 2;         // #num Lines
          case LineLoop: return vCount - 1;
          case Triangles: return vCount / 3;     // #num triangles in Triangles
          case TriangleStrip: return vCount - 2; // #num triangles in Strip
          case TriangleFan: return vCount - 2;   // #num triangles in Fan
          case Quads: return vCount / 4;         // #num quads in Quads
          default: return 0;
        }
      }
   }   
   
   /*
   static uint32_t
   getEdgeCount( PrimitiveType const primitiveType, uint32_t const indexCount )
   {
      switch( primitiveType )
      {
      case Points: return 0;
      case Lines: return indexCount / 2;
      case LineLoop: return indexCount - 1;
      case Triangles: return indexCount;
      case TriangleStrip: return indexCount;
      case TriangleFan: return indexCount;
      //case Polygon: return indexCount;
      //case Quads: return indexCount;
      default: return 0;
      }
   }
   */
};

} // end namespace de
