#include <de_os/de_win32.h>

namespace de {

// [Keyboard] Windows VK_Key enum -> Benni's EKEY enum
EKEY translateWinKey( uint32_t winKey )
{
   switch ( winKey )
   {
      case 48: return KEY_0; // No VK enums for these? Are they all language dependent aswell?
      case 49: return KEY_1;
      case 50: return KEY_2;
      case 51: return KEY_3;
      case 52: return KEY_4;
      case 53: return KEY_5;
      case 54: return KEY_6;
      case 55: return KEY_7;
      case 56: return KEY_8;
      case 57: return KEY_9;

      case 65: return KEY_A; // No VK enums for these? Are they all language dependent aswell?
      case 66: return KEY_B;
      case 67: return KEY_C;
      case 68: return KEY_D;
      case 69: return KEY_E;
      case 70: return KEY_F;
      case 71: return KEY_G;
      case 72: return KEY_H;
      case 73: return KEY_I;
      case 74: return KEY_J;
      case 75: return KEY_K;
      case 76: return KEY_L;
      case 77: return KEY_M;
      case 78: return KEY_N;
      case 79: return KEY_O;
      case 80: return KEY_P;
      case 81: return KEY_Q;
      case 82: return KEY_R;
      case 83: return KEY_S;
      case 84: return KEY_T;
      case 85: return KEY_U;
      case 86: return KEY_V;
      case 87: return KEY_W;
      case 88: return KEY_X;
      case 89: return KEY_Y;
      case 90: return KEY_Z;

      case VK_F1: return KEY_F1;
      case VK_F2: return KEY_F2;
      case VK_F3: return KEY_F3;
      case VK_F4: return KEY_F4;
      case VK_F5: return KEY_F5;
      case VK_F6: return KEY_F6;
      case VK_F7: return KEY_F7;
      case VK_F8: return KEY_F8;
      case VK_F9: return KEY_F9;
      case VK_F10: return KEY_F10; // F10 not working on Logitech K280e. d.k. the fuck why.
      case VK_F11: return KEY_F11;
      case VK_F12: return KEY_F12;
      case VK_F13: return KEY_F13;
      case VK_F14: return KEY_F14;
      case VK_F15: return KEY_F15;
      case VK_F16: return KEY_F16;
      case VK_F17: return KEY_F17;
      case VK_F18: return KEY_F18;
      case VK_F19: return KEY_F19;
      case VK_F20: return KEY_F20;
      case VK_F21: return KEY_F21;
      case VK_F22: return KEY_F22;
      case VK_F23: return KEY_F23;
      case VK_F24: return KEY_F24;

      case VK_ESCAPE: return KEY_ESCAPE;
      case VK_RETURN: return KEY_ENTER;
      case VK_BACK: return KEY_BACKSPACE;
      case VK_SPACE: return KEY_SPACE;
      case VK_TAB: return KEY_TAB;
      case VK_LSHIFT: return KEY_LEFT_SHIFT;
      case VK_RSHIFT: return KEY_RIGHT_SHIFT;

      //case VK_MENU: return KEY_LEFT_SUPER;
      //case VK_LMENU: return KEY_LEFT_SUPER;
      //case VK_RMENU: return KEY_RIGHT_SUPER;

      case VK_LWIN: return KEY_LEFT_SUPER;
      case VK_RWIN: return KEY_RIGHT_SUPER;
      case VK_LCONTROL: return KEY_LEFT_CTRL;
      case VK_RCONTROL: return KEY_RIGHT_CTRL;

//      else if (key == GLFW_KEY_LEFT_ALT ) { key_key = KEY_LEFT_ALT; }
//      else if (key == GLFW_KEY_RIGHT_ALT ) { key_key = KEY_RIGHT_ALT; }
//      else if (key == GLFW_KEY_LEFT_CONTROL ) { key_key = KEY_LEFT_CTRL; }
//      else if (key == GLFW_KEY_RIGHT_CONTROL ) { key_key = KEY_RIGHT_CTRL; }
//      else if (key == GLFW_KEY_LEFT_SUPER ) { key_key = KEY_LEFT_SUPER; }
//      else if (key == GLFW_KEY_RIGHT_SUPER ) { key_key = KEY_RIGHT_SUPER; }
//      else if (key == GLFW_KEY_CAPS_LOCK ) { key_key = KEY_CAPS_LOCK; }

      // OEM special keys: All German Umlaute so far and special characters
      // I dont own US or Japanese keyboards. Please add your special chars if needed.
      case VK_OEM_1: return KEY_OEM_1;            // Ü, VK_OEM_1 = 186 = 0xBA
      case VK_OEM_PLUS: return KEY_OEM_PLUS;      // VK_OEM_PLUS = 187 = 0xBB
      case VK_OEM_COMMA: return KEY_OEM_COMMA;    // VK_OEM_COMMA = 188 = 0xBC
      case VK_OEM_MINUS: return KEY_OEM_MINUS;    // VK_OEM_MINUS = 189 = 0xBD
      case VK_OEM_PERIOD: return KEY_OEM_PERIOD;  // VK_OEM_PERIOD = 190 = 0xBE
      case VK_OEM_2: return KEY_OEM_2;            // VK_OEM_2 = 191 = 0xBF
      case VK_OEM_3: return KEY_OEM_3;            // Ö, VK_OEM_3 = 192 = 0xC0
      case VK_OEM_4: return KEY_OEM_4;            // ß, VK_OEM_4 = 219 = 0xDB
      case VK_OEM_5: return KEY_OEM_5;            // VK_OEM_5 = 220 = 0xDC
      case VK_OEM_6: return KEY_OEM_6;            // VK_OEM_6 = 221 = 0xDD
      case VK_OEM_7: return KEY_OEM_7;            // Ä, VK_OEM_7 = 222 = 0xDE
      case VK_OEM_102: return KEY_OEM_102;        // <|>, VK_OEM_8 = 226 = 0xDE

      // 4x Arrow keys:
      case VK_UP: return KEY_UP;
      case VK_LEFT: return KEY_LEFT;
      case VK_DOWN: return KEY_DOWN;
      case VK_RIGHT: return KEY_RIGHT;

      // 3x control buttons:
      case VK_SNAPSHOT: return KEY_SNAPSHOT; // PRINT SCREEN, not the PRINT (only) key
      case VK_SCROLL: return KEY_SCROLL_LOCK;
      case VK_PAUSE: return KEY_PAUSE;

      // 6x control buttons:
      case VK_INSERT: return KEY_INSERT;
      case VK_HOME: return KEY_HOME;
      case VK_END: return KEY_END;
      case VK_DELETE: return KEY_DELETE;
      case VK_PRIOR: return KEY_PAGE_UP;
      case VK_NEXT: return KEY_PAGE_DOWN;

      // Numpad:
      case VK_NUMLOCK: return KEY_NUM_LOCK;
      case VK_NUMPAD0: return KEY_KP_0;
      case VK_NUMPAD1: return KEY_KP_1;
      case VK_NUMPAD2: return KEY_KP_2;
      case VK_NUMPAD3: return KEY_KP_3;
      case VK_NUMPAD4: return KEY_KP_4;
      case VK_NUMPAD5: return KEY_KP_5;
      case VK_NUMPAD6: return KEY_KP_6;
      case VK_NUMPAD7: return KEY_KP_7;
      case VK_NUMPAD8: return KEY_KP_8;
      case VK_NUMPAD9: return KEY_KP_9;
      case VK_MULTIPLY: return KEY_KP_MULTIPLY; // Is that correct mapping?
      case VK_ADD: return KEY_KP_ADD; // Is that correct mapping?
      //case VK_SEPARATOR: return KEY_KP_SEPARATOR; // Is that correct mapping?
      case VK_SUBTRACT: return KEY_KP_SUBTRACT; // Is that correct mapping?
      case VK_DECIMAL: return KEY_KP_DECIMAL; // Is that correct mapping?
      case VK_DIVIDE: return KEY_KP_DIVIDE; // Is that correct mapping?


      default:
      {
         #ifdef BENNI_USE_COUT
         std::cout << "Cant translate WinKey(" << winKey << ")" << std::endl;
         #endif
         return KEY_UNKNOWN;
      }
   }
}

// [Keyboard] Convert Benni's EKEY to Windows VK_Key [ used in getKeyState() ]

int convert_EKEY_to_WinVK( EKEY ekey )
{
   switch ( ekey )
   {
      case KEY_0: return 48; // No VK enums for these? Are they all language dependent aswell?
      case KEY_1: return 49;
      case KEY_2: return 50;
      case KEY_3: return 51;
      case KEY_4: return 52;
      case KEY_5: return 53;
      case KEY_6: return 54;
      case KEY_7: return 55;
      case KEY_8: return 56;
      case KEY_9: return 57;

      case KEY_A: return 65; // No VK enums for these? Are they all language dependent aswell?
      case KEY_B: return 66;
      case KEY_C: return 67;
      case KEY_D: return 68;
      case KEY_E: return 69;
      case KEY_F: return 70;
      case KEY_G: return 71;
      case KEY_H: return 72;
      case KEY_I: return 73;
      case KEY_J: return 74;
      case KEY_K: return 75;
      case KEY_L: return 76;
      case KEY_M: return 77;
      case KEY_N: return 78;
      case KEY_O: return 79;
      case KEY_P: return 80;
      case KEY_Q: return 81;
      case KEY_R: return 82;
      case KEY_S: return 83;
      case KEY_T: return 84;
      case KEY_U: return 85;
      case KEY_V: return 86;
      case KEY_W: return 87;
      case KEY_X: return 88;
      case KEY_Y: return 89;
      case KEY_Z: return 90;

      case KEY_F1: return VK_F1;
      case KEY_F2: return VK_F2;
      case KEY_F3: return VK_F3;
      case KEY_F4: return VK_F4;
      case KEY_F5: return VK_F5;
      case KEY_F6: return VK_F6;
      case KEY_F7: return VK_F7;
      case KEY_F8: return VK_F8;
      case KEY_F9: return VK_F9;
      case KEY_F10: return VK_F10;
      case KEY_F11: return VK_F11;
      case KEY_F12: return VK_F12;
      case KEY_F13: return VK_F13;
      case KEY_F14: return VK_F14;
      case KEY_F15: return VK_F15;
      case KEY_F16: return VK_F16;
      case KEY_F17: return VK_F17;
      case KEY_F18: return VK_F18;
      case KEY_F19: return VK_F19;
      case KEY_F20: return VK_F20;
      case KEY_F21: return VK_F21;
      case KEY_F22: return VK_F22;
      case KEY_F23: return VK_F23;
      case KEY_F24: return VK_F24;

      case KEY_ESCAPE: return VK_ESCAPE;
      case KEY_ENTER: return VK_RETURN;
      case KEY_BACKSPACE: return VK_BACK;
      case KEY_SPACE: return VK_SPACE;
      case KEY_TAB: return VK_TAB;
      case KEY_LEFT_SHIFT: return VK_LSHIFT;
      case KEY_RIGHT_SHIFT: return VK_RSHIFT;

      case KEY_LEFT_SUPER: return VK_LWIN;
      case KEY_RIGHT_SUPER: return VK_RWIN;
      case KEY_LEFT_CTRL: return VK_LCONTROL;
      case KEY_RIGHT_CTRL: return VK_RCONTROL;

      case KEY_LEFT_ALT: return VK_LMENU;
      case KEY_RIGHT_ALT: return VK_RMENU;

      // OEM special keys: All German Umlaute so far and special characters
      // I dont own US or Japanese keyboards. Please add your special chars if needed.
      case KEY_OEM_1: return VK_OEM_1;            // Ü, VK_OEM_1 = 186 = 0xBA
      case KEY_OEM_PLUS: return VK_OEM_PLUS;      // VK_OEM_PLUS = 187 = 0xBB
      case KEY_OEM_COMMA: return VK_OEM_COMMA;    // VK_OEM_COMMA = 188 = 0xBC
      case KEY_OEM_MINUS: return VK_OEM_MINUS;    // VK_OEM_MINUS = 189 = 0xBD
      case KEY_OEM_PERIOD: return VK_OEM_PERIOD;  // VK_OEM_PERIOD = 190 = 0xBE
      case KEY_OEM_2: return VK_OEM_2;            // VK_OEM_2 = 191 = 0xBF
      case KEY_OEM_3: return VK_OEM_3;            // Ö, VK_OEM_3 = 192 = 0xC0
      case KEY_OEM_4: return VK_OEM_4;            // ß, VK_OEM_4 = 219 = 0xDB
      case KEY_OEM_5: return VK_OEM_5;            // VK_OEM_5 = 220 = 0xDC
      case KEY_OEM_6: return VK_OEM_6;            // VK_OEM_6 = 221 = 0xDD
      case KEY_OEM_7: return VK_OEM_7;            // Ä, VK_OEM_7 = 222 = 0xDE
      case KEY_OEM_102: return VK_OEM_102;        // <|>, VK_OEM_8 = 226 = 0xDE

      // 4x Arrow keys:
      case KEY_UP: return VK_UP;
      case KEY_LEFT: return VK_LEFT;
      case KEY_DOWN: return VK_DOWN;
      case KEY_RIGHT: return VK_RIGHT;

      // 3x control buttons:
      case KEY_SNAPSHOT: return VK_SNAPSHOT; // PRINT SCREEN, not the PRINT (only) key
      case KEY_SCROLL_LOCK: return VK_SCROLL;
      case KEY_PAUSE: return VK_PAUSE;

      // 6x control buttons:
      case KEY_INSERT: return VK_INSERT;
      case KEY_HOME: return VK_HOME;
      case KEY_END: return VK_END;
      case KEY_DELETE: return VK_DELETE;
      case KEY_PAGE_UP: return VK_PRIOR;
      case KEY_PAGE_DOWN: return VK_NEXT;

      // Numpad:
      case KEY_NUM_LOCK: return VK_NUMLOCK;
      case KEY_KP_0: return VK_NUMPAD0;
      case KEY_KP_1: return VK_NUMPAD1;
      case KEY_KP_2: return VK_NUMPAD2;
      case KEY_KP_3: return VK_NUMPAD3;
      case KEY_KP_4: return VK_NUMPAD4;
      case KEY_KP_5: return VK_NUMPAD5;
      case KEY_KP_6: return VK_NUMPAD6;
      case KEY_KP_7: return VK_NUMPAD7;
      case KEY_KP_8: return VK_NUMPAD8;
      case KEY_KP_9: return VK_NUMPAD9;

      case KEY_KP_MULTIPLY: return VK_MULTIPLY; // Is that correct mapping?
      case KEY_KP_ADD: return VK_ADD; // Is that correct mapping?
      //case KEY_KP_SEPARATOR: return VK_SEPARATOR; // Is that correct mapping?
      case KEY_KP_SUBTRACT: return VK_SUBTRACT; // Is that correct mapping?
      case KEY_KP_DECIMAL: return VK_DECIMAL; // Is that correct mapping?
      case KEY_KP_DIVIDE: return VK_DIVIDE; // Is that correct mapping?

      default:
      {
         #ifdef BENNI_USE_COUT
         std::cout << "Cant translate EKEY(" << EKEY_to_String(ekey) << ")" << std::endl;
         #endif
         return 0;
      }
   }
}

// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
uint32_t
convertLocaleIdToCodepage( uint32_t localeId )
{
   switch ( localeId )
   {
      case 1098:  // Telugu
      case 1095:  // Gujarati
      case 1094:  // Punjabi
      case 1103:  // Sanskrit
      case 1111:  // Konkani
      case 1114:  // Syriac
      case 1099:  // Kannada
      case 1102:  // Marathi
      case 1125:  // Divehi
      case 1067:  // Armenian
      case 1081:  // Hindi
      case 1079:  // Georgian
      case 1097:  // Tamil
         return 0;
      case 1054:  // Thai
         return 874;
      case 1041:  // Japanese
         return 932;
      case 2052:  // Chinese (PRC)
      case 4100:  // Chinese (Singapore)
         return 936;
      case 1042:  // Korean
         return 949;
      case 5124:  // Chinese (Macau S.A.R.)
      case 3076:  // Chinese (Hong Kong S.A.R.)
      case 1028:  // Chinese (Taiwan)
         return 950;
      case 1048:  // Romanian
      case 1060:  // Slovenian
      case 1038:  // Hungarian
      case 1051:  // Slovak
      case 1045:  // Polish
      case 1052:  // Albanian
      case 2074:  // Serbian (Latin)
      case 1050:  // Croatian
      case 1029:  // Czech
         return 1250;
      case 1104:  // Mongolian (Cyrillic)
      case 1071:  // FYRO Macedonian
      case 2115:  // Uzbek (Cyrillic)
      case 1058:  // Ukrainian
      case 2092:  // Azeri (Cyrillic)
      case 1092:  // Tatar
      case 1087:  // Kazakh
      case 1059:  // Belarusian
      case 1088:  // Kyrgyz (Cyrillic)
      case 1026:  // Bulgarian
      case 3098:  // Serbian (Cyrillic)
      case 1049:  // Russian
         return 1251;
      case 8201:  // English (Jamaica)
      case 3084:  // French (Canada)
      case 1036:  // French (France)
      case 5132:  // French (Luxembourg)
      case 5129:  // English (New Zealand)
      case 6153:  // English (Ireland)
      case 1043:  // Dutch (Netherlands)
      case 9225:  // English (Caribbean)
      case 4108:  // French (Switzerland)
      case 4105:  // English (Canada)
      case 1110:  // Galician
      case 10249:  // English (Belize)
      case 3079:  // German (Austria)
      case 6156:  // French (Monaco)
      case 12297:  // English (Zimbabwe)
      case 1069:  // Basque
      case 2067:  // Dutch (Belgium)
      case 2060:  // French (Belgium)
      case 1035:  // Finnish
      case 1080:  // Faroese
      case 1031:  // German (Germany)
      case 3081:  // English (Australia)
      case 1033:  // English (United States)
      case 2057:  // English (United Kingdom)
      case 1027:  // Catalan
      case 11273:  // English (Trinidad)
      case 7177:  // English (South Africa)
      case 1030:  // Danish
      case 13321:  // English (Philippines)
      case 15370:  // Spanish (Paraguay)
      case 9226:  // Spanish (Colombia)
      case 5130:  // Spanish (Costa Rica)
      case 7178:  // Spanish (Dominican Republic)
      case 12298:  // Spanish (Ecuador)
      case 17418:  // Spanish (El Salvador)
      case 4106:  // Spanish (Guatemala)
      case 18442:  // Spanish (Honduras)
      case 3082:  // Spanish (International Sort)
      case 13322:  // Spanish (Chile)
      case 19466:  // Spanish (Nicaragua)
      case 2058:  // Spanish (Mexico)
      case 10250:  // Spanish (Peru)
      case 20490:  // Spanish (Puerto Rico)
      case 1034:  // Spanish (Traditional Sort)
      case 14346:  // Spanish (Uruguay)
      case 8202:  // Spanish (Venezuela)
      case 1089:  // Swahili
      case 1053:  // Swedish
      case 2077:  // Swedish (Finland)
      case 5127:  // German (Liechtenstein)
      case 1078:  // Afrikaans
      case 6154:  // Spanish (Panama)
      case 4103:  // German (Luxembourg)
      case 16394:  // Spanish (Bolivia)
      case 2055:  // German (Switzerland)
      case 1039:  // Icelandic
      case 1057:  // Indonesian
      case 1040:  // Italian (Italy)
      case 2064:  // Italian (Switzerland)
      case 2068:  // Norwegian (Nynorsk)
      case 11274:  // Spanish (Argentina)
      case 1046:  // Portuguese (Brazil)
      case 1044:  // Norwegian (Bokmal)
      case 1086:  // Malay (Malaysia)
      case 2110:  // Malay (Brunei Darussalam)
      case 2070:  // Portuguese (Portugal)
         return 1252;
      case 1032:  // Greek
         return 1253;
      case 1091:  // Uzbek (Latin)
      case 1068:  // Azeri (Latin)
      case 1055:  // Turkish
         return 1254;
      case 1037:  // Hebrew
         return 1255;
      case 5121:  // Arabic (Algeria)
      case 15361:  // Arabic (Bahrain)
      case 9217:  // Arabic (Yemen)
      case 3073:  // Arabic (Egypt)
      case 2049:  // Arabic (Iraq)
      case 11265:  // Arabic (Jordan)
      case 13313:  // Arabic (Kuwait)
      case 12289:  // Arabic (Lebanon)
      case 4097:  // Arabic (Libya)
      case 6145:  // Arabic (Morocco)
      case 8193:  // Arabic (Oman)
      case 16385:  // Arabic (Qatar)
      case 1025:  // Arabic (Saudi Arabia)
      case 10241:  // Arabic (Syria)
      case 14337:  // Arabic (U.A.E.)
      case 1065:  // Farsi
      case 1056:  // Urdu
      case 7169:  // Arabic (Tunisia)
         return 1256;
      case 1061:  // Estonian
      case 1062:  // Latvian
      case 1063:  // Lithuanian
         return 1257;
      case 1066:  // Vietnamese
         return 1258;
   }
   return 65001;   // utf-8
}


} // end namespace de
