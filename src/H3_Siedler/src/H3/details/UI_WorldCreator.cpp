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
    : de::gui::Widget(env, parent, id, pos)
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

void UI_World::resizeLayout()
{
    const int scr_w = m_env->getScreenWidth();
    const int scr_h = m_env->getScreenHeight();
    const float f = 0.01f * m_env->getScalePc();


}
void UI_World::onEvent( const de::Event& event )
{

}

void UI_World::draw()
{
    Widget::draw();

    if (!isVisible()) return;

    auto siz = getSize();
    auto pos = getAbsolutePos();
    float originX = pos.x + siz.w/2;
    float originY = pos.y + siz.h/2;

    auto texA = m_game->getTex(H3_Tex::TileA, "UI_World.draw()");
    auto texB = m_game->getTex(H3_Tex::TileB, "UI_World.draw()");
    auto texC = m_game->getTex(H3_Tex::TileC, "UI_World.draw()");
    auto texD = m_game->getTex(H3_Tex::TileD, "UI_World.draw()");
    auto texE = m_game->getTex(H3_Tex::TileE, "UI_World.draw()");
    auto texH = m_game->getTex(H3_Tex::TileWater, "UI_World.draw()");
    auto texW = m_game->getTex(H3_Tex::TileDesert, "UI_World.draw()");

    for (const H3_CfgTile & tile : m_cfg.tileList)
    {
        float fX = de::gpu::SMeshHexagon::computeBoardPosX(m_tileSize, m_tileSize, tile.i, tile.j);
        float fY = de::gpu::SMeshHexagon::computeBoardPosY(m_tileSize, m_tileSize, tile.i, tile.j);
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
    : de::gui::Widget(env, parent, id, pos)
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
    DE_WARN("")
}


void UI_WorldCreator::resizeLayout()
{
    const int scr_w = m_env->getScreenWidth();
    const int scr_h = m_env->getScreenHeight();
    const float f = 0.01f * m_env->getScalePc();

    // ::::: Fullscreen Panel ::::
    int p = 10;
    int w = scr_w;
    int h = scr_h;
    int x = 0;
    int y = 0;
    setRect( x,y,w,h );
    setFont( de::Font("Postamt", f * 16) );

    // ::::: Row[0] Title ::::
    w = scr_w/2;
    h = 100;
    m_title->setRect(x + w/2,y,w,h); y += h;

    // ::::: Row[1] PresetBar ::::
    x = 20;
    h = 100;
    w = 150;
    m_presetPrev->setRect(x,y,w,h); x += w + p;
    m_presetNext->setRect(x,y,w,h); x += w + p;
    w = 350;
    m_presetName->setRect(x,y,w,h); x += w + p;
    w = 100;
    m_presetNew->setRect(x,y,w,h); x += w + p;
    w = 200;
    m_presetClone->setRect(x,y,w,h); x += w + p;
    w = 100;
    m_presetDelete->setRect(x,y,w,h); x += w + p;
    w = 200;
    m_presetExport->setRect(x,y,w,h); x += w + p;
    m_presetImport->setRect(x,y,w,h); x += w + p;
    // w = 100;
    // m_presetEdit->setRect(x,y,w,h);
    y += h + 2*p;
    const int y2 = y;

    // ::::: Row[2] IntFields + HexWorldView ::::
    const int q = 10;
    w = f*300;
    h = f*32;
    x = 20;
    //m_edtTilesX->setRect(x,y,w,h);      y += h + q;
    //m_edtTilesY->setRect(x,y,w,h);      y += h + q;
    m_edtVictory->setRect(x,y,w,h);     y += h + q;
    m_edtCardLimit->setRect(x,y,w,h);   y += h + q;
    m_edtSpacing->setRect(x,y,w,h);     y += h + q;
    m_edtPlayer->setRect(x,y,w,h);

    // ::::: Row[2] Body ::::
    y = y2;
    x += w + p;
    w = scr_w - p - x;
    h = scr_h - 2*p - y - 200;
    m_body->setRect(x,y,w,h);

    // ::::: Row[N] Main-Buttons ::::
    w = 400;
    h = 100;
    x = 200;
    y = scr_h - 100 - h;
    m_btnClose->setRect(x,y,w,h); x += w + 100;
    m_btnStart->setRect(x,y,w,h);
}

const std::string& UI_WorldCreator::getPresetName() const
{
    return m_presetName->getText();
}

H3_Cfg& UI_WorldCreator::getPreset( const std::string& presetName )
{
    auto it = m_presets.find( presetName );
    if (it == m_presets.end())
    {
        auto s = dbStr("No preset with name ",presetName);
        throw std::runtime_error(s);
    }
    return it->second;
}

H3_Cfg& UI_WorldCreator::getPreset()
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

    //calcTileBounds();
    //m_edtTilesX->setValue( m_tileBounds.w );
    //m_edtTilesY->setValue( m_tileBounds.h );
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

// static
de::gui::Button*
UI_WorldCreator::createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, pos);
    btn->setRadius( 8 );
    btn->setFillColor(dbRGBA(255,255,255,200),0);
    btn->setTextColor(dbRGBA(255,255,255),1);
    btn->setTexture( ref );
    return btn;
}

// static
de::gui::IntField*
UI_WorldCreator::createIntField(std::string txt, int val, int min, int max)
{
    auto edt = new de::gui::IntField(txt, m_env, this, 0, de::Recti(0,0,10,10));
    edt->setMinimum(min);
    edt->setMaximum(max);
    edt->setValue(val);
    auto tex = m_game->getTex( H3_Tex::Scroll1, "UI_WorldCreator.createIntField()" );
    //edt->setFillColor( 0xffffffff );
    //edt->setTexture( tex );
    edt->getMinusButton()->setTexture( tex );
    edt->getPlusButton()->setTexture( tex );
    return edt;
}


void UI_WorldCreator::draw()
{
    Widget::draw();
}
