#include "NVG_Widget.h"

NVG_Widget::NVG_Widget(NVG_Widget* parent)
    : m_parent{ parent }
    , m_x{ 0 }
    , m_y{ 0 }
    , m_w{ 0 }
    , m_h{ 0 }
    , m_fb{ nullptr }
    , m_dirty{ true }
{
}

// -----------------------------
// CHILD MANAGEMENT
// -----------------------------
void NVG_Widget::addChild(NVG_Widget* child)
{
    child->m_parent = this;
    m_children.push_back(child);
}

// -----------------------------
// ABSOLUTE COORDINATES
// -----------------------------
float NVG_Widget::absX() const
{
    return m_parent ? m_parent->absX() + m_x : m_x;
}
float NVG_Widget::absY() const
{
    return m_parent ? m_parent->absY() + m_y : m_y;
}

// -----------------------------
// HIT TESTING
// -----------------------------
bool NVG_Widget::hit(float ax, float ay) const
{
    float X = absX();
    float Y = absY();
    return ax >= X && ay >= Y && ax < X + m_w && ay < Y + m_h;
}

// -----------------------------
// EVENT DISPATCH
// -----------------------------
bool NVG_Widget::mouseEvent(const NVG_MouseEvent& e)
{
    // Override in subclasses
    return false;
}

bool NVG_Widget::dispatchMouse(const NVG_MouseEvent& e)
{
    // 1. Dispatch to children first (topmost child last)
    for(int i = m_children.size() - 1; i >= 0; --i)
    {
        NVG_Widget* child = m_children[i];

        if(child->hit(e.absX, e.absY))
        {
            // Convert to child's local coords
            NVG_MouseEvent ce = e;
            ce.x = e.absX - child->absX();
            ce.y = e.absY - child->absY();

            if(child->dispatchMouse(ce))
                return true;
        }
    }

    // 2. Dispatch to self
    if (hit(e.absX, e.absY))
    {
        NVG_MouseEvent le = e;
        le.x = e.absX - absX();
        le.y = e.absY - absY();
        return mouseEvent(le);
    }

    return false;
}

// -----------------------------
// CLIPPING
// -----------------------------
void NVG_Widget::applyClip(NVGcontext* vg)
{
    float ax = absX();
    float ay = absY();

    if (m_parent)
    {
        m_parent->applyClip(vg);
        nvgIntersectScissor(vg, ax, ay, m_w, m_h);
    }
    else
    {
        nvgScissor(vg, ax, ay, m_w, m_h);
    }
}

// -----------------------------
// DRAWING
// -----------------------------
void NVG_Widget::drawContents(NVGcontext* vg)
{
    // Example background
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, m_w, m_h);
    nvgFillColor(vg, nvgRGBA(40, 40, 40, 255));
    nvgFill(vg);
}

void NVG_Widget::drawChildren(NVGcontext* vg)
{
    for(auto* child : m_children)
        child->draw(vg);
}

void NVG_Widget::draw(NVGcontext* vg)
{
    nvgSave(vg);

    applyClip(vg);
    nvgTranslate(vg, m_x, m_y);

    if(m_fb && !m_dirty)
    {
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, m_w, m_h);
        auto p = nvgImagePattern(vg, 0, 0, m_w, m_h, 0, m_fb->image, 1.0f);
        nvgFillPaint(vg, p);
        nvgFill(vg);
    }
    else
    {
        drawContents(vg);
        drawChildren(vg);

        if (m_fb)
        {
            redrawToFramebuffer(vg);
            m_dirty = false;
        }
    }

    nvgRestore(vg);
}

void NVG_Widget::redrawToFramebuffer(NVGcontext* vg)
{
    nvgluBindFramebuffer(m_fb);
    nvgBeginFrame(vg, m_w, m_h, 1.0f);

    nvgSave(vg);
    nvgScissor(vg, 0, 0, m_w, m_h);

    drawContents(vg);
    drawChildren(vg);

    nvgRestore(vg);
    nvgEndFrame(vg);
    nvgluBindFramebuffer(nullptr);
}
