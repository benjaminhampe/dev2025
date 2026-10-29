#include "UI_WorldCreator.h"
#include <H3/H3_Game.h>
// #include <H3/details/H3_Topology.h>
#include <H3/details/H3_UI.h>
// #include <de_opengl.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>
#include <de/gui/IntField.h>

/*
void drawHexagon( H3_Game & game, const de::Recti& pos,
                const uint32_t color, const de::gpu::TexRef& tex, int num )
{
    auto driver = game.getDriver();
    driver->getScreenRenderer()->draw2DHexagon( pos, color, tex );

    if ( num > -1 )
    {
        const float zoom = game.getZoom();
        driver->draw2DText(
            pos.centerX(),
            pos.centerY(),
            std::to_string(num),
            0xFFFFFFFF,
            de::Align::Centered,
            de::Font5x8(zoom,zoom) );
    }
}
*/

// =====================================================================
UI_WorldCreator::UI_WorldCreator( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
    , m_scale{ 128.f }
    , m_off_x{ 0.f }
    , m_off_y{ 0.f }
    , m_btnClose(nullptr)
{
    m_backgroundTex = game->getTex( H3_Tex::Grunge, "UI_WorldCreator()" );

    auto tex = m_game->getTex(H3_Tex::Scroll1, "UI_WorldCreator().Scroll1");
    m_title = new de::gui::Label("WorldCreator", env, this, 0, de::Recti());
    m_title->setTexture(tex);

    //m_presetLabel = new de::gui::Label("Presets:", env, this, 0, r);
    m_presetPrev = createButton("Prev");
    m_presetNext = createButton("Next");
    m_presetName = new de::gui::Label("Preset Name", env, this, 0, de::Recti());
    m_presetNew = createButton("New");
    m_presetClone = createButton("Clone");
    m_presetDelete = createButton("Del");
    m_presetExport = createButton("Export");
    m_presetImport = createButton("Import");
    //m_presetEdit = createButton("Edit");

    m_edtMinX = createIntField("Min X",-3, -50, -1);
    m_edtMaxX = createIntField("Max X", 3,   1, 50);
    m_edtMinY = createIntField("Min Y",-3, -50, -1);
    m_edtMaxY = createIntField("Max Y", 3,   1, 50);

    m_edtVictoryPoints = createIntField("Victory Points", 10, 3, 250);
    m_edtCardLimit = createIntField("Card Steal Limit", 7, 0, 100);
    m_edtBuildingSpacing = createIntField("Building Spacing", 1, 0, 1);
    m_edtNumPlayers = createIntField("Num Players", 4, 2, 5);
    m_edtSkipFirstRound = createIntField("Skip First Round", 1, 0, 1);
    //m_edtSkipFirstRound = createIntField("Skip First Round", 1, 0, 1);

    m_btnClose = createButton("Close");
    m_btnStart = createButton("Start");

    m_btnClose->onClicked = [&] () { m_game->exitGame(); };
    m_btnStart->onClicked = [&] () { m_game->startGame(); };
}

UI_WorldCreator::~UI_WorldCreator()
{

}

void UI_WorldCreator::draw()
{
    if (!isVisible()) return;

    if (!m_env) { DE_ERROR("No env") return; }
    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_ERROR("No driver") return; }

    const auto pos = getAbsoluteRect();

    // DE_DEBUG("Draw(",pos.str(),")")

    driver->getScreenQuadRenderer()->
        draw2DRect( pos, m_backgroundTex, false );

    drawBoard(m_rcBoard);


    m_presetPrev->draw();
    m_presetNext->draw();
    m_presetName->draw();
    m_presetNew->draw();
    m_presetClone->draw();
    m_presetDelete->draw();
    m_presetExport->draw();
    m_presetImport->draw();

    m_edtMinX->draw();
    m_edtMaxX->draw();
    m_edtMinY->draw();
    m_edtMaxY->draw();
    m_edtVictoryPoints->draw();
    m_edtCardLimit->draw();
    m_edtBuildingSpacing->draw();
    m_edtNumPlayers->draw();
    m_edtSkipFirstRound->draw();

    m_btnClose->draw();
    m_btnStart->draw();
}

void UI_WorldCreator::drawBoard(const de::Recti & rect)
{
    auto driver = m_env->getVideoDriver();

    if (rect.w < 4 || rect.h < 4) return;

    // [Draw] BackgroundTextureQuad
    de::gpu::TexRef m_grunge = m_game->getTex( H3_Tex::Grunge, __func__ );
    driver->getScreenQuadRenderer()->draw2DRect( rect, m_grunge );

    // const auto& tiles = m_cfg->tiles;

    auto pos = getAbsoluteRect().pos();

    glm::vec4 posTransform;
    posTransform.x = pos.x + rect.centerX();
    posTransform.y = pos.y + rect.centerY();
    posTransform.z = m_scale;
    posTransform.w = m_scale;

    m_game->m_board2DRenderer.render( posTransform );

/*

    float originX = pos.x + rect.centerX();
    float originY = pos.y + rect.centerY();

    for (size_t i = 0; i < m_tiles.size(); ++i)
    {
        const H3_CfgTile & tile = m_tiles[i];
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
            case H3_TileType::TileA: tex = m_texA; break;
            case H3_TileType::TileB: tex = m_texB; break;
            case H3_TileType::TileC: tex = m_texC; break;
            case H3_TileType::TileD: tex = m_texD; break;
            case H3_TileType::TileE: tex = m_texE; break;
            case H3_TileType::Water: tex = m_texH; break;
            case H3_TileType::Desert:tex = m_texW; break;
            case H3_TileType::PortA: tex = m_texH; break;
            case H3_TileType::PortB: tex = m_texH; break;
            case H3_TileType::PortC: tex = m_texH; break;
            case H3_TileType::PortD: tex = m_texH; break;
            case H3_TileType::PortE: tex = m_texH; break;
            case H3_TileType::Port3v1: tex = m_texH; break;
        }
        H3_drawHexagon(*m_game, de::Recti(oX,oY,pW,pH), 0xffffffff, tex, i);
    }
*/
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
    m_presetImport->resize(bx,by,bw,bh); // bx += bw + p;
    // w = 100;
    // m_presetEdit->setRect(x,y,w,h);
    by += bh + 2*p;
    const int y2 = by;

    // ::::: Row[2] IntFields + HexWorldView ::::
    ln = zoom * 40;
    bw = zoom * 300;
    bh = zoom * 32;
    bx = 20;
    m_edtMinX->resize(bx,by,bw/2,bh);           bx += bw/2;
    m_edtMaxX->resize(bx,by,bw/2,bh);           by += ln + q;
    bx = 20;
    m_edtMinY->resize(bx,by,bw/2,bh);           bx += bw/2;
    m_edtMaxY->resize(bx,by,bw/2,bh);           by += ln + q;

    m_edtVictoryPoints->resize(bx,by,bw,bh);    by += ln + q;
    m_edtCardLimit->resize(bx,by,bw,bh);        by += ln + q;
    m_edtBuildingSpacing->resize(bx,by,bw,bh);  by += ln + q;
    m_edtNumPlayers->resize(bx,by,bw,bh);       by += ln + q;
    m_edtSkipFirstRound->resize(bx,by,bw,bh);   //by += ln + q;

    // ::::: Row[2] World Body ::::
    by = y2;
    bx += bw + p;
    bw = w - p - bx;
    bh = h - 2*p - y - 200;
    m_rcBoard = de::Recti(bx,by,bw,bh);
    // DE_DEBUG("world.rect = ",m_world->getAbsoluteRect().str())

    // ::::: Row[N] Main-Buttons ::::
    bw = 400;
    bh = 100;
    bx = 200;
    by = h - 100 - bh;
    m_btnClose->resize(bx,by,bw,bh); bx += bw + 100;
    m_btnStart->resize(bx,by,bw,bh);
}

void UI_WorldCreator::setCfg( const H3_Cfg& cfg )
{
    if (cfg.presetName.empty())
    {
        auto s = dbStr("setPreset() :: Got empty presetName.");
        throw std::runtime_error(s);
    }

    m_cfg = cfg;

    m_game->m_board2DRenderer.
        collectAndUploadInstances(*m_game, m_cfg, m_game->m_tileSize);

    m_presetName->setText( m_cfg.presetName );
    m_edtVictoryPoints->setValue( m_cfg.victoryPoints );
    m_edtCardLimit->setValue( m_cfg.cardLimitNoSteal );
    m_edtBuildingSpacing->setValue( m_cfg.buildingSpacing );
    m_edtNumPlayers->setValue( m_cfg.numPlayers );
    m_edtSkipFirstRound->setValue( m_cfg.skipFirstRound );
}

H3_Cfg
UI_WorldCreator::getCfg()
{
    H3_Cfg cfg = H3_Cfg::createStandard();
    cfg.presetName = m_presetName->getText();
    cfg.buildingSpacing = m_edtBuildingSpacing->getValue();
    cfg.cardLimitNoSteal = m_edtCardLimit->getValue();
    cfg.victoryPoints = m_edtVictoryPoints->getValue();
    cfg.skipFirstRound = m_edtSkipFirstRound->getValue();
    cfg.numPlayers = m_edtNumPlayers->getValue();
    cfg.tiles = m_cfg.tiles;
    cfg.player = m_cfg.player;
    return cfg;
}

/*
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

    m_world->setPreset( &m_cfg );
    m_presetName->setText( m_cfg.presetName );
    m_edtVictoryPoints->setValue( m_cfg.victoryPoints );
    m_edtCardLimit->setValue( m_cfg.cardLimitNoSteal );
    m_edtBuildingSpacing->setValue( m_cfg.buildingSpacing );
    m_edtPlayer->setValue( m_cfg.playerList.size() );
}
*/

de::gui::Button*
UI_WorldCreator::createButton(std::string txt)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, de::Recti());
    btn->setRadius( 8 );

    auto tex = m_game->getTex(H3_Tex::Scroll1, "UI_WorldCreator().Scroll1");

    // btn->setFillColor(dbRGB(255,255,255,200),0);
    // btn->setTextColor(dbRGB(255,255,255),dbRGB(255,255,255), 1);
    btn->setTexture( tex );
    return btn;
}

de::gui::IntField*
UI_WorldCreator::createIntField(std::string txt, int val, int min, int max)
{
    auto edt = new de::gui::IntField(txt, m_env, this, 0, de::Recti());
    edt->setRange(min,max);
    edt->setValue(val);
    auto tex = m_game->getTex( H3_Tex::Scroll1, "UI_WorldCreator.createIntField()" );
    edt->getMinusButton()->setTexture( tex );
    edt->getPlusButton()->setTexture( tex );
    return edt;
}

