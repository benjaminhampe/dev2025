#pragma once

#include <juce_gui_basics/windows/juce_ComponentPeer.h>
#include <juce_gui_basics/components/juce_Component.h>
#include <juce_gui_basics/layout/juce_ComponentBoundsConstrainer.h>
#include <juce_gui_basics/keyboard/juce_TextInputTarget.h>
#include <juce_gui_basics/components/juce_ScalingHelpers.h>
#include <juce_gui_basics/desktop/juce_Desktop.h>


//#include <juce_gui_basics/components/juce_Component.h>
//#include <juce_gui_basics/windows/juce_VBlankAttachement.h>
#include <juce_gui_basics/native/juce_win32_Desktop.h>
#include <juce_gui_basics/native/juce_win32_DPIAwareness.h>
#include <juce_gui_basics/native/juce_win32_Accessibility.h>
#include <juce_gui_basics/native/juce_win32_BitmapImage.h>
#include <juce_gui_basics/native/juce_win32_MouseCursor.h> // IconConverters
#include <juce_gui_basics/native/juce_win32_Touch.h>
#include <juce_gui_basics/native/juce_win32_KeyPress.h>
#include <juce_gui_basics/native/juce_win32_Windowing.h> // SimpleTimer
#include <juce_gui_basics/native/juce_MultiTouchMapper.h>
#include <juce_gui_basics/misc/juce_DropShadower.h>
#include <juce_gui_basics/native/juce_win32_UWPUIViewSettings.h>

#if JUCE_DIRECT2D
#include <juce_graphics/native/juce_win32_Direct2DGraphicsContext.h>
#endif

#include <juce_gui_basics/lookandfeel/juce_LookAndFeel.h>
#include <juce_gui_basics/mouse/juce_getMouseEventTime.h>

#include <juce_events/native/juce_win32_HiddenMessageWindow.h>

namespace juce
{

JUCE_API ComponentPeer*
createNonRepaintingEmbeddedWindowsPeer (Component& component, void* parentHWND);

static bool shouldDeactivateTitleBar = true;

using CheckEventBlockedByModalComps = bool (*) (const MSG&);
extern CheckEventBlockedByModalComps isEventBlockedByModalComps;

//==============================================================================
class HWNDComponentPeer  : public ComponentPeer,
                           private ComponentPeer::VBlankListener,
                           private Timer
                          #if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
                           , public ModifierKeyReceiver
                          #endif
{
public:
    enum RenderingEngineType
    {
        softwareRenderingEngine = 0,
        direct2DRenderingEngine
    };

    //==============================================================================
    HWNDComponentPeer (Component& comp, int windowStyleFlags, HWND parent, bool nonRepainting);
    ~HWNDComponentPeer() override;

    //==============================================================================
    void* getNativeHandle() const override    { return hwnd; }

    void setVisible (bool shouldBeVisible) override
    {
        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        ShowWindow (hwnd, shouldBeVisible ? SW_SHOWNA : SW_HIDE);

        if (shouldBeVisible)
            InvalidateRect (hwnd, nullptr, 0);
        else
            lastPaintTime = 0;
    }

    void setTitle (const String& title) override
    {
        // Unfortunately some ancient bits of win32 mean you can only perform this operation from the message thread.
        JUCE_ASSERT_MESSAGE_THREAD

        SetWindowTextW(hwnd, title.toWideCharPointer());
    }

    void repaintNowIfTransparent()
    {
        if (isUsingUpdateLayeredWindow() && lastPaintTime > 0 && Time::getMillisecondCounter() > lastPaintTime + 30)
            handlePaintMessage();
    }

    void updateBorderSize()
    {
        WINDOWINFO info;
        info.cbSize = sizeof (info);

        if (GetWindowInfo (hwnd, &info))
            windowBorder = BorderSize<int> (roundToInt ((info.rcClient.top    - info.rcWindow.top)    / scaleFactor),
                                            roundToInt ((info.rcClient.left   - info.rcWindow.left)   / scaleFactor),
                                            roundToInt ((info.rcWindow.bottom - info.rcClient.bottom) / scaleFactor),
                                            roundToInt ((info.rcWindow.right  - info.rcClient.right)  / scaleFactor));

       #if JUCE_DIRECT2D
        if (direct2DContext != nullptr)
            direct2DContext->resized();
       #endif
    }

    void setBounds (const Rectangle<int>& bounds, bool isNowFullScreen) override
    {
        // If we try to set new bounds while handling an existing position change,
        // Windows may get confused about our current scale and size.
        // This can happen when moving a window between displays, because the mouse-move
        // generator in handlePositionChanged can cause the window to move again.
        if (inHandlePositionChanged)
            return;

        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        fullScreen = isNowFullScreen;

        auto newBounds = windowBorder.addedTo (bounds);

        if (isUsingUpdateLayeredWindow())
        {
            if (auto parentHwnd = GetParent (hwnd))
            {
                auto parentRect = convertPhysicalScreenRectangleToLogical (rectangleFromRECT (getWindowScreenRect (parentHwnd)), hwnd);
                newBounds.translate (parentRect.getX(), parentRect.getY());
            }
        }

        auto oldBounds = getBounds();

        const bool hasMoved = (oldBounds.getPosition() != bounds.getPosition());
        const bool hasResized = (oldBounds.getWidth() != bounds.getWidth()
                                  || oldBounds.getHeight() != bounds.getHeight());

        DWORD flags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER;
        if (! hasMoved)    flags |= SWP_NOMOVE;
        if (! hasResized)  flags |= SWP_NOSIZE;

        setWindowPos (hwnd, newBounds, flags, ! inDpiChange);

        if (hasResized && isValidPeer (this))
        {
            updateBorderSize();
            repaintNowIfTransparent();
        }
    }

    Rectangle<int> getBounds() const override
    {
        auto bounds = [this]
        {
            if (parentToAddTo == nullptr)
                return convertPhysicalScreenRectangleToLogical (rectangleFromRECT (getWindowScreenRect (hwnd)), hwnd);

            auto localBounds = rectangleFromRECT (getWindowClientRect (hwnd));

            if (isPerMonitorDPIAwareWindow (hwnd))
                return (localBounds.toDouble() / getPlatformScaleFactor()).toNearestInt();

            return localBounds;
        }();

        return windowBorder.subtractedFrom (bounds);
    }

    Point<int> getScreenPosition() const
    {
        auto r = convertPhysicalScreenRectangleToLogical (rectangleFromRECT (getWindowScreenRect (hwnd)), hwnd);

        return { r.getX() + windowBorder.getLeft(),
                 r.getY() + windowBorder.getTop() };
    }

    Point<float> localToGlobal (Point<float> relativePosition) override  { return relativePosition + getScreenPosition().toFloat(); }
    Point<float> globalToLocal (Point<float> screenPosition) override    { return screenPosition   - getScreenPosition().toFloat(); }

    using ComponentPeer::localToGlobal;
    using ComponentPeer::globalToLocal;

    void setAlpha (float newAlpha) override
    {
        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        auto intAlpha = (uint8) jlimit (0, 255, (int) (newAlpha * 255.0f));

        if (component.isOpaque())
        {
            if (newAlpha < 1.0f)
            {
                SetWindowLong (hwnd, GWL_EXSTYLE, GetWindowLong (hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes (hwnd, RGB (0, 0, 0), intAlpha, LWA_ALPHA);
            }
            else
            {
                SetWindowLong (hwnd, GWL_EXSTYLE, GetWindowLong (hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
                RedrawWindow (hwnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
            }
        }
        else
        {
            updateLayeredWindowAlpha = intAlpha;
            component.repaint();
        }
    }

    void setMinimised (bool shouldBeMinimised) override
    {
        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        if (shouldBeMinimised != isMinimised())
            ShowWindow (hwnd, shouldBeMinimised ? SW_MINIMIZE : SW_RESTORE);
    }

    bool isMinimised() const override
    {
        WINDOWPLACEMENT wp;
        wp.length = sizeof (WINDOWPLACEMENT);
        GetWindowPlacement (hwnd, &wp);

        return wp.showCmd == SW_SHOWMINIMIZED;
    }

    void setFullScreen (bool shouldBeFullScreen) override
    {
        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        setMinimised (false);

        if (isFullScreen() != shouldBeFullScreen)
        {
            if (constrainer != nullptr)
                constrainer->resizeStart();

            fullScreen = shouldBeFullScreen;
            const WeakReference<Component> deletionChecker (&component);

            if (! fullScreen)
            {
                auto boundsCopy = lastNonFullscreenBounds;

                if (hasTitleBar())
                    ShowWindow (hwnd, SW_SHOWNORMAL);

                if (! boundsCopy.isEmpty())
                    setBounds (ScalingHelpers::scaledScreenPosToUnscaled (component, boundsCopy), false);
            }
            else
            {
                if (hasTitleBar())
                    ShowWindow (hwnd, SW_SHOWMAXIMIZED);
                else
                    SendMessageW (hwnd, WM_SETTINGCHANGE, 0, 0);
            }

            if (deletionChecker != nullptr)
                handleMovedOrResized();

            if (constrainer != nullptr)
                constrainer->resizeEnd();
        }
    }

    bool isFullScreen() const override
    {
        if (! hasTitleBar())
            return fullScreen;

        WINDOWPLACEMENT wp;
        wp.length = sizeof (wp);
        GetWindowPlacement (hwnd, &wp);

        return wp.showCmd == SW_SHOWMAXIMIZED;
    }

    bool contains (Point<int> localPos, bool trueIfInAChildWindow) const override
    {
        auto r = convertPhysicalScreenRectangleToLogical (rectangleFromRECT (getWindowScreenRect (hwnd)), hwnd);

        if (! r.withZeroOrigin().contains (localPos))
            return false;

        auto w = WindowFromPoint (POINTFromPoint (convertLogicalScreenPointToPhysical (localPos + getScreenPosition(),
                                                                                       hwnd)));

        return w == hwnd || (trueIfInAChildWindow && (IsChild (hwnd, w) != 0));
    }

    OptionalBorderSize getFrameSizeIfPresent() const override
    {
        return ComponentPeer::OptionalBorderSize { windowBorder };
    }

    BorderSize<int> getFrameSize() const override
    {
        return windowBorder;
    }

    bool setAlwaysOnTop (bool alwaysOnTop) override
    {
        const bool oldDeactivate = shouldDeactivateTitleBar;
        shouldDeactivateTitleBar = ((styleFlags & windowIsTemporary) == 0);

        setWindowZOrder (hwnd, alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST);

        shouldDeactivateTitleBar = oldDeactivate;

        if (shadower != nullptr)
            handleBroughtToFront();

        return true;
    }

    void toFront (bool makeActive) override
    {
        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        setMinimised (false);

        const bool oldDeactivate = shouldDeactivateTitleBar;
        shouldDeactivateTitleBar = ((styleFlags & windowIsTemporary) == 0);

        callFunctionIfNotLocked (makeActive ? &toFrontCallback1 : &toFrontCallback2, hwnd);

        shouldDeactivateTitleBar = oldDeactivate;

        if (! makeActive)
        {
            // in this case a broughttofront call won't have occurred, so do it now..
            handleBroughtToFront();
        }
    }

    void toBehind (ComponentPeer* other) override
    {
        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        if (auto* otherPeer = dynamic_cast<HWNDComponentPeer*> (other))
        {
            setMinimised (false);

            // Must be careful not to try to put a topmost window behind a normal one, or Windows
            // promotes the normal one to be topmost!
            if (component.isAlwaysOnTop() == otherPeer->getComponent().isAlwaysOnTop())
                setWindowZOrder (hwnd, otherPeer->hwnd);
            else if (otherPeer->getComponent().isAlwaysOnTop())
                setWindowZOrder (hwnd, HWND_TOP);
        }
        else
        {
            jassertfalse; // wrong type of window?
        }
    }

    bool isFocused() const override
    {
        return callFunctionIfNotLocked (&getFocusCallback, nullptr) == (void*) hwnd;
    }

    void grabFocus() override
    {
        const ScopedValueSetter<bool> scope (shouldIgnoreModalDismiss, true);

        const bool oldDeactivate = shouldDeactivateTitleBar;
        shouldDeactivateTitleBar = ((styleFlags & windowIsTemporary) == 0);

        callFunctionIfNotLocked (&setFocusCallback, hwnd);

        shouldDeactivateTitleBar = oldDeactivate;
    }

    void textInputRequired (Point<int>, TextInputTarget&) override
    {
        if (! hasCreatedCaret)
            hasCreatedCaret = CreateCaret (hwnd, (HBITMAP) 1, 0, 0);

        if (hasCreatedCaret)
        {
            SetCaretPos (0, 0);
            ShowCaret (hwnd);
        }

        ImmAssociateContext (hwnd, nullptr);

        // MSVC complains about the nullptr argument, but the docs for this
        // function say that the second argument is ignored when the third
        // argument is IACE_DEFAULT.
        JUCE_BEGIN_IGNORE_WARNINGS_MSVC (6387)
        ImmAssociateContextEx (hwnd, nullptr, IACE_DEFAULT);
        JUCE_END_IGNORE_WARNINGS_MSVC
    }

    void closeInputMethodContext() override
    {
        imeHandler.handleSetContext (hwnd, false);
    }

    void dismissPendingTextInput() override
    {
        closeInputMethodContext();

        ImmAssociateContext (hwnd, nullptr);

        if (std::exchange (hasCreatedCaret, false))
            DestroyCaret();
    }

    void repaint (const Rectangle<int>& area) override
    {
        deferredRepaints.add ((area.toDouble() * getPlatformScaleFactor()).getSmallestIntegerContainer());
    }

    void dispatchDeferredRepaints()
    {
        for (auto deferredRect : deferredRepaints)
        {
            auto r = RECTFromRectangle (deferredRect);
            InvalidateRect (hwnd, &r, FALSE);
        }

        deferredRepaints.clear();
    }

    void performAnyPendingRepaintsNow() override
    {
        if (component.isVisible())
        {
            dispatchDeferredRepaints();

            WeakReference<Component> localRef (&component);
            MSG m;

            if (isUsingUpdateLayeredWindow() || PeekMessage (&m, hwnd, WM_PAINT, WM_PAINT, PM_REMOVE))
                if (localRef != nullptr) // (the PeekMessage call can dispatch messages, which may delete this comp)
                    handlePaintMessage();
        }
    }

    //==============================================================================
    void onVBlank() override
    {
        vBlankListeners.call ([] (auto& l) { l.onVBlank(); });
        dispatchDeferredRepaints();
    }

    //==============================================================================
    static HWNDComponentPeer* getOwnerOfWindow (HWND h) noexcept
    {
        if (h != nullptr && JuceWindowIdentifier::isJUCEWindow (h))
            return (HWNDComponentPeer*) GetWindowLongPtr (h, 8);

        return nullptr;
    }

    //==============================================================================
    bool isInside (HWND h) const noexcept
    {
        return GetAncestor (hwnd, GA_ROOT) == h;
    }

    //==============================================================================
    static bool isKeyDown (const int key) noexcept  { return (GetAsyncKeyState (key) & 0x8000) != 0; }

    static void updateKeyModifiers() noexcept
    {
        int keyMods = 0;
        if (isKeyDown (VK_SHIFT))   keyMods |= ModifierKeys::shiftModifier;
        if (isKeyDown (VK_CONTROL)) keyMods |= ModifierKeys::ctrlModifier;
        if (isKeyDown (VK_MENU))    keyMods |= ModifierKeys::altModifier;

        // workaround: Windows maps AltGr to left-Ctrl + right-Alt.
        if (isKeyDown (VK_RMENU) && !isKeyDown (VK_RCONTROL))
        {
            keyMods = (keyMods & ~ModifierKeys::ctrlModifier) | ModifierKeys::altModifier;
        }

        ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withOnlyMouseButtons().withFlags (keyMods);
    }

    static void updateModifiersFromWParam (const WPARAM wParam)
    {
        int mouseMods = 0;
        if (wParam & MK_LBUTTON)   mouseMods |= ModifierKeys::leftButtonModifier;
        if (wParam & MK_RBUTTON)   mouseMods |= ModifierKeys::rightButtonModifier;
        if (wParam & MK_MBUTTON)   mouseMods |= ModifierKeys::middleButtonModifier;

        ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons().withFlags (mouseMods);
        updateKeyModifiers();
    }

    //==============================================================================
    bool dontRepaint;
    static ModifierKeys modifiersAtLastCallback;

    //==============================================================================
    struct FileDropTarget    : public ComBaseClassHelper<IDropTarget>
    {
        FileDropTarget (HWNDComponentPeer& p)   : peer (p) {}

        JUCE_COMRESULT DragEnter (IDataObject* pDataObject, DWORD grfKeyState, POINTL mousePos, DWORD* pdwEffect) override
        {
            auto hr = updateFileList (pDataObject);

            if (FAILED (hr))
                return hr;

            return DragOver (grfKeyState, mousePos, pdwEffect);
        }

        JUCE_COMRESULT DragLeave() override
        {
            if (peerIsDeleted)
                return S_FALSE;

            peer.handleDragExit (dragInfo);
            return S_OK;
        }

        JUCE_COMRESULT DragOver (DWORD /*grfKeyState*/, POINTL mousePos, DWORD* pdwEffect) override
        {
            if (peerIsDeleted)
                return S_FALSE;

            dragInfo.position = getMousePos (mousePos).roundToInt();
            *pdwEffect = peer.handleDragMove (dragInfo) ? (DWORD) DROPEFFECT_COPY
                                                        : (DWORD) DROPEFFECT_NONE;
            return S_OK;
        }

        JUCE_COMRESULT Drop (IDataObject* pDataObject, DWORD /*grfKeyState*/, POINTL mousePos, DWORD* pdwEffect) override
        {
            auto hr = updateFileList (pDataObject);

            if (FAILED (hr))
                return hr;

            dragInfo.position = getMousePos (mousePos).roundToInt();
            *pdwEffect = peer.handleDragDrop (dragInfo) ? (DWORD) DROPEFFECT_COPY
                                                        : (DWORD) DROPEFFECT_NONE;
            return S_OK;
        }

        HWNDComponentPeer& peer;
        ComponentPeer::DragInfo dragInfo;
        bool peerIsDeleted = false;

    private:
        Point<float> getMousePos (POINTL mousePos) const
        {
            const auto originalPos = pointFromPOINT ({ mousePos.x, mousePos.y });
            const auto logicalPos = convertPhysicalScreenPointToLogical (originalPos, peer.hwnd);
            return ScalingHelpers::screenPosToLocalPos (peer.component, logicalPos.toFloat());
        }

        struct DroppedData
        {
            DroppedData (IDataObject* dataObject, CLIPFORMAT type)
            {
                FORMATETC format = { type, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

                if (SUCCEEDED (error = dataObject->GetData (&format, &medium)) && medium.hGlobal != nullptr)
                {
                    dataSize = GlobalSize (medium.hGlobal);
                    data = GlobalLock (medium.hGlobal);
                }
            }

            ~DroppedData()
            {
                if (data != nullptr && medium.hGlobal != nullptr)
                    GlobalUnlock (medium.hGlobal);
            }

            HRESULT error;
            STGMEDIUM medium { TYMED_HGLOBAL, { nullptr }, nullptr };
            void* data = {};
            SIZE_T dataSize;
        };

        void parseFileList (HDROP dropFiles)
        {
            dragInfo.files.clearQuick();

            std::vector<TCHAR> nameBuffer;

            const auto numFiles = DragQueryFile (dropFiles, ~(UINT) 0, nullptr, 0);

            for (UINT i = 0; i < numFiles; ++i)
            {
                const auto bufferSize = DragQueryFile (dropFiles, i, nullptr, 0);
                nameBuffer.clear();
                nameBuffer.resize (bufferSize + 1, 0); // + 1 for the null terminator

                [[maybe_unused]] const auto readCharacters = DragQueryFile (dropFiles, i, nameBuffer.data(), (UINT) nameBuffer.size());
                jassert (readCharacters == bufferSize);

                dragInfo.files.add (String (nameBuffer.data()));
            }
        }

        HRESULT updateFileList (IDataObject* const dataObject)
        {
            if (peerIsDeleted)
                return S_FALSE;

            dragInfo.clear();

            {
                DroppedData fileData (dataObject, CF_HDROP);

                if (SUCCEEDED (fileData.error))
                {
                    parseFileList (static_cast<HDROP> (fileData.data));
                    return S_OK;
                }
            }

            DroppedData textData (dataObject, CF_UNICODETEXT);

            if (SUCCEEDED (textData.error))
            {
                dragInfo.text = String (CharPointer_UTF16 ((const WCHAR*) textData.data),
                                        CharPointer_UTF16 ((const WCHAR*) addBytesToPointer (textData.data, textData.dataSize)));
                return S_OK;
            }

            return textData.error;
        }

        JUCE_DECLARE_NON_COPYABLE (FileDropTarget)
    };

    static bool offerKeyMessageToJUCEWindow (MSG& m)
    {
        if (m.message == WM_KEYDOWN || m.message == WM_KEYUP)
        {
            if (Component::getCurrentlyFocusedComponent() != nullptr)
            {
                if (auto* peer = getOwnerOfWindow (m.hwnd))
                {
                    ScopedThreadDPIAwarenessSetter threadDpiAwarenessSetter { m.hwnd };

                    return m.message == WM_KEYDOWN ? peer->doKeyDown (m.wParam)
                                                   : peer->doKeyUp (m.wParam);
                }
            }
        }

        return false;
    }

    double getPlatformScaleFactor() const noexcept override
    {
       #if ! JUCE_WIN_PER_MONITOR_DPI_AWARE
        return 1.0;
       #else
        if (! isPerMonitorDPIAwareWindow (hwnd))
            return 1.0;

        if (auto* parentHWND = GetParent (hwnd))
        {
            if (auto* parentPeer = getOwnerOfWindow (parentHWND))
                return parentPeer->getPlatformScaleFactor();

            if (getDPIForWindow != nullptr)
                return getScaleFactorForWindow (parentHWND);
        }

        return scaleFactor;
       #endif
    }

private:
    HWND hwnd, parentToAddTo;
    std::unique_ptr<DropShadower> shadower;
    RenderingEngineType currentRenderingEngine;
   #if JUCE_DIRECT2D
    std::unique_ptr<Direct2DLowLevelGraphicsContext> direct2DContext;
   #endif
    uint32 lastPaintTime = 0;
    ULONGLONG lastMagnifySize = 0;
    bool fullScreen = false, isDragging = false, isMouseOver = false,
         hasCreatedCaret = false, constrainerIsResizing = false;
    BorderSize<int> windowBorder;
    IconConverters::IconPtr currentWindowIcon;
    FileDropTarget* dropTarget = nullptr;
    uint8 updateLayeredWindowAlpha = 255;
    UWPUIViewSettings uwpViewSettings;
   #if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
    ModifierKeyProvider* modProvider = nullptr;
   #endif

    double scaleFactor = 1.0;
    bool inDpiChange = 0, inHandlePositionChanged = 0;
    HMONITOR currentMonitor = nullptr;

    bool isAccessibilityActive = false;

    //==============================================================================
    static MultiTouchMapper<DWORD> currentTouches;

    //==============================================================================
    struct TemporaryImage    : private Timer
    {
        TemporaryImage() {}

        Image& getImage (bool transparent, int w, int h)
        {
            auto format = transparent ? Image::ARGB : Image::RGB;

            if ((! image.isValid()) || image.getWidth() < w || image.getHeight() < h || image.getFormat() != format)
                image = Image (new WindowsBitmapImage (format, (w + 31) & ~31, (h + 31) & ~31, false));

            startTimer (3000);
            return image;
        }

        void timerCallback() override
        {
            stopTimer();
            image = {};
        }

    private:
        Image image;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemporaryImage)
    };

    TemporaryImage offscreenImageGenerator;

    //==============================================================================
    class WindowClassHolder    : private DeletedAtShutdown
    {
    public:
        WindowClassHolder()
        {
            // this name has to be different for each app/dll instance because otherwise poor old Windows can
            // get a bit confused (even despite it not being a process-global window class).
            String windowClassName ("JUCE_");
            windowClassName << String::toHexString (Time::currentTimeMillis());

            auto moduleHandle = (HINSTANCE) Process::getCurrentModuleInstanceHandle();

            TCHAR moduleFile[1024] = {};
            GetModuleFileName (moduleHandle, moduleFile, 1024);

            WNDCLASSEXW wcex = {};
            wcex.cbSize         = sizeof (wcex);
            wcex.style          = CS_OWNDC;
            wcex.lpfnWndProc    = (WNDPROC) windowProc;
            wcex.lpszClassName  = windowClassName.toWideCharPointer();
            wcex.cbWndExtra     = 32;
            wcex.hInstance      = moduleHandle;

            for (const auto& [index, field, ptr] : { std::tuple { 0, &wcex.hIcon,   &iconBig },
                                                     std::tuple { 1, &wcex.hIconSm, &iconSmall } })
            {
                auto iconNum = static_cast<WORD> (index);
                ptr->reset (*field = ExtractAssociatedIcon (moduleHandle, moduleFile, &iconNum));
            }

            atom = RegisterClassExW(&wcex);
            jassert (atom != 0);

            isEventBlockedByModalComps = checkEventBlockedByModalComps;
        }

        ~WindowClassHolder()
        {
            if (ComponentPeer::getNumPeers() == 0)
                UnregisterClassW(getWindowClassName(), (HINSTANCE) Process::getCurrentModuleInstanceHandle());

            clearSingletonInstance();
        }

        LPCWSTR getWindowClassName() const noexcept
        {
         return (LPCWSTR) (pointer_sized_uint) atom;
        }

        JUCE_DECLARE_SINGLETON_SINGLETHREADED_MINIMAL (WindowClassHolder)

    private:
        ATOM atom;

        static bool isHWNDBlockedByModalComponents (HWND h)
        {
            for (int i = Desktop::getInstance().getNumComponents(); --i >= 0;)
                if (auto* c = Desktop::getInstance().getComponent (i))
                    if ((! c->isCurrentlyBlockedByAnotherModalComponent())
                          && IsChild ((HWND) c->getWindowHandle(), h))
                        return false;

            return true;
        }

        static bool checkEventBlockedByModalComps (const MSG& m)
        {
            if (Component::getNumCurrentlyModalComponents() == 0 || JuceWindowIdentifier::isJUCEWindow (m.hwnd))
                return false;

            switch (m.message)
            {
                case WM_MOUSEMOVE:
                case WM_NCMOUSEMOVE:
                case 0x020A: /* WM_MOUSEWHEEL */
                case 0x020E: /* WM_MOUSEHWHEEL */
                case WM_KEYUP:
                case WM_SYSKEYUP:
                case WM_CHAR:
                case WM_APPCOMMAND:
                case WM_LBUTTONUP:
                case WM_MBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MOUSEACTIVATE:
                case WM_NCMOUSEHOVER:
                case WM_MOUSEHOVER:
                case WM_TOUCH:
                case WM_POINTERUPDATE:
                case WM_NCPOINTERUPDATE:
                case WM_POINTERWHEEL:
                case WM_POINTERHWHEEL:
                case WM_POINTERUP:
                case WM_POINTERACTIVATE:
                    return isHWNDBlockedByModalComponents(m.hwnd);
                case WM_NCLBUTTONDOWN:
                case WM_NCLBUTTONDBLCLK:
                case WM_NCRBUTTONDOWN:
                case WM_NCRBUTTONDBLCLK:
                case WM_NCMBUTTONDOWN:
                case WM_NCMBUTTONDBLCLK:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONDBLCLK:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONDBLCLK:
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                case WM_NCPOINTERDOWN:
                case WM_POINTERDOWN:
                    if (isHWNDBlockedByModalComponents (m.hwnd))
                    {
                        if (auto* modal = Component::getCurrentlyModalComponent (0))
                            modal->inputAttemptWhenModal();

                        return true;
                    }
                    break;

                default:
                    break;
            }

            return false;
        }

        IconConverters::IconPtr iconBig, iconSmall;

        JUCE_DECLARE_NON_COPYABLE (WindowClassHolder)
    };

    //==============================================================================
    static void* createWindowCallback (void* userData)
    {
        static_cast<HWNDComponentPeer*> (userData)->createWindow();
        return nullptr;
    }

    void createWindow()
    {
        DWORD exstyle = 0;
        DWORD type = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        if (hasTitleBar())
        {
            type |= WS_OVERLAPPED;

            if ((styleFlags & windowHasCloseButton) != 0)
            {
                type |= WS_SYSMENU;
            }
            else
            {
                // annoyingly, windows won't let you have a min/max button without a close button
                jassert ((styleFlags & (windowHasMinimiseButton | windowHasMaximiseButton)) == 0);
            }

            if ((styleFlags & windowIsResizable) != 0)
                type |= WS_THICKFRAME;
        }
        else if (parentToAddTo != nullptr)
        {
            type |= WS_CHILD;
        }
        else
        {
            type |= WS_POPUP | WS_SYSMENU;
        }

        if ((styleFlags & windowAppearsOnTaskbar) == 0)
            exstyle |= WS_EX_TOOLWINDOW;
        else
            exstyle |= WS_EX_APPWINDOW;

        if ((styleFlags & windowHasMinimiseButton) != 0)    type |= WS_MINIMIZEBOX;
        if ((styleFlags & windowHasMaximiseButton) != 0)    type |= WS_MAXIMIZEBOX;
        if ((styleFlags & windowIgnoresMouseClicks) != 0)   exstyle |= WS_EX_TRANSPARENT;
        if ((styleFlags & windowIsSemiTransparent) != 0)    exstyle |= WS_EX_LAYERED;

        hwnd = CreateWindowExW(exstyle,
                     WindowClassHolder::getInstance()->getWindowClassName(),
                               L"", type, 0, 0, 0, 0, parentToAddTo, nullptr,
                               (HINSTANCE) Process::getCurrentModuleInstanceHandle(), nullptr);

       #if JUCE_DEBUG
        // The DPI-awareness context of this window and JUCE's hidden message window are different.
        // You normally want these to match otherwise timer events and async messages will happen
        // in a different context to normal HWND messages which can cause issues with UI scaling.
        jassert (isPerMonitorDPIAwareWindow (hwnd) == isPerMonitorDPIAwareWindow (juce_messageWindowHandle)
                   || isInScopedDPIAwarenessDisabler());
       #endif

        if (hwnd != nullptr)
        {
            SetWindowLongPtr (hwnd, 0, 0);
            SetWindowLongPtr (hwnd, 8, (LONG_PTR) this);
            JuceWindowIdentifier::setAsJUCEWindow (hwnd, true);

            if (dropTarget == nullptr)
            {
                HWNDComponentPeer* peer = nullptr;

                if (dontRepaint)
                    peer = getOwnerOfWindow (parentToAddTo);

                if (peer == nullptr)
                    peer = this;

                dropTarget = new FileDropTarget (*peer);
            }

            RegisterDragDrop (hwnd, dropTarget);

            if (canUseMultiTouch())
                registerTouchWindow (hwnd, 0);

            setDPIAwareness();

            if (isPerMonitorDPIAwareThread())
                scaleFactor = getScaleFactorForWindow (hwnd);

            setMessageFilter();
            updateBorderSize();
            checkForPointerAPI();

            // This is needed so that our plugin window gets notified of WM_SETTINGCHANGE messages
            // and can respond to display scale changes
            if (! JUCEApplication::isStandaloneApp())
                settingChangeCallback = ComponentPeer::forceDisplayUpdate;

            // Calling this function here is (for some reason) necessary to make Windows
            // correctly enable the menu items that we specify in the wm_initmenu message.
            GetSystemMenu (hwnd, false);

            auto alpha = component.getAlpha();
            if (alpha < 1.0f)
                setAlpha (alpha);
        }
        else
        {
            TCHAR messageBuffer[256] = {};

            FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           nullptr, GetLastError(), MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                           messageBuffer, (DWORD) numElementsInArray (messageBuffer) - 1, nullptr);

            DBG (messageBuffer);
            jassertfalse;
        }
    }

    static BOOL CALLBACK revokeChildDragDropCallback (HWND hwnd, LPARAM)    { RevokeDragDrop (hwnd); return TRUE; }

    static void* destroyWindowCallback (void* handle)
    {
        auto hwnd = reinterpret_cast<HWND> (handle);

        if (IsWindow (hwnd))
        {
            RevokeDragDrop (hwnd);

            // NB: we need to do this before DestroyWindow() as child HWNDs will be invalid after
            EnumChildWindows (hwnd, revokeChildDragDropCallback, 0);

            DestroyWindow (hwnd);
        }

        return nullptr;
    }

    static void* toFrontCallback1 (void* h)
    {
        BringWindowToTop ((HWND) h);
        return nullptr;
    }

    static void* toFrontCallback2 (void* h)
    {
        setWindowZOrder ((HWND) h, HWND_TOP);
        return nullptr;
    }

    static void* setFocusCallback (void* h)
    {
        SetFocus ((HWND) h);
        return nullptr;
    }

    static void* getFocusCallback (void*)
    {
        return GetFocus();
    }

    bool isUsingUpdateLayeredWindow() const
    {
        return ! component.isOpaque();
    }

    bool hasTitleBar() const noexcept        { return (styleFlags & windowHasTitleBar) != 0; }

    void updateShadower()
    {
        if (! component.isCurrentlyModal() && (styleFlags & windowHasDropShadow) != 0
            && ((! hasTitleBar()) || SystemStats::getOperatingSystemType() < SystemStats::WinVista))
        {
            shadower = component.getLookAndFeel().createDropShadowerForComponent (component);

            if (shadower != nullptr)
                shadower->setOwner (&component);
        }
    }

    void setIcon (const Image& newIcon) override
    {
        if (IconConverters::IconPtr hicon { IconConverters::createHICONFromImage (newIcon, TRUE, 0, 0) })
        {
            SendMessage (hwnd, WM_SETICON, ICON_BIG,   reinterpret_cast<LPARAM> (hicon.get()));
            SendMessage (hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM> (hicon.get()));
            currentWindowIcon = std::move (hicon);
        }
    }

    void setMessageFilter()
    {
        using ChangeWindowMessageFilterExFunc = BOOL (WINAPI*) (HWND, UINT, DWORD, PVOID);

        static auto changeMessageFilter = (ChangeWindowMessageFilterExFunc) getUser32Function ("ChangeWindowMessageFilterEx");

        if (changeMessageFilter != nullptr)
        {
            changeMessageFilter (hwnd, WM_DROPFILES, 1 /*MSGFLT_ALLOW*/, nullptr);
            changeMessageFilter (hwnd, WM_COPYDATA, 1 /*MSGFLT_ALLOW*/, nullptr);
            changeMessageFilter (hwnd, 0x49, 1 /*MSGFLT_ALLOW*/, nullptr);
        }
    }

    struct ChildWindowClippingInfo
    {
        HDC dc;
        HWNDComponentPeer* peer;
        RectangleList<int>* clip;
        Point<int> origin;
        int savedDC;
    };

    static BOOL CALLBACK clipChildWindowCallback (HWND hwnd, LPARAM context)
    {
        if (IsWindowVisible (hwnd))
        {
            auto& info = *(ChildWindowClippingInfo*) context;

            if (GetParent (hwnd) == info.peer->hwnd)
            {
                auto clip = rectangleFromRECT (getWindowClientRect (hwnd));

                info.clip->subtract (clip - info.origin);

                if (info.savedDC == 0)
                    info.savedDC = SaveDC (info.dc);

                ExcludeClipRect (info.dc, clip.getX(), clip.getY(), clip.getRight(), clip.getBottom());
            }
        }

        return TRUE;
    }

    //==============================================================================
    void handlePaintMessage()
    {
       #if JUCE_DIRECT2D
        if (direct2DContext != nullptr)
        {
            RECT r;

            if (GetUpdateRect (hwnd, &r, false))
            {
                direct2DContext->start();
                direct2DContext->clipToRectangle (convertPhysicalScreenRectangleToLogical (rectangleFromRECT (r), hwnd));
                handlePaint (*direct2DContext);
                direct2DContext->end();
                ValidateRect (hwnd, &r);
            }
        }
        else
       #endif
        {
            HRGN rgn = CreateRectRgn (0, 0, 0, 0);
            const int regionType = GetUpdateRgn (hwnd, rgn, false);

            PAINTSTRUCT paintStruct;
            HDC dc = BeginPaint (hwnd, &paintStruct); // Note this can immediately generate a WM_NCPAINT
                                                      // message and become re-entrant, but that's OK

            // if something in a paint handler calls, e.g. a message box, this can become reentrant and
            // corrupt the image it's using to paint into, so do a check here.
            static bool reentrant = false;

            if (! reentrant)
            {
                const ScopedValueSetter<bool> setter (reentrant, true, false);

                if (dontRepaint)
                    component.handleCommandMessage (0); // (this triggers a repaint in the openGL context)
                else
                    performPaint (dc, rgn, regionType, paintStruct);
            }

            DeleteObject (rgn);
            EndPaint (hwnd, &paintStruct);

           #if JUCE_MSVC
            _fpreset(); // because some graphics cards can unmask FP exceptions
           #endif

        }

        lastPaintTime = Time::getMillisecondCounter();
    }

    void performPaint (HDC dc, HRGN rgn, int regionType, PAINTSTRUCT& paintStruct)
    {
        int x = paintStruct.rcPaint.left;
        int y = paintStruct.rcPaint.top;
        int w = paintStruct.rcPaint.right - x;
        int h = paintStruct.rcPaint.bottom - y;

        const bool transparent = isUsingUpdateLayeredWindow();

        if (transparent)
        {
            // it's not possible to have a transparent window with a title bar at the moment!
            jassert (! hasTitleBar());

            auto r = getWindowScreenRect (hwnd);
            x = y = 0;
            w = r.right - r.left;
            h = r.bottom - r.top;
        }

        if (w > 0 && h > 0)
        {
            Image& offscreenImage = offscreenImageGenerator.getImage (transparent, w, h);

            RectangleList<int> contextClip;
            const Rectangle<int> clipBounds (w, h);

            bool needToPaintAll = true;

            if (regionType == COMPLEXREGION && ! transparent)
            {
                HRGN clipRgn = CreateRectRgnIndirect (&paintStruct.rcPaint);
                CombineRgn (rgn, rgn, clipRgn, RGN_AND);
                DeleteObject (clipRgn);

                std::aligned_storage_t<8192, alignof (RGNDATA)> rgnData;
                const DWORD res = GetRegionData (rgn, sizeof (rgnData), (RGNDATA*) &rgnData);

                if (res > 0 && res <= sizeof (rgnData))
                {
                    const RGNDATAHEADER* const hdr = &(((const RGNDATA*) &rgnData)->rdh);

                    if (hdr->iType == RDH_RECTANGLES
                         && hdr->rcBound.right - hdr->rcBound.left >= w
                         && hdr->rcBound.bottom - hdr->rcBound.top >= h)
                    {
                        needToPaintAll = false;

                        auto rects = unalignedPointerCast<const RECT*> ((char*) &rgnData + sizeof (RGNDATAHEADER));

                        for (int i = (int) ((RGNDATA*) &rgnData)->rdh.nCount; --i >= 0;)
                        {
                            if (rects->right <= x + w && rects->bottom <= y + h)
                            {
                                const int cx = jmax (x, (int) rects->left);
                                contextClip.addWithoutMerging (Rectangle<int> (cx - x, rects->top - y,
                                                                               rects->right - cx, rects->bottom - rects->top)
                                                                   .getIntersection (clipBounds));
                            }
                            else
                            {
                                needToPaintAll = true;
                                break;
                            }

                            ++rects;
                        }
                    }
                }
            }

            if (needToPaintAll)
            {
                contextClip.clear();
                contextClip.addWithoutMerging (Rectangle<int> (w, h));
            }

            ChildWindowClippingInfo childClipInfo = { dc, this, &contextClip, Point<int> (x, y), 0 };
            EnumChildWindows (hwnd, clipChildWindowCallback, (LPARAM) &childClipInfo);

            if (! contextClip.isEmpty())
            {
                if (transparent)
                    for (auto& i : contextClip)
                        offscreenImage.clear (i);

                {
                    auto context = component.getLookAndFeel()
                                    .createGraphicsContext (offscreenImage, { -x, -y }, contextClip);

                    context->addTransform (AffineTransform::scale ((float) getPlatformScaleFactor()));
                    handlePaint (*context);
                }

                static_cast<WindowsBitmapImage*> (offscreenImage.getPixelData())
                    ->blitToWindow (hwnd, dc, transparent, x, y, updateLayeredWindowAlpha);
            }

            if (childClipInfo.savedDC != 0)
                RestoreDC (dc, childClipInfo.savedDC);
        }
    }

    //==============================================================================
    void doMouseEvent (Point<float> position, float pressure, float orientation = 0.0f, ModifierKeys mods = ModifierKeys::currentModifiers)
    {
        handleMouseEvent (MouseInputSource::InputSourceType::mouse, position, mods, pressure, orientation, getMouseEventTime());
    }

    StringArray getAvailableRenderingEngines() override
    {
        StringArray s ("Software Renderer");

       #if JUCE_DIRECT2D
        if (SystemStats::getOperatingSystemType() >= SystemStats::Windows7)
            s.add ("Direct2D");
       #endif

        return s;
    }

    int getCurrentRenderingEngine() const override    { return currentRenderingEngine; }

   #if JUCE_DIRECT2D
    void updateDirect2DContext()
    {
        if (currentRenderingEngine != direct2DRenderingEngine)
            direct2DContext = nullptr;
        else if (direct2DContext == nullptr)
            direct2DContext.reset (new Direct2DLowLevelGraphicsContext (hwnd));
    }
   #endif

    void setCurrentRenderingEngine ([[maybe_unused]] int index) override
    {
       #if JUCE_DIRECT2D
        if (getAvailableRenderingEngines().size() > 1)
        {
            currentRenderingEngine = index == 1 ? direct2DRenderingEngine : softwareRenderingEngine;
            updateDirect2DContext();
            repaint (component.getLocalBounds());
        }
       #endif
    }

    static uint32 getMinTimeBetweenMouseMoves()
    {
        if (SystemStats::getOperatingSystemType() >= SystemStats::WinVista)
            return 0;

        return 1000 / 60;  // Throttling the incoming mouse-events seems to still be needed in XP..
    }

    bool isTouchEvent() noexcept
    {
        if (registerTouchWindow == nullptr)
            return false;

        // Relevant info about touch/pen detection flags:
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms703320(v=vs.85).aspx
        // http://www.petertissen.de/?p=4

        return ((uint32_t) GetMessageExtraInfo() & 0xFFFFFF80 /*SIGNATURE_MASK*/) == 0xFF515780 /*MI_WP_SIGNATURE*/;
    }

    static bool areOtherTouchSourcesActive()
    {
        for (auto& ms : Desktop::getInstance().getMouseSources())
            if (ms.isDragging() && (ms.getType() == MouseInputSource::InputSourceType::touch
                                     || ms.getType() == MouseInputSource::InputSourceType::pen))
                return true;

        return false;
    }

    void doMouseMove (Point<float> position, bool isMouseDownEvent)
    {
        ModifierKeys modsToSend (ModifierKeys::currentModifiers);

        // this will be handled by WM_TOUCH
        if (isTouchEvent() || areOtherTouchSourcesActive())
            return;

        if (! isMouseOver)
        {
            isMouseOver = true;

            // This avoids a rare stuck-button problem when focus is lost unexpectedly, but must
            // not be called as part of a move, in case it's actually a mouse-drag from another
            // app which ends up here when we get focus before the mouse is released..
            if (isMouseDownEvent && getNativeRealtimeModifiers != nullptr)
                getNativeRealtimeModifiers();

            updateKeyModifiers();

           #if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
            if (modProvider != nullptr)
                ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withFlags (modProvider->getWin32Modifiers());
           #endif

            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof (tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            tme.dwHoverTime = 0;

            if (! TrackMouseEvent (&tme))
                jassertfalse;

            Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate();
        }
        else if (! isDragging)
        {
            if (! contains (position.roundToInt(), false))
                return;
        }

        static uint32 lastMouseTime = 0;
        static auto minTimeBetweenMouses = getMinTimeBetweenMouseMoves();
        auto now = Time::getMillisecondCounter();

        if (! Desktop::getInstance().getMainMouseSource().isDragging())
            modsToSend = modsToSend.withoutMouseButtons();

        if (now >= lastMouseTime + minTimeBetweenMouses)
        {
            lastMouseTime = now;
            doMouseEvent (position, MouseInputSource::defaultPressure,
                          MouseInputSource::defaultOrientation, modsToSend);
        }
    }

    void doMouseDown (Point<float> position, const WPARAM wParam)
    {
        // this will be handled by WM_TOUCH
        if (isTouchEvent() || areOtherTouchSourcesActive())
            return;

        if (GetCapture() != hwnd)
            SetCapture (hwnd);

        doMouseMove (position, true);

        if (isValidPeer (this))
        {
            updateModifiersFromWParam (wParam);

           #if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
            if (modProvider != nullptr)
                ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withFlags (modProvider->getWin32Modifiers());
           #endif

            isDragging = true;

            doMouseEvent (position, MouseInputSource::defaultPressure);
        }
    }

    void doMouseUp (Point<float> position, const WPARAM wParam)
    {
        // this will be handled by WM_TOUCH
        if (isTouchEvent() || areOtherTouchSourcesActive())
            return;

        updateModifiersFromWParam (wParam);

       #if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
        if (modProvider != nullptr)
            ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withFlags (modProvider->getWin32Modifiers());
       #endif

        const bool wasDragging = isDragging;
        isDragging = false;

        // release the mouse capture if the user has released all buttons
        if ((wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) == 0 && hwnd == GetCapture())
            ReleaseCapture();

        // NB: under some circumstances (e.g. double-clicking a native title bar), a mouse-up can
        // arrive without a mouse-down, so in that case we need to avoid sending a message.
        if (wasDragging)
            doMouseEvent (position, MouseInputSource::defaultPressure);
    }

    void doCaptureChanged()
    {
        if (constrainerIsResizing)
        {
            if (constrainer != nullptr)
                constrainer->resizeEnd();

            constrainerIsResizing = false;
        }

        if (isDragging)
            doMouseUp (getCurrentMousePos(), (WPARAM) 0);
    }

    void doMouseExit()
    {
        isMouseOver = false;

        if (! areOtherTouchSourcesActive())
            doMouseEvent (getCurrentMousePos(), MouseInputSource::defaultPressure);
    }

    ComponentPeer* findPeerUnderMouse (Point<float>& localPos)
    {
        auto currentMousePos = getPOINTFromLParam ((LPARAM) GetMessagePos());

        // Because Windows stupidly sends all wheel events to the window with the keyboard
        // focus, we have to redirect them here according to the mouse pos..
        auto* peer = getOwnerOfWindow (WindowFromPoint (currentMousePos));

        if (peer == nullptr)
            peer = this;

        localPos = peer->globalToLocal (convertPhysicalScreenPointToLogical (pointFromPOINT (currentMousePos), hwnd).toFloat());
        return peer;
    }

    static MouseInputSource::InputSourceType getPointerType (WPARAM wParam)
    {
        if (getPointerTypeFunction != nullptr)
        {
            POINTER_INPUT_TYPE pointerType;

            if (getPointerTypeFunction (GET_POINTERID_WPARAM (wParam), &pointerType))
            {
                if (pointerType == 2)
                    return MouseInputSource::InputSourceType::touch;

                if (pointerType == 3)
                    return MouseInputSource::InputSourceType::pen;
            }
        }

        return MouseInputSource::InputSourceType::mouse;
    }

    void doMouseWheel (const WPARAM wParam, const bool isVertical)
    {
        updateKeyModifiers();
        const float amount = jlimit (-1000.0f, 1000.0f, 0.5f * (short) HIWORD (wParam));

        MouseWheelDetails wheel;
        wheel.deltaX = isVertical ? 0.0f : amount / -256.0f;
        wheel.deltaY = isVertical ? amount / 256.0f : 0.0f;
        wheel.isReversed = false;
        wheel.isSmooth = false;
        wheel.isInertial = false;

        Point<float> localPos;

        if (auto* peer = findPeerUnderMouse (localPos))
            peer->handleMouseWheel (getPointerType (wParam), localPos, getMouseEventTime(), wheel);
    }

    bool doGestureEvent (LPARAM lParam)
    {
        GESTUREINFO gi;
        zerostruct (gi);
        gi.cbSize = sizeof (gi);

        if (getGestureInfo != nullptr && getGestureInfo ((HGESTUREINFO) lParam, &gi))
        {
            updateKeyModifiers();
            Point<float> localPos;

            if (auto* peer = findPeerUnderMouse (localPos))
            {
                switch (gi.dwID)
                {
                    case 3: /*GID_ZOOM*/
                        if (gi.dwFlags != 1 /*GF_BEGIN*/ && lastMagnifySize > 0)
                            peer->handleMagnifyGesture (MouseInputSource::InputSourceType::touch, localPos, getMouseEventTime(),
                                                        (float) ((double) gi.ullArguments / (double) lastMagnifySize));

                        lastMagnifySize = gi.ullArguments;
                        return true;

                    case 4: /*GID_PAN*/
                    case 5: /*GID_ROTATE*/
                    case 6: /*GID_TWOFINGERTAP*/
                    case 7: /*GID_PRESSANDTAP*/
                    default:
                        break;
                }
            }
        }

        return false;
    }

    LRESULT doTouchEvent (const int numInputs, HTOUCHINPUT eventHandle)
    {
        if ((styleFlags & windowIgnoresMouseClicks) != 0)
            if (auto* parent = getOwnerOfWindow (GetParent (hwnd)))
                if (parent != this)
                    return parent->doTouchEvent (numInputs, eventHandle);

        HeapBlock<TOUCHINPUT> inputInfo (numInputs);

        if (getTouchInputInfo (eventHandle, (UINT) numInputs, inputInfo, sizeof (TOUCHINPUT)))
        {
            for (int i = 0; i < numInputs; ++i)
            {
                auto flags = inputInfo[i].dwFlags;

                if ((flags & (TOUCHEVENTF_DOWN | TOUCHEVENTF_MOVE | TOUCHEVENTF_UP)) != 0)
                    if (! handleTouchInput (inputInfo[i], (flags & TOUCHEVENTF_DOWN) != 0, (flags & TOUCHEVENTF_UP) != 0))
                        return 0;  // abandon method if this window was deleted by the callback
            }
        }

        closeTouchInputHandle (eventHandle);
        return 0;
    }

    bool handleTouchInput (const TOUCHINPUT& touch, const bool isDown, const bool isUp,
                           const float touchPressure = MouseInputSource::defaultPressure,
                           const float orientation = 0.0f)
    {
        auto isCancel = false;

        const auto touchIndex = currentTouches.getIndexOfTouch (this, touch.dwID);
        const auto time = getMouseEventTime();
        const auto pos = globalToLocal (convertPhysicalScreenPointToLogical (pointFromPOINT ({ roundToInt (touch.x / 100.0f),
                                                                                               roundToInt (touch.y / 100.0f) }), hwnd).toFloat());
        const auto pressure = touchPressure;
        auto modsToSend = ModifierKeys::currentModifiers;

        if (isDown)
        {
            ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons().withFlags (ModifierKeys::leftButtonModifier);
            modsToSend = ModifierKeys::currentModifiers;

            // this forces a mouse-enter/up event, in case for some reason we didn't get a mouse-up before.
            handleMouseEvent (MouseInputSource::InputSourceType::touch, pos, modsToSend.withoutMouseButtons(),
                              pressure, orientation, time, {}, touchIndex);

            if (! isValidPeer (this)) // (in case this component was deleted by the event)
                return false;
        }
        else if (isUp)
        {
            modsToSend = modsToSend.withoutMouseButtons();
            ModifierKeys::currentModifiers = modsToSend;
            currentTouches.clearTouch (touchIndex);

            if (! currentTouches.areAnyTouchesActive())
                isCancel = true;
        }
        else
        {
            modsToSend = ModifierKeys::currentModifiers.withoutMouseButtons().withFlags (ModifierKeys::leftButtonModifier);
        }

        handleMouseEvent (MouseInputSource::InputSourceType::touch, pos, modsToSend,
                          pressure, orientation, time, {}, touchIndex);

        if (! isValidPeer (this))
            return false;

        if (isUp)
        {
            handleMouseEvent (MouseInputSource::InputSourceType::touch, MouseInputSource::offscreenMousePos, ModifierKeys::currentModifiers.withoutMouseButtons(),
                              pressure, orientation, time, {}, touchIndex);

            if (! isValidPeer (this))
                return false;

            if (isCancel)
            {
                currentTouches.clear();
                ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons();
            }
        }

        return true;
    }

    bool handlePointerInput (WPARAM wParam, LPARAM lParam, const bool isDown, const bool isUp)
    {
        if (! canUsePointerAPI)
            return false;

        auto pointerType = getPointerType (wParam);

        if (pointerType == MouseInputSource::InputSourceType::touch)
        {
            POINTER_TOUCH_INFO touchInfo;

            if (! getPointerTouchInfo (GET_POINTERID_WPARAM (wParam), &touchInfo))
                return false;

            const auto pressure = touchInfo.touchMask & TOUCH_MASK_PRESSURE ? static_cast<float> (touchInfo.pressure)
                                                                            : MouseInputSource::defaultPressure;
            const auto orientation = touchInfo.touchMask & TOUCH_MASK_ORIENTATION ? degreesToRadians (static_cast<float> (touchInfo.orientation))
                                                                                  : MouseInputSource::defaultOrientation;

            if (! handleTouchInput (emulateTouchEventFromPointer (touchInfo.pointerInfo.ptPixelLocationRaw, wParam),
                                    isDown, isUp, pressure, orientation))
                return false;
        }
        else if (pointerType == MouseInputSource::InputSourceType::pen)
        {
            POINTER_PEN_INFO penInfo;

            if (! getPointerPenInfo (GET_POINTERID_WPARAM (wParam), &penInfo))
                return false;

            const auto pressure = (penInfo.penMask & PEN_MASK_PRESSURE) ? (float) penInfo.pressure / 1024.0f : MouseInputSource::defaultPressure;

            if (! handlePenInput (penInfo, globalToLocal (convertPhysicalScreenPointToLogical (pointFromPOINT (getPOINTFromLParam (lParam)), hwnd).toFloat()),
                                  pressure, isDown, isUp))
                return false;
        }
        else
        {
            return false;
        }

        return true;
    }

    TOUCHINPUT emulateTouchEventFromPointer (POINT p, WPARAM wParam)
    {
        TOUCHINPUT touchInput;

        touchInput.dwID = GET_POINTERID_WPARAM (wParam);
        touchInput.x = p.x * 100;
        touchInput.y = p.y * 100;

        return touchInput;
    }

    bool handlePenInput (POINTER_PEN_INFO penInfo, Point<float> pos, const float pressure, bool isDown, bool isUp)
    {
        const auto time = getMouseEventTime();
        ModifierKeys modsToSend (ModifierKeys::currentModifiers);
        PenDetails penDetails;

        penDetails.rotation = (penInfo.penMask & PEN_MASK_ROTATION) ? degreesToRadians (static_cast<float> (penInfo.rotation)) : MouseInputSource::defaultRotation;
        penDetails.tiltX = (penInfo.penMask & PEN_MASK_TILT_X) ? (float) penInfo.tiltX / 90.0f : MouseInputSource::defaultTiltX;
        penDetails.tiltY = (penInfo.penMask & PEN_MASK_TILT_Y) ? (float) penInfo.tiltY / 90.0f : MouseInputSource::defaultTiltY;

        auto pInfoFlags = penInfo.pointerInfo.pointerFlags;

        if ((pInfoFlags & POINTER_FLAG_FIRSTBUTTON) != 0)
            ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons().withFlags (ModifierKeys::leftButtonModifier);
        else if ((pInfoFlags & POINTER_FLAG_SECONDBUTTON) != 0)
            ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons().withFlags (ModifierKeys::rightButtonModifier);

        if (isDown)
        {
            modsToSend = ModifierKeys::currentModifiers;

            // this forces a mouse-enter/up event, in case for some reason we didn't get a mouse-up before.
            handleMouseEvent (MouseInputSource::InputSourceType::pen, pos, modsToSend.withoutMouseButtons(),
                              pressure, MouseInputSource::defaultOrientation, time, penDetails);

            if (! isValidPeer (this)) // (in case this component was deleted by the event)
                return false;
        }
        else if (isUp || ! (pInfoFlags & POINTER_FLAG_INCONTACT))
        {
            modsToSend = modsToSend.withoutMouseButtons();
            ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons();
        }

        handleMouseEvent (MouseInputSource::InputSourceType::pen, pos, modsToSend, pressure,
                          MouseInputSource::defaultOrientation, time, penDetails);

        if (! isValidPeer (this)) // (in case this component was deleted by the event)
            return false;

        if (isUp)
        {
            handleMouseEvent (MouseInputSource::InputSourceType::pen, MouseInputSource::offscreenMousePos, ModifierKeys::currentModifiers,
                              pressure, MouseInputSource::defaultOrientation, time, penDetails);

            if (! isValidPeer (this))
                return false;
        }

        return true;
    }

    //==============================================================================
    void sendModifierKeyChangeIfNeeded()
    {
        if (modifiersAtLastCallback != ModifierKeys::currentModifiers)
        {
            modifiersAtLastCallback = ModifierKeys::currentModifiers;
            handleModifierKeysChange();
        }
    }

    bool doKeyUp (const WPARAM key)
    {
        updateKeyModifiers();

        switch (key)
        {
            case VK_SHIFT:
            case VK_CONTROL:
            case VK_MENU:
            case VK_CAPITAL:
            case VK_LWIN:
            case VK_RWIN:
            case VK_APPS:
            case VK_NUMLOCK:
            case VK_SCROLL:
            case VK_LSHIFT:
            case VK_RSHIFT:
            case VK_LCONTROL:
            case VK_LMENU:
            case VK_RCONTROL:
            case VK_RMENU:
                sendModifierKeyChangeIfNeeded();
        }

        return handleKeyUpOrDown (false)
                || Component::getCurrentlyModalComponent() != nullptr;
    }

    bool doKeyDown (const WPARAM key)
    {
        updateKeyModifiers();
        bool used = false;

        switch (key)
        {
            case VK_SHIFT:
            case VK_LSHIFT:
            case VK_RSHIFT:
            case VK_CONTROL:
            case VK_LCONTROL:
            case VK_RCONTROL:
            case VK_MENU:
            case VK_LMENU:
            case VK_RMENU:
            case VK_LWIN:
            case VK_RWIN:
            case VK_CAPITAL:
            case VK_NUMLOCK:
            case VK_SCROLL:
            case VK_APPS:
                used = handleKeyUpOrDown (true);
                sendModifierKeyChangeIfNeeded();
                break;

            case VK_LEFT:
            case VK_RIGHT:
            case VK_UP:
            case VK_DOWN:
            case VK_PRIOR:
            case VK_NEXT:
            case VK_HOME:
            case VK_END:
            case VK_DELETE:
            case VK_INSERT:
            case VK_F1:
            case VK_F2:
            case VK_F3:
            case VK_F4:
            case VK_F5:
            case VK_F6:
            case VK_F7:
            case VK_F8:
            case VK_F9:
            case VK_F10:
            case VK_F11:
            case VK_F12:
            case VK_F13:
            case VK_F14:
            case VK_F15:
            case VK_F16:
            case VK_F17:
            case VK_F18:
            case VK_F19:
            case VK_F20:
            case VK_F21:
            case VK_F22:
            case VK_F23:
            case VK_F24:
                used = handleKeyUpOrDown (true);
                used = handleKeyPress (extendedKeyModifier | (int) key, 0) || used;
                break;

            default:
                used = handleKeyUpOrDown (true);

                {
                    MSG msg;
                    if (! PeekMessage (&msg, hwnd, WM_CHAR, WM_DEADCHAR, PM_NOREMOVE))
                    {
                        // if there isn't a WM_CHAR or WM_DEADCHAR message pending, we need to
                        // manually generate the key-press event that matches this key-down.
                        const UINT keyChar  = MapVirtualKey ((UINT) key, 2);
                        const UINT scanCode = MapVirtualKey ((UINT) key, 0);
                        BYTE keyState[256];
                        [[maybe_unused]] const auto state = GetKeyboardState (keyState);

                        WCHAR text[16] = { 0 };
                        if (ToUnicode ((UINT) key, scanCode, keyState, text, 8, 0) != 1)
                            text[0] = 0;

                        used = handleKeyPress ((int) LOWORD (keyChar), (juce_wchar) text[0]) || used;
                    }
                }

                break;
        }

        return used || (Component::getCurrentlyModalComponent() != nullptr);
    }

    bool doKeyChar (int key, const LPARAM flags)
    {
        updateKeyModifiers();

        auto textChar = (juce_wchar) key;
        const int virtualScanCode = (flags >> 16) & 0xff;

        if (key >= '0' && key <= '9')
        {
            switch (virtualScanCode)  // check for a numeric keypad scan-code
            {
                case 0x52:
                case 0x4f:
                case 0x50:
                case 0x51:
                case 0x4b:
                case 0x4c:
                case 0x4d:
                case 0x47:
                case 0x48:
                case 0x49:
                    key = (key - '0') + KeyPress::numberPad0;
                    break;
                default:
                    break;
            }
        }
        else
        {
            // convert the scan code to an unmodified character code..
            const UINT virtualKey = MapVirtualKey ((UINT) virtualScanCode, 1);
            UINT keyChar = MapVirtualKey (virtualKey, 2);

            keyChar = LOWORD (keyChar);

            if (keyChar != 0)
                key = (int) keyChar;

            // avoid sending junk text characters for some control-key combinations
            if (textChar < ' ' && ModifierKeys::currentModifiers.testFlags (ModifierKeys::ctrlModifier | ModifierKeys::altModifier))
                textChar = 0;
        }

        return handleKeyPress (key, textChar);
    }

    void forwardMessageToParent (UINT message, WPARAM wParam, LPARAM lParam) const
    {
        if (HWND parentH = GetParent (hwnd))
            PostMessage (parentH, message, wParam, lParam);
    }

    bool doAppCommand (const LPARAM lParam)
    {
        int key = 0;

        switch (GET_APPCOMMAND_LPARAM (lParam))
        {
            case APPCOMMAND_MEDIA_PLAY_PAUSE:       key = KeyPress::playKey; break;
            case APPCOMMAND_MEDIA_STOP:             key = KeyPress::stopKey; break;
            case APPCOMMAND_MEDIA_NEXTTRACK:        key = KeyPress::fastForwardKey; break;
            case APPCOMMAND_MEDIA_PREVIOUSTRACK:    key = KeyPress::rewindKey; break;
            default: break;
        }

        if (key != 0)
        {
            updateKeyModifiers();

            if (hwnd == GetActiveWindow())
                return handleKeyPress (key, 0);
        }

        return false;
    }

    bool isConstrainedNativeWindow() const
    {
        return constrainer != nullptr
                && (styleFlags & (windowHasTitleBar | windowIsResizable)) == (windowHasTitleBar | windowIsResizable)
                && ! isKioskMode();
    }

    Rectangle<int> getCurrentScaledBounds() const
    {
        return ScalingHelpers::unscaledScreenPosToScaled (component, windowBorder.addedTo (ScalingHelpers::scaledScreenPosToUnscaled (component, component.getBounds())));
    }

    LRESULT handleSizeConstraining (RECT& r, const WPARAM wParam)
    {
        if (isConstrainedNativeWindow())
        {
            const auto logicalBounds = convertPhysicalScreenRectangleToLogical (rectangleFromRECT (r).toFloat(), hwnd);
            auto pos = ScalingHelpers::unscaledScreenPosToScaled (component, logicalBounds).toNearestInt();

            const auto original = getCurrentScaledBounds();

            constrainer->checkBounds (pos, original,
                                      Desktop::getInstance().getDisplays().getTotalBounds (true),
                                      wParam == WMSZ_TOP    || wParam == WMSZ_TOPLEFT    || wParam == WMSZ_TOPRIGHT,
                                      wParam == WMSZ_LEFT   || wParam == WMSZ_TOPLEFT    || wParam == WMSZ_BOTTOMLEFT,
                                      wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT,
                                      wParam == WMSZ_RIGHT  || wParam == WMSZ_TOPRIGHT   || wParam == WMSZ_BOTTOMRIGHT);

            r = RECTFromRectangle (convertLogicalScreenRectangleToPhysical (ScalingHelpers::scaledScreenPosToUnscaled (component, pos.toFloat()).toNearestInt(), hwnd));
        }

        return TRUE;
    }

    LRESULT handlePositionChanging (WINDOWPOS& wp)
    {
        if (isConstrainedNativeWindow() && ! isFullScreen())
        {
            if ((wp.flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE)
                 && (wp.x > -32000 && wp.y > -32000)
                 && ! Component::isMouseButtonDownAnywhere())
            {
                const auto logicalBounds = convertPhysicalScreenRectangleToLogical (rectangleFromRECT ({ wp.x, wp.y, wp.x + wp.cx, wp.y + wp.cy }).toFloat(), hwnd);
                auto pos = ScalingHelpers::unscaledScreenPosToScaled (component, logicalBounds).toNearestInt();

                const auto original = getCurrentScaledBounds();

                constrainer->checkBounds (pos, original,
                                          Desktop::getInstance().getDisplays().getTotalBounds (true),
                                          pos.getY() != original.getY() && pos.getBottom() == original.getBottom(),
                                          pos.getX() != original.getX() && pos.getRight()  == original.getRight(),
                                          pos.getY() == original.getY() && pos.getBottom() != original.getBottom(),
                                          pos.getX() == original.getX() && pos.getRight()  != original.getRight());

                auto physicalBounds = convertLogicalScreenRectangleToPhysical (ScalingHelpers::scaledScreenPosToUnscaled (component, pos.toFloat()), hwnd);

                auto getNewPositionIfNotRoundingError = [] (int posIn, float newPos)
                {
                    return (std::abs ((float) posIn - newPos) >= 1.0f) ? roundToInt (newPos) : posIn;
                };

                wp.x  = getNewPositionIfNotRoundingError (wp.x,  physicalBounds.getX());
                wp.y  = getNewPositionIfNotRoundingError (wp.y,  physicalBounds.getY());
                wp.cx = getNewPositionIfNotRoundingError (wp.cx, physicalBounds.getWidth());
                wp.cy = getNewPositionIfNotRoundingError (wp.cy, physicalBounds.getHeight());
            }
        }

        if (((wp.flags & SWP_SHOWWINDOW) != 0 && ! component.isVisible()))
            component.setVisible (true);
        else if (((wp.flags & SWP_HIDEWINDOW) != 0 && component.isVisible()))
            component.setVisible (false);

        return 0;
    }

    enum class ForceRefreshDispatcher
    {
        no,
        yes
    };

    void updateCurrentMonitorAndRefreshVBlankDispatcher (ForceRefreshDispatcher force = ForceRefreshDispatcher::no)
    {
        auto monitor = MonitorFromWindow (hwnd, MONITOR_DEFAULTTONULL);

        if (std::exchange (currentMonitor, monitor) != monitor || force == ForceRefreshDispatcher::yes)
            VBlankDispatcher::getInstance()->updateDisplay (*this, currentMonitor);
    }

    bool handlePositionChanged()
    {
        auto pos = getCurrentMousePos();

        if (contains (pos.roundToInt(), false))
        {
            const ScopedValueSetter<bool> scope (inHandlePositionChanged, true);

            if (! areOtherTouchSourcesActive())
                doMouseEvent (pos, MouseInputSource::defaultPressure);

            if (! isValidPeer (this))
                return true;
        }

        handleMovedOrResized();
        updateCurrentMonitorAndRefreshVBlankDispatcher();

        return ! dontRepaint; // to allow non-accelerated openGL windows to draw themselves correctly.
    }

    //==============================================================================
    LRESULT handleDPIChanging (int newDPI, RECT newRect)
    {
        // Sometimes, windows that should not be automatically scaled (secondary windows in plugins)
        // are sent WM_DPICHANGED. The size suggested by the OS is incorrect for our unscaled
        // window, so we should ignore it.
        if (! isPerMonitorDPIAwareWindow (hwnd))
            return 0;

        const auto newScale = (double) newDPI / USER_DEFAULT_SCREEN_DPI;

        if (approximatelyEqual (scaleFactor, newScale))
            return 0;

        scaleFactor = newScale;

        {
            const ScopedValueSetter<bool> setter (inDpiChange, true);
            SetWindowPos (hwnd,
                          nullptr,
                          newRect.left,
                          newRect.top,
                          newRect.right  - newRect.left,
                          newRect.bottom - newRect.top,
                          SWP_NOZORDER | SWP_NOACTIVATE);
        }

        // This is to handle reentrancy. If responding to a DPI change triggers further DPI changes,
        // we should only notify listeners and resize windows once all of the DPI changes have
        // resolved.
        if (inDpiChange)
        {
            // Danger! Re-entrant call to handleDPIChanging.
            // Please report this issue on the JUCE forum, along with instructions
            // so that a JUCE developer can reproduce the issue.
            jassertfalse;
            return 0;
        }

        updateShadower();
        InvalidateRect (hwnd, nullptr, FALSE);

        scaleFactorListeners.call ([this] (ScaleFactorListener& l) { l.nativeScaleFactorChanged (scaleFactor); });

        return 0;
    }

    //==============================================================================
    void handleAppActivation (const WPARAM wParam)
    {
        modifiersAtLastCallback = -1;
        updateKeyModifiers();

        if (isMinimised())
        {
            component.repaint();
            handleMovedOrResized();

            if (! isValidPeer (this))
                return;
        }

        auto* underMouse = component.getComponentAt (component.getMouseXYRelative());

        if (underMouse == nullptr)
            underMouse = &component;

        if (underMouse->isCurrentlyBlockedByAnotherModalComponent())
        {
            if (LOWORD (wParam) == WA_CLICKACTIVE)
                Component::getCurrentlyModalComponent()->inputAttemptWhenModal();
            else
                ModalComponentManager::getInstance()->bringModalComponentsToFront();
        }
        else
        {
            handleBroughtToFront();
        }
    }

    void handlePowerBroadcast (WPARAM wParam)
    {
        if (auto* app = JUCEApplicationBase::getInstance())
        {
            switch (wParam)
            {
                case PBT_APMSUSPEND:                app->suspended(); break;

                case PBT_APMQUERYSUSPENDFAILED:
                case PBT_APMRESUMECRITICAL:
                case PBT_APMRESUMESUSPEND:
                case PBT_APMRESUMEAUTOMATIC:        app->resumed(); break;

                default: break;
            }
        }
    }

    void handleLeftClickInNCArea (WPARAM wParam)
    {
        if (! sendInputAttemptWhenModalMessage())
        {
            switch (wParam)
            {
            case HTBOTTOM:
            case HTBOTTOMLEFT:
            case HTBOTTOMRIGHT:
            case HTGROWBOX:
            case HTLEFT:
            case HTRIGHT:
            case HTTOP:
            case HTTOPLEFT:
            case HTTOPRIGHT:
                if (isConstrainedNativeWindow())
                {
                    constrainerIsResizing = true;
                    constrainer->resizeStart();
                }
                break;

            default:
                break;
            }
        }
    }

    void initialiseSysMenu (HMENU menu) const
    {
        if (! hasTitleBar())
        {
            if (isFullScreen())
            {
                EnableMenuItem (menu, SC_RESTORE,  MF_BYCOMMAND | MF_ENABLED);
                EnableMenuItem (menu, SC_MOVE,     MF_BYCOMMAND | MF_GRAYED);
            }
            else if (! isMinimised())
            {
                EnableMenuItem (menu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);
            }
        }
    }

    void doSettingChange()
    {
        forceDisplayUpdate();

        if (fullScreen && ! isMinimised())
            setWindowPos (hwnd, ScalingHelpers::scaledScreenPosToUnscaled (component, Desktop::getInstance().getDisplays()
                                                                                              .getDisplayForRect (component.getScreenBounds())->userArea),
                          SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSENDCHANGING);

        auto* dispatcher = VBlankDispatcher::getInstance();
        dispatcher->reconfigureDisplays();
        updateCurrentMonitorAndRefreshVBlankDispatcher (ForceRefreshDispatcher::yes);
    }

    //==============================================================================
   #if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
    void setModifierKeyProvider (ModifierKeyProvider* provider) override
    {
        modProvider = provider;
    }

    void removeModifierKeyProvider() override
    {
        modProvider = nullptr;
    }
   #endif

public:
    static LRESULT CALLBACK windowProc (HWND h, UINT message, WPARAM wParam, LPARAM lParam)
    {
        // Ensure that non-client areas are scaled for per-monitor DPI awareness v1 - can't
        // do this in peerWindowProc as we have no window at this point
        if (message == WM_NCCREATE && enableNonClientDPIScaling != nullptr)
            enableNonClientDPIScaling (h);

        if (auto* peer = getOwnerOfWindow (h))
        {
            jassert (isValidPeer (peer));
            return peer->peerWindowProc (h, message, wParam, lParam);
        }

        return DefWindowProcW (h, message, wParam, lParam);
    }

private:
    static void* callFunctionIfNotLocked (MessageCallbackFunction* callback, void* userData)
    {
        auto& mm = *MessageManager::getInstance();

        if (mm.currentThreadHasLockedMessageManager())
            return callback (userData);

        return mm.callFunctionOnMessageThread (callback, userData);
    }

    static POINT getPOINTFromLParam (LPARAM lParam) noexcept
    {
        return { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
    }

    Point<float> getPointFromLocalLParam (LPARAM lParam) noexcept
    {
        auto p = pointFromPOINT (getPOINTFromLParam (lParam));

        if (isPerMonitorDPIAwareWindow (hwnd))
        {
            // LPARAM is relative to this window's top-left but may be on a different monitor so we need to calculate the
            // physical screen position and then convert this to local logical coordinates
            auto r = getWindowScreenRect (hwnd);
            return globalToLocal (Desktop::getInstance().getDisplays().physicalToLogical (pointFromPOINT ({ r.left + p.x + roundToInt (windowBorder.getLeft() * scaleFactor),
                                                                                                            r.top  + p.y + roundToInt (windowBorder.getTop()  * scaleFactor) })).toFloat());
        }

        return p.toFloat();
    }

    Point<float> getCurrentMousePos() noexcept
    {
        return globalToLocal (convertPhysicalScreenPointToLogical (pointFromPOINT (getPOINTFromLParam ((LPARAM) GetMessagePos())), hwnd).toFloat());
    }

    LRESULT peerWindowProc (HWND h, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            //==============================================================================
            case WM_NCHITTEST:
                if ((styleFlags & windowIgnoresMouseClicks) != 0)
                    return HTTRANSPARENT;

                if (! hasTitleBar())
                    return HTCLIENT;

                break;

            //==============================================================================
            case WM_PAINT:
                handlePaintMessage();
                return 0;

            case WM_NCPAINT:
                handlePaintMessage(); // this must be done, even with native titlebars, or there are rendering artifacts.

                if (hasTitleBar())
                    break; // let the DefWindowProc handle drawing the frame.

                return 0;

            case WM_ERASEBKGND:
            case WM_NCCALCSIZE:
                if (hasTitleBar())
                    break;

                return 1;

            //==============================================================================
            case WM_POINTERUPDATE:
                if (handlePointerInput (wParam, lParam, false, false))
                    return 0;
                break;

            case WM_POINTERDOWN:
                if (handlePointerInput (wParam, lParam, true, false))
                    return 0;
                break;

            case WM_POINTERUP:
                if (handlePointerInput (wParam, lParam, false, true))
                    return 0;
                break;

            //==============================================================================
            case WM_MOUSEMOVE:          doMouseMove (getPointFromLocalLParam (lParam), false); return 0;

            case WM_POINTERLEAVE:
            case WM_MOUSELEAVE:         doMouseExit(); return 0;

            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:        doMouseDown (getPointFromLocalLParam (lParam), wParam); return 0;

            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:          doMouseUp (getPointFromLocalLParam (lParam), wParam); return 0;

            case WM_POINTERWHEEL:
            case 0x020A: /* WM_MOUSEWHEEL */   doMouseWheel (wParam, true);  return 0;

            case WM_POINTERHWHEEL:
            case 0x020E: /* WM_MOUSEHWHEEL */  doMouseWheel (wParam, false); return 0;

            case WM_CAPTURECHANGED:     doCaptureChanged(); return 0;

            case WM_NCPOINTERUPDATE:
            case WM_NCMOUSEMOVE:
                if (hasTitleBar())
                    break;

                return 0;

            case WM_TOUCH:
                if (getTouchInputInfo != nullptr)
                    return doTouchEvent ((int) wParam, (HTOUCHINPUT) lParam);

                break;

            case 0x119: /* WM_GESTURE */
                if (doGestureEvent (lParam))
                    return 0;

                break;

            //==============================================================================
            case WM_SIZING:                  return handleSizeConstraining (*(RECT*) lParam, wParam);
            case WM_WINDOWPOSCHANGING:       return handlePositionChanging (*(WINDOWPOS*) lParam);
            case 0x2e0: /* WM_DPICHANGED */  return handleDPIChanging ((int) HIWORD (wParam), *(RECT*) lParam);

            case WM_WINDOWPOSCHANGED:
            {
                const WINDOWPOS& wPos = *reinterpret_cast<WINDOWPOS*> (lParam);

                if ((wPos.flags & SWP_NOMOVE) != 0 && (wPos.flags & SWP_NOSIZE) != 0)
                    startTimer (100);
                else
                    if (handlePositionChanged())
                        return 0;
            }
            break;

            //==============================================================================
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                if (doKeyDown (wParam))
                    return 0;

                forwardMessageToParent (message, wParam, lParam);
                break;

            case WM_KEYUP:
            case WM_SYSKEYUP:
                if (doKeyUp (wParam))
                    return 0;

                forwardMessageToParent (message, wParam, lParam);
                break;

            case WM_CHAR:
                if (doKeyChar ((int) wParam, lParam))
                    return 0;

                forwardMessageToParent (message, wParam, lParam);
                break;

            case WM_APPCOMMAND:
                if (doAppCommand (lParam))
                    return TRUE;

                break;

            case WM_MENUCHAR: // triggered when alt+something is pressed
                return MNC_CLOSE << 16; // (avoids making the default system beep)

            //==============================================================================
            case WM_SETFOCUS:
                updateKeyModifiers();
                handleFocusGain();
                break;

            case WM_KILLFOCUS:
                if (hasCreatedCaret)
                {
                    hasCreatedCaret = false;
                    DestroyCaret();
                }

                handleFocusLoss();

                if (auto* modal = Component::getCurrentlyModalComponent())
                    if (auto* peer = modal->getPeer())
                        if ((peer->getStyleFlags() & ComponentPeer::windowIsTemporary) != 0)
                            sendInputAttemptWhenModalMessage();

                break;

            case WM_ACTIVATEAPP:
                // Windows does weird things to process priority when you swap apps,
                // so this forces an update when the app is brought to the front
                if (wParam != FALSE)
                    juce_repeatLastProcessPriority();
                else
                    Desktop::getInstance().setKioskModeComponent (nullptr); // turn kiosk mode off if we lose focus

                juce_checkCurrentlyFocusedTopLevelWindow();
                modifiersAtLastCallback = -1;
                return 0;

            case WM_ACTIVATE:
                if (LOWORD (wParam) == WA_ACTIVE || LOWORD (wParam) == WA_CLICKACTIVE)
                {
                    handleAppActivation (wParam);
                    return 0;
                }

                break;

            case WM_NCACTIVATE:
                // while a temporary window is being shown, prevent Windows from deactivating the
                // title bars of our main windows.
                if (wParam == 0 && ! shouldDeactivateTitleBar)
                    wParam = TRUE; // change this and let it get passed to the DefWindowProc.

                break;

            case WM_POINTERACTIVATE:
            case WM_MOUSEACTIVATE:
                if (! component.getMouseClickGrabsKeyboardFocus())
                    return MA_NOACTIVATE;

                break;

            case WM_SHOWWINDOW:
                if (wParam != 0)
                {
                    component.setVisible (true);
                    handleBroughtToFront();
                }

                break;

            case WM_CLOSE:
                if (! component.isCurrentlyBlockedByAnotherModalComponent())
                    handleUserClosingWindow();

                return 0;

           #if JUCE_REMOVE_COMPONENT_FROM_DESKTOP_ON_WM_DESTROY
            case WM_DESTROY:
                getComponent().removeFromDesktop();
                return 0;
           #endif

            case WM_QUERYENDSESSION:
                if (auto* app = JUCEApplicationBase::getInstance())
                {
                    app->systemRequestedQuit();
                    return MessageManager::getInstance()->hasStopMessageBeenSent();
                }
                return TRUE;

            case WM_POWERBROADCAST:
                handlePowerBroadcast (wParam);
                break;

            case WM_SYNCPAINT:
                return 0;

            case WM_DISPLAYCHANGE:
                InvalidateRect (h, nullptr, 0);
                // intentional fall-through...
                JUCE_FALLTHROUGH
            case WM_SETTINGCHANGE:  // note the fall-through in the previous case!
                doSettingChange();
                break;

            case WM_INITMENU:
                initialiseSysMenu ((HMENU) wParam);
                break;

            case WM_SYSCOMMAND:
                switch (wParam & 0xfff0)
                {
                case SC_CLOSE:
                    if (sendInputAttemptWhenModalMessage())
                        return 0;

                    if (hasTitleBar())
                    {
                        PostMessage (h, WM_CLOSE, 0, 0);
                        return 0;
                    }
                    break;

                case SC_KEYMENU:
                   #if ! JUCE_WINDOWS_ALT_KEY_TRIGGERS_MENU
                    // This test prevents a press of the ALT key from triggering the ancient top-left window menu.
                    // By default we suppress this behaviour because it's unlikely that more than a tiny subset of
                    // our users will actually want it, and it causes problems if you're trying to use the ALT key
                    // as a modifier for mouse actions. If you really need the old behaviour, then just define
                    // JUCE_WINDOWS_ALT_KEY_TRIGGERS_MENU=1 in your app.
                    if ((lParam >> 16) <= 0) // Values above zero indicate that a mouse-click triggered the menu
                        return 0;
                   #endif

                    // (NB mustn't call sendInputAttemptWhenModalMessage() here because of very obscure
                    // situations that can arise if a modal loop is started from an alt-key keypress).
                    if (hasTitleBar() && h == GetCapture())
                        ReleaseCapture();

                    break;

                case SC_MAXIMIZE:
                    if (! sendInputAttemptWhenModalMessage())
                        setFullScreen (true);

                    return 0;

                case SC_MINIMIZE:
                    if (sendInputAttemptWhenModalMessage())
                        return 0;

                    if (! hasTitleBar())
                    {
                        setMinimised (true);
                        return 0;
                    }
                    break;

                case SC_RESTORE:
                    if (sendInputAttemptWhenModalMessage())
                        return 0;

                    if (hasTitleBar())
                    {
                        if (isFullScreen())
                        {
                            setFullScreen (false);
                            return 0;
                        }
                    }
                    else
                    {
                        if (isMinimised())
                            setMinimised (false);
                        else if (isFullScreen())
                            setFullScreen (false);

                        return 0;
                    }
                    break;
                }

                break;

            case WM_NCPOINTERDOWN:
            case WM_NCLBUTTONDOWN:
                handleLeftClickInNCArea (wParam);
                break;

            case WM_NCRBUTTONDOWN:
            case WM_NCMBUTTONDOWN:
                sendInputAttemptWhenModalMessage();
                break;

            case WM_IME_SETCONTEXT:
                imeHandler.handleSetContext (h, wParam == TRUE);
                lParam &= ~(LPARAM) ISC_SHOWUICOMPOSITIONWINDOW;
                break;

            case WM_IME_STARTCOMPOSITION:  imeHandler.handleStartComposition (*this); return 0;
            case WM_IME_ENDCOMPOSITION:    imeHandler.handleEndComposition (*this, h); break;
            case WM_IME_COMPOSITION:       imeHandler.handleComposition (*this, h, lParam); return 0;

            case WM_GETDLGCODE:
                return DLGC_WANTALLKEYS;

            case WM_GETOBJECT:
            {
                if (static_cast<long> (lParam) == WindowsAccessibility::getUiaRootObjectId())
                {
                    if (auto* handler = component.getAccessibilityHandler())
                    {
                        LRESULT res = 0;

                        if (WindowsAccessibility::handleWmGetObject (handler, wParam, lParam, &res))
                        {
                            isAccessibilityActive = true;
                            return res;
                        }
                    }
                }

                break;
            }
            default:
                break;
        }

        return DefWindowProcW (h, message, wParam, lParam);
    }

    bool sendInputAttemptWhenModalMessage()
    {
        if (! component.isCurrentlyBlockedByAnotherModalComponent())
            return false;

        if (auto* current = Component::getCurrentlyModalComponent())
            if (auto* owner = getOwnerOfWindow ((HWND) current->getWindowHandle()))
                if (! owner->shouldIgnoreModalDismiss)
                    current->inputAttemptWhenModal();

        return true;
    }

    //==============================================================================
    struct IMEHandler
    {
        IMEHandler()
        {
            reset();
        }

        void handleSetContext (HWND hWnd, const bool windowIsActive)
        {
            if (compositionInProgress && ! windowIsActive)
            {
                if (HIMC hImc = ImmGetContext (hWnd))
                {
                    ImmNotifyIME (hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
                    ImmReleaseContext (hWnd, hImc);
                }

                // If the composition is still in progress, calling ImmNotifyIME may call back
                // into handleComposition to let us know that the composition has finished.
                // We need to set compositionInProgress *after* calling handleComposition, so that
                // the text replaces the current selection, rather than being inserted after the
                // caret.
                compositionInProgress = false;
            }
        }

        void handleStartComposition (ComponentPeer& owner)
        {
            reset();

            if (auto* target = owner.findCurrentTextInputTarget())
                target->insertTextAtCaret (String());
        }

        void handleEndComposition (ComponentPeer& owner, HWND hWnd)
        {
            if (compositionInProgress)
            {
                // If this occurs, the user has cancelled the composition, so clear their changes..
                if (auto* target = owner.findCurrentTextInputTarget())
                {
                    target->setHighlightedRegion (compositionRange);
                    target->insertTextAtCaret (String());
                    compositionRange.setLength (0);

                    target->setHighlightedRegion (Range<int>::emptyRange (compositionRange.getEnd()));
                    target->setTemporaryUnderlining ({});
                }

                if (auto hImc = ImmGetContext (hWnd))
                {
                    ImmNotifyIME (hImc, NI_CLOSECANDIDATE, 0, 0);
                    ImmReleaseContext (hWnd, hImc);
                }
            }

            reset();
        }

        void handleComposition (ComponentPeer& owner, HWND hWnd, const LPARAM lParam)
        {
            if (auto* target = owner.findCurrentTextInputTarget())
            {
                if (auto hImc = ImmGetContext (hWnd))
                {
                    if (compositionRange.getStart() < 0)
                        compositionRange = Range<int>::emptyRange (target->getHighlightedRegion().getStart());

                    if ((lParam & GCS_RESULTSTR) != 0) // (composition has finished)
                    {
                        replaceCurrentSelection (target, getCompositionString (hImc, GCS_RESULTSTR),
                                                 Range<int>::emptyRange (-1));

                        reset();
                        target->setTemporaryUnderlining ({});
                    }
                    else if ((lParam & GCS_COMPSTR) != 0) // (composition is still in-progress)
                    {
                        replaceCurrentSelection (target, getCompositionString (hImc, GCS_COMPSTR),
                                                 getCompositionSelection (hImc, lParam));

                        target->setTemporaryUnderlining (getCompositionUnderlines (hImc, lParam));
                        compositionInProgress = true;
                    }

                    moveCandidateWindowToLeftAlignWithSelection (hImc, owner, target);
                    ImmReleaseContext (hWnd, hImc);
                }
            }
        }

    private:
        //==============================================================================
        Range<int> compositionRange; // The range being modified in the TextInputTarget
        bool compositionInProgress;

        //==============================================================================
        void reset()
        {
            compositionRange = Range<int>::emptyRange (-1);
            compositionInProgress = false;
        }

        String getCompositionString (HIMC hImc, const DWORD type) const
        {
            jassert (hImc != HIMC{});

            const auto stringSizeBytes = ImmGetCompositionString (hImc, type, nullptr, 0);

            if (stringSizeBytes > 0)
            {
                HeapBlock<TCHAR> buffer;
                buffer.calloc ((size_t) stringSizeBytes / sizeof (TCHAR) + 1);
                ImmGetCompositionString (hImc, type, buffer, (DWORD) stringSizeBytes);
                return String (buffer.get());
            }

            return {};
        }

        int getCompositionCaretPos (HIMC hImc, LPARAM lParam, const String& currentIMEString) const
        {
            jassert (hImc != HIMC{});

            if ((lParam & CS_NOMOVECARET) != 0)
                return compositionRange.getStart();

            if ((lParam & GCS_CURSORPOS) != 0)
            {
                const int localCaretPos = ImmGetCompositionString (hImc, GCS_CURSORPOS, nullptr, 0);
                return compositionRange.getStart() + jmax (0, localCaretPos);
            }

            return compositionRange.getStart() + currentIMEString.length();
        }

        // Get selected/highlighted range while doing composition:
        // returned range is relative to beginning of TextInputTarget, not composition string
        Range<int> getCompositionSelection (HIMC hImc, LPARAM lParam) const
        {
            jassert (hImc != HIMC{});
            int selectionStart = 0;
            int selectionEnd = 0;

            if ((lParam & GCS_COMPATTR) != 0)
            {
                // Get size of attributes array:
                const int attributeSizeBytes = ImmGetCompositionString (hImc, GCS_COMPATTR, nullptr, 0);

                if (attributeSizeBytes > 0)
                {
                    // Get attributes (8 bit flag per character):
                    HeapBlock<char> attributes (attributeSizeBytes);
                    ImmGetCompositionString (hImc, GCS_COMPATTR, attributes, (DWORD) attributeSizeBytes);

                    selectionStart = 0;

                    for (selectionStart = 0; selectionStart < attributeSizeBytes; ++selectionStart)
                        if (attributes[selectionStart] == ATTR_TARGET_CONVERTED || attributes[selectionStart] == ATTR_TARGET_NOTCONVERTED)
                            break;

                    for (selectionEnd = selectionStart; selectionEnd < attributeSizeBytes; ++selectionEnd)
                        if (attributes[selectionEnd] != ATTR_TARGET_CONVERTED && attributes[selectionEnd] != ATTR_TARGET_NOTCONVERTED)
                            break;
                }
            }

            return Range<int> (selectionStart, selectionEnd) + compositionRange.getStart();
        }

        void replaceCurrentSelection (TextInputTarget* const target, const String& newContent, Range<int> newSelection)
        {
            if (compositionInProgress)
                target->setHighlightedRegion (compositionRange);

            target->insertTextAtCaret (newContent);
            compositionRange.setLength (newContent.length());

            if (newSelection.getStart() < 0)
                newSelection = Range<int>::emptyRange (compositionRange.getEnd());

            target->setHighlightedRegion (newSelection);
        }

        Array<Range<int>> getCompositionUnderlines (HIMC hImc, LPARAM lParam) const
        {
            Array<Range<int>> result;

            if (hImc != HIMC{} && (lParam & GCS_COMPCLAUSE) != 0)
            {
                auto clauseDataSizeBytes = ImmGetCompositionString (hImc, GCS_COMPCLAUSE, nullptr, 0);

                if (clauseDataSizeBytes > 0)
                {
                    const auto numItems = (size_t) clauseDataSizeBytes / sizeof (uint32);
                    HeapBlock<uint32> clauseData (numItems);

                    if (ImmGetCompositionString (hImc, GCS_COMPCLAUSE, clauseData, (DWORD) clauseDataSizeBytes) > 0)
                        for (size_t i = 0; i + 1 < numItems; ++i)
                            result.add (Range<int> ((int) clauseData[i], (int) clauseData[i + 1]) + compositionRange.getStart());
                }
            }

            return result;
        }

        void moveCandidateWindowToLeftAlignWithSelection (HIMC hImc, ComponentPeer& peer, TextInputTarget* target) const
        {
            if (auto* targetComp = dynamic_cast<Component*> (target))
            {
                auto area = peer.getComponent().getLocalArea (targetComp, target->getCaretRectangle());

                CANDIDATEFORM pos = { 0, CFS_CANDIDATEPOS, { area.getX(), area.getBottom() }, { 0, 0, 0, 0 } };
                ImmSetCandidateWindow (hImc, &pos);
            }
        }

        JUCE_DECLARE_NON_COPYABLE (IMEHandler)
    };

    void timerCallback() override
    {
        handlePositionChanged();
        stopTimer();
    }

    static bool isAncestor (HWND outer, HWND inner)
    {
        if (outer == nullptr || inner == nullptr)
            return false;

        if (outer == inner)
            return true;

        return isAncestor (outer, GetAncestor (inner, GA_PARENT));
    }

    void windowShouldDismissModals (HWND originator)
    {
        if (shouldIgnoreModalDismiss)
            return;

        if (isAncestor (originator, hwnd))
            sendInputAttemptWhenModalMessage();
    }

    // Unfortunately SetWindowsHookEx only allows us to register a static function as a hook.
    // To get around this, we keep a static list of listeners which are interested in
    // top-level window events, and notify all of these listeners from the callback.
    class TopLevelModalDismissBroadcaster
    {
    public:
        TopLevelModalDismissBroadcaster()
            : hook (SetWindowsHookEx (WH_CALLWNDPROC,
                                      callWndProc,
                                      (HINSTANCE) juce::Process::getCurrentModuleInstanceHandle(),
                                      GetCurrentThreadId()))
        {}

        ~TopLevelModalDismissBroadcaster() noexcept
        {
            UnhookWindowsHookEx (hook);
        }

    private:
        static void processMessage (int nCode, const CWPSTRUCT* info)
        {
            if (nCode < 0 || info == nullptr)
                return;

            constexpr UINT events[] { WM_MOVE,
                                      WM_SIZE,
                                      WM_WINDOWPOSCHANGING,
                                      WM_NCPOINTERDOWN,
                                      WM_NCLBUTTONDOWN,
                                      WM_NCRBUTTONDOWN,
                                      WM_NCMBUTTONDOWN };

            if (std::find (std::begin (events), std::end (events), info->message) == std::end (events))
                return;

            if (info->message == WM_WINDOWPOSCHANGING)
            {
                const auto* windowPos = reinterpret_cast<const WINDOWPOS*> (info->lParam);
                const auto windowPosFlags = windowPos->flags;

                constexpr auto maskToCheck = SWP_NOMOVE | SWP_NOSIZE;

                if ((windowPosFlags & maskToCheck) == maskToCheck)
                    return;
            }

            // windowMayDismissModals could affect the number of active ComponentPeer instances
            for (auto i = ComponentPeer::getNumPeers(); --i >= 0;)
                if (i < ComponentPeer::getNumPeers())
                    if (auto* hwndPeer = dynamic_cast<HWNDComponentPeer*> (ComponentPeer::getPeer (i)))
                        hwndPeer->windowShouldDismissModals (info->hwnd);
        }

        static LRESULT CALLBACK callWndProc (int nCode, WPARAM wParam, LPARAM lParam)
        {
            processMessage (nCode, reinterpret_cast<CWPSTRUCT*> (lParam));
            return CallNextHookEx ({}, nCode, wParam, lParam);
        }

        HHOOK hook;
    };

    SharedResourcePointer<TopLevelModalDismissBroadcaster> modalDismissBroadcaster;
    IMEHandler imeHandler;
    bool shouldIgnoreModalDismiss = false;

    RectangleList<int> deferredRepaints;
    ScopedSuspendResumeNotificationRegistration suspendResumeRegistration;
    std::optional<SimpleTimer> monitorUpdateTimer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HWNDComponentPeer)
};

} // end namespace juce
