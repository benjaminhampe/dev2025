#include "UI_WorldCreator.h"
#include <H3/H3_Game.h>
// #include <H3/details/H3_Topology.h>
#include <H3/details/H3_UI.h>
// #include <de_opengl.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>
#include <de/gui/IntField.h>

de::gui::IntField*
UI_TileEditor::createIntField(std::string txt, int val, int min, int max)
{
    auto edt = new de::gui::IntField(txt, m_env, this, 0, de::Recti());
    edt->setRange(min,max);
    edt->setValue(val);
    auto tex = m_game->getTex( H3_Tex::Scroll1, "UI_WorldCreator.createIntField()" );
    edt->setMinusButtonTexture( tex );
    edt->setPlusButtonTexture( tex );
    return edt;
}

// ===========================================================================
UI_TileEditor::UI_TileEditor( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// ===========================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game{ game }
{
    m_i = createIntField("X-Col",0, -100, 100);
    m_j = createIntField("Y-Row",0, -100, 100);
    m_tileType = createIntField("Tile Type", 0, 0, H3_TileType::Count-1);
    m_diceValue = createIntField("Dice Value", 8, 2, 12);
    m_angle60 = createIntField("Angle", 0, 0, 5);

    m_tileType->onValueChanged = [&] (int) { emit_onChange(); };
    m_diceValue->onValueChanged = [&] (int) { emit_onChange(); };
    m_angle60->onValueChanged = [&] (int) { emit_onChange(); };
}

UI_TileEditor::~UI_TileEditor()
{
}
void UI_TileEditor::draw()
{
    if (!isVisible()) return;

    if (!m_env) { DE_ERROR("No env") return; }
    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_ERROR("No driver") return; }

    const auto pos = getAbsoluteRect();

    // [Draw] BackgroundTexture
    auto bg = m_game->getTex( H3_Tex::Grunge, "UI_TileEditor" );
    driver->getScreenQuadRenderer()->draw2DRect( pos, bg, false );

    m_i->draw();
    m_j->draw();
    m_tileType->draw();
    m_diceValue->draw();
    m_angle60->draw();
}

void UI_TileEditor::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    setFont( de::Font("Postamt", zoom * 16) );

    int x = 0;
    int y = 0;
    int w = m_rect.w;
    int ln = m_rect.h/5;
    int h = 4*ln/5;

    m_i->resize(x,y,w,h); y += ln;
    m_j->resize(x,y,w,h); y += ln;
    m_tileType->resize(x,y,w,h); y += ln;
    m_diceValue->resize(x,y,w,h); y += ln;
    m_angle60->resize(x,y,w,h);
}

void UI_TileEditor::emit_onChange()
{
    if (onChange)
    {
        onChange();
    }
}

void
UI_TileEditor::setCfg( const H3_CfgTile& tile )
{
    m_tile = tile;
    m_i->setValue(tile.i());
    m_j->setValue(tile.j());
    m_tileType->setValue( tile.tt(), true);
    m_diceValue->setValue( tile.diceValue(), true);
    m_angle60->setValue( tile.angle60(), true);
}

H3_CfgTile
UI_TileEditor::getCfg() const
{
    H3_CfgTile cfg;
    cfg.setPos(m_tile.i(), m_tile.j());
    cfg.setDiceValue( m_diceValue->getValue() );
    cfg.setTileType( m_tileType->getValue() );
    cfg.setAngle60( m_angle60->getValue() );
    return cfg;
}


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
    , m_presetIndex{ -1 }
    , m_btnClose(nullptr)
{
    m_backgroundTex = game->getTex( H3_Tex::Grunge, "UI_WorldCreator()" );

    auto tex = m_game->getTex(H3_Tex::Scroll1, "UI_WorldCreator().Scroll1");
    m_title = new de::gui::Label("WorldCreator", env, this, 0, de::Recti());
    m_title->setTexture(tex);

    m_presetPrev = createButton("Prev");
    m_presetNext = createButton("Next");
    m_presetName = createButton("Preset Name");
    m_presetName->setTexture(de::gpu::TexRef());
    m_presetNew = createButton("New");
    m_presetClone = createButton("Clone");
    m_presetDelete = createButton("Del");
    m_presetExport = createButton("Save");
    m_presetImport = createButton("Load");

    m_presetPrev->onClicked = [&] () { on_presetPrev(); };
    m_presetNext->onClicked = [&] () { on_presetNext(); };
    m_presetName->onClicked = [&] () { on_presetName(); };
    m_presetNew->onClicked = [&] () { on_presetNew(); };
    m_presetClone->onClicked = [&] () { on_presetClone(); };
    m_presetDelete->onClicked = [&] () { on_presetDelete(); };
    m_presetExport->onClicked = [&] () { on_presetExport(); };
    m_presetImport->onClicked = [&] () { on_presetImport(); };

    m_edtMinX = createIntField("Xmin",-3, -50, -1);
    m_edtMaxX = createIntField("Xmax", 3,   1, 50);
    m_edtMinY = createIntField("Ymin",-3, -50, -1);
    m_edtMaxY = createIntField("Ymax", 3,   1, 50);

    m_edtMinX->onValueChanged = [&] (int) { resizeBoard(); };
    m_edtMaxX->onValueChanged = [&] (int) { resizeBoard(); };
    m_edtMinY->onValueChanged = [&] (int) { resizeBoard(); };
    m_edtMaxY->onValueChanged = [&] (int) { resizeBoard(); };

    m_edtVictoryPoints = createIntField("Victory Points", 10, 3, 250);
    m_edtCardLimit = createIntField("Card Steal Limit", 7, 0, 100);
    m_edtBuildingSpacing = createIntField("Building Spacing", 1, 0, 1);
    m_edtNumPlayers = createIntField("Num Players", 4, 2, 5);
    m_edtSkipFirstRound = createIntField("Skip First Round", 1, 0, 1);

    m_tileEditor = new UI_TileEditor(m_game,m_env,this);
    m_tileEditor->setVisible(false);
    m_tileEditor->onChange = [&] () {
        if (m_selectedTileId > -1)
        {
            m_cfg->tiles[ m_selectedTileId ] = m_tileEditor->getCfg();
            updateBoard();
        }
    };

    m_btnClose = createButton("Close");
    m_btnStart = createButton("Start");

    m_btnClose->onClicked = [&] () { m_game->exitGame(); };
    m_btnStart->onClicked = [&] () { m_game->startGame(); };

    setPickable(true);

    // Create vector of internal presets
    m_presets.emplace_back( H3_Cfg::createStandard() );
    m_presets.emplace_back( H3_Cfg::createLanding() );
    m_presets.emplace_back( H3_Cfg::createWarmth() );
    m_presets.emplace_back( H3_Cfg::createCattleMania() );
    setCfg(0);
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

    m_tileEditor->draw();

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

void UI_WorldCreator::draw2DPortBridges(int angle60,
    int i, int j, u32 color, int width,
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
    const V2 pM = (pos + (scale * V2( 0.00,  0.00))) * size + offset;
    const V2 pA = (pos + (scale * V2( 0.00,  0.50))) * size + offset;
    const V2 pB = (pos + (scale * V2(-0.50,  0.25))) * size + offset;
    const V2 pC = (pos + (scale * V2(-0.50, -0.25))) * size + offset;
    const V2 pD = (pos + (scale * V2( 0.00, -0.50))) * size + offset;
    const V2 pE = (pos + (scale * V2( 0.50, -0.25))) * size + offset;
    const V2 pF = (pos + (scale * V2( 0.50,  0.25))) * size + offset;
    de::smesh::S3DVertex M( V3(pM,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex A( V3(pA,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex B( V3(pB,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex C( V3(pC,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex D( V3(pD,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex E( V3(pE,0),{0,0,-1},color,{0,0});
    de::smesh::S3DVertex F( V3(pF,0),{0,0,-1},color,{0,0});

    de::smesh::SMeshBuffer m(de::gpu::PrimitiveType::Lines);
    switch (angle60)
    {
        case 0: { m.addLine(M,A); m.addLine(M,B); break; }
        case 1: { m.addLine(M,B); m.addLine(M,C); break; }
        case 2: { m.addLine(M,C); m.addLine(M,D); break; }
        case 3: { m.addLine(M,D); m.addLine(M,E); break; }
        case 4: { m.addLine(M,E); m.addLine(M,F); break; }
        case 5: { m.addLine(M,F); m.addLine(M,A); break; }
        default: break;
    }
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

    driver->setScissor(r_board);

    glm::vec2 offset;
    offset.x = r_board.centerX();
    offset.y = r_board.centerY();
    glm::vec2 scale;
    scale.x = m_scale;
    scale.y = m_scale;
    glm::vec4 posTransform(offset.x,offset.y,scale.x,scale.y);

    m_game->m_tile2DRenderer.render( posTransform, r_board );
    m_game->m_chip2DRenderer.render( posTransform, r_board );
    //m_game->m_road2DRenderer.render( posTransform, r_board );

    if (m_bHoveredBoard)
    {
        driver->getScreenRenderer()->draw2DRectLine( r_board, dbRGB(255,128,20) );
    }

    if (m_selectedTileId > -1)
    {
        const H3_CfgTile & tile = m_cfg->tiles[ m_selectedTileId ];
        draw2DHexOutline(tile.i(), tile.j(), tile.tt().color(), 10, offset, scale);
    }

    if (m_hoveredTileId > -1)
    {
        const H3_CfgTile & tile = m_cfg->tiles[ m_hoveredTileId ];
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

    for (size_t i = 0; i < m_cfg->tiles.size(); ++i)
    {
        const H3_CfgTile & tile = m_cfg->tiles[i];

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

    // [Draw] PortBridges
    size_t i = 0;
    for (const H3_CfgTile & tile : m_cfg->tiles)
    {
        if (!tile.tt().isPort()) { continue; }

        draw2DPortBridges(tile.angle60(),tile.i(),tile.j(),0xFFFFFFFF,
            10, offset, scale);

        i++;
    }

    //DE_DEBUG("Ports = ",i)

    driver->setScissor(de::Recti());
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
    m_edtVictoryPoints->resize(bx,by,bw,bh);    by += ln + q;
    m_edtCardLimit->resize(bx,by,bw,bh);        by += ln + q;
    m_edtBuildingSpacing->resize(bx,by,bw,bh);  by += ln + q;
    m_edtNumPlayers->resize(bx,by,bw,bh);       by += ln + q;
    m_edtSkipFirstRound->resize(bx,by,bw,bh);

    // ::::: Row[2] World Body ::::
    by = y2;
    bx += bw + p;
    bw = w - p - bx;
    bh = h - 2*p - y - 200;
    m_rcBoard = de::Recti(bx,by,bw,bh);
    // DE_DEBUG("world.rect = ",m_world->getAbsoluteRect().str())

    bw = zoom * 300;
    bh = 5*ln;
    bx = 20;
    by = m_rcBoard.y2() - bh;
    m_tileEditor->resize(bx,by,bw,bh);

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
    for (const H3_CfgTile & tile : m_cfg->tiles)
    {
        if (tile.i() < minX || tile.i() > maxX) continue;
        if (tile.j() < minY || tile.j() > maxY) continue;

        int x = tile.i() - minX;
        int y = tile.j() - minY;
        int linearIndex = dx * y + x;
        tiles[ linearIndex ] = tile;
    }

    // Store
    std::swap(m_cfg->tiles, tiles);

    // Update
    m_cfg->computeBounds();

    updateBoard();
}

void UI_WorldCreator::updateBoard()
{
    m_game->m_tile2DRenderer.collectAndUploadInstances(*m_game, *m_cfg);
    m_game->m_chip2DRenderer.collectAndUploadInstances(*m_game, *m_cfg);
    //m_game->m_road2DRenderer.collectAndUploadInstances(*m_game, *m_cfg);
}

void UI_WorldCreator::setCfg(int i)
{
    m_cfg = nullptr;
    m_presetIndex = i;
    m_hoveredTileId = -1;
    m_selectedTileId = -1;
    m_tileEditor->setVisible(false);

    if (i < 0 || i >= int(m_presets.size()))
    {
        DE_ERROR("Invalid preset index ",i," of ",m_presets.size()," presets")
        return;
    }

    m_cfg = &m_presets.at(i);

    DE_DEBUG("presetName = ",m_cfg->presetName)

    m_cfg->computeBounds();

    m_edtMinX->setValue(m_cfg->min.x, true);
    m_edtMinY->setValue(m_cfg->min.y, true);
    m_edtMaxX->setValue(m_cfg->max.x, true);
    m_edtMaxY->setValue(m_cfg->max.y, true);
    resizeBoard();

    m_presetName->setText( m_cfg->presetName );
    m_edtVictoryPoints->setValue( m_cfg->victoryPoints );
    m_edtCardLimit->setValue( m_cfg->cardLimitNoSteal );
    m_edtBuildingSpacing->setValue( m_cfg->buildingSpacing );
    m_edtNumPlayers->setValue( m_cfg->numPlayers );
    m_edtSkipFirstRound->setValue( m_cfg->skipFirstRound );
}

H3_Cfg
UI_WorldCreator::getCfg()
{
    H3_Cfg cfg = *m_cfg;
    cfg.presetName = m_presetName->getText();
    cfg.buildingSpacing = m_edtBuildingSpacing->getValue();
    cfg.cardLimitNoSteal = m_edtCardLimit->getValue();
    cfg.victoryPoints = m_edtVictoryPoints->getValue();
    cfg.skipFirstRound = m_edtSkipFirstRound->getValue();
    cfg.numPlayers = m_edtNumPlayers->getValue();
    cfg.player = m_cfg->player;
    cfg.tiles.clear();
    cfg.tiles.reserve(m_cfg->tiles.size());

    // Erase 'Unknown' tiles
    for (const H3_CfgTile & tile : m_cfg->tiles)
    {
        if (tile.tt() == H3_TileType::Unknown)
        {
            continue; // Skip.
        }
        cfg.tiles.emplace_back( tile );
    }
    cfg.tiles.shrink_to_fit();

    return cfg;
}

void UI_WorldCreator::onEvent(const de::Event& event)
{
    de::gui::Widget::onEvent(event);

    auto pos = getAbsoluteRect().pos();

    if (event.type == de::EventType::MOUSE_MOVE)
    {
        m_mouseX = event.mouseMoveEvent.x;
        m_mouseY = event.mouseMoveEvent.y;
        de::Recti r_board = m_rcBoard + pos;
        //DE_TRACE("MouseMove(",mx,",",my,"), board(",r_board.str(),")")

        m_relativePosToBoard.x = m_mouseX - r_board.x;
        m_relativePosToBoard.y = m_mouseY - r_board.y;

        if (r_board.contains(m_mouseX,m_mouseY))
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
        float my = event.mouseWheelEvent.y;
        if (my > 0)
        {
            m_scale *= 1.05f;
        }
        else if (my < 0)
        {
            m_scale /= 1.05f;
        }
    }

    if (event.type == de::EventType::MOUSE_RELEASE)
    {
        if (event.mouseReleaseEvent.isLeft())
        {
            DE_DEBUG("m_hoveredTileId = ",m_hoveredTileId)
            if (m_hoveredTileId > -1 && m_cfg)
            {
                m_selectedTileId = m_hoveredTileId;
                m_tileEditor->setCfg( m_cfg->tiles.at( m_selectedTileId ) );
                m_tileEditor->setVisible(true);
            }
            else
            {
                m_tileEditor->setVisible(false);
            }
        }
    }
}

de::gui::Button*
UI_WorldCreator::createButton(std::string txt)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, de::Recti());
    btn->setRadius( 8 );
    auto tex = m_game->getTex(H3_Tex::Scroll1, "UI_WorldCreator().Scroll1");
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

void UI_WorldCreator::on_presetPrev()
{
    if (m_presetIndex < 1)
    {
        DE_ERROR("Reached left end ",m_presetIndex)
        return;
    }

    m_presetIndex--;
    setCfg(m_presetIndex);
}

void UI_WorldCreator::on_presetNext()
{
    if (m_presetIndex >= int(m_presets.size())-1)
    {
        DE_ERROR("Reached right end ",m_presetIndex)
        return;
    }

    m_presetIndex++;
    setCfg(m_presetIndex);
}

void UI_WorldCreator::on_presetName()
{
    de::PromptStringParamsW params;
    params.title = dbStrW(L"H3_PresetName: ",de_wstr(m_cfg->presetName));
    params.description = L"Change the preset name.";
    params.initPrompt = de_wstr(m_cfg->presetName);
    auto presetName = dbPromptString(params);

    if (presetName.empty())
    {
        DE_DEBUG("Nothing todo")
        return;
    }

    m_cfg->presetName = de_mbstr(presetName);
    m_presetName->setText(m_cfg->presetName);
}
void UI_WorldCreator::on_presetNew()
{
    m_presets.emplace_back();

    int count = m_presets.size();

    m_presets.back().presetName = dbStr("New preset ",count);
    setCfg(count - 1);
}
void UI_WorldCreator::on_presetClone()
{
    std::string presetName = m_cfg->presetName;
    int old = m_presetIndex;

    m_presets.emplace_back(*m_cfg);

    int count = m_presets.size();

    m_presets.back().presetName = dbStr("New ",presetName," ",count);

    setCfg(count - 1);
}
void UI_WorldCreator::on_presetDelete()
{

}

void UI_WorldCreator::on_presetExport()
{
    H3_Cfg cfg = getCfg();
    std::string uri_xml = dbStr("H3_preset_",cfg.presetName,".xml");
    std::string uri_hpp = dbStr("H3_preset_",cfg.presetName,".h");

    de::SaveFileParamsW params;
    params.initFileName = de_wstr(uri_xml);
    params.initDir = de_wstr(m_game->getExeDir());
    std::wstring wuri = dbSaveFileDlg(params);

    if (wuri.empty())
    {
        DE_ERROR("No save file")
        return;
    }

    std::string fileBase = de_mbstr( dbFileBase(wuri) );
    std::string fileDir = de_mbstr( dbFileDir(wuri) );
    DE_DEBUG("fileDir: ",fileDir)
    DE_DEBUG("fileBase: ",fileBase)

    uri_xml = fileDir + "/" + fileBase + ".xml";
    uri_hpp = fileDir + "/" + fileBase + ".hpp";
    DE_DEBUG("ExportXML: ",uri_xml)
    DE_DEBUG("ExportHPP: ",uri_hpp)

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* preset = doc.NewElement( "preset" );
    preset->SetAttribute( "name", cfg.presetName.c_str() );

    auto writeXmlU32 = [&] (std::string key, u32 value)
    {
        auto node = doc.NewElement( key.c_str() );
        node->SetText(value);
        preset->InsertEndChild(node);
    };

    writeXmlU32("victoryPoints",cfg.victoryPoints);
    writeXmlU32("cardLimitNoSteal",cfg.cardLimitNoSteal);
    writeXmlU32("buildingSpacing",cfg.buildingSpacing);
    writeXmlU32("numPlayers",cfg.numPlayers);
    writeXmlU32("useCardKnight",cfg.useCardKnight);
    writeXmlU32("useCardVictoryPoint",cfg.useCardVictoryPoint);
    writeXmlU32("useCardMonopol",cfg.useCardMonopol);
    writeXmlU32("useCardInvention",cfg.useCardInvention);
    writeXmlU32("skipFirstRound",cfg.skipFirstRound);
    writeXmlU32("bonusRoadLimit",cfg.bonusRoadLimit);
    writeXmlU32("bonusArmyLimit",cfg.bonusArmyLimit);

    tinyxml2::XMLElement* player = doc.NewElement( "player" );
    player->SetAttribute("name", cfg.player.name.c_str() );
    player->SetAttribute("color", cfg.player.color );
    player->SetAttribute("icon", cfg.player.avatar );
    player->SetAttribute("type", cfg.player.typ.str().c_str() );
    player->SetAttribute("connect", cfg.player.connect.str().c_str() );
    preset->InsertEndChild( player );

    tinyxml2::XMLElement* tiles = doc.NewElement( "tiles" );
    tiles->SetAttribute("count", cfg.tiles.size() );

    for (const H3_CfgTile & t : cfg.tiles)
    {
        tinyxml2::XMLElement* tile = doc.NewElement( "tile" );
        tile->SetAttribute("i", t.i() );
        tile->SetAttribute("j", t.j() );
        tile->SetAttribute("tt", t.tt().str().c_str() );
        tile->SetAttribute("angle60", t.angle60() );
        tile->SetAttribute("dice", t.diceValue() );
        tiles->InsertEndChild( tile );
    }

    preset->InsertEndChild( tiles );

    doc.InsertEndChild( preset );
    tinyxml2::XMLError err = doc.SaveFile( uri_xml.c_str() );
    if ( err != tinyxml2::XML_SUCCESS )
    {
        DE_ERROR("Cant save xml ", uri_xml)
    }

    // Write *.hpp to ease integration into program source code

    std::ostringstream o; o <<
    "H3_Cfg cfg;\n"
    "cfg.presetName = \"" << cfg.presetName << "\";\n"
    "cfg.victoryPoints = " << cfg.victoryPoints << ";\n"
    "cfg.cardLimitNoSteal = " << cfg.cardLimitNoSteal << ";\n"
    "cfg.buildingSpacing = " << cfg.buildingSpacing << ";\n"
    "cfg.numPlayers = " << cfg.numPlayers << ";\n"
    "cfg.useCardKnight = " << cfg.useCardKnight << ";\n"
    "cfg.useCardVictoryPoint = " << cfg.useCardVictoryPoint << ";\n"
    "cfg.useCardMonopol = " << cfg.useCardMonopol << ";\n"
    "cfg.useCardInvention = " << cfg.useCardInvention << ";\n"
    "cfg.skipFirstRound = " << cfg.skipFirstRound << ";\n"
    "cfg.bonusRoadLimit = " << cfg.bonusRoadLimit << ";\n"
    "cfg.bonusArmyLimit = " << cfg.bonusArmyLimit << ";\n";

    for (const H3_CfgTile& tile : cfg.tiles)
    {
        o << "cfg.addTile("
            << tile.i() << ","
            << tile.j() << ","
            << "H3_TileType::" << tile.tt().str() << ","
            << tile.angle60() << ","
            << tile.diceValue() << ");\n";
    }

    o <<
    "cfg.player.name = \"" << cfg.player.name << "\";\n"
    "cfg.player.color = " << cfg.player.color << ";\n"
    "cfg.player.typ = H3_PlayerType::" << cfg.player.typ.str() << ";\n"
    "cfg.player.connect = H3_PlayerConnect::" << cfg.player.connect.str() << ";\n"
    "cfg.player.avatar = (H3_Tex::eID)" << cfg.player.avatar << ";\n";

    if (dbSaveText(uri_hpp,o.str()))
    {
        DE_OK("Saved hpp ",uri_hpp)
    }
    else
    {
        DE_ERROR("Cannot save hpp ",uri_hpp)
    }
}

void UI_WorldCreator::on_presetImport()
{
    de::OpenFileParamsW params;
    std::wstring wuri = dbOpenFileDlg(params);

    std::string uri = de_mbstr(wuri);
    if (!dbExistFile(uri))
    {
        DE_ERROR("No file ",uri)
        return;
    }

    auto readTextAttrib = [] (tinyxml2::XMLElement* elem, std::string key)
    {
        std::string value;

        if (!elem)
        {
            DE_ERROR("No elem for ",key)
            return value;
        }

        if ( elem->Attribute( key.c_str() ) )
        {
            value = elem->Attribute( key.c_str() );
        }
        else
        {
            DE_ERROR("No attribute ",key)
        }

        return value;
    };

    auto readU32Elem = [] (tinyxml2::XMLElement* elem, std::string key, u32 & value)
    {
        auto node = elem->FirstChildElement( key.c_str() );
        if (node && node->GetText())
        {
            value = std::atol(node->GetText());
        }
        else
        {
            DE_ERROR("No key=",key,".")
        }
    };

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile( uri.c_str() );
    if ( err != tinyxml2::XML_SUCCESS )
    {
        DE_ERROR("No xml ", uri)
        return;
    }

    DE_DEBUG("=========================================" )
    DE_DEBUG("loadXml(",uri,")" )
    DE_DEBUG("=========================================" )

    tinyxml2::XMLElement* preset = doc.FirstChildElement( "preset" );
    if ( !preset )
    {
        DE_ERROR("No preset node in xml ", uri)
        return;
    }

    H3_Cfg cfg;
    cfg.presetName = readTextAttrib(preset,"name");

    // Config
    readU32Elem(preset,"victoryPoints",cfg.victoryPoints);
    readU32Elem(preset,"cardLimitNoSteal",cfg.cardLimitNoSteal);
    readU32Elem(preset,"buildingSpacing",cfg.buildingSpacing);
    readU32Elem(preset,"numPlayers",cfg.numPlayers);
    readU32Elem(preset,"useCardKnight",cfg.useCardKnight);
    readU32Elem(preset,"useCardVictoryPoint",cfg.useCardVictoryPoint);
    readU32Elem(preset,"useCardMonopol",cfg.useCardMonopol);
    readU32Elem(preset,"useCardInvention",cfg.useCardInvention);
    readU32Elem(preset,"skipFirstRound",cfg.skipFirstRound);
    readU32Elem(preset,"bonusRoadLimit",cfg.bonusRoadLimit);
    readU32Elem(preset,"bonusArmyLimit",cfg.bonusArmyLimit);

    // Tiles:
    auto tiles = preset->FirstChildElement( "tiles" );
    if (!tiles)
    {
        DE_ERROR("No tiles")
        return;
    }

    if ( tiles->Attribute( "count" ) )
    {
        size_t nTiles = tiles->IntAttribute( "count" );
        DE_DEBUG("Reserve space for ",nTiles," tiles")
        cfg.tiles.reserve( nTiles );
    }
    else
    {
        DE_ERROR("No tiles attribute 'count'")
    }

    // Tiles:
    auto tile = tiles->FirstChildElement( "tile" );
    while (tile)
    {
        cfg.tiles.emplace_back();
        H3_CfgTile& cfgTile = cfg.tiles.back();

        int i = tile->IntAttribute("i");
        int j = tile->IntAttribute("j");
        cfgTile.setPos( i, j );

        std::string tt = readTextAttrib(tile,"tt");
        cfgTile.setTileType( H3_TileType::parse(tt) );

        int angle60 = tile->IntAttribute("angle60");
        cfgTile.setAngle60( angle60 );

        int dice = tile->IntAttribute("dice");
        cfgTile.setDiceValue( dice );

        tile = tile->NextSiblingElement( "tile" );
    }

    DE_DEBUG("Loaded ",cfg.tiles.size()," tiles from uri ", uri)

    m_cfg = nullptr;
    m_presets.emplace_back();
    setCfg(m_presets.size() - 1);
}

