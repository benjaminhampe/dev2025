#pragma once
#include <de/gui/Widget.h>

namespace de::gpu { struct VideoDriver; }

namespace de {
namespace gui {

// =====================================================================
class Env
// =====================================================================
{
public:
    Env();
    ~Env();
    void init( gpu::VideoDriver* driver);
    void draw();

    float getZoom() const { return m_zoom; }
    void setZoom( float zoom ) { m_zoom = zoom; }

    int getMouseX() const;
    int getMouseY() const;
    int getScreenWidth() const;
    int getScreenHeight() const;
    gpu::VideoDriver* getVideoDriver();

    void addTopLevelWidget( Widget* widget );

    void clear();
    void onEvent( const Event & event );
    Widget* pick( int mx, int my );
protected:
    gpu::VideoDriver* m_driver = nullptr;
    Widget* m_hoveredElement = nullptr; // Needed for events like Enter/Leave/etc...
    Widget* m_lastHoveredElement = nullptr; // Needed for events like Enter/Leave/etc...
    float m_zoom;
    int m_mouseX;
    int m_mouseY;
    bool m_mouseMoved;
    std::vector< Widget* > m_topLevelWidgets;
};

} // end namespace gui.
} // end namespace de.
