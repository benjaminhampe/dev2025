#pragma once
#include <de/gui/Label.h>
#include <de/gui/Button.h>

namespace de {
namespace gui {
    
// =====================================================================
class IntField : public Widget
// =====================================================================
{
public:
    IntField(const std::string& label, Env* env, Widget* parent, u32 id, const Recti& r );
    ~IntField() override;
    u32 getType() const override { return eWT_IntField; }
    //void draw() override;
    //void onEvent( const Event& event ) override;
    void setSize( const Sizei& dim ) override;
    void setFont( const Font& font ) override;

    // void setFont( const Font& font ) override
    // {
    //     m_lblName->setFont(font);
    //     m_btnPlus->setFont(font);
    //     m_lblValue->setFont(font);
    //     m_btnMinus->setFont(font);
    // }

    Label* getNameLabel() const { return m_lblName; }
    Label* getValueLabel() const { return m_lblValue; }
    Button* getMinusButton() { return m_btnMinus; }
    Button* getPlusButton() { return m_btnPlus; }

    int getValue() const { return m_value; }
	int getMinimum() const { return m_minimum; }
	int getMaximum() const { return m_maximum; }
	int getStep() const { return m_step; }

    void setValue( int value )
    {
        m_value = std::clamp( value, m_minimum, m_maximum );
        m_lblValue->setText( std::to_string(m_value) );
    }
    void setMinimum( int value )
    {
        m_minimum = value;
    }
    void setMaximum( int value )
    {
        m_maximum = value;
    }
    void setStep( int value )
    {
        m_step = value;
    }

protected:
    int m_value;
    int m_minimum;
    int m_maximum;
    int m_step;

    Label* m_lblName;
    Label* m_lblValue;
    Button* m_btnMinus;
    Button* m_btnPlus;


};

} // end namespace gui.
} // end namespace de.
