#pragma once
//#include <irrlicht.h>
#include <helper.h>

namespace irr {
namespace gui {
    
class CGUIImagePanel : public IGUIElement
{
private:
    video::ITexture* m_texPtr;
    core::recti m_texRect;

public:
    CGUIImagePanel(IGUIEnvironment* env,
                    core::recti const& pos,
                    IGUIElement* parent,
                    s32 id=-1)
        : IGUIElement(EGUIET_ELEMENT, env, parent, id, pos)
        , m_texPtr(nullptr)
        , m_texRect(0,0,0,0)
    {

    }

    void setBackgroundImage( video::ITexture* tex, core::recti r_src)
    {
        u32 w = r_src.getWidth();
        u32 h = r_src.getHeight();
        if (w < 1)
        {
            w = tex->getSize().Width;
        }

        if (h > 1)
        {
            h = tex->getSize().Height;
        }

        m_texRect = mk_recti(r_src.UpperLeftCorner.X, r_src.UpperLeftCorner.Y, w, h);
        m_texPtr = tex;
    }

    void draw() override
    {
        auto driver = Environment->getVideoDriver();
        if (driver)
        {
            driver->draw2DImage(m_texPtr, m_texRect, AbsoluteClippingRect);
        }
        IGUIElement::draw();
    }

    // Workaround for a problem with comboboxes.
    // We have to get in front when the combobox wants to get in front or combobox-list might be drawn below other elements.
    bool bringToFront(IGUIElement* element) override
    {
        bool result = IGUIElement::bringToFront(element);
        //if ( Parent && element == m_pComboBox ) result &= Parent->bringToFront(this);
        return result;
    }

    /*
    bool OnEvent(SEvent const& event) override
    {
        if ( event.EventType != EET_GUI_EVENT )
            return false;

        // if ( event.GUIEvent.Caller == m_pComboBox && event.GUIEvent.EventType == EGET_COMBO_BOX_CHANGED )
        // {
        //     m_bDirty = true;
        // }

        return false;
    }

    // return selected texturename (if any, otherwise 0)
    // const wchar_t * getSelectedTextureName() const
    {
        s32 selected = m_pComboBox->getSelected();
        if ( selected < 0 ) return 0;
        return m_pComboBox->getItem(selected);
    }

    */
};


} // end namespace gui
} // end namespace irr
    
