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
    , m_mouseX{ -1 }
    , m_mouseY{ -1 }
    , m_hoveredTileId{ -1 }
    , m_selectedTileId{ -1 }
    , m_bHoveredBoard{ false }
    , m_scale{ 2.f }
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

    m_edtMinX = createIntField("Xmin",-3, -50, -1);
    m_edtMaxX = createIntField("Xmax", 3,   1, 50);
    m_edtMinY = createIntField("Ymin",-3, -50, -1);
    m_edtMaxY = createIntField("Ymax", 3,   1, 50);

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

    m_edtMinX->onValueChanged = [&] (int) { resizeBoard(); };
    m_edtMinY->onValueChanged = [&] (int) { resizeBoard(); };
    m_edtMaxX->onValueChanged = [&] (int) { resizeBoard(); };
    m_edtMaxY->onValueChanged = [&] (int) { resizeBoard(); };

    setPickable(true);
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

void UI_WorldCreator::draw2DHexOutline( int i, int j, u32 color, int width,
                                        const V2& offset, const V2& size)
{
    float w = 100.0f;
    float h = 100.0f;
    const V2 scale(w,h);
    float x =  de::smesh::SMeshHexagon::computeBoardPosX(w,h,i,j);
    float y = -de::smesh::SMeshHexagon::computeBoardPosY(w,h,i,j);
    const V2 pos(x,y);

    // ===============================================
    /// @brief The Hexagon
    // ===============================================
    ///
    ///               M| x=0 | y=0 | z=0 | u=.5| v=.5|
    ///       D       -|-----|-----|-----|-----|-----|
    ///      / \      A|   0 |  0  | -.5 | .5  | 0   |
    ///   C /   \ E   B| -.5 |  0  |-.25 |  0  | 0.25|
    ///    |-----|    C| -.5 |  0  | .25 |  0  | 0.75|
    ///    |  M  |    D| 0.0 |  0  |  .5 |  .5 | 1   |
    ///    |-----|    E| 0.5 |  0  | .25 |  1  | 0.75|
    ///   B \   / F   F| 0.5 |  0  |-.25 |  1  | 0.25|
    ///      \ /
    ///       A       triangles: BFA, BEF, BCE, CDE
    ///
    const V2 pA = (pos + (scale * V2( 0.00, -0.50))) * size + offset;
    const V2 pB = (pos + (scale * V2(-0.50, -0.25))) * size + offset;
    const V2 pC = (pos + (scale * V2(-0.50,  0.25))) * size + offset;
    const V2 pD = (pos + (scale * V2( 0.00,  0.50))) * size + offset;
    const V2 pE = (pos + (scale * V2( 0.50,  0.25))) * size + offset;
    const V2 pF = (pos + (scale * V2( 0.50, -0.25))) * size + offset;
    de::smesh::S3DVertex A( V3(pA,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex B( V3(pB,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex C( V3(pC,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex D( V3(pD,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex E( V3(pE,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex F( V3(pF,0),{0,0,-1},color,{0,0});

    de::smesh::SMeshBuffer m(de::gpu::PrimitiveType::Lines);
    m.addLine(A,B);
    m.addLine(B,C);
    m.addLine(C,D);
    m.addLine(D,E);
    m.addLine(E,F);
    m.addLine(F,A);
    m.upload();

    auto driver = m_env->getVideoDriver();
    driver->getScreenRenderer()->draw2D( m );

    m.destroy();
}

void UI_WorldCreator::drawBoard(const de::Recti & rect)
{
    auto driver = m_env->getVideoDriver();

    if (rect.w < 4 || rect.h < 4) return;

    // [Draw] BackgroundTextureQuad
    de::gpu::TexRef m_grunge = m_game->getTex( H3_Tex::Grunge, __func__ );
    driver->getScreenQuadRenderer()->draw2DRect( rect, m_grunge );

    // const auto& tiles = m_cfg->tiles;
    auto r_board = rect + getAbsoluteRect().pos();

    glm::vec2 offset;
    offset.x = r_board.centerX();
    offset.y = r_board.centerY();
    glm::vec2 scale;
    scale.x = m_scale;
    scale.y = m_scale;
    glm::vec4 posTransform(offset.x,offset.y,scale.x,scale.y);

    // Scissor origin is bottom-left, not like UI top-left
    de::Recti scissor = r_board;
    scissor.y = driver->getScreenHeight() - (r_board.y + r_board.h);

    m_game->m_board2DRenderer.render( posTransform, scissor );

    if (m_bHoveredBoard)
    {
        driver->getScreenRenderer()->draw2DRectLine( r_board, dbRGB(255,128,20) );
    }

    if (m_selectedTileId > -1)
    {
        const H3_CfgTile & tile = m_cfg.tiles[ m_selectedTileId ];
        draw2DHexOutline(tile.i(), tile.j(), tile.tt().color(), 10, offset, scale);
    }

    if (m_hoveredTileId > -1)
    {
        const H3_CfgTile & tile = m_cfg.tiles[ m_hoveredTileId ];
        draw2DHexOutline(tile.i(), tile.j(), 0xFFFFFFFF, 20, offset, scale);
    }

    driver->draw2DText(
        r_board.x2() - 4,
        r_board.y1() + 4,
        dbStr("Scale(",m_scale,")"),
        dbRGB(255,255,0),
        de::Align::RightTop,
        de::Font5x8(3,3,0,0,2,1));

    m_hoveredTileId = -1;

    for (size_t i = 0; i < m_cfg.tiles.size(); ++i)
    {
        const H3_CfgTile & tile = m_cfg.tiles[i];

        if (tile.isMouseOver(m_mouseX,m_mouseY,scale,offset))
        {
            m_hoveredTileId = i;
            driver->draw2DText(
                m_mouseX + 20,
                m_mouseY + 20,
                dbStr("Tile[",i,"] i(",tile.i(),"), j(",tile.j(),"), typ(",tile.tt().str(),")"),
                dbRGB(255,255,0),
                de::Align::RightTop,
                de::Font5x8(3,3,0,0,2,1));
        }
    }

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

    bx = 20;
    // m_edtMinX->resize(bx,by,bw,bh);           by += ln + q;
    // m_edtMaxX->resize(bx,by,bw,bh);           by += ln + q;
    // m_edtMinY->resize(bx,by,bw,bh);           by += ln + q;
    // m_edtMaxY->resize(bx,by,bw,bh);           by += ln + q;

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

void UI_WorldCreator::resizeBoard()
{
    int minX = m_edtMinX->getValue();
    int minY = m_edtMinY->getValue();
    int maxX = m_edtMaxX->getValue();
    int maxY = m_edtMaxY->getValue();
    DE_DEBUG("min(",minX,",",minY,"), max(",maxX,",",maxY,")")
    resizeBoard(minX,minY,maxX,maxY);
}

void UI_WorldCreator::resizeBoard(int minX, int minY, int maxX, int maxY)
{
    m_hoveredTileId = -1;
    m_selectedTileId = -1;

    // Build new board
    std::vector<H3_CfgTile> tiles;
    int dx = maxX - minX + 1;
    int dy = maxY - minY + 1;
    tiles.reserve(dx * dy);

    // Build new board with only Unknown tiles
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            tiles.emplace_back();
            H3_CfgTile & tile = tiles.back();
            tile.setTileType(H3_TileType::Unknown);
            tile.setPos(x,y);
        }
    }

    // Replace possible Unknown tiles.
    for (const H3_CfgTile & tile : m_cfg.tiles)
    {
        if (tile.i() < minX || tile.i() > maxX) continue;
        if (tile.j() < minY || tile.j() > maxY) continue;

        int x = tile.i() - minX;
        int y = tile.j() - minY;
        int linearIndex = dx * y + x;
        tiles[ linearIndex ] = tile;
    }

    // Store
    std::swap(m_cfg.tiles, tiles);

    // Update
    m_cfg.computeBounds();

    m_game->m_board2DRenderer.
        collectAndUploadInstances(*m_game, m_cfg);
}

void UI_WorldCreator::setCfg( const H3_Cfg& cfg )
{
    if (cfg.presetName.empty())
    {
        auto s = dbStr("setPreset() :: Got empty presetName.");
        throw std::runtime_error(s);
    }

    m_hoveredTileId = -1;
    m_selectedTileId = -1;
    m_cfg = cfg;
    m_cfg.computeBounds();
    resizeBoard(m_cfg.min.x, m_cfg.min.y, m_cfg.max.x, m_cfg.max.y);
    m_edtMinX->setValue(m_cfg.min.x);
    m_edtMinY->setValue(m_cfg.min.y);
    m_edtMaxX->setValue(m_cfg.max.x);
    m_edtMaxY->setValue(m_cfg.max.y);

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

void UI_WorldCreator::onEvent(const de::Event& event)
{
    de::gui::Widget::onEvent(event);

    auto pos = getAbsoluteRect().pos();

    if (event.type == de::EventType::MOUSE_MOVE)
    {
        int mx = event.mouseMoveEvent.x;
        int my = event.mouseMoveEvent.y;
        m_mouseX = mx;
        m_mouseY = my;
        de::Recti r_board = m_rcBoard + pos;
        //DE_TRACE("MouseMove(",mx,",",my,"), r_board(",r_board.str(),")")

        m_relativePosToBoard.x = mx - r_board.x;
        m_relativePosToBoard.y = my - r_board.y;

        if (r_board.contains(mx,my))
        {
            m_bHoveredBoard = true;
        }
        else
        {
            m_bHoveredBoard = false;
        }
    }

    if (event.type == de::EventType::MOUSE_WHEEL)
    {
        float mx = event.mouseWheelEvent.x;
        float my = event.mouseWheelEvent.y;

        if (my > 0)
        {
            m_scale *= 1.025f;
            //DE_TRACE("ZoomOut(",m_scale,") MouseWheel(",mx,",",my,")")
        }
        else if (my < 0)
        {
            m_scale /= 1.025f;
            //DE_TRACE("ZoomIn(",m_scale,") MouseWheel(",mx,",",my,")")
        }
    }

    if (event.type == de::EventType::MOUSE_RELEASE)
    {
        if (event.mouseReleaseEvent.isLeft())
        {
            DE_DEBUG("m_hoveredTileId = ",m_hoveredTileId)
            if (m_hoveredTileId > -1)
            {
                m_selectedTileId = m_hoveredTileId;
            }
        }
    }
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
    edt->setMinusButtonTexture( tex );
    edt->setPlusButtonTexture( tex );
    return edt;
}

