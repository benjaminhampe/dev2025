#include <de/gui/IntField.h>
#include <de/gpu/VideoDriver.h>

namespace de {
namespace gui {

// =====================================================================
IntField::IntField( const std::string& label, Env* env, Widget* parent, u32 id, const Recti& r )
// =====================================================================
    : Widget(env, parent, id, r)
    , m_value(0)
    , m_minimum(0)
    , m_maximum(100)
    , m_step(1)
{
    int w1 = r.h;

    m_lblName = new Label(label, env, this, 0, Recti(w1, 0, r.w - 2*w1, w1));
    m_btnMinus = new Button("-", env, this, 0, Recti(0,0,w1,w1));
    m_lblValue = new Label("", env, this, 0, Recti(0,0, r.w - 2*w1, w1));
    m_btnPlus = new Button("+", env, this, 0, Recti(r.w - 1 - 2*w1,0,w1,w1));

    // addChild(m_lblName);
    // addChild(m_btnMinus);
    // addChild(m_lblValue);
    // addChild(m_btnPlus);

    m_btnMinus->setOnClicked( [&](){ setValue(m_value - m_step); } );
    m_btnPlus->setOnClicked( [&](){ setValue(m_value + m_step); } );
}

IntField::~IntField()
{
    DE_DEBUG("")
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

void IntField::resize(int x, int y, int w, int h)
{
    m_rect = de::Recti(x,y,w,h);
    int c = 3*w/5;
    int d = w - c - 2*h;

    int X = 0;
    int Y = 0;
    m_lblName->resize(X,Y,c,h);    X += c;
    m_btnMinus->resize(X,Y,h,h);   X += h;
    m_lblValue->resize(X,Y,d,h);   X += d;
    m_btnPlus->resize(X,Y,h,h);
    // DE_OK("(",dim.str(),")")
    // DE_BENNI("m_lblName = ",m_lblName->getRect().str())
    // DE_BENNI("m_btnMinus = ",m_btnMinus->getRect().str())
    // DE_BENNI("m_lblValue = ",m_lblValue->getRect().str())
    // DE_BENNI("m_btnPlus = ",m_btnPlus->getRect().str())
}

} // end namespace gui.
} // end namespace de.
