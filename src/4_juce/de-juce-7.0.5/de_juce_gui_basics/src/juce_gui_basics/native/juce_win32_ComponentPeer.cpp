#include <juce_gui_basics/native/juce_win32_ComponentPeer.h>
#include <juce_gui_basics/components/juce_Component.h>
#include <juce_gui_basics/native/juce_win32_Touch.h>
#include <juce_gui_basics/native/juce_win32_Accessibility.h>

namespace juce
{

MultiTouchMapper<DWORD> HWNDComponentPeer::currentTouches;
ModifierKeys HWNDComponentPeer::modifiersAtLastCallback;

//==============================================================================
HWNDComponentPeer::HWNDComponentPeer (Component& comp, int windowStyleFlags, HWND parent, bool nonRepainting)
  : ComponentPeer (comp, windowStyleFlags),
    dontRepaint (nonRepainting),
    parentToAddTo (parent),
    currentRenderingEngine (softwareRenderingEngine)
{
  callFunctionIfNotLocked (&createWindowCallback, this);

  setTitle (component.getName());
  updateShadower();

  getNativeRealtimeModifiers = []
  {
      HWNDComponentPeer::updateKeyModifiers();

      int mouseMods = 0;
      if (HWNDComponentPeer::isKeyDown (VK_LBUTTON))  mouseMods |= ModifierKeys::leftButtonModifier;
      if (HWNDComponentPeer::isKeyDown (VK_RBUTTON))  mouseMods |= ModifierKeys::rightButtonModifier;
      if (HWNDComponentPeer::isKeyDown (VK_MBUTTON))  mouseMods |= ModifierKeys::middleButtonModifier;

      ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons().withFlags (mouseMods);

      return ModifierKeys::currentModifiers;
  };

  updateCurrentMonitorAndRefreshVBlankDispatcher();

  if (parentToAddTo != nullptr)
      monitorUpdateTimer.emplace (1000, [this] { updateCurrentMonitorAndRefreshVBlankDispatcher(); });

  suspendResumeRegistration = ScopedSuspendResumeNotificationRegistration { hwnd };
}

HWNDComponentPeer::~HWNDComponentPeer()
{
  suspendResumeRegistration = {};

  VBlankDispatcher::getInstance()->removeListener (*this);

  // do this first to avoid messages arriving for this window before it's destroyed
  JuceWindowIdentifier::setAsJUCEWindow (hwnd, false);

  if (isAccessibilityActive)
      WindowsAccessibility::revokeUIAMapEntriesForWindow (hwnd);

  shadower = nullptr;
  currentTouches.deleteAllTouchesForPeer (this);

  callFunctionIfNotLocked (&destroyWindowCallback, (void*) hwnd);

  if (dropTarget != nullptr)
  {
      dropTarget->peerIsDeleted = true;
      dropTarget->Release();
      dropTarget = nullptr;
  }

 #if JUCE_DIRECT2D
  direct2DContext = nullptr;
 #endif
}






ComponentPeer*
Component::createNewPeer (int styleFlags, void* parentHWND)
{
    return new HWNDComponentPeer (*this, styleFlags, (HWND) parentHWND, false);
}

JUCE_API ComponentPeer*
createNonRepaintingEmbeddedWindowsPeer (Component& component, void* parentHWND)
{
    return new HWNDComponentPeer (component, ComponentPeer::windowIgnoresMouseClicks,
                                  (HWND) parentHWND, true);
}

JUCE_IMPLEMENT_SINGLETON (HWNDComponentPeer::WindowClassHolder)

} // end namespace juce
