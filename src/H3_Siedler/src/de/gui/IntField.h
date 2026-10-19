#pragma once
#include <de/gui/Widget.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

namespace de {
namespace gui {

// =====================================================================
class IntField : public Widget
// =====================================================================
{
public:
    IntField(const std::string& label, Env* env, Widget* parent, u32 id, const Recti& pos = Recti() );
    ~IntField() override;
    u32 getType() const override { return eWT_IntField; }

    void setFont( const Font& font ) override;

    void setRange( int min_value, int max_value );
    void setStep( int value );
    void setValue( int value );

    void draw() override;
    void resize(int x, int y, int w, int h) override;

    void resize(const Recti& pos)
    {
        resize(pos.x, pos.y, pos.w, pos.h);
    }

    //void onEvent( const Event& event ) override;

    const std::string& getLabelText() const { return m_lblName->getText(); }

    Label* getKeyLabel() const { return m_lblName; }
    Label* getValueLabel() const { return m_lblValue; }
    Button* getMinusButton() { return m_btnMinus; }
    Button* getPlusButton() { return m_btnPlus; }

    int getValue() const { return m_value; }
    int getMinimum() const { return m_minimum; }
    int getMaximum() const { return m_maximum; }
    int getStep() const { return m_step; }



protected:
    void updateLayout();

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
