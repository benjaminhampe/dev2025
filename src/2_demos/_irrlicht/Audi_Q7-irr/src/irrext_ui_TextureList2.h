#pragma once
#include <irrlicht.h>

namespace irr {
namespace gui {
    
class CGUITextureList2 : public IGUIElement
{
private:
    bool m_bDirty;
    IGUIComboBox* m_pComboBox;
    
public:
    CGUITextureList2(IGUIEnvironment* env,
                    core::recti const& pos,
                    IGUIElement* parent,
                    s32 id=-1)
        : IGUIElement(EGUIET_ELEMENT, env, parent, id, pos)
        , m_bDirty(true)
        , m_pComboBox(0)
    {
        core::rect<s32> r_combo(0, 0, AbsoluteRect.getWidth(),AbsoluteRect.getHeight());
        m_pComboBox = env->addComboBox (r_combo, this);
        populateComboBox();
    }

    void populateComboBox()     // Put the names of all currently loaded textures in a combobox
    {
        video::IVideoDriver* driver = Environment->getVideoDriver();
        s32 oldSelected = m_pComboBox->getSelected();
        s32 selectNew = -1;
        core::stringw oldTextureName;
        if ( oldSelected >= 0 )
        {
            oldTextureName = m_pComboBox->getItem(oldSelected);
        }
        m_pComboBox->clear();
        for ( u32 i=0; i < driver->getTextureCount(); ++i )
        {
            video::ITexture* texture = driver->getTextureByIndex(i);
            core::stringw name( texture->getName() );
            m_pComboBox->addItem( name.c_str() );
            if ( !oldTextureName.empty() && selectNew < 0 && name == oldTextureName )
                selectNew = i;
        }

        // add another name which can be used to clear the texture
        m_pComboBox->addItem( L"CLEAR texture" );
        if ( oldTextureName == L"CLEAR texture" )
            selectNew = m_pComboBox->getItemCount()-1;

        if ( selectNew >= 0 )
            m_pComboBox->setSelected(selectNew);

        m_bDirty = true;
    }

    virtual bool OnEvent(const SEvent &event)
    {
        if ( event.EventType != EET_GUI_EVENT )
            return false;

        if ( event.GUIEvent.Caller == m_pComboBox && event.GUIEvent.EventType == EGET_COMBO_BOX_CHANGED )
        {
            m_bDirty = true;
        }

        return false;
    }

    // Workaround for a problem with comboboxes.
    // We have to get in front when the combobox wants to get in front or combobox-list might be drawn below other elements.
    virtual bool bringToFront(IGUIElement* element)
    {
        bool result = IGUIElement::bringToFront(element);
        if ( Parent && element == m_pComboBox )
            result &= Parent->bringToFront(this);
        return result;
    }

    // return selected texturename (if any, otherwise 0)
    const wchar_t * getSelectedTextureName() const
    {
        s32 selected = m_pComboBox->getSelected();
        if ( selected < 0 )
            return 0;
        return m_pComboBox->getItem(selected);
    }

    // reset the dirty flag
    void resetDirty()
    {
        m_bDirty = false;
    }

    // when the texture was changed the dirty flag is set
    bool isDirty() const
    {
        return m_bDirty;
    };


};


} // end namespace gui
} // end namespace irr
    
