//////////////////////////////////////////////////////////////////////////////
/// @file EventReceiver.hpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <sstream>

/*
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

// #include <DarkImage.hpp>
*/

namespace de {

   // ========================================================================
   inline bool isMouseOver( int mx, int my, int x1, int y1, int x2, int y2 )
   // ========================================================================
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
   enum class SEventType
   // ========================================================================
   {
      LOG = 0,       //! A log event
      RESIZE,        //! An event of the (underlying) native window.
      WINDOW_MOVE,   //! An event of the (underlying) native window.
      //WINDOW_AREA,   //! WindowRect + ClientRect -> retrieve FrameBuffer.wh = ClientRect.wh.
      //WINDOW_RESIZE, //! An event of the (underlying) native window.
      //WINDOW_MOVE,
      //KEYBOARD,    //! A key input event.
      KEY_PRESS,     //! A key input event.
      KEY_RELEASE,   //! A key input event.
      //MOUSE,       //! A mouse input event.
      MOUSE_PRESS,   //! A mouse input event.
      MOUSE_RELEASE, //! A mouse input event.
      MOUSE_MOVE,    //! A mouse input event.
      MOUSE_WHEEL,   //! A mouse input event.
      JOYSTICK,      //! A joystick (joypad, gamepad) input event.
      GUI,           //! An event of the hw accelerated graphical user interface.
      USER,          //! A user event with user data.
      MAX
   };

   inline std::string
   getEventTypeStr( SEventType const eventType )
   {
      switch ( eventType )
      {
         case SEventType::LOG: return "LOG";
         case SEventType::RESIZE: return "RESIZE";
         case SEventType::WINDOW_MOVE: return "WINDOW_MOVE";
         case SEventType::KEY_PRESS: return "KEY_PRESS";
         case SEventType::KEY_RELEASE: return "KEY_RELEASE";
         case SEventType::MOUSE_PRESS: return "MOUSE_PRESS";
         case SEventType::MOUSE_RELEASE: return "MOUSE_RELEASE";
         case SEventType::MOUSE_MOVE: return "MOUSE_MOVE";
         case SEventType::MOUSE_WHEEL: return "MOUSE_WHEEL";
         case SEventType::JOYSTICK: return "JOYSTICK";
         case SEventType::GUI: return "GUI";
         case SEventType::USER: return "USER";
         default: return "N/A";
      }
   }

   // ========================================================================
   struct ResizeEvent
   // ========================================================================
   {
      int w = 0;
      int h = 0;
      //Recti m_rect;
      //Recti m_windowRect;
      //Recti m_clientRect;
      //void* m_userData = nullptr;

      std::string toString() const
      {
         std::ostringstream s;
         s << w << "," << h;
         return s.str();
      }
   };

   // ========================================================================
   struct WindowMoveEvent
   // ========================================================================
   {
      int x = 0;
      int y = 0;

      std::string toString() const
      {
         std::ostringstream s;
         s << x << "," << y;
         return s.str();
      }
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

      std::string toString() const
      {
         std::stringstream s;
         s << "type(" << int(m_eventType) << "), caller(" << m_caller << ")";
         return s.str();
      }
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

   // Enumerates all keys on keyboard. So the physical buttons only.
   // Does not represent composed chars like @ which needs 2 buttons to be pressed at once.
   // These composed characters should be interpreted in language dependent functions.
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

      // Function Keys
      KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
      KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
      KEY_F13, KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18,
      KEY_F19, KEY_F20, KEY_F21, KEY_F22, KEY_F23, KEY_F24,

      // ControlBlock 3x
      KEY_SNAPSHOT,     // PRINT SCREEN, there is another PRINT only key, d.k. where.
      KEY_SCROLL_LOCK,  // SCROLL LOCK key
      KEY_PAUSE,

      // AlphaNum
      KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
      KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
      KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
      KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P,
      KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
      KEY_Y, KEY_Z,

      // KeyPad/Numpad
      KEY_KP_0, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4,
      KEY_KP_5, KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9,
      KEY_KP_DIVIDE,
      KEY_KP_MULTIPLY,
      KEY_KP_SUBTRACT,
      KEY_KP_ADD,
      KEY_KP_DECIMAL,
      KEY_KP_ENTER,
      //KEY_KP_EQUAL    // ? TODO: remove

      // OEM keys: All German Umlaute so far and special characters
      // I dont own any non GERMAN keyboard. Please add your special chars if needed.
      KEY_OEM_1,        // for GERMAN = Ü, US = ? ;:
      KEY_OEM_PLUS,     // for GERMAN = +*~, US = ?
      KEY_OEM_COMMA,    // for GERMAN = ,; US = ?
      KEY_OEM_MINUS,    // for GERMAN = -_ US = ?
      KEY_OEM_PERIOD,   // for GERMAN = .: US = ?
      KEY_OEM_2,        // for GERMAN = #' for US = ?
      KEY_OEM_3,        // for GERMAN = Ö for US = ? `~
      KEY_OEM_4,        // for GERMAN = ß?\ for US = ? [{
      KEY_OEM_5,        // for GERMAN = ^° for US = ? \|
      KEY_OEM_6,        // for GERMAN = ´` for US = ? ]}
      KEY_OEM_7,        // for GERMAN = Ä for US = ? '"
      //KEY_OEM_8,      // for ?
      //KEY_OEM_AX,     // for Japan "AX"
      KEY_OEM_102,      // for GERMAN <|>, 226 = 0xE2

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

   /*
      // Special characters
      // TODO: Rework all these. Are they necessary? They depend on modifiers like Shift, Alt, etc...
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
      //KEY_WORLD_1     // 161 non-US #1
      //KEY_WORLD_2     // 162 non-US #2
      //KEY_MENU        // 348

      // EXTRA
      KEY_SELECT,
      KEY_EXECUT,
      KEY_SNAPSHOT,
      KEY_HELP,
      KEY_APPS,
      KEY_SLEEP,
   */

      EKEY_COUNT // this is not a key, but the amount of keycodes there are.
   };


   // ========================================================================
   inline char const *
   EKEY_to_String( EKEY key )
   // ========================================================================
   {
      switch (key)
      {
      case KEY_ESCAPE: return "KEY_ESCAPE";
      case KEY_ENTER: return "KEY_ENTER";
      case KEY_SPACE: return "KEY_SPACE";
      case KEY_BACKSPACE: return "KEY_BACKSPACE";
      case KEY_TAB: return "KEY_TAB";
      // Arrow keys
      case KEY_LEFT: return "KEY_LEFT";
      case KEY_UP: return "KEY_UP";
      case KEY_RIGHT: return "KEY_RIGHT";
      case KEY_DOWN: return "KEY_DOWN";

      case KEY_LEFT_SHIFT: return "KEY_LEFT_SHIFT";
      case KEY_RIGHT_SHIFT: return "KEY_RIGHT_SHIFT";
      case KEY_LEFT_ALT: return "KEY_LEFT_ALT";
      case KEY_RIGHT_ALT: return "KEY_RIGHT_ALT";
      // ALTGR?
      case KEY_LEFT_CTRL: return "KEY_LEFT_CTRL";
      case KEY_RIGHT_CTRL: return "KEY_RIGHT_CTRL";
      case KEY_LEFT_SUPER: return "KEY_LEFT_SUPER";
      case KEY_RIGHT_SUPER: return "KEY_RIGHT_SUPER";
      case KEY_CAPS_LOCK: return "KEY_CAPS_LOCK";
      case KEY_NUM_LOCK: return "KEY_NUM_LOCK";

      // ControlBlock 6x
      case KEY_INSERT: return "KEY_INSERT";
      case KEY_DELETE: return "KEY_DELETE";
      case KEY_HOME: return "KEY_HOME";
      case KEY_END: return "KEY_END";
      case KEY_PAGE_UP: return "KEY_PAGE_UP";
      case KEY_PAGE_DOWN: return "KEY_PAGE_DOWN";

      // Function Keys
      case KEY_F1: return "KEY_F1";
      case KEY_F2: return "KEY_F2";
      case KEY_F3: return "KEY_F3";
      case KEY_F4: return "KEY_F4";
      case KEY_F5: return "KEY_F5";
      case KEY_F6: return "KEY_F6";
      case KEY_F7: return "KEY_F7";
      case KEY_F8: return "KEY_F8";
      case KEY_F9: return "KEY_F9";
      case KEY_F10: return "KEY_F10"; // Not working on my kb Logitech K280e, d.k. the fuck why.
      case KEY_F11: return "KEY_F11";
      case KEY_F12: return "KEY_F12";
      case KEY_F13: return "KEY_F13";
      case KEY_F14: return "KEY_F14";
      case KEY_F15: return "KEY_F15";
      case KEY_F16: return "KEY_F16";
      case KEY_F17: return "KEY_F17";
      case KEY_F18: return "KEY_F18";
      case KEY_F19: return "KEY_F19";
      case KEY_F20: return "KEY_F20";
      case KEY_F21: return "KEY_F21";
      case KEY_F22: return "KEY_F22";
      case KEY_F23: return "KEY_F23";
      case KEY_F24: return "KEY_F24";

      // ControlBlock 3x
      case KEY_SNAPSHOT: return "KEY_SNAPSHOT";
      case KEY_SCROLL_LOCK: return "KEY_SCROLL_LOCK";
      case KEY_PAUSE: return "KEY_PAUSE";

      // AlphaNum
      case KEY_0: return "KEY_0";
      case KEY_1: return "KEY_1";
      case KEY_2: return "KEY_2";
      case KEY_3: return "KEY_3";
      case KEY_4: return "KEY_4";
      case KEY_5: return "KEY_5";
      case KEY_6: return "KEY_6";
      case KEY_7: return "KEY_7";
      case KEY_8: return "KEY_8";
      case KEY_9: return "KEY_9";

      case KEY_A: return "KEY_A";
      case KEY_B: return "KEY_B";
      case KEY_C: return "KEY_C";
      case KEY_D: return "KEY_D";
      case KEY_E: return "KEY_E";
      case KEY_F: return "KEY_F";
      case KEY_G: return "KEY_G";
      case KEY_H: return "KEY_H";
      case KEY_I: return "KEY_I";
      case KEY_J: return "KEY_J";
      case KEY_K: return "KEY_K";
      case KEY_L: return "KEY_L";
      case KEY_M: return "KEY_M";
      case KEY_N: return "KEY_N";
      case KEY_O: return "KEY_O";
      case KEY_P: return "KEY_P";
      case KEY_Q: return "KEY_Q";
      case KEY_R: return "KEY_R";
      case KEY_S: return "KEY_S";
      case KEY_T: return "KEY_T";
      case KEY_U: return "KEY_U";
      case KEY_V: return "KEY_V";
      case KEY_W: return "KEY_W";
      case KEY_X: return "KEY_X";
      case KEY_Y: return "KEY_Y";
      case KEY_Z: return "KEY_Z";

      // KeyPad/Numpad
      case KEY_KP_0: return "KEY_KP_0";
      case KEY_KP_1: return "KEY_KP_1";
      case KEY_KP_2: return "KEY_KP_2";
      case KEY_KP_3: return "KEY_KP_3";
      case KEY_KP_4: return "KEY_KP_4";
      case KEY_KP_5: return "KEY_KP_5";
      case KEY_KP_6: return "KEY_KP_6";
      case KEY_KP_7: return "KEY_KP_7";
      case KEY_KP_8: return "KEY_KP_8";
      case KEY_KP_9: return "KEY_KP_9";
      case KEY_KP_DIVIDE: return "KEY_KP_DIVIDE";
      case KEY_KP_MULTIPLY: return "KEY_KP_MULTIPLY";
      case KEY_KP_SUBTRACT: return "KEY_KP_SUBTRACT";
      case KEY_KP_ADD: return "KEY_KP_ADD";
      case KEY_KP_DECIMAL: return "KEY_KP_DECIMAL";
      case KEY_KP_ENTER: return "KEY_KP_ENTER";

      // OEM keys: All German Umlaute so far and special characters
      // I dont own any non GERMAN keyboard. Please add your special chars if needed.
      case KEY_OEM_1: return "KEY_OEM_1";        // for GERMAN = Ü, US = ? ;:
      case KEY_OEM_PLUS: return "KEY_OEM_PLUS";     // for GERMAN = +*~, US = ?
      case KEY_OEM_COMMA: return "KEY_OEM_COMMA";    // for GERMAN = ,; US = ?
      case KEY_OEM_MINUS: return "KEY_OEM_MINUS";    // for GERMAN = -_ US = ?
      case KEY_OEM_PERIOD: return "KEY_OEM_PERIOD";   // for GERMAN = .: US = ?
      case KEY_OEM_2: return "KEY_OEM_2";        // for GERMAN = #' for US = ?
      case KEY_OEM_3: return "KEY_OEM_3";        // for GERMAN = Ö for US = ? `~
      case KEY_OEM_4: return "KEY_OEM_4";        // for GERMAN = ß?\ for US = ? [{
      case KEY_OEM_5: return "KEY_OEM_5";        // for GERMAN = ^° for US = ? \|
      case KEY_OEM_6: return "KEY_OEM_6";        // for GERMAN = ´` for US = ? ]}
      case KEY_OEM_7: return "KEY_OEM_7";        // for GERMAN = Ä for US = ? '"
      //case KEY_OEM_8,      // for ?
      //case KEY_OEM_AX,     // for Japan "AX"
      case KEY_OEM_102: return "KEY_OEM_102";      // for GERMAN <|>, 226 = 0xE2

//      case KEY_OEM_1: return "KEY_OEM_1";  // for US    ";:"
//      case KEY_OEM_2: return "";  // for US    "/?"
//      case KEY_OEM_3: return "";  // for US    "`~"
//      case KEY_OEM_4: return "";  // for US    "[{"
//      case KEY_OEM_5: return "";  // for US    "\|"
//      case KEY_OEM_6: return "";  // for US    "]}"
//      case KEY_OEM_7: return "";  // for US    "'""
//      case KEY_OEM_8: return "";  // None
//      case KEY_OEM_AX: return "";  // for Japan "AX"
//      case KEY_OEM_102: return "";  // "<>" or "\|"

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

/*
      // Special characters
      case KEY_BACKSLASH: return "KEY_BACKSLASH";    // '\\'
      case KEY_PLUS: return "KEY_PLUS";         // '+'
      case KEY_COMMA: return "KEY_COMMA";        // ','
      case KEY_MINUS: return "KEY_MINUS";        // '-'
      case KEY_PERIOD: return "KEY_PERIOD";       // '.'
      case KEY_SLASH: return "KEY_SLASH";        // '/'
      case KEY_COLON: return "KEY_COLON";        // ':'
      case KEY_APOSTROPHE: return "KEY_APOSTROPHE";   // '''
      case KEY_SEMICOLON: return "KEY_SEMICOLON";    // ';'
      case KEY_EQUAL: return "KEY_EQUAL";        // '='
      case KEY_LEFT_BRACKET: return "KEY_LEFT_BRACKET"; // '['
      case KEY_RIGHT_BRACKET: return "KEY_RIGHT_BRACKET";// ']'
      case KEY_GRAVE_ACCENT: return "KEY_GRAVE_ACCENT"; // '`' 96
      case KEY_LOWERTHEN: return "KEY_LOWERTHEN";    // '<'
      case KEY_GREATERTHEN: return "KEY_GREATERTHEN";  // '>'
      case KEY_SEPARATOR: return "KEY_SEPARATOR";    // '|'
      //KEY_WORLD_1  // 161 non-US #1
      //KEY_WORLD_2  // 162 non-US #2
      //KEY_MENU     // 348

      // EXTRA
      case KEY_SELECT: return "KEY_SELECT";
      case KEY_EXECUT: return "KEY_EXECUT";
      case KEY_SNAPSHOT: return "KEY_SNAPSHOT";
      case KEY_HELP: return "KEY_HELP";
      case KEY_APPS: return "KEY_APPS";
      case KEY_SLEEP: return "KEY_SLEEP";
*/
      default: return "KEY_UNKNOWN";
      }

   };
/*
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
*/
   // ========================================================================
   struct KeyModifier
   // ========================================================================
   {
      enum EModifier : uint16_t
      {
         None = 0,
         Released = 1,
         Pressed  = 1<<1,
         Repeat   = 1<<2,
         Shift = 1<<3,
         Ctrl = 1<<4,
         Alt = 1<<5,
         AltGr = 1<<6,
         Super = 1<<7,
         Fn = 1<<8,
         CapsLock = 1<<9,
         NumLock = 1<<10,
         EModifierCount = 11
      };

      KeyModifier() : m_flags(None) {}
      KeyModifier( uint16_t flags ) : m_flags(flags) {}
      operator uint16_t() const { return m_flags; }

      uint16_t m_flags;
   };

   // ========================================================================
   struct KeyPressEvent
   // ========================================================================
   {
      uint32_t unicode; // For unknown enum value, corresponds to the key (0, if not a character)
      uint32_t key;     // recognized enum value from EKEY
      uint32_t modifiers;
      uint32_t scancode;// Platform dependent scan key code

      KeyPressEvent()
         : unicode( 0 )
         , key( EKEY_COUNT )
         , modifiers( KeyModifier::None )
         , scancode(0)
      {}

      bool shift() const { return modifiers & KeyModifier::Shift; }
      bool ctrl() const { return modifiers & KeyModifier::Ctrl; }
      bool alt() const { return modifiers & KeyModifier::Alt; }
      bool altgr() const { return modifiers & KeyModifier::AltGr; }
      bool win() const { return modifiers & KeyModifier::Super; }
      bool fn() const { return modifiers & KeyModifier::Fn; }

      //      void setUnicode( uint32_t unicode ) { this->unicode = unicode; }
      //      void setKey( EKEY key ) { this->key = key; }
      //      void setModifer( uint32_t modifier ) { this->modifiers = modifier & 0xFF; }

      //      uint32_t unicode() const { return Unicode; }
      //      EKEY key() const { return Key; }
      //      uint32_t modifers() const { return m_modifiers; }

      std::wstring toWString() const
      {
         std::wostringstream o; o <<
         "Key(" << EKEY_to_String( EKEY(key) ) << "), "
         "WChar(" << wchar_t(unicode) << "), "
         "Unicode(" << unicode << "), "
         "Modifiers(" << modifiers << "), "
         "ScanCode(" << scancode << ")";
         return o.str();
      }

      std::string toString() const
      {
         std::ostringstream o; o <<
         "Key(" << EKEY_to_String( EKEY(key) ) << "), "
         "Char(" << char(unicode) << "), "
         "Unicode(" << unicode << "), "
         "Modifiers(" << modifiers << "), "
         "ScanCode(" << scancode << ")";
         return o.str();
      }
   };

   // ========================================================================
   struct KeyReleaseEvent
   // ========================================================================
   {
      uint32_t unicode; // For unknown enum value, corresponds to the key (0, if not a character)
      uint32_t key;     // recognized enum value from EKEY
      uint32_t modifiers;
      uint32_t scancode;// Platform dependent scan key code

      KeyReleaseEvent()
         : unicode( 0 )
         , key( EKEY_COUNT )
         , modifiers( KeyModifier::None )
         , scancode(0)
      {}

      bool shift() const { return modifiers & KeyModifier::Shift; }
      bool ctrl() const { return modifiers & KeyModifier::Ctrl; }
      bool alt() const { return modifiers & KeyModifier::Alt; }
      bool altgr() const { return modifiers & KeyModifier::AltGr; }
      bool win() const { return modifiers & KeyModifier::Super; }
      bool fn() const { return modifiers & KeyModifier::Fn; }

      //      void setUnicode( uint32_t unicode ) { m_unicode = unicode; }
      //      void setKey( EKEY key ) { m_key = key; }
      //      void setModifer( uint32_t modifier ) { m_modifiers = modifier & 0xFF; }

      //      uint32_t unicode() const { return Unicode; }
      //      EKEY key() const { return Key; }
      //      uint32_t modifers() const { return m_modifiers; }

      std::wstring toWString() const
      {
         std::wostringstream o; o <<
         "Key(" << EKEY_to_String( EKEY(key) ) << "), "
         "WChar(" << wchar_t(unicode) << "), "
         "Unicode(" << unicode << "), "
         "Modifiers(" << modifiers << "), "
         "ScanCode(" << scancode << ")";
         return o.str();
      }

      std::string toString() const
      {
         std::ostringstream o; o <<
         "Key(" << EKEY_to_String( EKEY(key) ) << "), "
         "Char(" << char(unicode) << "), "
         "Unicode(" << unicode << "), "
         "Modifiers(" << modifiers << "), "
         "ScanCode(" << scancode << ")";
         return o.str();
      }
   };

/*
   // ========================================================================
   struct KeyEvent
   // ========================================================================
   {
      uint32_t m_unicode; // For unknown enum value, corresponds to the key (0, if not a character)
      uint32_t m_key;     // recognized enum value from EKEY
      uint32_t m_scancode;// Platform dependent scan key code
      uint16_t m_modifiers;
      wchar_t m_wsymbol;
      //char m_csymbol;
      //int32_t m_x;
      //int32_t m_y;

      KeyEvent()
         : m_unicode( 0 )
         , m_key( EKEY_COUNT )
         , m_modifiers( KeyModifier::None )
         , m_wsymbol('?')
         //, m_csymbol('?')
         //, m_x(0)
         //, m_y(0)
      {

      }

      // Modifiers making KeyEvent usable for both keyPress and keyRelease events.
      bool isPressed() const
      {
         return m_modifiers & KeyModifier::Pressed;
      }

      void setPressed( bool pressed )
      {
         if (pressed)
         {
            m_modifiers |= KeyModifier::Pressed;
         }
         else
         {
            m_modifiers &= ~uint32_t(KeyModifier::Pressed);
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
              "Scancode(" << int(m_scancode) << "), Modifiers(" << m_modifiers << ")"
              //", X(" << int(m_x) << "), Y(" << m_y << ")"
              ;
         return s.str();
      }
   };
*/

   // ========================================================================
   struct MouseButton
   // ========================================================================
   {
      enum EButton : uint16_t
      {
         None       = 0,
         Left    = 1,
         Right   = 1 << 1,
         Middle  = 1 << 2,
         Extra1  = 1 << 3,   // A mouse with 4 buttons the developer never had.
         Extra2  = 1 << 4,   // A mouse with 4 buttons the developer never had.
         Extra3  = 1 << 5,   // A mouse with 4 buttons the developer never had.
         Extra4  = 1 << 6,   // A mouse with 4 buttons the developer never had.
         Extra5  = 1 << 7,   // A mouse with 4 buttons the developer never had.
         Extra6  = 1 << 8,   // A mouse with 4 buttons the developer never had.
         Extra7  = 1 << 9,   // A mouse with 4 buttons the developer never had.
         Extra8  = 1 << 10,   // A mouse with 4 buttons the developer never had.
         Extra9  = 1 << 11,   // A mouse with 5+ buttons the developer never had.
         Extra10 = 1 << 12,   // A mouse with 5+ buttons the developer never had.
         Extra11 = 1 << 13,   // A mouse with 5+ buttons the developer never had.
         Extra12 = 1 << 14,   // A mouse with 5+ buttons the developer never had.
         Extra13 = 1 << 15,   // A mouse with 5+ buttons the developer never had.
      };
   };

   // ========================================================================
   struct MouseFlag
   // ========================================================================
   {
      enum EFlag : uint16_t
      {
         None        = 0,
         Pressed     = 1 << 1,
         Released    = 1 << 2,
         Moved       = 1 << 3,
         Wheel       = 1 << 4,
         DoubleClick = 1 << 5,
         TripleClick = 1 << 6,
         WithShift   = 1 << 7, // True if SHIFT was also pressed
         WithCtrl    = 1 << 8, // True if CTRL was also pressed
         WithAlt     = 1 << 9,  // True if ALT was also pressed
         WithAltGr   = 1 << 10,// True if Alt-Gr was also pressed
         WithSuper   = 1 << 11,// True if WinKey/Super1Key was also pressed
         Repeated    = 1 << 12
      };
   };

   // =======================================================================
   struct MousePressEvent // MouseClickEvent
   // =======================================================================
   {
      int32_t x = 0;
      int32_t y = 0;
      uint16_t buttons = 0; // Combination of MouseButton::EButton flags
      uint16_t flags = 0;   // Combination of MouseFlag::EFlag flags

      uint32_t buttonCount() const
      {
         uint32_t n = 0;
         if ( isLeft() ) n++;
         if ( isRight() ) n++;
         if ( isMiddle() ) n++;
         return n;
      }

      bool isLeft() const { return buttons & MouseButton::Left; }
      bool isRight() const { return buttons & MouseButton::Right; }
      bool isMiddle() const { return buttons & MouseButton::Middle; }

      std::string toString() const
      {
         std::stringstream s;
         s << "x(" << x << "), y(" << y << "), ";
         if ( isLeft() ) s << ", LEFT";
         if ( isRight() ) s << ", RIGHT";
         if ( isMiddle() ) s << ", MIDDLE";
         s << ", flags(" << flags << ")";
         return s.str();
      }

   };

   // =======================================================================
   struct MouseReleaseEvent // MouseClickEvent
   // =======================================================================
   {
      int32_t x = 0;
      int32_t y = 0;
      uint16_t buttons = 0; // Combination of MouseButton::EButton flags
      uint16_t flags = 0;   // Combination of MouseFlag::EFlag flags

      bool isLeft() const { return buttons & MouseButton::Left; }
      bool isRight() const { return buttons & MouseButton::Right; }
      bool isMiddle() const { return buttons & MouseButton::Middle; }


      std::string toString() const
      {
         std::stringstream s;
         s << "x(" << x << "), y(" << y << "), ";
         if ( isLeft() ) s << ", LEFT";
         if ( isRight() ) s << ", RIGHT";
         if ( isMiddle() ) s << ", MIDDLE";
         s << ", flags(" << flags << ")";
         return s.str();
      }
   };

   // =======================================================================
   struct MouseMoveEvent // MoveEvent
   // =======================================================================
   {
      int32_t x = 0;
      int32_t y = 0;

      std::string toString() const
      {
         std::stringstream s;
         s << "x(" << x << "), y(" << y << ")";
         return s.str();
      }
   };

   // =======================================================================
   struct MouseWheelEvent // ScrollEvent
   // =======================================================================
   {
      float x = 0.f;
      float y = 0.f;

      std::string toString() const
      {
         std::stringstream s;
         s << "x(" << x << "), y(" << y << ")";
         return s.str();
      }
   };

/*
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
         Repeated = 1 << 27,
         FORCE_32_BIT = 0x7fffffff
      };

      MouseEvent()
         : m_flags( 0 )
         , m_x(0)
         , m_y(0)
         , m_wheelX( 0.0f )
         , m_wheelY( 0.0f )
      {}

      bool isMoved() const { return m_flags & Moved; }
      bool isWheeled() const { return m_flags & Wheel; }
      bool isPressed() const { return m_flags & Pressed; }
      bool isReleased() const { return m_flags & Released; }
      bool isRepeated() const { return m_flags & Repeated; }

      bool isLeftPressEvent() const { return m_flags & ( BtnLeft | Pressed ); }
      bool isRightPressEvent() const { return m_flags & ( BtnRight | Pressed ); }
      bool isMiddlePressEvent() const { return m_flags & ( BtnMiddle | Pressed ); }

      bool isLeftReleaseEvent() const { return isLeftButton() && !isPressed(); }
      bool isRightReleaseEvent() const { return isRightButton() && !isPressed(); }
      bool isMiddleReleaseEvent() const { return isMiddleButton() && !isPressed(); }

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
*/

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

      JoystickEvent()
         : m_buttonStates(0)
         , m_pov(0)
         , m_id(0)
      {}

      bool isButtonPressed(uint32_t button) const
      {
         if(button >= NUMBER_OF_BUTTONS) return false;
         return (m_buttonStates & (1 << button)) ? true : false;
      }

      std::string toString() const
      {
         std::stringstream s;
         s << m_buttonStates << "," << m_pov << "," << m_id;
         return s.str();
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

      std::string toString() const
      {
         std::stringstream s;
         s << int(m_id) << "," << m_name << "," << m_buttons << "," << m_axes << "," << int(m_povHat);
         return s.str();
      }
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
      int m_logLevel; // LogLevel

      std::string toString() const
      {
         std::stringstream s;
         s << int(m_logLevel) << "," << m_text;
         return s.str();
      }
   };


   // ========================================================================
   struct UserEvent
   // ========================================================================
   {
      void* userData;
      uint32_t userData1;
      uint32_t userData2;

      std::string toString() const
      {
         std::stringstream s;
         s << userData << "," << userData1 << "," << userData2;
         return s.str();
      }
   };

   // ========================================================================
   struct SEvent
   // ========================================================================
   {
      SEventType type;
      union
      {
         struct ResizeEvent resizeEvent;
         struct WindowMoveEvent windowMoveEvent;
         struct KeyPressEvent keyPressEvent;
         struct KeyReleaseEvent keyReleaseEvent;
         //struct MouseEvent mouseEvent;
         struct MousePressEvent mousePressEvent;
         struct MouseReleaseEvent mouseReleaseEvent;
         struct MouseMoveEvent mouseMoveEvent;
         struct MouseWheelEvent mouseWheelEvent;
         struct JoystickEvent joystickEvent;
         struct LogEvent logEvent;
         struct UserEvent userEvent;
         struct GuiEvent guiEvent;
      };

      /*
      DE_CREATE_LOGGER("de.SEvent")

      static void dump()
      {
         DE_DEBUG("sizeof(SEvent) = ",sizeof(SEvent))
         DE_DEBUG("sizeof(ResizeEvent) = ",sizeof(ResizeEvent))
         DE_DEBUG("sizeof(GuiEvent) = ",sizeof(GuiEvent))
         //DE_DEBUG("sizeof(MouseEvent) = ",sizeof(MouseEvent))
         DE_DEBUG("sizeof(MousePressEvent) = ",sizeof(MousePressEvent))
         DE_DEBUG("sizeof(MouseReleaseEvent) = ",sizeof(MouseReleaseEvent))
         DE_DEBUG("sizeof(MouseMoveEvent) = ",sizeof(MouseMoveEvent))
         DE_DEBUG("sizeof(MouseWheelEvent) = ",sizeof(MouseWheelEvent))
         //DE_DEBUG("sizeof(KeyEvent) = ",sizeof(KeyEvent))
         DE_DEBUG("sizeof(KeyPressEvent) = ",sizeof(KeyPressEvent))
         DE_DEBUG("sizeof(KeyReleaseEvent) = ",sizeof(KeyReleaseEvent))
         DE_DEBUG("sizeof(JoystickEvent) = ",sizeof(JoystickEvent))
         DE_DEBUG("sizeof(LogEvent) = ",sizeof(LogEvent))
         DE_DEBUG("sizeof(UserEvent) = ",sizeof(UserEvent))
      }

      */

      std::string toString() const
      {
         std::ostringstream o;
         o << getEventTypeStr( type ) << " ";

         switch ( type )
         {
         case SEventType::LOG: o << logEvent.toString(); break;
         case SEventType::RESIZE: o << resizeEvent.toString(); break;
         case SEventType::WINDOW_MOVE: o << windowMoveEvent.toString(); break;
         case SEventType::KEY_PRESS: o << keyPressEvent.toString(); break;
         case SEventType::KEY_RELEASE: o << keyReleaseEvent.toString(); break;
         case SEventType::MOUSE_PRESS: o << mousePressEvent.toString(); break;
         case SEventType::MOUSE_RELEASE: o << mouseReleaseEvent.toString(); break;
         case SEventType::MOUSE_MOVE: o << mouseMoveEvent.toString(); break;
         case SEventType::MOUSE_WHEEL: o << mouseWheelEvent.toString(); break;
         case SEventType::JOYSTICK: o << joystickEvent.toString(); break;
         case SEventType::GUI: o << guiEvent.toString(); break;
         case SEventType::USER: o << userEvent.toString(); break;
         default: break;
         }
         return o.str();
      }

      // Default ctr
      SEvent() : type(SEventType::USER), userEvent() {}
      ~SEvent() {}
      SEvent(ResizeEvent event) : type(SEventType::RESIZE), resizeEvent(event) {}
      SEvent(WindowMoveEvent event) : type(SEventType::WINDOW_MOVE), windowMoveEvent(event) {}
      SEvent(KeyPressEvent event) : type(SEventType::KEY_PRESS), keyPressEvent(event) {}
      SEvent(KeyReleaseEvent event) : type(SEventType::KEY_RELEASE), keyReleaseEvent(event) {}
      SEvent(MousePressEvent event) : type(SEventType::MOUSE_PRESS), mousePressEvent(event) {}
      SEvent(MouseReleaseEvent event) : type(SEventType::MOUSE_RELEASE), mouseReleaseEvent(event) {}
      SEvent(MouseMoveEvent event) : type(SEventType::MOUSE_MOVE), mouseMoveEvent(event) {}
      SEvent(MouseWheelEvent event) : type(SEventType::MOUSE_WHEEL), mouseWheelEvent(event) {}
      SEvent(UserEvent event) : type(SEventType::USER), userEvent(event) {}
      SEvent(JoystickEvent event) : type(SEventType::JOYSTICK), joystickEvent(event) {}
   };

/*
   // ========================================================================
   struct IEventReceiver
   // ========================================================================
   {
      virtual ~IEventReceiver() {}

      virtual void onEvent( SEvent const & event ) = 0;

      //virtual bool isEventProcessingEnabled() const { return true; }
      //virtual void setEventProcessingEnabled( bool enable ) {}
   };
*/

   // =======================================================================
   struct IEventReceiver
   // =======================================================================
   {
      virtual ~IEventReceiver() {}

      //virtual void paintEvent( PaintEvent event ) {}
      virtual void resizeEvent( ResizeEvent const & event ) {}
      virtual void windowMoveEvent( WindowMoveEvent const & event ) {}

      virtual void keyPressEvent( KeyPressEvent const & event ) {}
      virtual void keyReleaseEvent( KeyReleaseEvent const & event ) {}

      virtual void mouseMoveEvent( MouseMoveEvent const & event ) {}
      virtual void mousePressEvent( MousePressEvent const & event ) {}
      virtual void mouseReleaseEvent( MouseReleaseEvent const & event ) {}
      virtual void mouseWheelEvent( MouseWheelEvent const & event ) {}

//      virtual void showEvent( ShowEvent event ) {}
//      virtual void hideEvent( HideEvent event ) {}

//      virtual void enterEvent( EnterEvent event ) {}
//      virtual void leaveEvent( LeaveEvent event ) {}

//      virtual void focusInEvent( FocusEvent event ) {}
//      virtual void focusOutEvent( FocusEvent event ) {}

//      virtual void joystickEvent( JoystickEvent event ) {}
   };


} // end namespace de.

