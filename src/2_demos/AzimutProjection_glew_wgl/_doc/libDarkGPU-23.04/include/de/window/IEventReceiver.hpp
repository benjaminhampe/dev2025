#pragma once

//////////////////////////////////////////////////////////////////////////////
/// @file EventReceiver.hpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////

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

#include <de/Logger.hpp>

namespace de {

   namespace gpu { struct VideoDriver; }

   // ========================================================================
   //inline bool dbIsMouseOver( int mx, int my, int x1, int y1, int x2, int y2 )
   // ========================================================================
   //{
   //   if ( x1 > x2 ) std::swap( x1, x2 );
   //   if ( y1 > y2 ) std::swap( y1, y2 );
   //   if ( mx < x1 ) return false;
   //   if ( my < y1 ) return false;
   //   if ( mx > x2 ) return false;
   //   if ( my > y2 ) return false;
   //   return true;
   //}

   // ========================================================================
   enum class EventType
   // ========================================================================
   {
      RESIZE = 0, //! An event of the (underlying) native window.
      GUI,        //! An event of the graphical user interface.
      MOUSE,      //! A mouse input event.
      KEYBOARD,   //! A key input event.
      JOYSTICK,   //! A joystick (joypad, gamepad) input event.
      LOG,        //! A log event
      USER        //! A user event with user data.
   };

   // ========================================================================
   struct ResizeEvent
   // ========================================================================
   {
      int32_t m_w = 0;
      int32_t m_h = 0;
      void* m_userData = nullptr;
   };


   // ========================================================================
   enum class GuiEventType
   // ========================================================================
   {
      // A gui element has lost its focus.
      /* GUIEvent.Caller is losing the focus to GUIEvent.Element.
      If the event is absorbed then the focus will not be changed. */
      ELEMENT_FOCUS_LOST = 0,
      // A gui element has got the focus.
      /* If the event is absorbed then the focus will not be changed. */
      ELEMENT_FOCUSED,
      // The mouse cursor hovered over a gui element.
      /* If an element has sub-elements you also get this message for the subelements */
      ELEMENT_HOVERED,
      // The mouse cursor left the hovered element.
      /* If an element has sub-elements you also get this message for the subelements */
      ELEMENT_LEFT,
      // An element would like to close.
      /* Windows and context menus use this event when they would like to close,
      this can be cancelled by absorbing the event. */
      ELEMENT_CLOSED,
      // A button was clicked.
      BUTTON_CLICKED,
      // A scrollbar has changed its position.
      SCROLLBAR_CHANGED,
      // A checkbox has changed its check state.
      CHECKBOX_CHANGED,
      // The selection in a combo box has been changed
      COMBOBOX_CHANGED,
      // A new item in a listbox was selected.
      /* NOTE: You also get this event currently when the same item was clicked again after more than 500 ms. */
      LISTBOX_CHANGED,
      // An item in the listbox was selected, which was already selected.
      /* NOTE: You get the event currently only if the item was clicked again within 500 ms or selected by "enter" or "space". */
      LISTBOX_SELECTED_AGAIN,
      // A file has been selected in the file dialog
      FILE_SELECTED,
      // A directory has been selected in the file dialog
      DIRECTORY_SELECTED,
      // A file open dialog has been closed without choosing a file
      FILE_CHOOSE_DIALOG_CANCELLED,
      // 'Yes' was clicked on a messagebox
      MESSAGEBOX_YES,
      // 'No' was clicked on a messagebox
      MESSAGEBOX_NO,
      // 'OK' was clicked on a messagebox
      MESSAGEBOX_OK,
      // 'Cancel' was clicked on a messagebox
      MESSAGEBOX_CANCEL,
      // In an editbox 'ENTER' was pressed
      EDITBOX_ENTER,
      // The text in an editbox was changed. This does not include automatic changes in text-breaking.
      EDITBOX_CHANGED,
      // The marked area in an editbox was changed.
      EDITBOX_MARKING_CHANGED,
      // The tab was changed in an tab control
      TAB_CHANGED,
      // A menu item was selected in a (context) menu
      MENU_ITEM_SELECTED,
      // The value of a spin box has changed
      SPINBOX_CHANGED,
      // A table has changed
      TABLE_CHANGED,
      TABLE_HEADER_CHANGED,
      TABLE_SELECTED_AGAIN,
      // A tree view node lost selection. See IGUITreeView::getLastEventNode().
      TREEVIEW_NODE_DESELECT,
      // A tree view node was selected. See IGUITreeView::getLastEventNode().
      TREEVIEW_NODE_SELECT,
      // A tree view node was expanded. See IGUITreeView::getLastEventNode().
      TREEVIEW_NODE_EXPAND,
      // A tree view node was collapsed. See IGUITreeView::getLastEventNode().
      TREEVIEW_NODE_COLLAPSE,
      // deprecated - use EGET_TREEVIEW_NODE_COLLAPSE instead. This
      // may be removed by Irrlicht 1.9
      TREEVIEW_NODE_COLLAPS = GuiEventType::TREEVIEW_NODE_COLLAPSE,
      // No real event. Just for convenience to get number of events
      COUNT
   };

   // ========================================================================
   struct GuiEvent
   // ========================================================================
   {
      // Widget who created the event
      void* m_caller = nullptr;

      // If the event has something to do with another element, it will be held here.
      void* m_optionalElement = nullptr;

      // Type of GUI Event
      GuiEventType m_eventType = GuiEventType::COUNT;
   };

/*
   // =======================================================================
   struct PaintEvent
   // =======================================================================
   {
      gpu::VideoDriver* m_driver = nullptr;
      //ICamera* camera;
   };

   // =======================================================================
   struct ShowEvent
   // =======================================================================
   {
      void* m_window = nullptr;
   };

   // =======================================================================
   struct HideEvent
   // =======================================================================
   {
      void* m_window = nullptr;
   };

   // =======================================================================
   struct FocusEvent
   // =======================================================================
   {
      void* m_window = nullptr;
   };

   // =======================================================================
   struct EnterEvent
   // =======================================================================
   {
      void* m_window = nullptr;
      int32_t m_x = 0;
      int32_t m_y = 0;
   };

   // =======================================================================
   struct LeaveEvent
   // =======================================================================
   {
      void* m_window = nullptr;
      int32_t m_x = 0;
      int32_t m_y = 0;
   };
*/
   // ========================================================================
   struct KeyModifier
   // ========================================================================
   {
      enum EModifier {
         None = 0,
         Pressed  =  0x01, Repeat  =  0x02, Shift = 0x04, Ctrl = 0x08,
         Alt      =  0x10, AltGr   =  0x20, Super = 0x40, Fn   = 0x80,
         CapsLock = 0x100, NumLock = 0x200, EModifierCount = 10
      };

      KeyModifier() : m_flags(None) {}
      KeyModifier( uint32_t flags ) : m_flags(flags) {}
      operator uint32_t() const { return m_flags; }

      uint32_t m_flags;
   };

   // ========================================================================
   enum EKEY
   // ========================================================================
   {
      KEY_UNKNOWN = 0,

      KEY_ESCAPE, KEY_ENTER, KEY_SPACE, KEY_BACKSPACE, KEY_TAB,

      KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN, // Arrow keys

      KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT,
      KEY_LEFT_ALT, KEY_RIGHT_ALT, // ALTGR
      KEY_LEFT_CTRL, KEY_RIGHT_CTRL,
      KEY_LEFT_SUPER, KEY_RIGHT_SUPER,
      KEY_CAPS_LOCK, KEY_NUM_LOCK,

      // ControlBlock 6x
      KEY_INSERT,
      KEY_DELETE,
      KEY_HOME,  // POS1
      KEY_END,
      KEY_PAGE_UP,
      KEY_PAGE_DOWN,

      // Special characters
      KEY_BACKSLASH,    // '\\'
      KEY_PLUS,         // '+'
      KEY_COMMA,        // ','
      KEY_MINUS,        // '-'
      KEY_PERIOD,       // '.'
      KEY_SLASH,        // '/'
      KEY_COLON,        // ':'
      KEY_APOSTROPHE,   // '''
      KEY_SEMICOLON,    // ';'
      KEY_EQUAL,        // '='
      KEY_LEFT_BRACKET, // '['
      KEY_RIGHT_BRACKET,// ']'
      KEY_GRAVE_ACCENT, // '`' 96
      KEY_LOWERTHEN,    // '<'
      KEY_GREATERTHEN,  // '>'
      KEY_SEPARATOR,    // '|'
      //KEY_WORLD_1  // 161 non-US #1
      //KEY_WORLD_2  // 162 non-US #2
      //KEY_MENU     // 348

      // Function Keys
      KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
      KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
      KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18,
      KEY_F19, KEY_F20, KEY_F21, KEY_F22, KEY_F23, KEY_F24,

      // ControlBlock 3x
      KEY_PRINT,
      KEY_SCROLL_LOCK,  // SCROLL LOCK key
      KEY_PAUSE,

      // AlphaNum
      KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
      KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

      KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
      KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P,
      KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
      KEY_Y, KEY_Z,

      // EXTRA
      KEY_SELECT,
      KEY_EXECUT,
      KEY_SNAPSHOT,
      KEY_HELP,
      KEY_APPS,
      KEY_SLEEP,

      // KeyPad/Numpad
      KEY_KP_0, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4, // 320,321,322,323,324
      KEY_KP_5, KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9, // 325,326,327,328,329
      KEY_KP_DIVIDE,    // 331
      KEY_KP_MULTIPLY,  // 332
      KEY_KP_SUBTRACT,  // 333
      KEY_KP_ADD,       // 334
      KEY_KP_DECIMAL,   // 330
      KEY_KP_ENTER,     // 335
      //KEY_KP_EQUAL    // 336

      KEY_OEM_1,  // for US    ";:"
      KEY_OEM_2,  // for US    "/?"
      KEY_OEM_3,  // for US    "`~"
      KEY_OEM_4,  // for US    "[{"
      KEY_OEM_5,  // for US    "\|"
      KEY_OEM_6,  // for US    "]}"
      KEY_OEM_7,  // for US    "'""
      KEY_OEM_8,  // None
      KEY_OEM_AX,  // for Japan "AX"
      KEY_OEM_102,  // "<>" or "\|"

//      KEY_KANA             = 0x15,  // IME Kana mode
//      KEY_HANGUEL          = 0x15,  // IME Hanguel mode (maintained for compatibility use HANGUL)
//      KEY_HANGUL           = 0x15,  // IME Hangul mode
//      KEY_JUNJA            = 0x17,  // IME Junja mode
//      KEY_FINAL            = 0x18,  // IME final mode
//      KEY_HANJA            = 0x19,  // IME Hanja mode
//      KEY_KANJI            = 0x19,  // IME Kanji mode

//      KEY_CONVERT          = 0x1C,  // IME convert
//      KEY_NONCONVERT       = 0x1D,  // IME nonconvert
//      KEY_ACCEPT           = 0x1E,  // IME accept
//      KEY_MODECHANGE       = 0x1F,  // IME mode change request



      EKEY_COUNT // this is not a key, but the amount of keycodes there are.
   };

   // ========================================================================
   struct StringConverter
   // ========================================================================
   {
      // GET string FROM wstring
      static std::string
      toString( std::wstring const & txt )
      {
         std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
         return converter.to_bytes( txt );
      }

      // GET string FROM wchar.
      static std::string
      toString( wchar_t const wc )
      {
         std::wstringstream w; w << wc;
         std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
         return converter.to_bytes( w.str() );
      }
   };

   // ========================================================================
   struct KeyEvent
   // ========================================================================
   {
      uint32_t m_unicode; // For unknown enum value, corresponds to the key (0, if not a character)
      uint32_t m_key;     // DarkGPU EKEY
      uint32_t m_scancode;// Platform dependent scan key code
      uint32_t m_modifiers;
      int32_t m_x;
      int32_t m_y;
      char m_csymbol;
      wchar_t m_wsymbol;

      KeyEvent()
         : m_unicode( 0 )
         , m_key( EKEY_COUNT )
         , m_modifiers( KeyModifier::None )
         , m_x(0)
         , m_y(0)
         , m_csymbol('?')
         , m_wsymbol('?')
      {

      }

      // Modifiers making KeyEvent usable for both keyPress and keyRelease events.
      bool isPressed() const
      {
         return m_modifiers & KeyModifier::Pressed;
      }

      void setpressed( bool pressed )
      {
         if (pressed)
         {
            m_modifiers |= KeyModifier::Pressed;
         }
         else
         {
            m_modifiers &= ~KeyModifier::Pressed;
         }

      }

      bool shift() const { return m_modifiers & KeyModifier::Shift; }
      bool ctrl() const { return m_modifiers & KeyModifier::Ctrl; }
      bool alt() const { return m_modifiers & KeyModifier::Alt; }
      bool altgr() const { return m_modifiers & KeyModifier::AltGr; }
      bool win() const { return m_modifiers & KeyModifier::Super; }
      bool fn() const { return m_modifiers & KeyModifier::Fn; }

      void setUnicode( uint32_t unicode ) { m_unicode = unicode; }
      void setKey( EKEY key ) { m_key = key; }
      void setModifer( uint32_t modifier ) { m_modifiers = modifier & 0xFF; }

//      uint32_t unicode() const { return Unicode; }
//      EKEY key() const { return Key; }
//      uint32_t modifers() const { return m_modifiers; }

      std::string toString() const
      {
         std::ostringstream s;
         s << "Unicode(" << m_unicode << "), Key(" << int(m_key) << "), "
              "Scancode(" << int(m_scancode) << "), Modifiers(" << m_modifiers << "), "
              "X(" << int(m_x) << "), Y(" << m_y << ")" ;
         return s.str();
      }
   };

/*
   // =======================================================================
   struct MousePressEvent // MouseClickEvent
   // =======================================================================
   {
      int32_t m_x = 0;
      int32_t m_y = 0;
      int32_t m_button = 0;
      uint32_t m_flags = 0;
   };

   // =======================================================================
   struct MouseReleaseEvent // MouseClickEvent
   // =======================================================================
   {
      int32_t m_x = 0;
      int32_t m_y = 0;
      int32_t m_button = 0;
      uint32_t m_flags = 0;
   };

   // =======================================================================
   struct MouseMoveEvent // MoveEvent
   // =======================================================================
   {
      int32_t m_x = 0;
      int32_t m_y = 0;
   };

   // =======================================================================
   struct MouseWheelEvent // ScrollEvent
   // =======================================================================
   {
      double m_x = 0;
      double m_y = 0;
      //double m_z = 0;
   };
*/

   // ========================================================================
   struct MouseEvent
   // ========================================================================
   {
      //! Masks for mouse button states
      enum EFlags : uint32_t
      {
         None       = 0,
         BtnLeft    = 1,
         BtnRight   = 1 << 1,
         BtnMiddle  = 1 << 2,
         BtnExtra1  = 1 << 3,   // A mouse with 4 buttons the developer never had.
         BtnExtra2  = 1 << 4,   // A mouse with 4 buttons the developer never had.
         BtnExtra3  = 1 << 5,   // A mouse with 4 buttons the developer never had.
         BtnExtra4  = 1 << 6,   // A mouse with 4 buttons the developer never had.
         BtnExtra5  = 1 << 7,   // A mouse with 4 buttons the developer never had.
         BtnExtra6  = 1 << 8,   // A mouse with 4 buttons the developer never had.
         BtnExtra7  = 1 << 9,   // A mouse with 4 buttons the developer never had.
         BtnExtra8  = 1 << 10,   // A mouse with 4 buttons the developer never had.
         BtnExtra9  = 1 << 11,   // A mouse with 5+ buttons the developer never had.
         BtnExtra10 = 1 << 12,   // A mouse with 5+ buttons the developer never had.
         BtnExtra11 = 1 << 13,   // A mouse with 5+ buttons the developer never had.
         BtnExtra12 = 1 << 14,   // A mouse with 5+ buttons the developer never had.
         BtnExtra13 = 1 << 15,   // A mouse with 5+ buttons the developer never had.
         Pressed = 1 << 16,
         Released= 1 << 17,
         Moved   = 1 << 18,
         Wheel   = 1 << 19,
         DoubleClick= 1 << 20,
         TripleClick= 1 << 21,
         WithShift  = 1 << 22, // True if SHIFT was also pressed
         WithCtrl   = 1 << 23, // True if CTRL was also pressed
         WithAlt    = 1 << 24,  // True if ALT was also pressed
         WithAltGr  = 1 << 25,// True if Alt-Gr was also pressed
         WithSuper  = 1 << 26,// True if WinKey/Super1Key was also pressed
         FORCE_32_BIT = 0x7fffffff
      };

      MouseEvent()
         : m_flags( 0 )
         , m_x(0)
         , m_y(0)
         , m_wheelX( 0.0f )
         , m_wheelY( 0.0f )
      {}

      bool isMoveEvent() const { return m_flags & Moved; }
      bool isPressEvent() const { return m_flags & Pressed; }
      bool isWheelEvent() const { return m_flags & Wheel; }

      bool isLeftPressed() const { return m_flags & ( BtnLeft | Pressed ); }
      bool isRightPressed() const { return m_flags & ( BtnRight | Pressed ); }
      bool isMiddlePressed() const { return m_flags & ( BtnMiddle | Pressed ); }

      bool isLeftButton() const { return m_flags & BtnLeft; }
      bool isRightButton() const { return m_flags & BtnRight; }
      bool isMiddleButton() const { return m_flags & BtnMiddle; }

      int x() const { return m_x; }
      int y() const { return m_y; }
      float wheelX() const { return m_wheelX; }
      float wheelY() const { return m_wheelY; }

      int getButton() const
      {
         int btn = 0;
         btn += ( isLeftButton() ) ? 1 : 0;
         btn += ( isRightButton() ) ? 2 : 0;
         btn += ( isMiddleButton() ) ? 3 : 0;
         return btn;
      }

      std::string toString() const
      {
         std::stringstream s;
         s << "flags(" << m_flags << "), "
            "button(" << getButton() << "), "
            "pos(" << m_x << "," << m_y << "), "
            "wheel(" << m_wheelX << "," << m_wheelX << "), "
            "shift(" << (m_flags & WithShift) << "), "
            "ctrl(" << (m_flags & WithCtrl) << ")";
         return s.str();
      }

      uint32_t m_flags;
      int32_t m_x;
      int32_t m_y;
      float m_wheelX;
      float m_wheelY;        //! mouse wheel delta, often 1.0 or -1.0, but can have other values < 0.f or > 0.f;
   };

   // ========================================================================
   struct MouseMultiClicks
   // ========================================================================
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

   // ========================================================================
   struct JoystickEvent
   // ========================================================================
   {
      enum
      {
         NUMBER_OF_BUTTONS = 32,
         AXIS_X = 0, // e.g. analog stick 1 left to right
         AXIS_Y,		// e.g. analog stick 1 top to bottom
         AXIS_Z,		// e.g. throttle, or analog 2 stick 2 left to right
         AXIS_R,		// e.g. rudder, or analog 2 stick 2 top to bottom
         AXIS_U,
         AXIS_V,
         NUMBER_OF_AXES
      };

      /** A bitmap of button states.  You can use IsButtonPressed() to
      ( check the state of each button from 0 to (NUMBER_OF_BUTTONS - 1) */
      uint32_t m_buttonStates;

      /** For AXIS_X, AXIS_Y, AXIS_Z, AXIS_R, AXIS_U and AXIS_V
      * Values are in the range -32768 to 32767, with 0 representing
      * the center position.  You will receive the raw value from the
      * joystick, and so will usually want to implement a dead zone around
      * the center of the range. Axes not supported by this joystick will
      * always have a value of 0. On Linux, POV hats are represented as axes,
      * usually the last two active axis.
      */
      int16_t m_axis[NUMBER_OF_AXES];

      /** The POV represents the angle of the POV hat in degrees * 100,
      * from 0 to 35,900.  A value of 65535 indicates that the POV hat
      * is centered (or not present).
      * This value is only supported on Windows.  On Linux, the POV hat
      * will be sent as 2 axes instead. */
      uint16_t m_pov;

      //! The ID of the joystick which generated this event.
      /** This is an internal Irrlicht index; it does not map directly
      * to any particular hardware joystick. */
      uint8_t m_id;

      //! A helper function to check if a button is pressed.
      bool IsButtonPressed(uint32_t button) const
      {
         if(button >= (uint32_t)NUMBER_OF_BUTTONS)
         return false;

         return (m_buttonStates & (1 << button)) ? true : false;
      }
   };

   // ========================================================================
   struct JoystickInfo
   // ========================================================================
   {
      uint8_t m_id; //! The ID of the joystick
      std::string	m_name; //! The name that the joystick uses to identify itself.
      uint32_t m_buttons;//! The number of buttons that the joystick has.
      uint32_t m_axes;       //! The number of axes that the joystick has, X,Y,Z,R,U,V. POV hat will use two axes. These will be included in this count.
      enum //! An indication of whether the joystick has a POV hat.
      {
         POV_HAT_PRESENT,//! A hat is definitely present.
         POV_HAT_ABSENT, //! A hat is definitely not present.
         POV_HAT_UNKNOWN //! The presence or absence of a hat cannot be determined. On Linux default! POV_HAT_UNKNOWN
      } m_povHat;
   };

/*
   // ========================================================================
   struct LogLevel
   // ========================================================================
   {
      enum ELogLevel { None=0, Info=1, Warn=2, Error=3, Debug=4, Trace=5, Max = 6, Default=None };
      operator ELogLevel() const { return m_level; }
      LogLevel() : m_level( None ) {}
      LogLevel( ELogLevel level ) : m_level(level) {}
      LogLevel( LogLevel other ) : m_level(other.m_level) {}
      enum ELogLevel m_level;
   };
*/
   // ========================================================================
   struct LogEvent
   // ========================================================================
   {
      std::string m_text;
      LogLevel m_level;
   };


   // ========================================================================
   struct UserEvent
   // ========================================================================
   {
      void* userData;
      uint32_t userData1;
      uint32_t userData2;
   };

   // ========================================================================
   struct SEvent
   // ========================================================================
   {
      EventType type;
      union
      {
         struct ResizeEvent resizeEvent;
         struct GuiEvent guiEvent;
         struct MouseEvent mouseEvent;
         struct KeyEvent keyEvent;
         struct JoystickEvent joystickEvent;
         struct LogEvent logEvent;
         struct UserEvent userEvent;
      };

      DE_CREATE_LOGGER("de.SEvent")

      static void dump()
      {
         DE_DEBUG("sizeof(SEvent) = ",sizeof(SEvent))
         DE_DEBUG("sizeof(ResizeEvent) = ",sizeof(ResizeEvent))
         DE_DEBUG("sizeof(GuiEvent) = ",sizeof(GuiEvent))
         DE_DEBUG("sizeof(MouseEvent) = ",sizeof(MouseEvent))
         DE_DEBUG("sizeof(KeyEvent) = ",sizeof(KeyEvent))
         DE_DEBUG("sizeof(JoystickEvent) = ",sizeof(JoystickEvent))
         DE_DEBUG("sizeof(LogEvent) = ",sizeof(LogEvent))
         DE_DEBUG("sizeof(UserEvent) = ",sizeof(UserEvent))
      }

      ~SEvent() {}
      SEvent()                      : type( EventType::USER ), userEvent() {}
      SEvent( KeyEvent event )      : type( EventType::KEYBOARD ), keyEvent( event ) {}
      SEvent( MouseEvent event )    : type( EventType::MOUSE ), mouseEvent( event ) {}
      SEvent( UserEvent event )     : type( EventType::USER ), userEvent( event ) {}
      SEvent( ResizeEvent event )   : type( EventType::RESIZE ), resizeEvent( event ) {}
      SEvent( JoystickEvent event ) : type( EventType::JOYSTICK ), joystickEvent( event ) {}
   };

   // ========================================================================
   struct IEventReceiver
   // ========================================================================
   {
      virtual ~IEventReceiver() {}

      virtual void onEvent( SEvent const & event ) = 0;

      //virtual bool isEventProcessingEnabled() const { return true; }
      //virtual void setEventProcessingEnabled( bool enable ) {}
   };

/*
   // =======================================================================
   struct IEventReceiver
   // =======================================================================
   {
      virtual ~IEventReceiver() {}

      virtual void paintEvent( PaintEvent event ) {}
      virtual void resizeEvent( ResizeEvent event ) {}

      virtual void keyPressEvent( KeyEvent event ) {}
      virtual void keyReleaseEvent( KeyEvent event ) {}

      virtual void mouseMoveEvent( MouseMoveEvent event ) {}
      virtual void mousePressEvent( MousePressEvent event ) {}
      virtual void mouseReleaseEvent( MouseReleaseEvent event ) {}
      virtual void mouseWheelEvent( MouseWheelEvent event ) {}

      virtual void showEvent( ShowEvent event ) {}
      virtual void hideEvent( HideEvent event ) {}

      virtual void enterEvent( EnterEvent event ) {}
      virtual void leaveEvent( LeaveEvent event ) {}

      virtual void focusInEvent( FocusEvent event ) {}
      virtual void focusOutEvent( FocusEvent event ) {}

      virtual void joystickEvent( JoystickEvent event ) {}
   };
*/

} // end namespace de.

