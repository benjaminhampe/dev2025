#pragma once
#include <irrlicht.h>
#include <helper.h>

namespace irr {
namespace gui {
    
inline s32 makeUniqueId()
{
    constexpr const u32 GUI_ID_MAX = 3*1024;
	static int unique = GUI_ID_MAX;
	++unique;
	return unique;
}

/*
	Custom GUI-control to edit colorvalues.
*/
class CGUIColorControl : public IGUIElement
{
	s32 ButtonSetId;
    video::SColor m_color;
	IGUIStaticText* m_lblColor;
	IGUIEditBox * m_edtAlpha;
	IGUIEditBox * m_edtRed;
	IGUIEditBox * m_edtGreen;
	IGUIEditBox * m_edtBlue;
public:
	// Add a staticbox for a description + an editbox so users can enter numbers
    static IGUIEditBox* createEditBox(IGUIEnvironment* env, core::rect<s32> const& pos, wchar_t const* text, s32 id, IGUIElement* parent)
	{
        //auto lbl = env->addStaticText(text, pos, false, false, parent, -1, false);
        //pos.LowerRightCorner.X += 20;
		IGUIEditBox* edit = env->addEditBox(L"0", pos, true, parent, id);
		return edit;
	}

	// Constructor
    CGUIColorControl(IGUIEnvironment* env, core::recti const & pos, const wchar_t *text, IGUIElement* parent, s32 id=-1 )
		: IGUIElement(EGUIET_ELEMENT, env, parent, id, pos)
        , ButtonSetId(makeUniqueId()), m_color(255,255,230,180), m_lblColor(0)
        , m_edtAlpha(0), m_edtRed(0), m_edtGreen(0), m_edtBlue(0)
	{
        s32 w = AbsoluteRect.getWidth();
        s32 h = AbsoluteRect.getHeight();
        s32 h4 = h / 5;
        core::recti const r_controls = mk_recti(0,0,w,h);
        
		IGUIStaticText* panel = env->addStaticText (L"", r_controls, true, false, this, -1, false);
		panel->setNotClipped(true);

        env->addStaticText(text, mk_recti(0,0,w,h4), false, false, panel, -1, false);

        m_edtAlpha = createEditBox(env, mk_recti(0,1*h4,w,h4), L"A", -1, panel );
        m_edtRed = createEditBox(env, mk_recti(0,2*h4,w,h4), L"R", -1, panel );
        m_edtGreen = createEditBox(env, mk_recti(0,3*h4,w,h4), L"G", -1, panel );
        m_edtBlue = createEditBox(env, mk_recti(0,4*h4,w,h4), L"B", -1, panel );

		m_lblColor = env->addStaticText (L"", core::rect<s32>(60,15,80,75), true, false, panel, -1, true);

		env->addButton (core::rect<s32>(60,35,80,50), panel, ButtonSetId, L"set");
        
		setColor( m_color );
	}


	virtual bool OnEvent(const SEvent &event)
	{
		if ( event.EventType != EET_GUI_EVENT )
			return false;

		if ( event.GUIEvent.Caller->getID() == ButtonSetId && event.GUIEvent.EventType == EGET_BUTTON_CLICKED )
		{
			updateFromColor();
		}

		return false;
	}

	void setColor(const video::SColor& col)
	{
		m_color = col;
		updateFromColor();
	}

	video::SColor getColor() const
	{
		video::SColor c(255,0,0,0);

		if (m_edtAlpha)
		{
			u32 a = core::strtoul10(core::stringc(m_edtAlpha->getText()).c_str());
			if (a > 255) a = 255;
			c.setAlpha(a);
		}

		if (m_edtRed)
		{
			u32 r = core::strtoul10(core::stringc(m_edtRed->getText()).c_str());
			if (r > 255) r = 255;
			c.setRed(r);
		}

		if (m_edtGreen)
		{
			u32 g = core::strtoul10(core::stringc(m_edtGreen->getText()).c_str());
			if (g > 255) g = 255;
			c.setGreen(g);
		}

		if (m_edtBlue)
		{
			u32 b = core::strtoul10(core::stringc(m_edtBlue->getText()).c_str());
			if (b > 255) b = 255;
			c.setBlue(b);
		}

		return c;
	}

protected:

	void updateFromColor()
	{
		if ( m_edtAlpha ) m_edtAlpha->setText( core::stringw(m_color.getAlpha()).c_str() );
		if ( m_edtRed ) m_edtRed->setText( core::stringw(m_color.getRed()).c_str() );
		if ( m_edtGreen ) m_edtGreen->setText( core::stringw(m_color.getGreen()).c_str() );
		if ( m_edtBlue ) m_edtBlue->setText( core::stringw(m_color.getBlue()).c_str() );
		if ( m_lblColor ) m_lblColor->setBackgroundColor(m_color);
	}

};


} // end namespace gui
} // end namespace irr
    
