//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////

#ifndef DE_MOUSE_EVENT_HPP
#define DE_MOUSE_EVENT_HPP

#include <cstdint>
#include <sstream>
#include <vector>
#include <array>

#include <cwchar>
#include <codecvt>
#include <cstring> // strlen()
#include <cmath>
#include <codecvt> // convert wstring ( unicode ) <-> string ( multibyte utf8 )
#include <algorithm>
#include <iomanip> // string_converter
#include <memory>

namespace de {

   inline bool
   isMouseOver( int mx, int my, int x1, int y1, int x2, int y2 )
   {
     if ( x1 > x2 ) std::swap( x1, x2 );
     if ( y1 > y2 ) std::swap( y1, y2 );
     if ( mx < x1 ) return false;
     if ( my < y1 ) return false;
     if ( mx > x2 ) return false;
     if ( my > y2 ) return false;
     return true;
   }

   // ========================================================================
   /// @brief MouseEvent
   // ========================================================================
   struct MouseEvent
   {
      //! Masks for mouse button states
      enum EFlags : uint32_t
      {
         None       = 0,
         BtnLeft    = 1,
         BtnRight   = 2,
         BtnMiddle  = 4,
         BtnExtra1  = 8,  // A mouse with 4 buttons the developer never had.
         BtnExtra2  = 16,  // A mouse with 5 buttons the developer never had.
         BtnExtra3  = 32,  // A mouse with 6 buttons the developer never had.
         BtnExtra4  = 64,  // A mouse with 7 buttons the developer never had.
         BtnExtra5  = 128,  // A mouse with 8 buttons the developer never had.
         Pressed    = 1 << 8,  // One or more buttons can only be entirely pressed or released in one event
         Released   = 1 << 9,  // One or more buttons can only be entirely pressed or released in one event
         Moved      = 1 << 10,
         DoubleClick= 1 << 11,
         TripleClick= 1 << 12,
         Wheel      = 1 << 13,
         WheelX     = 1 << 14,
         WheelY     = 1 << 15,
         WithShift  = 1 << 20, // True if SHIFT was also pressed
         WithCtrl   = 1 << 21, // True if CTRL was also pressed
         WithAlt    = 1 << 22, // True if ALT was also pressed
         WithAltGr  = 1 << 23, // True if Alt-Gr was also pressed
         WithSuper  = 1 << 24, // True if WinKey/Super1Key was also pressed
         ButtonCount = 8,
         ButtonMask = 0x0F,
         FORCE_32_BIT = 0x7fffffff
      };

      int64_t m_pts;       // The event time stamp in nanoseconds since clock epoch
      uint32_t m_flags;
      int32_t m_x;
      int32_t m_y;
      float m_wheelY;        //! mouse wheel delta, often 1.0 or -1.0, but can have other values < 0.f or > 0.f;

      bool isMoved() const { return m_flags & Moved; }
      bool isReleased() const { return m_flags & Released; }
      bool isPressed() const { return m_flags & Pressed; }
      bool isWheeled() const { return m_flags & ( WheelX | WheelY ); }

      bool isLeftPressed() const { return m_flags & ( BtnLeft | Pressed ); }
      bool isRightPressed() const { return m_flags & ( BtnRight | Pressed ); }
      bool isMiddlePressed() const { return m_flags & ( BtnMiddle | Pressed ); }

      bool isLeft() const { return m_flags & BtnLeft; }
      bool isRight() const { return m_flags & BtnRight; }
      bool isMiddle() const { return m_flags & BtnMiddle; }

//      bool isLeftPressed() const { return m_flags & BtnLeft; }
//      bool isRightPressed() const { return m_flags & BtnRight; }
//      bool isMiddlePressed() const { return m_flags & BtnMiddle; }

      int x() const { return m_x; }
      int y() const { return m_y; }
      float z() const { return m_wheelY; }
      float wheel() const { return m_wheelY; }

      int getButton() const
      {
         int btn = 0;
         btn += ( isLeft() ) ? 1 : 0;
         btn += ( isRight() ) ? 2 : 0;
         btn += ( isMiddle() ) ? 3 : 0;
         return btn;
      }

      MouseEvent()
         : m_flags( 0 )
         , m_x(0)
         , m_y(0)
         , m_wheelY( 0.0f )
      {}

      std::string toString() const
      {
         std::stringstream s;
         s << "flags(" << m_flags << "), "
            "button(" << getButton() << "), "
            "pos(" << m_x << "," << m_y << "), "
            "wheel(" << m_wheelY << "), "
            "shift(" << (m_flags & WithShift) << "), "
            "ctrl(" << (m_flags & WithCtrl) << ")";
         return s.str();
      }
   };

   // ========================================================================
   /// @brief MouseMultiClicks
   // ========================================================================
   struct MouseMultiClicks
   {
      MouseMultiClicks()
         : DoubleClickTime(500)
         , CountSuccessiveClicks(0)
         , LastClickTime(0)
         , LastClickX(0)
         , LastClickY(0)
         , LastFlags(0)
      {}

      uint32_t DoubleClickTime;
      uint32_t CountSuccessiveClicks;
      uint32_t LastClickTime;
      int32_t LastClickX;
      int32_t LastClickY;
      uint32_t LastFlags;
   };

} // end namespace de.


#endif // DARKENGINE_IWINDOW_HPP

