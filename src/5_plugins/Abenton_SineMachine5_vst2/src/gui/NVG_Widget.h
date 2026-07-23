#pragma once

#include <de/Common.h>

struct NVG_MouseEvent {
    float x;      // local coords
    float y;      // local coords
    float absX;   // absolute coords
    float absY;   // absolute coords
    int button;
    bool pressed;
    bool released;
    bool moved;
    bool dragged;
};

struct NVG_Widget
{
    float m_x = 0;
    float m_y = 0;
    float m_w = 0;
    float m_h = 0;

    NVG_Widget* m_parent = nullptr;
    std::vector<NVG_Widget*> m_children;

    // Optional framebuffer caching
    NVGLUframebuffer* m_fb = nullptr;
    bool m_dirty = true;

    explicit NVG_Widget(NVG_Widget* parent = nullptr);

    // -----------------------------
    // CHILD MANAGEMENT
    // -----------------------------
    void addChild(NVG_Widget* child);

    // -----------------------------
    // ABSOLUTE COORDINATES
    // -----------------------------
    float absX() const;
    float absY() const;

    // -----------------------------
    // HIT TESTING
    // -----------------------------
    bool hit(float ax, float ay) const;

    // -----------------------------
    // EVENT DISPATCH
    // -----------------------------
    virtual bool mouseEvent(const NVG_MouseEvent& e);

    virtual bool dispatchMouse(const NVG_MouseEvent& e);
    // -----------------------------
    // CLIPPING
    // -----------------------------
    void applyClip(NVGcontext* vg);

    // -----------------------------
    // DRAWING
    // -----------------------------
    virtual void drawContents(NVGcontext* vg);

    void drawChildren(NVGcontext* vg);

    void draw(NVGcontext* vg);

    void redrawToFramebuffer(NVGcontext* vg);
};

