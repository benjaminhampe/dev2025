#include <de/gui/IntField.h>
#include <de/gui/Env.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {

// =====================================================================
IntField::IntField( const std::string& label, Env* env, Widget* parent, u32 id, const Recti& pos )
// =====================================================================
    : Widget(env, parent, id, pos)
    , m_value(0)
    , m_minimum(0)
    , m_maximum(100)
    , m_step(1)
{
    m_lblName = new Label(label, env, this);
    m_btnMinus = new Button("-", env, this);
    m_lblValue = new Label("0", env, this);
    m_btnPlus = new Button("+", env, this);

    m_btnMinus->onClicked = [&]()
    {
        setValue(m_value - m_step);
        if (onMinusClicked)
        {
            onMinusClicked();
        }
    };
    m_btnPlus->onClicked = [&]()
    {
        setValue(m_value + m_step);
        if (onPlusClicked)
        {
            onPlusClicked();
        }
    };

    setDrawChildren(false);
}

IntField::~IntField()
{
    // DE_DEBUG("")
}

void IntField::setFont( const Font& font )
{
    Font font2 = font;
    font2.pixelSize *= 2;
    m_lblName->setFont(font);
    m_btnMinus->setFont(font2);
    m_lblValue->setFont(font);
    m_btnPlus->setFont(font2);
}

void IntField::setRange( int min_value, int max_value )
{
    m_minimum = min_value;
    m_maximum = max_value;
}

void IntField::setStep( int value )
{
    m_step = value;
}
void IntField::setValue( int value )
{
    int oldValue = m_value;
    m_value = std::clamp( value, m_minimum, m_maximum );
    if (m_value != oldValue)
    {
        m_lblValue->setText( std::to_string(m_value) );
        if (onValueChanged)
        {
            onValueChanged(m_value);
        }
    }
    // updateLayout();
}

void IntField::draw()
{
    if (!isVisible()) return;
    m_lblName->draw();
    m_btnMinus->draw();
    m_lblValue->draw();
    m_btnPlus->draw();
}

void IntField::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);
    updateLayout();
}

void IntField::updateLayout()
{
    int x = 0;
    int y = 0;
    int w = m_rect.w;
    int h = m_rect.h;

    const auto& font = m_lblValue->getFont();
    const auto& text = m_lblValue->getText();

    float zoom = m_env->getZoom();

    auto ts = dbTextSize( de_wstr(text), font );

    if (m_lblName->getText().empty())
    {
        int btn_w = h;
        int edt_w = w - 2*btn_w;
        m_lblName->resize(0,0,0,0);
        m_btnMinus->resize(x,y,btn_w,h);    x += btn_w;
        m_lblValue->resize(x,y,edt_w,h);    x += edt_w;
        m_btnPlus->resize(x,y,btn_w,h);
    }
    else
    {
        int btn_w = h;
        int edt_w = std::max<long>(zoom*30, w - 2*btn_w - 3*w/5);
        int lbl_w = w - 2*btn_w - edt_w;
        m_lblName->resize(x,y,lbl_w,h);    x += lbl_w;
        m_btnMinus->resize(x,y,btn_w,h);   x += btn_w;
        m_lblValue->resize(x,y,edt_w,h);   x += edt_w;
        m_btnPlus->resize(x,y,btn_w,h);
    }
}

} // end namespace gui.
} // end namespace de.
