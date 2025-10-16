#include <juce_gui_basics/native/juce_win32_Desktop.h>
#include <juce_gui_basics/windows/juce_TopLevelWindow.h>
#include <juce_gui_basics/native/juce_win32_ComponentPeer.h>
#include <stringapiset.h> // CompareStringOrdinal

namespace juce
{
JUCE_IMPLEMENT_SINGLETON (VBlankDispatcher)

//==============================================================================

HMONITOR getMonitorFromOutput (ComSmartPtr<IDXGIOutput> output)
{
    DXGI_OUTPUT_DESC desc = {};
    return (FAILED (output->GetDesc (&desc)) || ! desc.AttachedToDesktop)
        ? nullptr
        : desc.Monitor;
}

void setWindowPos (HWND hwnd, Rectangle<int> bounds, UINT flags, bool adjustTopLeft )
{
    ScopedThreadDPIAwarenessSetter setter { hwnd };

    if (isPerMonitorDPIAwareWindow (hwnd))
    {
        if (adjustTopLeft)
            bounds = convertLogicalScreenRectangleToPhysical (bounds, hwnd)
                      .withPosition (Desktop::getInstance().getDisplays().logicalToPhysical (bounds.getTopLeft()));
        else
            bounds = convertLogicalScreenRectangleToPhysical (bounds, hwnd);
    }

    SetWindowPos (hwnd, nullptr, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), flags);
}

RECT getWindowScreenRect (HWND hwnd)
{
    ScopedThreadDPIAwarenessSetter setter { hwnd };

    RECT rect;
    GetWindowRect (hwnd, &rect);
    return rect;
}

RECT getWindowClientRect (HWND hwnd)
{
    auto rect = getWindowScreenRect (hwnd);

    if (auto parentH = GetParent (hwnd))
    {
        ScopedThreadDPIAwarenessSetter setter { hwnd };
        MapWindowPoints (HWND_DESKTOP, parentH, (LPPOINT) &rect, 2);
    }

    return rect;
}

void setWindowZOrder (HWND hwnd, HWND insertAfter)
{
    SetWindowPos (hwnd, insertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
}

//==============================================================================
Rectangle<int> rectangleFromRECT (RECT r) noexcept
{
   return { r.left, r.top, r.right - r.left, r.bottom - r.top };
}

RECT RECTFromRectangle (Rectangle<int> r) noexcept
{
   return { r.getX(), r.getY(), r.getRight(), r.getBottom() };
}

Point<int> pointFromPOINT (POINT p) noexcept
{
   return { p.x, p.y };
}

POINT POINTFromPoint (Point<int> p) noexcept
{
   return { p.x, p.y };
}

Point<int> convertPhysicalScreenPointToLogical (Point<int> p, HWND h) noexcept
{
    if (isPerMonitorDPIAwareWindow (h))
        return Desktop::getInstance().getDisplays().physicalToLogical (p, getCurrentDisplayFromScaleFactor (h));

    return p;
}

Point<int> convertLogicalScreenPointToPhysical (Point<int> p, HWND h) noexcept
{
    if (isPerMonitorDPIAwareWindow (h))
        return Desktop::getInstance().getDisplays().logicalToPhysical (p, getCurrentDisplayFromScaleFactor (h));

    return p;
}

//==============================================================================

double Desktop::getDefaultMasterScale()
{
    if (! JUCEApplicationBase::isStandaloneApp() || isPerMonitorDPIAwareProcess())
        return 1.0;

    return getGlobalDPI() / USER_DEFAULT_SCREEN_DPI;
}

bool Desktop::canUseSemiTransparentWindows() noexcept
{
    return true;
}

std::unique_ptr<Desktop::NativeDarkModeChangeDetectorImpl>
Desktop::createNativeDarkModeChangeDetectorImpl()
{
    return std::make_unique<NativeDarkModeChangeDetectorImpl>();
}

bool
Desktop::isDarkModeActive() const
{
    return nativeDarkModeChangeDetectorImpl->isDarkModeEnabled();
}

Desktop::DisplayOrientation
Desktop::getCurrentOrientation() const
{
    return upright;
}


//==============================================================================
void Desktop::setKioskComponent (Component* kioskModeComp, bool enableOrDisable, bool /*allowMenusAndBars*/)
{
    if (auto* tlw = dynamic_cast<TopLevelWindow*> (kioskModeComp))
        tlw->setUsingNativeTitleBar (! enableOrDisable);

    if (kioskModeComp != nullptr && enableOrDisable)
        kioskModeComp->setBounds (getDisplays().getDisplayForRect (kioskModeComp->getScreenBounds())->totalArea);
}

void Desktop::allowedOrientationsChanged() {}

//==============================================================================
Displays::Display const*
getCurrentDisplayFromScaleFactor (HWND hwnd)
{
    Array<const Displays::Display*> candidateDisplays;

    const auto scaleToLookFor = [&]
    {
        if (auto* peer = HWNDComponentPeer::getOwnerOfWindow (hwnd))
            return peer->getPlatformScaleFactor();

        return getScaleFactorForWindow (hwnd);
    }();

    auto globalScale = Desktop::getInstance().getGlobalScaleFactor();

    for (auto& d : Desktop::getInstance().getDisplays().displays)
        if (approximatelyEqual (d.scale / globalScale, scaleToLookFor))
            candidateDisplays.add (&d);

    if (candidateDisplays.size() > 0)
    {
        if (candidateDisplays.size() == 1)
            return candidateDisplays[0];

        const auto bounds = [&]
        {
            if (auto* peer = HWNDComponentPeer::getOwnerOfWindow (hwnd))
                return peer->getComponent().getTopLevelComponent()->getBounds();

            return Desktop::getInstance().getDisplays().physicalToLogical (rectangleFromRECT (getWindowScreenRect (hwnd)));
        }();

        const Displays::Display* retVal = nullptr;
        int maxArea = -1;

        for (auto* d : candidateDisplays)
        {
            auto intersection = d->totalArea.getIntersection (bounds);
            auto area = intersection.getWidth() * intersection.getHeight();

            if (area > maxArea)
            {
                maxArea = area;
                retVal = d;
            }
        }

        if (retVal != nullptr)
            return retVal;
    }

    return Desktop::getInstance().getDisplays().getPrimaryDisplay();
}

//==============================================================================
struct MonitorInfo
{
    MonitorInfo (bool main, RECT totalArea, RECT workArea, double d) noexcept
        : isMain (main),
          totalAreaRect (totalArea),
          workAreaRect (workArea),
          dpi (d)
    {
    }

    bool isMain;
    RECT totalAreaRect, workAreaRect;
    double dpi;
};

static BOOL CALLBACK enumMonitorsProc (HMONITOR hm, HDC, LPRECT, LPARAM userInfo)
{
    MONITORINFO info = {};
    info.cbSize = sizeof (info);
    GetMonitorInfo (hm, &info);

    auto isMain = (info.dwFlags & 1 /* MONITORINFOF_PRIMARY */) != 0;
    auto dpi = 0.0;

    if (getDPIForMonitor != nullptr)
    {
        UINT dpiX = 0, dpiY = 0;

        if (SUCCEEDED (getDPIForMonitor (hm, MDT_Default, &dpiX, &dpiY)))
            dpi = (dpiX + dpiY) / 2.0;
    }

    ((Array<MonitorInfo>*) userInfo)->add ({ isMain, info.rcMonitor, info.rcWork, dpi });
    return TRUE;
}

void Displays::findDisplays (float masterScale)
{
    setDPIAwareness();

    Array<MonitorInfo> monitors;
    EnumDisplayMonitors (nullptr, nullptr, &enumMonitorsProc, (LPARAM) &monitors);

    auto globalDPI = getGlobalDPI();

    if (monitors.size() == 0)
    {
        auto windowRect = getWindowScreenRect (GetDesktopWindow());
        monitors.add ({ true, windowRect, windowRect, globalDPI });
    }

    // make sure the first in the list is the main monitor
    for (int i = 1; i < monitors.size(); ++i)
        if (monitors.getReference (i).isMain)
            monitors.swap (i, 0);

    for (auto& monitor : monitors)
    {
        Display d;

        d.isMain = monitor.isMain;
        d.dpi = monitor.dpi;

        if (d.dpi == 0)
        {
            d.dpi = globalDPI;
            d.scale = masterScale;
        }
        else
        {
            d.scale = (d.dpi / USER_DEFAULT_SCREEN_DPI) * (masterScale / Desktop::getDefaultMasterScale());
        }

        d.totalArea = rectangleFromRECT (monitor.totalAreaRect);
        d.userArea  = rectangleFromRECT (monitor.workAreaRect);

        displays.add (d);
    }

   #if JUCE_WIN_PER_MONITOR_DPI_AWARE
    if (isPerMonitorDPIAwareThread())
        updateToLogical();
    else
   #endif
    {
        for (auto& d : displays)
        {
            d.totalArea /= masterScale;
            d.userArea  /= masterScale;
        }
    }
}


} // end namespace juce
