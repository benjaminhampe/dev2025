#pragma once
#include "Base.h"
// =============================================================
class ImageWidget : public Fl_Widget
{
    de::Image m_img;
public:
    // =============================================================
    ImageWidget(int X,int Y,int W,int H)
    // =============================================================
        : Fl_Widget(X,Y,W,H)
    {
        renderImage(W,H);
    }

    void renderImage(int w, int h)
    {
        if (w < 1 || h < 1)
            return;
        m_img = de::Image(w,h);
        m_img.fill(dbRGB(55,55,0));

        int d = std::max(1, std::min(w,h) - 2);
        int x = (w - d)/2;
        int y = (h - d)/2;
        de::Recti pos(x,y,d,d);
        de::ImagePainter::drawCircle(m_img,pos,dbRGB(255,0,0));
    }

    void draw() override
    {
        if (w() < 1 || h() < 1)
        {
            DE_WARN("Null")
            return;
        }

        fl_draw_image(m_img.data(), x(), y(), m_img.w(), m_img.h(), 4);
    }

    void resize(int X, int Y, int W, int H) override
    {
        //DE_DEBUG("Resize(",X,",",Y,",",W,",",H,")")

        Fl_Widget::resize(X, Y, W, H);

        // Reagiere hier auf neue Größe
        // z.B. internen Buffer neu anlegen

        renderImage(W,H);

        // Wenn du sofort neu zeichnen willst:
        redraw();
    }
};
