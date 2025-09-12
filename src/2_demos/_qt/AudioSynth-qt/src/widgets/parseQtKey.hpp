#pragma once
#include <QKeyEvent>
#include <de/os/Events.h>

inline de::EKEY
convertQtKey( int qtKeyEnum )
{
   switch ( qtKeyEnum )
   {
      case Qt::Key_0: return de::KEY_0;
      case Qt::Key_1: return de::KEY_1;
      case Qt::Key_2: return de::KEY_2;
      case Qt::Key_3: return de::KEY_3;
      case Qt::Key_4: return de::KEY_4;
      case Qt::Key_5: return de::KEY_5;
      case Qt::Key_6: return de::KEY_6;
      case Qt::Key_7: return de::KEY_7;
      case Qt::Key_8: return de::KEY_8;
      case Qt::Key_9: return de::KEY_9;

      case Qt::Key_A: return de::KEY_A;
      case Qt::Key_B: return de::KEY_B;
      case Qt::Key_C: return de::KEY_C;
      case Qt::Key_D: return de::KEY_D;
      case Qt::Key_E: return de::KEY_E;
      case Qt::Key_F: return de::KEY_F;
      case Qt::Key_G: return de::KEY_G;
      case Qt::Key_H: return de::KEY_H;
      case Qt::Key_I: return de::KEY_I;
      case Qt::Key_J: return de::KEY_J;
      case Qt::Key_K: return de::KEY_K;
      case Qt::Key_L: return de::KEY_L;
      case Qt::Key_M: return de::KEY_M;
      case Qt::Key_N: return de::KEY_N;
      case Qt::Key_O: return de::KEY_O;
      case Qt::Key_P: return de::KEY_P;
      case Qt::Key_Q: return de::KEY_Q;
      case Qt::Key_R: return de::KEY_R;
      case Qt::Key_S: return de::KEY_S;
      case Qt::Key_T: return de::KEY_T;
      case Qt::Key_U: return de::KEY_U;
      case Qt::Key_V: return de::KEY_V;
      case Qt::Key_W: return de::KEY_W;
      case Qt::Key_X: return de::KEY_X;
      case Qt::Key_Y: return de::KEY_Y;
      case Qt::Key_Z: return de::KEY_Z;

      case Qt::Key_Escape: return de::KEY_ESCAPE;
      case Qt::Key_F1: return de::KEY_F1;
      case Qt::Key_F2: return de::KEY_F2;
      case Qt::Key_F3: return de::KEY_F3;
      case Qt::Key_F4: return de::KEY_F4;
      case Qt::Key_F5: return de::KEY_F5;
      case Qt::Key_F6: return de::KEY_F6;
      case Qt::Key_F7: return de::KEY_F7;
      case Qt::Key_F8: return de::KEY_F8;
      case Qt::Key_F9: return de::KEY_F9;
      case Qt::Key_F10: return de::KEY_F10;
      case Qt::Key_F11: return de::KEY_F11;
      case Qt::Key_F12: return de::KEY_F12;

      case Qt::Key_Enter: return de::KEY_ENTER;
      case Qt::Key_Backspace: return de::KEY_BACKSPACE;
      case Qt::Key_Backslash: return de::KEY_OEM_4;
      case Qt::Key_Shift: return de::KEY_LEFT_SHIFT;
         //return de::KEY_LEFT_SHIFT;
         //return de::KEY_RIGHT_SHIFT;
      case Qt::Key_Control: return de::KEY_LEFT_CTRL;
         //return de::KEY_LEFT_CTRL;
         //return de::KEY_RIGHT_CTRL;

      case Qt::Key_Up: return de::KEY_UP;
      case Qt::Key_Down: return de::KEY_DOWN;
      case Qt::Key_Left: return de::KEY_LEFT;
      case Qt::Key_Right: return de::KEY_RIGHT;
      default: return de::KEY_UNKNOWN;
   }
}

