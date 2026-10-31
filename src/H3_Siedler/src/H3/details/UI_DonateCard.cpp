#include "UI_DonateCard.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_DonateCard::UI_DonateCard( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
    , m_cardType(1)
    , m_cardAmount(1)
{
    m_edtDst = new de::gui::IntField("",env,this,0);
    m_btnSrc = new de::gui::Button("",env,this,0);
    m_lblSrc = new de::gui::Label("",env,this,0);

    m_btnSrc->setRadius(10);

    auto tex = m_game->getTex(H3_Tex::Scroll2,__func__);
    m_edtDst->setMinusButtonTexture(tex);
    m_edtDst->setPlusButtonTexture(tex);

    m_lblSrc->setFillColors(dbRGB(255,255,255,200));
    m_edtDst->getValueLabel()->setFillColors(dbRGB(255,255,255,200));

    auto plus = [&] ()
        {
            int dst = m_edtDst->getValue();
            if (dst < m_cardAmount)
            {
                dst++;
                m_edtDst->setValue(dst);
                m_lblSrc->setText(std::to_string(m_cardAmount - dst));
            }

            if (onValueChanged)
            {
                onValueChanged(m_edtDst->getValue());
            }
        };

    auto minus = [&] ()
        {
            int dst = m_edtDst->getValue();
            if (dst > 0)
            {
                dst--;
                m_edtDst->setValue(dst);
                m_lblSrc->setText(std::to_string(m_cardAmount - dst));
            }

            if (onValueChanged)
            {
                onValueChanged(m_edtDst->getValue());
            }
        };

    m_edtDst->onPlusClicked = plus;

    m_edtDst->onMinusClicked = minus;

    m_btnSrc->onClickedLeft = plus;

    m_btnSrc->onClickedRight = minus;
}

UI_DonateCard::~UI_DonateCard()
{
}

void UI_DonateCard::setCardData( int cardType, int amount )
{
    m_cardType = cardType;
    m_cardAmount = amount;
    m_edtDst->setRange(0,amount);
    m_edtDst->setValue(0);

    de::gpu::TexRef m_tex;
    switch (cardType)
    {
        case 0: m_tex = m_game->getTex(H3_Tex::CardA, __func__); break;
        case 1: m_tex = m_game->getTex(H3_Tex::CardB, __func__); break;
        case 2: m_tex = m_game->getTex(H3_Tex::CardC, __func__); break;
        case 3: m_tex = m_game->getTex(H3_Tex::CardD, __func__); break;
        case 4: m_tex = m_game->getTex(H3_Tex::CardE, __func__); break;
        default: break;
    }

    m_btnSrc->setTexture(m_tex);

    m_lblSrc->setText(std::to_string(amount));
}

void UI_DonateCard::draw()
{
    if (!isVisible())
    {
        return;
    }

    m_edtDst->draw();
    m_btnSrc->draw();
    m_lblSrc->draw();

}

void UI_DonateCard::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    int p = zoom * 10;
    int w = m_rect.w;
    int h = m_rect.h;
    int x = 0;
    int y = 0;
    int h1 = h / 8;
    int h2 = h - h1 - h1 - p - p;

    m_r_head = de::Recti(x,y,w,h1); y += h1 + p;
    m_r_body = de::Recti(x,y,w,h2); y += h2 + p;
    m_r_foot = de::Recti(x,y,w,h1);

    m_edtDst->resize(m_r_head.x,m_r_head.y,m_r_head.w,m_r_head.h);
    m_btnSrc->resize(m_r_body.x,m_r_body.y,m_r_body.w,m_r_body.h);
    m_lblSrc->resize(m_r_foot.x,m_r_foot.y,m_r_foot.w,m_r_foot.h);
}
