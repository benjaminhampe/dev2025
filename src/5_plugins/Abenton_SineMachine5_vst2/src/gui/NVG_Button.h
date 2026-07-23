#pragma once
#include "NVG_Widget.h"

struct NVG_Button : NVG_Widget
{
    std::string m_label = "Button";

    bool m_hovered = false;
    bool m_pressed = false;

    NVGcolor m_baseColor     = nvgRGBA(70, 70, 70, 255);
    NVGcolor m_hoverColor    = nvgRGBA(100, 100, 100, 255);
    NVGcolor m_pressColor    = nvgRGBA(150, 60, 60, 255);
    NVGcolor m_textColor     = nvgRGBA(255, 255, 255, 255);

    float m_cornerRadius = 6.0f;

    // -----------------------------
    // Mouse handling
    // -----------------------------
    bool mouseEvent(const NVG_MouseEvent& e) override
    {
        if(e.moved) {
            m_hovered = true;
            return false; // allow bubbling
        }

        if(e.pressed) {
            m_pressed = true;
            return true; // consume
        }

        if(e.released) {
            m_pressed = false;
            // trigger callback here if needed
            return true;
        }

        return false;
    }

    // When mouse leaves the widget region
    void onMouseLeave()
    {
        m_hovered = false;
        m_pressed = false;
    }

    // -----------------------------
    // Override dispatch to detect leave
    // -----------------------------
    bool dispatchMouse(const NVG_MouseEvent& e) override
    {
        bool inside = hit(e.absX, e.absY);

        if(!inside && m_hovered) {
            onMouseLeave();
        }

        return NVG_Widget::dispatchMouse(e);
    }

    // -----------------------------
    // Draw button
    // -----------------------------
    void drawContents(NVGcontext* vg) override
    {
        NVGcolor col =
            m_pressed ? m_pressColor :
            m_hovered ? m_hoverColor : m_baseColor;

        // Background
        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0, 0, m_w, m_h, m_cornerRadius);
        nvgFillColor(vg, col);
        nvgFill(vg);

        // Text
        nvgFontSize(vg, 16.0f);
        nvgFontFace(vg, "sans");
        nvgFillColor(vg, m_textColor);

        float tw = nvgTextBounds(vg, 0, 0, m_label.c_str(), nullptr, nullptr);
        float tx = (m_w - tw) * 0.5f;
        float ty = (m_h * 0.5f) + 6.0f; // vertical centering

        nvgText(vg, tx, ty, m_label.c_str(), nullptr);
    }


};
