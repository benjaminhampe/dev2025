#include "UI_WinnerScreen.h"
#include <H3/H3_Game.h>

// =====================================================================
UI_WinnerScreen::UI_WinnerScreen( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
    // =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
{
    auto texPanel = game->getTex(H3_Tex::Grunge, __func__);
    //auto texPanel = game->getTex(H3_Tex::Scroll, __func__);
    auto texClose = game->getTex(H3_Tex::Scroll2, __func__);

    setTexture( texPanel );
    setFillColor( 0xffffffff );

    m_btnContinueGame = createButton("Continue Game", texClose);
    m_btnContinueGame->onClicked = [&]() { m_game->enterIdle(); };

    m_btnReturnToMenu = createButton("Return to Menu", texClose);
    m_btnReturnToMenu->onClicked = [&]() { m_game->exitGame(); };

    setDrawChildren(false);
}

UI_WinnerScreen::~UI_WinnerScreen()
{

}

void UI_WinnerScreen::updateMaxima()
{
    m_max.clear();

    for (const H3_Player& player : m_game->m_players)
    {
        for (const H3_PlayerState& state : player.history)
        {
            m_max.vicPoints = std::max<u32>( m_max.vicPoints, state.victoryPoints);
            // bool hasPort3v1;
            // bool hasPortA;
            // bool hasPortB;
            // bool hasPortC;
            // bool hasPortD;
            // bool hasPortE;
            // bool hasBonusArmy;
            // bool hasBonusRoad;

            m_max.bank.A = std::max<u32>( m_max.bank.A, state.bank.A);
            m_max.bank.B = std::max<u32>( m_max.bank.B, state.bank.B);
            m_max.bank.C = std::max<u32>( m_max.bank.C, state.bank.C);
            m_max.bank.D = std::max<u32>( m_max.bank.D, state.bank.D);
            m_max.bank.E = std::max<u32>( m_max.bank.E, state.bank.E);
            // H3_Dice dice;

            // tiles = std::max<u32>( m_max.tiles, state.tiles);
            // corners = std::max<u32>( m_max.corners, state.corners);
            // edges = std::max<u32>( m_max.edges, state.edges);

            m_max.roads = std::max<u32>( m_max.roads, state.roads.size());
            m_max.farms = std::max<u32>( m_max.farms, state.farms.size());
            m_max.citys = std::max<u32>( m_max.citys, state.citys.size());
            m_max.cards = std::max<u32>( m_max.cards, state.numEventCards(true));
            m_max.ships = std::max<u32>( m_max.ships, state.ships.size());

            m_max.points = std::max<u32>( m_max.points, state.points.size());
            m_max.knights = std::max<u32>( m_max.knights, state.knights.size());
            m_max.invents = std::max<u32>( m_max.invents, state.invents.size());
            m_max.monopols = std::max<u32>( m_max.monopols, state.monopols.size());

            m_max.pointCards = std::max<u32>( m_max.pointCards, state.pointCards.size());
            m_max.knightCards = std::max<u32>( m_max.knightCards, state.knightCards.size());
            m_max.inventCards = std::max<u32>( m_max.inventCards, state.inventCards.size());
            m_max.monopolCards = std::max<u32>( m_max.monopolCards, state.monopolCards.size());
        }
    }
}

void UI_WinnerScreen::updatePlots( const de::Recti& rcPlot )
{
    int categories = 7;
    m_categories.clear();

    const int numPoints = m_game->m_players.front().history.size();
    if (numPoints < 2)
    {
        DE_ERROR("Not enoungh history")
        return;
    }

    m_categories.reserve( categories );

    const auto& players = m_game->m_players;

    const float dx = float(rcPlot.w) / float(numPoints-1);

    for (int category = 0; category < categories; category++)
    {
        m_categories.emplace_back();
        Plots & plots = m_categories.back();
        plots.clear();
        plots.resize( players.size() );

        for (int iPlayer = 0; iPlayer < int(plots.size()); iPlayer++)
        {
            Plot & plot = plots[iPlayer];
            switch (category)
            {
                case Plot::VicPoints: plot.name = "Victory Points"; break;
                case Plot::BankSum: plot.name = "BankSum"; break;
                case Plot::Road: plot.name = "Roads"; break;
                case Plot::Farm: plot.name = "Farms"; break;
                case Plot::City: plot.name = "Citys"; break;
                case Plot::EventCard: plot.name = "EventCards"; break;
                default: break;
            }

            switch (category)
            {
                case Plot::VicPoints: plot.maxValue = m_max.vicPoints; break;
                case Plot::BankSum: plot.maxValue = m_max.bank.sum(); break;
                case Plot::Road: plot.maxValue = m_max.roads; break;
                case Plot::Farm: plot.maxValue = m_max.farms; break;
                case Plot::City: plot.maxValue = m_max.citys; break;
                case Plot::EventCard: plot.maxValue = m_max.cards; break;
                default: break;
            }

            const H3_Player& player = players[iPlayer];

            float dy = float(rcPlot.h) / float(std::max<int>(1,plot.maxValue));

            plot.lineStrip.clear();
            plot.lineStrip.primitiveType = de::gpu::PrimitiveType::LineStrip;

            for (size_t i = 0; i < player.history.size(); ++i)
            {
                const H3_PlayerState& state = player.history[i];

                int value = 0;
                switch (category)
                {
                    case Plot::VicPoints: value = state.farms.size(); break;
                    case Plot::BankSum: value = state.bank.sum(); break;
                    case Plot::Road: value = state.roads.size(); break;
                    case Plot::Farm: value = state.farms.size(); break;
                    case Plot::City: value = state.citys.size(); break;
                    case Plot::EventCard: value = state.numEventCards(true); break;
                    default: break;
                }

                float x = rcPlot.x + (dx * i) + iPlayer;
                float y = rcPlot.y + rcPlot.h - 1 - dy*value + iPlayer;
                plot.lineStrip.addVertex( { x,y,0, 0,0,-1, player.color, 0,0 } );
            }
        }
    }

    m_xAxis.primitiveType = de::gpu::PrimitiveType::Lines;
    m_xAxis.vertices.clear();
    m_xAxis.vertices.reserve( 2 * numPoints);
    int gridH = 10;
    int gridT = 20;

    for (int i = 0; i < numPoints; ++i)
    {
        float x1 = rcPlot.x + (dx * i);
        float y1 = rcPlot.y;
        float x2 = x1;
        float y2 = rcPlot.y + rcPlot.h;
        if ( i%10 == 0 )
        {
            m_xAxis.addVertex( { x1,y1,0, 0,0,-1, dbRGB(255,255,255), 0,0 } );
            m_xAxis.addVertex( { x2,y2,0, 0,0,-1, dbRGB(255,255,255), 0,0 } );
        }
        else
        {
            m_xAxis.addVertex( { x1,y1,0, 0,0,-1, dbRGB(140,140,140), 0,0 } );
            m_xAxis.addVertex( { x2,y2,0, 0,0,-1, dbRGB(140,140,140), 0,0 } );
        }
    }

}

void UI_WinnerScreen::draw()
{
    Panel::draw();

    if (!isVisible()) return;

    const float zoom = m_env->getZoom();
    auto driver = m_env->getVideoDriver();

    const int x = m_rect.x;
    const int y = m_rect.y;
    const int w = m_rect.w;
    const int h = m_rect.h;
    const int p = zoom * 10;

    int d_head = 0;
    uint32_t winnerId = m_game->m_winnerId;
    if (winnerId > 0)
    {
        int head_w = w - 100;
        int head_h = zoom * 64;
        int head_x = x + 50;
        int head_y = y + p;
        de::Recti rcHead(head_x, head_y, head_w, head_h);
        de::Recti rcIcon(head_x, head_y, head_h, head_h);
        de::Recti rcText(head_x + head_h + p, head_y, head_w - head_h - p, head_h);
        d_head = rcHead.h;

        // --- Draw Winner Icon ---
        const H3_Player& winner = m_game->getPlayer(winnerId);
        const int vicPoints = winner.state.computeVictoryPoints();
        auto icon = m_game->getTex((H3_Tex::eID)winner.avatar, "WinnerScreen");

        driver->getScreenRenderer()->
            draw2DRect( rcIcon, 0xFFFFFFFF, icon );

        // --- Draw Winner Title ---
        driver->draw2DText(
            rcText.x,rcText.centerY(),
            dbStr("Winner is ",winner.name," with ",vicPoints," points."),
            winner.color,
            de::Align::LeftMiddle,
            de::Font("Postamt", zoom * 32));
    }

    int h_buttons = 64;
    de::Recti rcBody(   x + p,
                        y + d_head + p,
                        w - 2*p,
                        h - 2*p - d_head - h_buttons);

    de::Recti rcPlot(   rcBody.x + 50,
                        rcBody.y + 50,
                        rcBody.w - 100,
                        rcBody.h - 100);

    updateMaxima();
    updatePlots(rcPlot);

    driver->getScreenRenderer()->draw2DRect( rcPlot, dbRGB(0,0,0,128) );

    for (Plots& category : m_categories)
    {
        for (Plot& plot : category)
        {
            plot.lineStrip.upload(true);
            driver->getScreenRenderer()->draw2D(plot.lineStrip);
            plot.lineStrip.destroy();
        }
    }

    m_xAxis.upload(true);
    driver->getScreenRenderer()->draw2D(m_xAxis);
    m_xAxis.destroy();

    driver->getScreenRenderer()->draw2DRectLine( rcPlot, dbRGB(255,128,0) );
/*
    int cw = zoom * 32;
    int ch = cw;
    int cx = x+(w-cw)/2;
    int cy = py;

    driver->getScreenRenderer()->draw2DCircle(
        de::Recti(cx, cy, cw, ch),
        winner.color
    );

    de::Font pointsFont("Postamt", zoom * 20);
    driver->getFontRenderer()->draw2DText(
        cx + cw/2, cy + ch/2,
        dbStr(vicPoints),
        0xFF000000,
        de::Align::Centered,
        pointsFont,
        winner.color
    );
*/

    m_btnContinueGame->draw();
    m_btnReturnToMenu->draw();
}

void UI_WinnerScreen::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    setFont( de::Font("Postamt", zoom * 24) );

    // ::::: CloseButton ::::
    int w = (m_rect.w - 200) / 2;
    int h = 64;
    int x = 100;
    int y = m_rect.h - 100;
    m_btnContinueGame->resize(x, y, w, h);  x += w + h;
    m_btnReturnToMenu->resize(x, y, w, h);
}

// static
de::gui::Button*
UI_WinnerScreen::createButton(std::string txt, de::gpu::TexRef ref)
{
    auto btn = new de::gui::Button(txt, m_env, this);
    btn->setRadius( 8 );
    // btn->setFillColor(dbRGB(255,255,255,200),0);
    // btn->setTextColor(dbRGB(255,255,255),1);
    btn->setTexture( ref );
    return btn;
}
