#include <juce_gui_basics/native/juce_win32_KeyPress.h>
#include <juce_gui_basics/native/juce_win32_ComponentPeer.h>

namespace juce
{

const int KeyPress::spaceKey                = VK_SPACE;
const int KeyPress::returnKey               = VK_RETURN;
const int KeyPress::escapeKey               = VK_ESCAPE;
const int KeyPress::backspaceKey            = VK_BACK;
const int KeyPress::deleteKey               = VK_DELETE         | extendedKeyModifier;
const int KeyPress::insertKey               = VK_INSERT         | extendedKeyModifier;
const int KeyPress::tabKey                  = VK_TAB;
const int KeyPress::leftKey                 = VK_LEFT           | extendedKeyModifier;
const int KeyPress::rightKey                = VK_RIGHT          | extendedKeyModifier;
const int KeyPress::upKey                   = VK_UP             | extendedKeyModifier;
const int KeyPress::downKey                 = VK_DOWN           | extendedKeyModifier;
const int KeyPress::homeKey                 = VK_HOME           | extendedKeyModifier;
const int KeyPress::endKey                  = VK_END            | extendedKeyModifier;
const int KeyPress::pageUpKey               = VK_PRIOR          | extendedKeyModifier;
const int KeyPress::pageDownKey             = VK_NEXT           | extendedKeyModifier;
const int KeyPress::F1Key                   = VK_F1             | extendedKeyModifier;
const int KeyPress::F2Key                   = VK_F2             | extendedKeyModifier;
const int KeyPress::F3Key                   = VK_F3             | extendedKeyModifier;
const int KeyPress::F4Key                   = VK_F4             | extendedKeyModifier;
const int KeyPress::F5Key                   = VK_F5             | extendedKeyModifier;
const int KeyPress::F6Key                   = VK_F6             | extendedKeyModifier;
const int KeyPress::F7Key                   = VK_F7             | extendedKeyModifier;
const int KeyPress::F8Key                   = VK_F8             | extendedKeyModifier;
const int KeyPress::F9Key                   = VK_F9             | extendedKeyModifier;
const int KeyPress::F10Key                  = VK_F10            | extendedKeyModifier;
const int KeyPress::F11Key                  = VK_F11            | extendedKeyModifier;
const int KeyPress::F12Key                  = VK_F12            | extendedKeyModifier;
const int KeyPress::F13Key                  = VK_F13            | extendedKeyModifier;
const int KeyPress::F14Key                  = VK_F14            | extendedKeyModifier;
const int KeyPress::F15Key                  = VK_F15            | extendedKeyModifier;
const int KeyPress::F16Key                  = VK_F16            | extendedKeyModifier;
const int KeyPress::F17Key                  = VK_F17            | extendedKeyModifier;
const int KeyPress::F18Key                  = VK_F18            | extendedKeyModifier;
const int KeyPress::F19Key                  = VK_F19            | extendedKeyModifier;
const int KeyPress::F20Key                  = VK_F20            | extendedKeyModifier;
const int KeyPress::F21Key                  = VK_F21            | extendedKeyModifier;
const int KeyPress::F22Key                  = VK_F22            | extendedKeyModifier;
const int KeyPress::F23Key                  = VK_F23            | extendedKeyModifier;
const int KeyPress::F24Key                  = VK_F24            | extendedKeyModifier;
const int KeyPress::F25Key                  = 0x31000;          // Windows doesn't support F-keys 25 or higher
const int KeyPress::F26Key                  = 0x31001;
const int KeyPress::F27Key                  = 0x31002;
const int KeyPress::F28Key                  = 0x31003;
const int KeyPress::F29Key                  = 0x31004;
const int KeyPress::F30Key                  = 0x31005;
const int KeyPress::F31Key                  = 0x31006;
const int KeyPress::F32Key                  = 0x31007;
const int KeyPress::F33Key                  = 0x31008;
const int KeyPress::F34Key                  = 0x31009;
const int KeyPress::F35Key                  = 0x3100a;

const int KeyPress::numberPad0              = VK_NUMPAD0        | extendedKeyModifier;
const int KeyPress::numberPad1              = VK_NUMPAD1        | extendedKeyModifier;
const int KeyPress::numberPad2              = VK_NUMPAD2        | extendedKeyModifier;
const int KeyPress::numberPad3              = VK_NUMPAD3        | extendedKeyModifier;
const int KeyPress::numberPad4              = VK_NUMPAD4        | extendedKeyModifier;
const int KeyPress::numberPad5              = VK_NUMPAD5        | extendedKeyModifier;
const int KeyPress::numberPad6              = VK_NUMPAD6        | extendedKeyModifier;
const int KeyPress::numberPad7              = VK_NUMPAD7        | extendedKeyModifier;
const int KeyPress::numberPad8              = VK_NUMPAD8        | extendedKeyModifier;
const int KeyPress::numberPad9              = VK_NUMPAD9        | extendedKeyModifier;
const int KeyPress::numberPadAdd            = VK_ADD            | extendedKeyModifier;
const int KeyPress::numberPadSubtract       = VK_SUBTRACT       | extendedKeyModifier;
const int KeyPress::numberPadMultiply       = VK_MULTIPLY       | extendedKeyModifier;
const int KeyPress::numberPadDivide         = VK_DIVIDE         | extendedKeyModifier;
const int KeyPress::numberPadSeparator      = VK_SEPARATOR      | extendedKeyModifier;
const int KeyPress::numberPadDecimalPoint   = VK_DECIMAL        | extendedKeyModifier;
const int KeyPress::numberPadEquals         = 0x92 /*VK_OEM_NEC_EQUAL*/  | extendedKeyModifier;
const int KeyPress::numberPadDelete         = VK_DELETE         | extendedKeyModifier;
const int KeyPress::playKey                 = 0x30000;
const int KeyPress::stopKey                 = 0x30001;
const int KeyPress::fastForwardKey          = 0x30002;
const int KeyPress::rewindKey               = 0x30003;



//==============================================================================
bool KeyPress::isKeyCurrentlyDown (const int keyCode)
{
    auto k = (SHORT) keyCode;

    if ((keyCode & extendedKeyModifier) == 0)
    {
        if (k >= (SHORT) 'a' && k <= (SHORT) 'z')
            k += (SHORT) 'A' - (SHORT) 'a';

        // Only translate if extendedKeyModifier flag is not set
        const SHORT translatedValues[] = { (SHORT) ',', VK_OEM_COMMA,
                                           (SHORT) '+', VK_OEM_PLUS,
                                           (SHORT) '-', VK_OEM_MINUS,
                                           (SHORT) '.', VK_OEM_PERIOD,
                                           (SHORT) ';', VK_OEM_1,
                                           (SHORT) ':', VK_OEM_1,
                                           (SHORT) '/', VK_OEM_2,
                                           (SHORT) '?', VK_OEM_2,
                                           (SHORT) '[', VK_OEM_4,
                                           (SHORT) ']', VK_OEM_6 };

        for (int i = 0; i < numElementsInArray (translatedValues); i += 2)
            if (k == translatedValues[i])
                k = translatedValues[i + 1];
    }

    return HWNDComponentPeer::isKeyDown (k);
}


} // end namespace juce
