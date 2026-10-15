#include "UI_WorldCreator.h"
#include <H3/H3_Game.h>
// #include <H3/details/H3_Topology.h>
#include <H3/details/H3_UI.h>
// #include <de_opengl.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>
#include <de/gui/IntField.h>

// =====================================================================
UI_World::UI_World( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
{

}

UI_World::~UI_World()
{
    DE_WARN("")
}

void UI_World::setPreset( const H3_Cfg& cfg )
{
    m_cfg = cfg;
}

H3_Cfg& UI_World::getPreset()
{
    return m_cfg;
}

void UI_World::draw()
{
    Panel::draw();

    if (!isVisible()) return;

    // for (auto & child : m_children)
    // {
    //     child->draw();
    // }

    auto r = getAbsoluteRect();
    float originX = r.centerX();
    float originY = r.centerY();

    auto texA = m_game->getTex(H3_Tex::TileA, "UI_World.draw()");
    auto texB = m_game->getTex(H3_Tex::TileB, "UI_World.draw()");
    auto texC = m_game->getTex(H3_Tex::TileC, "UI_World.draw()");
    auto texD = m_game->getTex(H3_Tex::TileD, "UI_World.draw()");
    auto texE = m_game->getTex(H3_Tex::TileE, "UI_World.draw()");
    auto texH = m_game->getTex(H3_Tex::TileWater, "UI_World.draw()");
    auto texW = m_game->getTex(H3_Tex::TileDesert, "UI_World.draw()");

    for (const H3_CfgTile & tile : m_cfg.tileList)
    {
        float fX = de::smesh::SMeshHexagon::computeBoardPosX(m_tileSize, m_tileSize, tile.i, tile.j);
        float fY = de::smesh::SMeshHexagon::computeBoardPosY(m_tileSize, m_tileSize, tile.i, tile.j);
        float pX = m_scale * fX;
        float pY = m_scale * fY;
        float pW = m_scale * m_tileSize;
        float pH = m_scale * m_tileSize;

        float oX = originX + pX + m_off_x;
        float oY = originY - pY + m_off_y - (0.5f * pH);

        de::gpu::TexRef tex;
        switch (tile.tileType)
        {
            case H3_TileType::TileA: tex = texA; break;
            case H3_TileType::TileB: tex = texB; break;
            case H3_TileType::TileC: tex = texC; break;
            case H3_TileType::TileD: tex = texD; break;
            case H3_TileType::TileE: tex = texE; break;
            case H3_TileType::Water: tex = texH; break;
            case H3_TileType::Desert:tex = texW; break;
            case H3_TileType::PortA: tex = texH; break;
            case H3_TileType::PortB: tex = texH; break;
            case H3_TileType::PortC: tex = texH; break;
            case H3_TileType::PortD: tex = texH; break;
            case H3_TileType::PortE: tex = texH; break;
            case H3_TileType::Port3v1: tex = texH; break;
        }
        H3_drawHexagon(*m_game, de::Recti(oX,oY,pW,pH), 0xffffffff, tex);
    }
}

// =====================================================================
UI_WorldCreator::UI_WorldCreator( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
    , m_btnClose(nullptr)
{
    auto tex1 = game->getTex( H3_Tex::Grunge, "UI_WorldCreator()" );
    setTexture( tex1 );
    setFillColor( 0xffffffff );

    auto texB = m_game->getTex(H3_Tex::Scroll1, "UI_WorldCreator().Scroll1");
    auto r = de::Recti();

    m_title = new de::gui::Label("WorldCreator", env, this, 0, r);
    m_title->setTexture( texB );

    //m_presetLabel = new de::gui::Label("Presets:", env, this, 0, r);
    m_presetPrev = createButton("Prev", texB, r );
    m_presetNext = createButton("Next", texB, r);
    m_presetName = new de::gui::Label("Preset Name", env, this, 0, r);
    m_presetNew = createButton("New", texB, r);
    m_presetClone = createButton("Clone", texB, r);
    m_presetDelete = createButton("Del", texB, r);
    m_presetExport = createButton("Export", texB, r);
    m_presetImport = createButton("Import", texB, r);
    //m_presetEdit = createButton("Edit", texB, r);

    //m_edtTilesX = createIntField("Tiles X", 10, 3, 200);
    //m_edtTilesY = createIntField("Tiles Y", 10, 3, 200);
    m_edtVictory = createIntField("Victory Points", 10, 3, 250);
    m_edtCardLimit = createIntField("Card Steal Limit", 7, 0, 100);
    m_edtSpacing = createIntField("Building Spacing", 1, 0, 1);
    m_edtPlayer = createIntField("Num Players", 4, 2, 16);

    m_btnClose = createButton("Close", texB, r);
    m_btnStart = createButton("Start", texB, r);

    m_btnClose->setOnClicked([&] () { m_game->enterStartMenu(); });
    m_btnStart->setOnClicked([&] () { m_game->enterRound1(); });

    m_body = new UI_World( m_game, env, this, 0, r);
    m_body->setTexture( tex1 );
    m_body->setFillColor( 0xffffffff );
}

UI_WorldCreator::~UI_WorldCreator()
{

}

void UI_WorldCreator::draw()
{
    Panel::draw();

    if (!isVisible()) return;

    for (auto & child : m_children)
    {
        child->draw();
    }

    // m_presetPrev->draw();
    // m_presetNext->draw();
    // m_presetName->draw();
    // m_presetNew->draw();
    // m_presetClone->draw();
    // m_presetDelete->draw();
    // m_presetExport->draw();
    // m_presetImport->draw();
    // m_edtVictory->draw();
    // m_edtCardLimit->draw();
    // m_edtSpacing->draw();
    // m_edtPlayer->draw();
    // m_body->draw();
    // m_btnClose->draw();
    // m_btnStart->draw();
}

void UI_WorldCreator::resize(int x, int y, int w, int h)
{
    m_rect = de::Recti(x,y,w,h);

    const float zoom = m_env->getZoom();
    setFont( de::Font("Postamt", zoom * 16) );

    // ::::: Fullscreen Panel ::::
    int p = zoom * 10;
    int q = p;
    int ln = zoom * 100;

    int bx = x;
    int by = y;
    // ::::: Row[0] Title ::::
    m_title->resize(bx + w/2,by,w,ln); y += ln;

    // ::::: Row[1] PresetBar ::::
    bx = 20;
    int bh = 100;
    int bw = 150;
    m_presetPrev->resize(bx,by,bw,bh); bx += bw + p;
    m_presetNext->resize(bx,by,bw,bh); bx += bw + p;
    bw = 350;
    m_presetName->resize(bx,by,bw,bh); bx += bw + p;
    bw = 100;
    m_presetNew->resize(bx,by,bw,bh); bx += bw + p;
    bw = 200;
    m_presetClone->resize(bx,by,bw,bh); bx += bw + p;
    bw = 100;
    m_presetDelete->resize(bx,by,bw,bh); bx += bw + p;
    bw = 200;
    m_presetExport->resize(bx,by,bw,bh); bx += bw + p;
    m_presetImport->resize(bx,by,bw,bh); bx += bw + p;
    // w = 100;
    // m_presetEdit->setRect(x,y,w,h);
    by += bh + 2*p;
    const int y2 = by;

    // ::::: Row[2] IntFields + HexWorldView ::::

    bw = zoom*300;
    bh = zoom*32;
    bx = 20;
    //m_edtTilesX->setRect(bx,by,bw,bh);   by += ln + q;
    //m_edtTilesY->setRect(bx,by,bw,bh);   by += ln + q;
    m_edtVictory->resize(bx,by,bw,bh);     by += ln + q;
    m_edtCardLimit->resize(bx,by,bw,bh);   by += ln + q;
    m_edtSpacing->resize(bx,by,bw,bh);     by += ln + q;
    m_edtPlayer->resize(bx,by,bw,bh);

    // ::::: Row[2] Body ::::
    by = y2;
    bx += bw + p;
    bw = bw - p - bx;
    bh = h - 2*p - y - 200;
    m_body->resize(bx,by,bw,bh);

    // ::::: Row[N] Main-Buttons ::::
    bw = 400;
    bh = 100;
    bx = 200;
    by = h - 100 - bh;
    m_btnClose->resize(bx,by,bw,bh); bx += bw + 100;
    m_btnStart->resize(bx,by,bw,bh);
}

const std::string&
UI_WorldCreator::getPresetName() const
{
    return m_presetName->getText();
}

H3_Cfg&
UI_WorldCreator::getPreset( const std::string& presetName )
{
    auto it = m_presets.find( presetName );
    if (it == m_presets.end())
    {
        auto s = dbStr("No preset with name ",presetName);
        throw std::runtime_error(s);
    }
    return it->second;
}

H3_Cfg&
UI_WorldCreator::getPreset()
{
    const std::string& presetName = getPresetName();
    if (presetName.empty())
    {
        auto s = dbStr("getPreset() :: Got empty presetName.");
        throw std::runtime_error(s);
    }
    return getPreset( presetName );
}

void UI_WorldCreator::setPreset( const H3_Cfg& cfg )
{
    if (cfg.presetName.empty())
    {
        auto s = dbStr("setPreset() :: Got empty presetName.");
        throw std::runtime_error(s);
    }

    auto it = m_presets.find( cfg.presetName );
    if (it == m_presets.end())
    {
        DE_OK("Add preset ",cfg.presetName);
        m_presets[ cfg.presetName ] = cfg;
    }

    auto& m_cfg = m_presets[ cfg.presetName ];

    m_body->setPreset( m_cfg );
    m_presetName->setText( m_cfg.presetName );
    m_edtVictory->setValue( m_cfg.victoryPoints );
    m_edtCardLimit->setValue( m_cfg.cardLimitNoSteal );
    m_edtSpacing->setValue( m_cfg.buildingSpacing );
    m_edtPlayer->setValue( m_cfg.playerList.size() );
}

void UI_WorldCreator::calcTileBounds()
{
    int x1 = std::numeric_limits<int>::max();
    int y1 = std::numeric_limits<int>::max();
    int x2 = std::numeric_limits<int>::lowest();
    int y2 = std::numeric_limits<int>::lowest();

    auto& m_cfg = getPreset();

    for ( const auto & tile : m_cfg.tileList )
    {
        x1 = std::min( x1, tile.i );
        y1 = std::min( y1, tile.j );
        x2 = std::max( x2, tile.i );
        y2 = std::max( y2, tile.j );
    }

    m_tileBounds = de::Recti(x1, y1, x2-x1, y2-y1);
}

de::gui::Button*
UI_WorldCreator::createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, pos);
    btn->setRadius( 8 );
    btn->setFillColor(dbRGB(255,255,255,200),0);
    btn->setTextColor(dbRGB(255,255,255),1);
    btn->setTexture( ref );
    return btn;
}

de::gui::IntField*
UI_WorldCreator::createIntField(std::string txt, int val, int min, int max)
{
    auto edt = new de::gui::IntField(txt, m_env, this, 0, de::Recti(0,0,10,10));
    edt->setMinimum(min);
    edt->setMaximum(max);
    edt->setValue(val);
    auto tex = m_game->getTex( H3_Tex::Scroll1, "UI_WorldCreator.createIntField()" );
    edt->getMinusButton()->setTexture( tex );
    edt->getPlusButton()->setTexture( tex );
    return edt;
}

