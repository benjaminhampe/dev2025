#pragma once
#include <cstdint>

namespace de {

// =======================================================================
struct Align
// =======================================================================
{
   uint8_t m_align;

   Align() : m_align(NoAlign) {}
   Align( uint8_t align ) : m_align(align) {}
   Align( Align const & other ) : m_align(other.m_align) {}

   operator uint8_t() const { return m_align; }

   enum EAlign
   {
      NoAlign = 0,
      Left = 1,
      Center = 2,
      Right = 4,
      Top = 8,
      Middle = 16,
      Bottom = 32,
      Baseline = 64,

      Centered = Align::Center | Align::Middle,

      TopLeft = Align::Left | Align::Top,
      TopCenter = Align::Center | Align::Top,
      TopRight = Align::Right | Align::Top,
      MiddleLeft = Align::Left | Align::Middle,
      MiddleCenter = Align::Centered,
      MiddleRight = Align::Right | Align::Middle,
      BottomLeft = Align::Left | Align::Bottom,
      BottomCenter = Align::Center | Align::Bottom,
      BottomRight = Align::Right | Align::Bottom,

      LeftTop = Align::TopLeft,
      CenterTop = Align::TopCenter,
      RightTop = Align::TopRight,
      LeftMiddle = Align::MiddleLeft,
      CenterMiddle = Align::Centered,
      RightMiddle = Align::MiddleRight,
      LeftBottom = Align::BottomLeft,
      CenterBottom = Align::BottomCenter,
      RightBottom = Align::BottomRight,

      Default = Align::TopLeft
   };

};

} // end namespace de.
