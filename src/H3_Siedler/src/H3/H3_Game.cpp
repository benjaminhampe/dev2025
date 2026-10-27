#include "H3_Game.h"
#include <de/os/CommonDialogs.h>
#include <H3/details/H3_Topology.h>
#include <H3/details/H3_AI.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

#include "H3_Window_wgl.h"

// #ifndef USE_BENNI_DEBUG_RESOURCES
// #define USE_BENNI_DEBUG_RESOURCES
// #endif

// ===========================================================================
H3_Game::H3_Game()
// ===========================================================================
    : m_window(nullptr)
    , m_driver(nullptr)
    , m_state(H3_State::Idle)
    , m_round(0)
    , m_turn(0)
    , m_playerId(0)
    , m_uiMenu(nullptr)
    , m_uiGame(nullptr)
// , m_fpsTimerId(0)
// , m_capTimerId(0)
{
    //m_audioEngine.play();
    DE_OK()
}

H3_Game::~H3_Game()
{
    //m_audioEngine.stop();
}

u32 H3_Game::computeWinnerId()
{
    DE_DEBUG("")

    // TODO: Comment those out and do them only when necessary
    computeBonusRoad();
    computeBonusArmy();

    u32 winnerId = 0;
    u32 maxPoints = 0;

    for ( H3_Player & player : m_players )
    {
        player.state.updateVictoryPoints();

        if (maxPoints < player.state.victoryPoints)
        {
            maxPoints = player.state.victoryPoints;
            winnerId = player.id;
        }
    }

    if (maxPoints < m_cfg.victoryPoints)
    {
        DE_DEBUG("No winner, maxPoints(",maxPoints,"), playerId(",winnerId,")")
        return 0;
    }

    return winnerId;
}

void H3_Game::requestClose()
{
    // DE_OK()
    m_bShouldRun = false;
}

void H3_Game::destroy()
{
    DE_DEBUG()

    requestClose();

    // killTimers();

    // === Destroy geometry ===
    // m_dcChips.destroy();
    // m_dcCorners.destroy();
    // m_dcEdges.destroy();
    m_smeshSun.destroy();
    m_smeshTile.destroy();
    m_smeshChip.destroy();
    m_smeshCorner.destroy();
    m_smeshEdge.destroy();
    m_smeshRoad.destroy();
    m_smeshFarm.destroy();
    m_smeshCity.destroy();
    m_smeshThief.destroy();

    m_tmeshRoad.destroy();
    m_tmeshFarm.destroy();
    m_tmeshCity.destroy();

    m_tileRenderer.destroy();
    m_waterTileRenderer.destroy();
    //m_tileRenderer.destroy();
    m_chipRenderer.destroy();
    m_objRenderer.destroy();
    m_diceRenderer.destroy();
    //m_uiRenderer.destroy();
    m_animatedTexRenderer.destroy();
    m_animations.destroy();

    m_guienv.destroy();
    m_tex.destroy();
    m_font.destroy();
    m_img.destroy();

    if (m_driver)
    {
        m_driver->getSkyboxRenderer()->destroy();
        m_driver->close();
        delete m_driver;
        m_driver = nullptr;
        DE_INFO("Closed driver.")
    }

    if (m_window)
    {
        m_window->destroy();
        delete m_window;
        m_window = nullptr;
        DE_INFO("Closed window.")
    }
}

bool H3_Game::run()
{
    if (!m_window) return false;
    if (!m_driver) return false;

    return m_bShouldRun && m_window && m_window->run();
}

void H3_Game::resize(int w, int h)
{
    if (m_driver)
    {
        m_driver->resize(w,h);
        UI_resizeLayouts();
    }
    else
    {
        DE_ERROR("No driver")
    }
}

bool H3_Game::init( de::Window* window )
{
    DE_INFO("")
    m_isCameraMouseInputEnabled = true;
    m_window = window;
    m_driver = new de::gpu::VideoDriver();

    int w = m_window->getClientRect().w;
    int h = m_window->getClientRect().h;
    DE_INFO("Got client size(",w,",",h,").")

    if ( !m_driver->open( w, h ) )
    {
        DE_ERROR("Cant create opengl video driver, abort app!")
        delete m_window;
        delete m_driver;
        m_window = nullptr;
        m_driver = nullptr;
        return false;
    }

    // ===================
    // === Setup images ===
    // ===================
    // m_img.init();
    // m_img.save("media/H3/atlas");
    m_img.load();

    // ==================
    // === Setup fonts ===
    // ==================
    //m_font.init();
    //m_font.save("media/H3/font_atlas");
    m_font.load();

    // ====================
    // === Setup camera ===
    // ====================
    auto camera = m_driver->getCamera();
    camera->lookAt( glm::vec3(10,100,-100), glm::vec3(0,0,0) );
    camera->setMoveSpeed(1.0);
    camera->setStrafeSpeed(1.0);
    camera->setUpSpeed(1.0);

    // ====================
    // === Setup skybox ===
    // ====================
    auto nx = H3_getImg( *this, H3_Img::Sky1NegX);
    auto px = H3_getImg( *this, H3_Img::Sky1PosX);
    auto ny = H3_getImg( *this, H3_Img::Sky1NegY);
    auto py = H3_getImg( *this, H3_Img::Sky1PosY);
    auto nz = H3_getImg( *this, H3_Img::Sky1NegZ);
    auto pz = H3_getImg( *this, H3_Img::Sky1PosZ);
    m_driver->getSkyboxRenderer()->load( nx, px, ny, py, nz, pz);

    // ======================
    // === init textures ===
    // ======================
    m_tex.init( *this );

    // =============================
    // === Setup GUI Environment ===
    // =============================
    m_guienv.init( m_driver );

    // =========================
    // === init lights (sun) ===
    // =========================
    m_driver->addLight(
        glm::vec3(0,300,0),
        glm::vec3(1,1,0.9));

    de::smesh::SMeshSphere::add(
        m_smeshSun,
        glm::vec3(50,50,50),
        dbRGB(255,255,200),
        33, 33);
    m_smeshSun.name = "m_sunMesh";
    m_smeshSun.setLighting(0);
    m_smeshSun.recalculateBoundingBox();
    m_smeshSun.upload();
    DE_BENNI(m_smeshSun.str())

    // === Create geometry ===
    m_thief.triangles.clear();

    // === Create geometry ===
    m_smeshTile.clear();
    de::smesh::SMeshHexagon::add(
        m_smeshTile,
        m_tileSize.x,
        m_tileSize.z,
        0xFFFFFFFF );
    m_smeshTile.recalculateBoundingBox();

    // === Create geometry ===
    m_smeshChip.clear();
    de::smesh::SMeshCylinder::addXZ(
        m_smeshChip,
        m_chipSize.x,
        m_chipSize.y,
        m_chipSize.z,
        0xFFFFFFFF,
        33);
    m_smeshChip.recalculateBoundingBox();
    m_dcChips.m_smesh = &m_smeshChip;
    m_dcChips.initVAO();

    // === Create geometry ===
    m_smeshCorner.clear();
    de::smesh::SMeshCylinder::addXZ(
        m_smeshCorner,
        m_cornerSize.x,
        m_cornerSize.y,
        m_cornerSize.z,
        0xFFFFFFFF,
        24);
    m_smeshCorner.recalculateBoundingBox();
    m_dcCorners.m_smesh = &m_smeshCorner;
    m_dcCorners.initVAO();

    // === Create geometry ===
    m_smeshEdge.clear();
    de::smesh::SMeshCylinder::addXZ(
        m_smeshEdge,
        m_edgeSize.x,
        m_edgeSize.y,
        m_edgeSize.z,
        0xFFFFFFFF,
        21);
    m_smeshEdge.recalculateBoundingBox();
    m_dcEdges.m_smesh = &m_smeshEdge;
    m_dcEdges.initVAO();

    // === Create geometry ===
    H3_ObjMeshBuffer::initRoadGeometry( m_tmeshRoad, m_roadSize );
    m_tmeshRoad.upload();
    m_smeshRoad = H3_ObjMeshBuffer::toSMesh( m_tmeshRoad );
    m_smeshRoad.material.DiffuseMap = getTex( H3_Tex::Wood, "m_smeshRoad" );
    m_smeshRoad.upload();

    // === Create geometry ===
    H3_ObjMeshBuffer::initFarmGeometry( m_tmeshFarm, m_farmSize );
    m_tmeshFarm.upload();
    m_smeshFarm = H3_ObjMeshBuffer::toSMesh( m_tmeshFarm );
    m_smeshFarm.material.DiffuseMap = getTex( H3_Tex::Wood, "m_smeshFarm" );
    m_smeshFarm.upload();

    // === Create geometry ===
    H3_ObjMeshBuffer::initCityGeometry( m_tmeshCity, m_citySize );
    m_tmeshCity.upload();
    m_smeshCity = H3_ObjMeshBuffer::toSMesh( m_tmeshCity );
    m_smeshCity.material.DiffuseMap = getTex( H3_Tex::Wood, "m_smeshCity" );
    m_smeshCity.upload();

    // === Create geometry ===
    m_smeshThief.clear();
    de::smesh::SMeshCone::addXZ(
        m_smeshThief,
        m_thiefSize,
        0xFFFFFFFF,
        0xFFFFFFFF,
        21,
        1);
    m_smeshThief.material.Lighting = 1;
    m_smeshThief.material.DiffuseMap = getTex(H3_Tex::Thief, "m_smeshThief");
    m_smeshThief.recalculateBoundingBox();
    m_smeshThief.upload();

    m_tileRenderer.init(*this);
    m_waterTileRenderer.init( *this );
    //m_tileRenderer.init( *this );
    m_chipRenderer.init( *this );
    m_objRenderer.init( *this );
    m_diceRenderer.init( *this );
    //m_uiRenderer.init( *this );
    m_animatedTexRenderer.init(m_driver);
    m_animations.init( this );

    auto g = this;
    auto e = &m_guienv;
    auto n = nullptr;
    auto r = de::Recti(100,100,800,600 );
    auto k = 1;
    m_uiMenu = new UI_MainMenu( g, e, n, k++, r );
    m_uiMenu->setVisible( false );

    m_uiGame = new UI_Game(g, e, n, k++, r);
    m_uiGame->setVisible( false );

    // m_fpsTimerId = m_device->startTimer(33);
    m_capTimerId = m_window->startTimer(200);
    m_bPaintEvents = true;

    // m_driver->dumpTextures();

    m_uiGame->m_worldCreator->setCfg( H3_Cfg::createStandard() );

    toggleMainMenu();
    return true;
}

void H3_Game::draw()
{
    if (!m_bPaintEvents)
    {
        DE_WARN("No m_acceptPaintEvents")
        return;
    }

    //DE_OK("")
    auto driver = getDriver();
    if (!driver) { DE_ERROR("No driver") return; }

    driver->beginRender();

    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();

    if ( isGameRunning() )
    {
        drawSkybox();
        drawSun();
        drawTiles();
        drawPortBridges();
        drawChips();
        drawCorners();
        drawEdges();
        drawRoads();
        drawFarms();
        drawCitys();
        drawThief();
        //drawDice();
        m_animations.draw();
        drawHoveredTile();
        drawHoveredEdge();
        drawHoveredCorner();
    }
    else
    {
        auto wallpaper = getTex(H3_Tex::Satan, "draw().wallpaper");
        driver->getScreenRenderer()->draw2DRect(de::Recti(0,0,w,h),0xFFFFFFFF, wallpaper);
    }

    driver->beginPerf("GUIEnv");
    m_guienv.draw();
    driver->endPerf();

    if (m_uiGame->isDebug())
    {
        driver->drawPerf( de::Recti( w - 300, 600, 300, 400));
    }

    driver->endRender();

    // m_window->swapBuffers();
}


void H3_Game::exitProgram()
{
    if (isGameRunning())
    {
        bool wannaExit = H3_MessageBox("Exit Game",
                                       "Do you want to exit the running Game?");

        if (!wannaExit)
        {
            return;
        }
    }


    requestClose();
}

void H3_Game::exitGame()
{
    m_round = 0;
    m_uiMenu->setVisible(false);
    toggleMainMenu();
}


void H3_Game::toggleMainMenu()
{
    const bool bVis = m_uiMenu->isVisible();
    m_uiMenu->setVisible(!bVis);
    m_uiGame->setVisible(bVis);

    int w = 800;
    int h = 600;

    if (m_driver)
    {
        w = m_driver->getScreenWidth();
        h = m_driver->getScreenHeight();
    }

    if (m_uiMenu->isVisible())
    {
        m_uiMenu->resize(0,0,w,h);
    }

    if (m_uiGame->isVisible())
    {
        m_uiGame->resize(0,0,w,h);
    }
}

H3_GuiState H3_Game::UI_getState() const
{
    if (!m_uiGame) { DE_ERROR("No UI_Game") return {}; }
    return m_uiGame->getState();
}

void H3_Game::UI_setState(const H3_GuiState& guiState)
{
    if (m_uiGame)
    {
        m_uiGame->setState(guiState);
    }
    UI_updateActions();
}

void H3_Game::UI_resizeLayouts()
{
    int w = 800;
    int h = 600;
    if (m_driver)
    {
        w = m_driver->getScreenWidth();
        h = m_driver->getScreenHeight();
    }

    if (m_uiMenu) { m_uiMenu->resize(0,0,w,h); }
    if (m_uiGame) { m_uiGame->resize(0,0,w,h); }
}

void H3_Game::enterBanking()
{
    DE_WARN("playerId = ",m_playerId)

    m_uiGame->m_banking->startBanking(m_playerId);

    H3_GuiState guiState;
    guiState.bBanking = true;
    m_uiGame->pushState( guiState );

}

void H3_Game::enterTrading()
{
    DE_WARN("Not implemented")

    H3_MessageBox("Trade", "DoTrade");

    // H3_GuiState guiState;
    // guiState.bBanking = true;
    // m_uiGame->pushState( guiState );
}

void H3_Game::computeBonusRoad()
{
    DE_WARN("Not implemented")

    if (m_cfg.bonusRoadLimit < 1) // Disabled game feature
    {
        DE_WARN("BonusRoad:disabled")
        m_bonusRoad.owner = 0;
        m_bonusRoad.roads = 0;
        return; // Reset and abort.
    }

    // Finally enough connected roads for a new owner, and a new owner ....

    // 1. Finally delete bonus from all players ...

    for ( H3_Player & player : m_players )
    {
        player.setBonusRoad(false);
    }

}

void H3_Game::computeBonusArmy()
{
    if (m_cfg.bonusArmyLimit < 1) // Disabled game feature
    {
        DE_WARN("BonusArmy:disabled")
        m_bonusArmy.owner = 0;
        m_bonusArmy.knights = 0;
        return; // Reset and abort.
    }

    // Compute player with biggest army ...
    uint32_t ownerId = 0;
    uint32_t knights = 0;

    for ( H3_Player & player : m_players )
    {
        // Only count revealed knights...
        if (knights < player.state.knights.size())
        {
            knights = player.state.knights.size();
            ownerId = player.id;
        }
    }

    // Delete bonus from all players ...

    for ( H3_Player & player : m_players )
    {
        player.setBonusArmy(false);
    }

    // Is there a bonus to give?

    if (knights < m_cfg.bonusArmyLimit)
    {
        DE_DEBUG("No bonusArmy. ownerId(",ownerId,"), knights(",knights,"), threshold(",m_cfg.bonusArmyLimit,")")
        return; // No bonus to give.
    }

    int found = findPlayer(ownerId);
    if (found < 0)
    {
        DE_DEBUG("Invalid ownerId(",ownerId,"), knights(",knights,"), threshold(",m_cfg.bonusArmyLimit,")")
        return; // No bonus to give.
    }

    // New owner?
    H3_Player & owner = getPlayer(ownerId);

    // New owner?
    if (owner.id != m_bonusArmy.owner) // Same owner, minor update.
    {
        auto s = dbStr("BonusArmy goes to ",ownerId," with ",knights," knights");
        marqueeH3(de_wstr(s),2000,owner.color);

        DE_DEBUG("[",ownerId,"] ",owner.name," got BonusArmy with ",knights, " knights")

        // TODO: trigger some animation or so ...
    }

    // Update state
    owner.setBonusArmy(true); // Just make sure player has bonus.
    m_bonusArmy.owner = ownerId; // Update bonus owner
    m_bonusArmy.knights = knights; // Update bonus value
}

void H3_Game::enterWinnerScreen()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
    }

    DE_DEBUG("winnerId(",m_winnerId,")")

    if (m_winnerId == 0)
    {
        return;
    }

    m_state = H3_State::Winner;

    H3_GuiState guiState;
    guiState.bWinnerScreen = true;
    m_uiGame->pushState( guiState );
}

void H3_Game::restartGame()
{
    DE_DEBUG("")

    if (m_round > 0)
    {
        bool wannaNewGame = H3_MessageBox("Restart Game",
                                          "Do you want to restart the Game?\n"
                                          "The same configuration will be used.");

        if (!wannaNewGame)
        {
            return;
        }
    }

    startGame();
}

void H3_Game::newGame()
{
    DE_DEBUG("")

    if (m_round > 0)
    {
        bool wannaNewGame = H3_MessageBox("New Game",
                                          "Game is already running.\n"
                                          "Do you want to start a new Game?");

        if (!wannaNewGame)
        {
            return;
        }
    }

    // --- Hide Main Menu ---
    m_uiMenu->setVisible(false);

    // --- Show Game Menu ---
    int w = 800;
    int h = 600;
    if (m_driver)
    {
        w = m_driver->getScreenWidth();
        h = m_driver->getScreenHeight();
    }

    H3_GuiState guiState;
    guiState.bWorldCreator = true;
    m_uiGame->setState( guiState );
    m_uiGame->setVisible(true);
    m_uiGame->resize(0,0,w,h);
}


void H3_Game::startGame()
{
    m_cfg = m_uiGame->m_worldCreator->getCfg();

    m_cfg.dump();

    // === Reset Winner ===
    m_winnerId = 0;

    // === Reset Bonus ===
    m_bonusArmy.owner = 0;
    m_bonusArmy.knights = 0;

    // === Reset Bonus ===
    m_bonusRoad.owner = 0;
    m_bonusRoad.roads = 0;

    // === Players ===
    m_humans.clear();
    m_enemies.clear();
    m_players.clear();

    m_players.reserve(m_cfg.numPlayers);

    for (size_t i = 0; i < m_cfg.numPlayers; ++i)
    {
        m_players.emplace_back();
        H3_Player & player = m_players.back();
        player.id = i + 1;
        player.connect = H3_PlayerConnect::LOCAL;
        player.color = H3_CfgPlayer::getPlayerColor(i);
        player.name = H3_CfgPlayer::getPlayerName(H3_Tex::Play1+i);
        player.avatar = H3_Tex::Play1 + i;

        if (i == 0)
        {
            player.name = m_cfg.player.name;
            player.typ = H3_PlayerType::HUMAN;
        }
        else
        {
            player.typ = H3_PlayerType::CPU;
        }
        player.startGame();

    #ifdef USE_BENNI_DEBUG_RESOURCES
        player.state.bank.A = 3;
        player.state.bank.B = 4;
        player.state.bank.C = 5;
        player.state.bank.D = 6;
        player.state.bank.E = 7;
    #endif
    }
    DE_DEBUG("Created players = ", m_players.size())

    for (auto& player : m_players)
    {
        if (player.typ.isHuman())
        {
            m_humans.emplace_back(&player);
        }
        else if (player.typ.isCPU())
        {
            m_enemies.emplace_back(&player);
        }
    }
    DE_DEBUG("Stored enemies = ", m_enemies.size())
    DE_DEBUG("Stored humans = ", m_humans.size())

    const auto tileTriangles = m_smeshTile.createCollisionTriangles();
    const auto chipTriangles = m_smeshChip.createCollisionTriangles();

    // === Board (Tiles + Players) ===
    m_tiles.clear();
    m_corners.clear();
    m_edges.clear();

    // === Board (Tiles) ===
    for (const auto& cfg : m_cfg.tileList)
    {
        H3_Tile tile;
        tile.id = H3_createId();
        tile.i = cfg.i;
        tile.j = cfg.j;
        tile.tileType = cfg.tileType;
        tile.chipValue = cfg.chipValue;
        tile.angle60 = cfg.angle60;
        tile.visible = true;
        //tile.hasRobber = false;
        tile.corners.clear();
        tile.edges.clear();
        tile.next.clear();

        const auto& tx = m_tileSize.x;
        const auto& tz = m_tileSize.z;
        const auto x = de::smesh::SMeshHexagon::computeBoardPosX(tx,tz,tile.i,tile.j);
        const auto z = de::smesh::SMeshHexagon::computeBoardPosY(tx,tz,tile.i,tile.j);
        tile.pos = glm::vec3(x,0,z);

        // Collision aabbox:
        tile.collisionBoundingBox = m_smeshTile.boundingBox + tile.pos;

        // Collision triangles:
        tile.collisionTriangles.clear();
        tile.collisionTriangles.reserve( tileTriangles.size() );
        for (const auto& triangle : tileTriangles)
        {
            auto A = tile.pos + triangle.A;
            auto B = tile.pos + triangle.B;
            auto C = tile.pos + triangle.C;
            tile.collisionTriangles.emplace_back(A,B,C);
        }

        if (tile.tileType.hasChip())
        {
            tile.trsChip.pos = tile.pos + glm::vec3(0, m_chipSize.y * 0.5f,0);
            tile.trsChip.rotate = glm::vec3(0,0,0);
            tile.trsChip.scale = m_chipSize;

            switch (tile.tileType.value)
            {
            case H3_TileType::TileA: tile.trsChip.pos += glm::vec3(0,7,0); break;
            case H3_TileType::TileD: tile.trsChip.pos += glm::vec3(0,5,0); break;
            case H3_TileType::TileE: tile.trsChip.pos += glm::vec3(0,8,0); break;
            default: break;
            }

            tile.trsChip.update();
        }

        m_tiles.emplace_back( std::move(tile) );
    }

    H3_createTopology( *this );

    computeVisibleCorners( 0 );
    computeVisibleEdges( 0 );
    m_bUpdateDrawCall_Tiles = true;
    m_bUpdateDrawCall_Chips = true;
    m_bUpdateDrawCall_Corners = true;
    m_bUpdateDrawCall_Edges = true;
    m_bUpdateDrawCall_Roads = true;
    m_bUpdateDrawCall_Farms = true;
    m_bUpdateDrawCall_Citys = true;

    // Camera:
    getCamera()->lookAt( glm::vec3(-1,260,-123), glm::vec3(0,0,-55) );

    // Place thief:
    H3_Tile& desertTile = H3_findFirstDesertTile( *this );
    m_thief.occupiedTileId = desertTile.id;
    m_thief.trs.reset();
    m_thief.trs.pos = desertTile.pos + glm::vec3(10,0,-30);
    m_thief.trs.update();

    // startGame()

    m_state = H3_State::Idle;
    m_turn = 1;
    m_ranking.clear();
    m_ranking.reserve( m_players.size() );

    for ( const auto& player : m_players )
    {
        H3_RankedPlayer item;
        item.playerId = player.id;
        item.diceSum = 0;
        m_ranking.push_back( item );
    }

    H3_GuiState guiState;
    UI_setState( guiState );

    if (m_cfg.skipFirstRound)
    {
        m_round = 2;

        dbRandomize();

        for (H3_Player & player : m_players)
        {
            m_dice.a = (rand()%6) + 1;
            m_dice.b = (rand()%6) + 1;
            player.state.dice = m_dice;
        }
    }
    else
    {
        m_round = 1;
    }

    enterRound();
}

void H3_Game::computeNextPlayer()
{
    int index = 0;

    // ... by start order
    if ( m_round == 1 )
    {
        index = m_turn - 1;

        m_playerId = m_players.at( index ).id;
    }
    // ... by ranking ( normal case )
    else
    {
        // index is forward ranking ( normal case )
        index = m_turn - 1;

        // index is revered ranking ( special case )
        if ( m_round == 3 )
        {
            index = int(m_ranking.size()) - m_turn;
        }

        m_playerId = m_ranking.at( index ).playerId;

        DE_DEBUG("Round[",m_round,"].Turn[",m_turn,"] Computed playerId(",m_playerId,"), index(",index,")")
        for (size_t i = 0; i < m_ranking.size(); ++i)
        {
            const H3_RankedPlayer& r = m_ranking[i];
            const H3_Player& p = getPlayer(r.playerId);

            DE_DEBUG("Ranking[",i,"] "
                "rank(",r.rank,"), "
                "name(",p.name,"), "
                "id(",p.id,"), "
                "color(",dbHex(p.color),")")
        }
    }
}

void H3_Game::enterRound()
{
    DE_DEBUG("")

    // Prepare new round
    if ( m_turn != 1 )
    {
        DE_ERROR("Invalid turn ",m_turn)
    }

    // Check win condition
    m_winnerId = computeWinnerId();

    if (m_winnerId)
    {
        enterWinnerScreen();
        return;
    }

    // Compute ranking from dice results.
    // Must be before "player.pushState()" which resets dice values.
    if ( m_round == 2 )
    {
        computeRanking();
    }

    // Prepare new round
    for (H3_Player & player : m_players)
    {
        player.pushState();
    }

    // Compute next player ...
    computeNextPlayer();

    H3_Player& player = getPlayer(m_playerId);

    DE_DEBUG("Round[",m_round,"], "
         "player[",player.id,"](",player.name,"), "
         "dice(",player.state.dice.sum(),")" )

    if (m_round == 1)
    {
        // m_playerId = m_ranking.front().playerId;

        marqueeH1( L"Round 1 - Ranking by Dice", 2000, 0xFF00CC10 );
        marqueeH1( L"Runde 1 - Würfeln der Rangfolge", 3000, 0xFF00CC10 );

        auto s1 = dbStrW(L"New Game with (", m_players.size(), L") Players" );
        marqueeH3( s1, 4000, 0xFF00CF00 );
        auto s2 = dbStrW(L"Neues Spiel mit (", m_players.size(), L") Spielern" );
        marqueeH3( s2, 4000, 0xFF00CF00 );

        marqueeH3( L"Please dice to determine your start rank.", 4000, 0xFF00DC10 );
        marqueeH3( L"Bitte würfeln um deinen Rang zu bestimmen.", 4000, 0xFF00DC10 );

    }
    else if (m_round == 2)
    {
        // m_playerId = m_ranking.front().playerId;

        marqueeH1( L"Round 2 - First farm and road.", 3000, 0xFF00CC10 );
        marqueeH3( L"Place a farm and a road next to the farm.", 5000, dbRGB(255,128,0) );
    }
    else if (m_round == 3)
    {
        // m_playerId = m_ranking.back().playerId;

        marqueeH1( L"Round 3 - Second farm adn road.", 3000, 0xFF00CC10 );
        marqueeH3( L"Place another farm and road nearby.", 5000, dbRGB(255,128,0) );
    }
    else
    {
        // m_playerId = m_ranking.front().playerId;

        marqueeH1( dbStrW(L"Round ",m_round), 3000, 0xFF00CC10 );

        auto & player = getCurrentPlayer();
        auto s = dbStr("Turn[",m_turn,"] of player ",player.name, " with id ",player.id);
        marqueeH3( de_wstr(s), 2000, player.color );
    }

    enterTurn();
}

void H3_Game::enterTurn()
{
    DE_DEBUG("")

    m_state = H3_State::Idle;

    computeNextPlayer();

    H3_Player& player = getPlayer(m_playerId);

    DE_DEBUG("Round[",m_round,"].Turn[",m_turn,"].Player[",player.id,"]"
                " ",player.name,", dice(",player.state.dice.sum(),")" )

    if (player.typ == H3_PlayerType::HUMAN)
    {
        H3_GuiState guiState;
        guiState.bActions = true;
        UI_setState( guiState );
        enterIdle();
    }
    else
    {
        if ( m_round == 1 )
        {
            CPU_doRound1( *this );
        }
        else if ( m_round == 2 )
        {
            CPU_doRound2( *this );
        }
        else if ( m_round == 3 )
        {
            CPU_doRound3( *this );
        }
        else
        {
            CPU_doRoundN( *this );
        }
    }
}


void H3_Game::doEndTurn()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    UI_logEndTurn( *this );

    auto& player = getCurrentPlayer();

    DE_DEBUG("[EndTurn] "
             "turn(",m_turn,"), "
             "round(",m_round,"), "
             "player[",player.id,"](",player.name,"), "
             "dice(",player.state.dice.sum(),")" )

    m_turn++;

    // End of Round:
    if ( m_turn > m_players.size() )
    {
        {
            H3_Player& player = getCurrentPlayer();
            DE_DEBUG("[EndRound] "
                 "round(",m_round,"), "
                 "player[",player.id,"](",player.name,"), "
                 "dice(",player.state.dice.sum(),")" )
        }

        m_turn = 1;   // Next turn
        m_round++;    // Next round

        enterRound();
    }
    // End of Turn:
    else
    {
        enterTurn();
    }
}

void H3_Game::enterIdle()
{
    DE_DEBUG("")

    m_state = H3_State::Idle;

    H3_Player & player = getPlayer(m_playerId);

    if (player.typ == H3_PlayerType::HUMAN)
    {
        H3_GuiState guiState;
        guiState.bActions = true;
        UI_setState( guiState );
    }
}



void H3_Game::computeRanking()
{
    DE_DEBUG("")
    m_ranking.clear();
    m_ranking.reserve( m_players.size() );

    for ( auto& player : m_players )
    {
        H3_RankedPlayer item;
        item.playerId = player.id;
        item.diceSum = player.state.dice.sum();
        m_ranking.push_back( item );
    }

    std::sort( m_ranking.begin(), m_ranking.end(),
              [&] ( H3_RankedPlayer const & a, H3_RankedPlayer const & b )
              {
                  return a.diceSum > b.diceSum;
              } );

    DE_DEBUG( "DiceRanking after first round...")
    for ( size_t i = 0; i < m_ranking.size(); ++i )
    {
        auto & item = m_ranking[ i ];
        DE_DEBUG( "DiceRank[",i,"] "
                                 "playerId = ", item.playerId, ", "
                 "diceSum = ", item.diceSum )
    }
}

void H3_Game::doDice()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state, ", m_state.str())
    }

    auto& player = getCurrentPlayer();

    if (player.state.dice.sum())
    {
        DE_ERROR("player[",player.id,"] ",player.name," already threw "
            "dice(",player.state.dice.a," + ",player.state.dice.b,").")
        return;
    }

    m_state = H3_State::Dice;

    dbRandomize();
    m_dice.a = (rand()%6) + 1;
    m_dice.b = (rand()%6) + 1;

    player.state.dice = m_dice;


    DE_DEBUG("player[",player.id,"] ",player.name," "
            "dice(",player.state.dice.a," + ",player.state.dice.b,").")

    auto s1 = dbStr(
        "Player[", player.id, "] ", player.name, " has "
        "Dice Values (", int(player.state.dice.a), " + ", int(player.state.dice.b), ")"
        );


    m_animations.addAnimation( new H3_Dice2DAnimation(this) );

    marqueeH3( de_wstr(s1), 2000, player.color );

    UI_logDice( *this );

    // Beginner Round:

    if ( m_round < 4)
    {
        enterIdle();
        return;
    }

    // Normal Round:

    const int sum = m_dice.sum();

    // --> --> --> --> --> --> --> --> --> --> --> --> --> -->
    // Donate cards, Move robber, Select Player to steal card
    // --> --> --> --> --> --> --> --> --> --> --> --> --> -->
    if (sum == 7)
    {
        for (H3_Player* cpu_player : m_enemies)
        {
            AI_donateCardsToRobber(*this, cpu_player->id);
        }

        for (H3_Player* human_player : m_humans)
        {
            enterDonateCards( human_player->id );
        }
    }
    else
    {
        // --> --> --> --> --> --> --> --> --> --> --> -->
        // Collect ressources from tiles
        // --> --> --> --> --> --> --> --> --> --> --> -->
        for ( size_t i = 0; i < m_tiles.size(); ++i )
        {
            auto& tile = m_tiles[ i ];
            if ( sum == tile.chipValue )
            {
                // Skip tile with thief...
                if (tile.id == m_thief.occupiedTileId)
                {
                    DE_DEBUG("Tile[",tile.str(),"] has Thief")
                    continue;
                }

                //DE_DEBUG("Tile[",i,"] produces resource type ", PioTileType::getString( tile.tt ) )

                for ( int cornerId : tile.corners )
                {
                    auto& corner = H3_getCorner( *this, __func__, cornerId );
                    if ( corner.owner > 0 )
                    {
                        auto& player = getPlayer( corner.owner );

                        //DE_DEBUG("Player(",player.id,") ",player.name," has before bank = ", player.bank.toString() )
                        //DE_DEBUG("Tile[",i,"] has owner ", corner.owner, " and state = ", corner.state )
                        //DE_DEBUG("Player(",player.id,") ",player.name," has after bank = ", player.bank.toString() )
                        player.state.bank.addAmount(
                                tile.tileType,
                                corner.building_type );

DE_DEBUG("Player(",player.id,") ",player.name," got ",corner.building_type," ",tile.tileType.str() )

                // <Begin> Animations:

                        glm::vec3 worldPos = corner.trs.pos;
                        glm::vec2 mousePos(0,0);

                        if (player.typ == H3_PlayerType::HUMAN)
                        {
                            mousePos.x = m_driver->getScreenWidth()/2;
                            mousePos.y = m_driver->getScreenHeight()-1;
                        }
                        else
                        {
                            int e_found = -1;
                            for (int e_index = 0; e_index < m_enemies.size(); ++e_index)
                            {
                                H3_Player* enemy = m_enemies[e_index];
                                if (enemy->id == player.id)
                                {
                                    e_found = e_index;
                                    break;
                                }
                            }

                            if (e_found > -1)
                            {
                                int e_width = m_driver->getScreenWidth()/4;
                                mousePos.x = e_found * e_width + e_width / 2;
                                mousePos.y = 0;
                            }
                        }

                        auto ani = new H3_CardAnimation(this,
                            worldPos,
                            mousePos,
                            tile.tileType);

                        m_animations.addAnimation(ani);

                // <End> Animations.
                    }
                }
            }
        }

        enterIdle();
    }
}

void H3_Game::enterDonateCards( uint32_t playerId )
{
    // if (m_state != H3_State::Dice)
    // {
    //     DE_ERROR("Invalid state ", m_state.str())
    //     m_state = H3_State::Idle;
    //     return;
    // }

    H3_Player & player = getPlayer(playerId);

    if (player.typ != H3_PlayerType::HUMAN)
    {
        DE_ERROR("Player[",player.id,"] ",player.name," is not a Human.")
        enterIdle();
        return;
    }

    const uint32_t cards = player.state.bank.sum();
    if (cards <= m_cfg.cardLimitNoSteal)
    {
        DE_DEBUG("Player[",player.id,"] ",player.name," has only ",cards," cards. No donation.")
        enterThiefPlacement();
        return;
    }

    m_state = H3_State::DonateCards;

    DE_DEBUG("Player[",player.id,"] ",player.name," has ",cards," cards. "
            "Must donate ", cards/2, ". "
            "cardLimitNoSteal(",m_cfg.cardLimitNoSteal,")")

    // UI_updateActions();

    H3_GuiState guiState;
    guiState.bDonateCards = true;
    m_uiGame->pushState( guiState );

    m_uiGame->m_donateCards->startDonation( player.id );

}

void H3_Game::enterThiefPlacement()
{
    if (m_state != H3_State::Dice &&
        m_state != H3_State::DonateCards &&
        m_state != H3_State::UseKnightCard )
    {
        DE_ERROR("Invalid state ", m_state.str())
        enterIdle();
        return;
    }

    m_state = H3_State::PlaceThief;

    auto & player = getCurrentPlayer();

    DE_DEBUG(player.name," places thief to tile")

    m_animations.addAnimation( new H3_ArrowToThiefAnimation(this) );

    marqueeH1(L"Place thief to another Tile", 2000, player.color);

    if (player.typ == H3_PlayerType::CPU)
    {
        DE_WARN("AI should do something here...")
    }

    UI_updateActions();
}

void H3_Game::leaveThiefPlacement( H3_Tile& tile )
{
    if (m_state != H3_State::PlaceThief )
    {
        DE_ERROR("Invalid state ", m_state.str())
        m_state = H3_State::Idle;
        return;
    }

    if (m_thief.occupiedTileId == tile.id)
    {
        DE_WARN("Cannot place robber on same tile ", tile.id)
        return;
    }

    DE_OK("")

    // Place Thief:
    m_thief.occupiedTileId = tile.id;
    m_thief.trs.pos = tile.pos + glm::vec3(20,0,-30);
    m_thief.trs.update();

    // Get list of owners that are not the current player.
    std::vector<uint32_t> owners;
    owners.reserve(m_players.size());

    for ( uint32_t cornerId : tile.corners )
    {
        H3_Corner & corner = H3_getCorner(*this, __func__, cornerId);
        // Owner must exist
        if (corner.owner < 1) continue;
        // Owner must not be current player
        if (corner.owner == m_playerId) continue;
        // Owner must not be in list of unique owners already..., could use a set.
        auto it = std::find_if(owners.begin(), owners.end(),
            [&]( uint32_t cached ) { return cached == corner.owner; });
        if (it != owners.end()) continue;
        // Add playerId to owner-list.
        owners.push_back( corner.owner );
    }

    if (owners.empty())
    {
        DE_DEBUG("Nothing to steal")
        enterIdle();
        return;
    }
    else if (owners.size() == 1)
    {
        DE_DEBUG("Auto steal from player ", getPlayer(owners[0]).name);
        stealRandomCard( owners[0], m_playerId );
        enterIdle();
        return;
    }

    // We have atleast 2 players on the Robber tile
    // Ergo the current player needs to select one of them using the mouse and 3d collision detection
    m_state = H3_State::StealRandomCard;
    marqueeH1(L"Select Player to steal a card...", 1500, 0xFFC0C0C0);
}

void H3_Game::leaveThiefOutro( H3_Corner& corner )
{
    if (m_state != H3_State::StealRandomCard)
    {
        DE_ERROR("Invalid state ",m_state.str())
    }

    // [1.] Corner must be on Robber's tile...
    H3_Tile & tileOfRobber = H3_getTile(*this, __func__, m_thief.occupiedTileId);

    // [1.] Find corner on Robber's tile:
    auto itFound = std::find_if(
        tileOfRobber.corners.begin(),
        tileOfRobber.corners.end(),
        [&] ( uint32_t cornerOfRobberTile )
        {
            return cornerOfRobberTile == corner.id;
        });

    // [1.] Corner is NOT on Robber's tile
    if (itFound == tileOfRobber.corners.end())
    {
        DE_WARN("Not a corner of Robber's tile")
        return;
    }

    // [2.] Stealing card only possible if corner has owner
    //      different from current player.
    if (corner.owner > 0 && corner.owner != m_playerId)
    {
        stealRandomCard( corner.owner, m_playerId );
    }

    // [3.] Reset state and update
    enterIdle();
}

void H3_Game::leaveThiefOutro( H3_Edge& edge )
{
    if (m_state != H3_State::StealRandomCard)
    {
        DE_ERROR("Invalid state ",m_state.str())
    }

    // [1.] Corner must be on Robber's tile...
    H3_Tile & tileOfRobber = H3_getTile(*this, __func__, m_thief.occupiedTileId);

    // [1.] Find corner on Robber's tile:
    auto itFound = std::find_if(
        tileOfRobber.edges.begin(),
        tileOfRobber.edges.end(),
        [&] ( uint32_t edgeOfRobberTile )
        {
            return edgeOfRobberTile == edge.id;
        });

    // [1.] Corner is NOT on Robber's tile
    if (itFound == tileOfRobber.edges.end())
    {
        DE_WARN("Not an edge of Robber's tile")
        return;
    }

    // [2.] Stealing card only possible if corner has owner
    //      different from current player.
    if (edge.owner > 0 && edge.owner != m_playerId)
    {
        stealRandomCard( edge.owner, m_playerId );
    }

    // [3.] Reset state and update
    enterIdle();
}

void H3_Game::stealRandomCard( int playerFromId, int playerToId )
{
    H3_Player & playerFrom = getPlayer( playerFromId );
    H3_Player & playerTo = getPlayer( playerToId );

    if (playerFrom.state.bank.sum() < 1)
    {
        std::wostringstream o;
        o << "Too bad, player [" << playerFrom.name.c_str() << "] has no cards to steal.";
        marqueeH3(o.str(), 2000, playerTo.color);
        return;
    }

    // Get random index into bank of victim
    int cardIndex = rand() % playerFrom.state.bank.sum();

    // Get tileType at victim's bank index
    H3_TileType tt = playerFrom.state.bank.getTileTypeFromCardIndex( cardIndex );

    // Add card to destination player
    playerTo.state.bank.addAmount( tt, 1 );

    // Sub card from victim
    playerFrom.state.bank.subAmount( tt, 1 );
}

void H3_Game::enterUseKnightCard()
{
    if (m_state != H3_State::Idle )
    {
        DE_ERROR("Invalid state ", m_state.str())
        m_state = H3_State::Idle;
        return;
    }

    UI_updateActions();
}

void H3_Game::doPlaceCancel()
{
    if (m_state != H3_State::PlaceRoad &&
        m_state != H3_State::PlaceFarm &&
        m_state != H3_State::PlaceCity )
    {
        DE_ERROR("Invalid state ", m_state.str())
        return;
    }

    UI_logCancel( *this );

    m_state = H3_State::Idle;
    computeVisibleCorners( m_playerId );
    computeVisibleEdges( m_playerId );
    m_bUpdateDrawCall_Corners = true;
    m_bUpdateDrawCall_Edges = true;
    enterIdle();
}

void H3_Game::doBuyRoad()
{
    DE_DEBUG("BuyRoad")

    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    m_state = H3_State::PlaceRoad;
    UI_updateActions();
    computeVisibleEdges( m_playerId );
    m_bUpdateDrawCall_Edges = true;
}

void H3_Game::finalizeBuyRoad( H3_Edge & edge )
{
    //DE_DEBUG("EndRoad(", edge.pos, ")")

    if ( m_state != H3_State::PlaceRoad )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    if ( edge.owner > 0 )
    {
        DE_ERROR("Error, cant build on Edge owned by player ", edge.owner )
        //H3_doPlaceCancel( game );
        return;
    }

    auto& player = getCurrentPlayer();

    if ( !player.state.bank.canBuyRoad() && m_round >= 4 )
    {
        DE_ERROR("Error, player ", player.id, " has not enought resources" )
        //H3_doPlaceCancel( game );
        return;
    }

    // Create H3_Road:
    m_roads.emplace_back();
    auto& road = m_roads.back();
    road.id = int( m_roads.size() );
    road.owner = player.id;
    road.edgeId = edge.id;
    road.color = player.color;
    road.trs.pos = edge.trs.pos;
    road.trs.rotate = edge.trs.rotate;
    road.trs.update();

    // Create H3_Road::collisionTriangles:
    const auto roadTriangles = m_smeshRoad.createCollisionTriangles();
    road.collisionTriangles.clear();
    road.collisionTriangles.reserve( roadTriangles.size() );
    for (const auto& triangle : roadTriangles)
    {
        auto A = road.trs.transform( triangle.A );
        auto B = road.trs.transform( triangle.B );
        auto C = road.trs.transform( triangle.C );
        road.collisionTriangles.emplace_back(A,B,C);
    }

    player.state.roads.emplace_back( m_roads.back().id );
    // player.didBuyRoad++;

    edge.owner = player.id;
    if ( m_round > 3 )
    {
        player.state.bank.buyRoad();
    }

    UI_logBuyRoad( *this, edge.id );

    m_state = H3_State::Idle;
    computeVisibleEdges( m_playerId );
    m_bUpdateDrawCall_Edges = true;
    m_bUpdateDrawCall_Roads = true;
    enterIdle();
}

void H3_Game::doBuyFarm()
{
    DE_DEBUG("")

    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    m_state = H3_State::PlaceFarm;
    UI_updateActions();
    computeVisibleCorners( m_playerId );
    m_bUpdateDrawCall_Corners = true;
}

void H3_Game::finalizeBuyFarm( H3_Corner & corner )
{
    DE_DEBUG("")

    if ( m_state != H3_State::PlaceFarm )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    if ( corner.owner > 0 )
    {
        DE_ERROR("Corner already owned by playerId ", corner.owner )
        return;
    }

    H3_Player & player = getCurrentPlayer();

    if ( !player.state.bank.canBuyFarm() && m_round > 3 )
    {
        DE_ERROR("Player ", player.id, " cannot afford a farm." )
        return;
    }

    // Create H3_Farm:
    m_farms.emplace_back();
    auto& farm = m_farms.back();
    farm.id = int( m_farms.size() );
    farm.owner = player.id;
    farm.cornerId = corner.id;
    farm.color = player.color;
    farm.trs.pos = corner.trs.pos;
    farm.trs.rotate = corner.trs.rotate;
    farm.trs.update();

    // Create H3_Farm::collisionTriangles:
    const auto farmTriangles = m_smeshFarm.createCollisionTriangles();
    farm.collisionTriangles.clear();
    farm.collisionTriangles.reserve( farmTriangles.size() );
    for (const auto& triangle : farmTriangles)
    {
        auto A = farm.trs.transform( triangle.A );
        auto B = farm.trs.transform( triangle.B );
        auto C = farm.trs.transform( triangle.C );
        farm.collisionTriangles.emplace_back(A,B,C);
    }

    player.state.farms.emplace_back( m_farms.back().id );
    player.state.victoryPoints++;
    // player.didBuyFarm++;

    corner.building_type = 1; // Farm
    corner.owner = player.id;

    // TODO: Repair to only use 2 edges of port tile:
    for ( int tileId : corner.tiles )
    {
        if ( !tileId ) continue;
        const auto& tile = H3_getTile( *this, __func__, tileId );

        switch (tile.tileType)
        {
        case H3_TileType::Port3v1: player.state.hasPort3v1 = true; break;
        case H3_TileType::PortA: player.state.hasPortA = true; break;
        case H3_TileType::PortB: player.state.hasPortB = true; break;
        case H3_TileType::PortC: player.state.hasPortC = true; break;
        case H3_TileType::PortD: player.state.hasPortD = true; break;
        case H3_TileType::PortE: player.state.hasPortE = true; break;
        default: break;
        }
    }

    if ( m_round > 3 )
    {
        if (!player.state.bank.buyFarm())
        {
            throw std::runtime_error("Cannot buy farm");
        }
    }

    UI_logBuyFarm( *this, corner.id );
    m_state = H3_State::Idle;
    UI_updateActions();
    computeVisibleCorners( m_playerId );
    m_bUpdateDrawCall_Corners = true;
    m_bUpdateDrawCall_Farms = true;
    enterIdle();

    // Winner is computed in doEndTurn();
}

void H3_Game::doBuyCity()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state)
        return;
    }

    H3_Player & player = getPlayer(m_playerId);
    bool bCanBuy = player.state.bank.canBuyCity();

    if (m_bDebugLogic)
    {
        DE_DEBUG("Player[",player.id,"] ",player.name, " canBuyCity = ",bCanBuy)
        DE_ERROR("State ", m_state.str())
    }

    if (!bCanBuy)
    {
        DE_ERROR("Invalid state ", m_state.str())
        return;
    }

    m_state = H3_State::PlaceCity;
    UI_updateActions();
}

void H3_Game::finalizeBuyCity( H3_Farm & farm )
{
    DE_DEBUG("H3_finalizeBuyCity()")

    H3_Player & player = getCurrentPlayer();

    if ( m_state != H3_State::PlaceCity )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    if ( m_round < 4 )
    {
        DE_ERROR("Incorrect round ", m_round)
        return;
    }

    if ( farm.owner != m_playerId )
    {
        DE_ERROR("Player[", player.id,"] ",player.name," is not farm owner. Farm:", farm.id)
        return;
    }

    if ( !player.state.bank.canBuyCity() )
    {
        DE_ERROR("Player[", player.id,"] ",player.name," cannot afford a city" )
        return;
    }

    H3_Corner & corner = H3_getCorner(*this, __func__, farm.cornerId);
    if ( corner.building_type != 1 )
    {
        DE_ERROR("Player[", player.id,"] ",player.name," corner[",corner.id,"] has no farm.")
        return;
    }

    if ( !player.state.bank.buyCity() )
    {
        DE_ERROR("Player[", player.id,"] ",player.name," cannot buy a city ", player.state.bank.str() )
        return;
    }

    // [Create] H3_City:
    m_citys.emplace_back();
    auto& city = m_citys.back();
    city.id = H3_createId();
    city.owner = player.id;
    city.color = player.color;
    city.trs.pos = corner.trs.pos;
    city.trs.rotate = corner.trs.rotate;
    city.trs.update();
    //city.collisitonTriangles:

    // [Create] H3_City::collisionTriangles:
    const auto cityTriangles = m_smeshCity.createCollisionTriangles();
    city.collisionTriangles.clear();
    city.collisionTriangles.reserve( cityTriangles.size() );
    for (const auto& triangle : cityTriangles)
    {
        auto A = city.trs.transform( triangle.A );
        auto B = city.trs.transform( triangle.B );
        auto C = city.trs.transform( triangle.C );
        city.collisionTriangles.emplace_back(A,B,C);
    }

    player.state.citys.emplace_back( m_citys.back().id );
    player.state.citys.emplace_back( m_citys.back().id );
    // player.didBuyCity++;

    auto & game = *this;

    // ======================================
    // Erase 'farm.id' from 'player.farms':
    // ======================================
    auto itFarmId = std::find_if( player.state.farms.begin(), player.state.farms.end(),
        [&] ( const uint32_t cachedFarmId ) { return cachedFarmId == farm.id; } );
    if (itFarmId == player.state.farms.end())
    {
        throw std::runtime_error("Invalid itFarmId");
    }
    else
    {
        // Erase player's farmId
        player.state.farms.erase( itFarmId );
    }

    // ======================================
    // Erase 'farm' from 'board.m_farms';
    // ======================================
    auto itFarm = std::remove_if( m_farms.begin(), m_farms.end(),
        [&](const H3_Farm& cachedFarm)
    {
        return cachedFarm.id == farm.id;
    });

    if (itFarm == m_farms.end())
    {
        throw std::runtime_error("Invalid itFarm");
    }
    else
    {
        m_farms.erase(itFarm, m_farms.end());
    }

    // ======================================
    //
    player.state.victoryPoints++;
    corner.building_type = 2; // City

    //DE_DEBUG("EndCity()")
    UI_logBuyCity( *this, corner.id );
    m_state = H3_State::Idle;
    m_bUpdateDrawCall_Farms = true;
    m_bUpdateDrawCall_Citys = true;
    enterIdle();
}

void H3_Game::doBuyCard()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    auto& player = getCurrentPlayer();
    if ( !player.state.bank.canBuyCard() )
    {
        DE_ERROR("Error, player ", player.id, " has not enought resources" )
        return;
    }

    if ( !player.state.bank.buyCard() )
    {
        DE_ERROR("Error, buy failed player ", player.id )
        return;
    }

    m_cards.emplace_back();
    auto& card = m_cards.back();
    card.id = int( m_cards.size() );
    card.owner = player.id;

    if (dbTimeInMilliseconds() % 3 == 2)
    {
        card.typ = H3_Tex::EventCardPoint;
        player.state.pointCards.push_back( card.id );
    }
    else
    {
        card.typ = H3_Tex::EventCardKnight;
        player.state.knightCards.push_back( card.id );
    }

    UI_logBuyCard( *this, int(card.typ) );
    //DE_DEBUG("DoBuyCard type:", card.typ ? "KnightCard" : "PointCard" )
    UI_updateActions();
}

void H3_Game::doBank()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("No correct idle state")
        return;
    }
    DE_DEBUG("DoBank")
    UI_updateActions();
}

void H3_Game::doTrade()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("No correct idle state")
        return;
    }
    DE_DEBUG("DoTrade")
    UI_updateActions();
}

void H3_Game::revealPointCard()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state)
        return;
    }

    auto & player = getCurrentPlayer();

    DE_DEBUG("Reveal pointCard of player ",player.name)

    UI_updateActions();
}
void H3_Game::revealKnightCard()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state)
        return;
    }

    auto & player = getCurrentPlayer();

    DE_DEBUG("Reveal pointCard of player ",player.name)

    // computeBonusArmy()

    enterThiefPlacement();

    UI_updateActions();
}
void H3_Game::revealInventCard()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state)
        return;
    }

    //m_state = H3_State::PlaceCity;
    //UI_updateActions();
}
void H3_Game::revealMonopolCard()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state)
        return;
    }

    //m_state = H3_State::PlaceCity;
    //UI_updateActions();
}


void H3_Game::pick()
{
    auto driver = getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    auto camera = driver->getCamera();
    if ( !camera ) { DE_ERROR("No camera") return; }

    // PickUI - Collision-Detection 2D Game UI
    /*
    H3_UI & ui = game.m_ui;
    ui.m_hoverElement = 0;

    for ( int i = 0; i < ui.m_elements.size(); ++i )
    {
        auto & elem = ui.m_elements[ i ];
        if ( !elem.visible ) continue;
        if ( elem.typ == H3_UIElem::Button )
        {
            if ( elem.pos.contains( game.m_mouseX, game.m_mouseY ) )
            {
                //DEM_DEBUG("Hit element ", i, ", name:", dbStr(elem.text) )
                ui.m_hoverElement = elem.id;
                break;
            }
        }
    }
    */

    // Reset pick:
    m_hoverTileId = 0;
    m_hoverCornerId = 0;
    m_hoverEdgeId = 0;
    m_hoverRoadId = 0;
    m_hoverFarmId = 0;
    m_hoverCityId = 0;

    // [Intersect 2D] UI collision has precedence over 3D.
    auto hoverElement = m_guienv.pick( m_mouseX, m_mouseY );
    if ( hoverElement )
    {
        return;
    }

    // [Intersect 3D] Tiles + Edges + Corners + Farms (for placing City)
    de::Ray3f ray = camera->computeRayf( m_mouseX, m_mouseY );
    m_hitDist = std::numeric_limits< float >::max();
    m_hitPos = glm::vec3(0,0,0);

    // [Intersect 3D] Corners:
    for (const H3_Corner& corner : m_corners)
    {
        if (!corner.visible) continue;
        de::IntersectResult<float> hitResult;
        if (de::intersectRay<float>( corner.collisionTriangles, ray, hitResult ))
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverCornerId = corner.id;
            }
        }
    }

    // [Intersect 3D] Edges:
    for (const H3_Edge & edge : m_edges)
    {
        if (!edge.visible) continue;
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( edge.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverEdgeId = edge.id;
            }
        }
    }

    // [Intersect] 3D Roads:
    for (const H3_Road & road : m_roads)
    {
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( road.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverRoadId = road.id;
            }
        }
    }

    // [Intersect] 3D Farms:
    for (const H3_Farm & farm : m_farms)
    {
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( farm.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverFarmId = farm.id;
            }
        }
    }

    // [Intersect] 3D Cities:
    for (const H3_City & city : m_citys)
    {
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( city.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverCityId = city.id;
            }
        }
    }

    // [Intersect 3D] Tiles:
    for (const H3_Tile & tile : m_tiles)
    {
        if (!tile.visible) continue;
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( tile.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverTileId = tile.id;
            }
        }
    }
}

int H3_Game::findPlayer( const u32 playerId ) const
{
    auto it = std::find_if( m_players.begin(),
                            m_players.end(),
                            [=] (const H3_Player& player)
                            {
                                return player.id == playerId;
                            });

    if (it == m_players.end())
    {
        return -1;
    }

    return std::distance(m_players.begin(),it);
}

H3_Player& H3_Game::getPlayer( const u32 playerId )
{
    int found = findPlayer(playerId);
    if (found < 0)
    {
        std::ostringstream o;
        o << __func__ << "() :: Invalid playerId " << playerId;
        o << ", PlayerCount = " << m_players.size() << "\n";
        for ( size_t i = 0; i < m_players.size(); ++i )
        {
            o << "Player[" << i << "] = " << m_players[i].id << "\n";
        }
        throw std::runtime_error( o.str() );
    }

    return m_players[ found ];
}

H3_Player& H3_Game::getCurrentPlayer()
{
    return getPlayer( m_playerId );
}

void H3_Game::computeVisibleCorners( uint32_t playerId )
{
    // Reset all corners to invisible:
    for (H3_Corner& corner : m_corners)
    {
        corner.visible = false;
    }

    if (playerId < 1)
    {
        //DE_DEBUG("Invalid playerId ", playerId)
        return;
    }

    if (m_state != H3_State::PlaceFarm)
    {
        DE_DEBUG("Invalid state ", m_state.str())
        return;
    }

    // Dice ranking Round:
    if (m_round == 1)
    {
        // No corners visible.
    }
    // Initial placement rounds:
    // Only corners that are m_cfg.buildingSpacing away from
    // already owned corners:
    else if (m_round == 2 || m_round == 3)
    {
        if (m_cfg.buildingSpacing == 0)
        {
            for (H3_Corner& corner : m_corners)
            {
                // [1.] Corner not already owned.
                if (corner.owner > 0) continue;
                corner.visible = true;
            }
        }
        else
        {
            // Important is that corners are not already owned by someone
            // and that those corners have one empty corner in between.
            for (H3_Corner& corner : m_corners)
            {
                // [1.] Corner not already owned.
                if (corner.owner > 0) continue;

                // [2.] Next corner not already owned.
                bool bOwnedNextCorner = false;
                for (const uint32_t nextId : corner.next)
                {
                    const H3_Corner& next = H3_getCorner(*this, __func__, nextId);
                    if (next.owner > 0)
                    {
                        bOwnedNextCorner = true;
                        break;
                    }
                }

                if (!bOwnedNextCorner)
                {
                    corner.visible = true;
                }
            }
        }
    }
    // Normal Round:
    else
    {
        if (m_cfg.buildingSpacing == 0)
        {
            for (H3_Corner& corner : m_corners)
            {
                // [1.] Corner not already owned.
                if (corner.owner > 0) continue;

                // [2.] Atleast one egde already owned by playerId.
                bool bOwnsAnEdge = false;
                for (const uint32_t edgeId : corner.edges)
                {
                    const H3_Edge& edge = H3_getEdge(*this, __func__, edgeId);
                    if (edge.owner == playerId)
                    {
                        bOwnsAnEdge= true;
                        break;
                    }
                }

                if (bOwnsAnEdge)
                {
                    corner.visible = true;
                }
            }
        }
        else
        {
            for (H3_Corner& corner : m_corners)
            {
                // [1.] Corner not already owned.
                if (corner.owner > 0) continue;

                // [2.] Next corner not already owned.
                bool bOwnedNextCorner = false;
                for (const uint32_t nextId : corner.next)
                {
                    const H3_Corner& next = H3_getCorner(*this, __func__, nextId);
                    if (next.owner > 0)
                    {
                        bOwnedNextCorner = true;
                        break;
                    }
                }

                // [3.]
                if (!bOwnedNextCorner)
                {
                    // [3.]
                    bool bOwnsEdge = false;
                    for (const uint32_t edgeId : corner.edges)
                    {
                        const H3_Edge& edge = H3_getEdge(*this, __func__, edgeId);
                        if (edge.owner == playerId)
                        {
                            bOwnsEdge = true;
                            break;
                        }

                    }
                    // [4.] Finally we set this corner visible
                    if (bOwnsEdge)
                    {
                        corner.visible = true;
                    }
                }
            }
        }
    }
}

void H3_Game::computeVisibleEdges( uint32_t playerId )
{
    // Reset all edges to invisible:
    for (H3_Edge& edge : m_edges)
    {
        edge.visible = false;
    }

    if (playerId < 1)
    {
        DE_DEBUG("Invalid playerId ", playerId)
        return;
    }

    if (m_state != H3_State::PlaceRoad)
    {
        DE_DEBUG("Invalid state ", m_state.str())
        return;
    }

    // Dice ranking Round:
    if (m_round == 1)
    {
        // No edges visible.
    }
    // Placement + normal Rounds:
    else
    {
        for (H3_Edge& edge : m_edges)
        {
            // [1.] Edge not already owned.
            if (edge.owner > 0) continue;

            // [2.] Next corner already owned.
            bool bOwnsCorner = false;
            for (const uint32_t nextId : edge.corners)
            {
                if (playerId == H3_getCorner(*this, __func__, nextId).owner)
                {
                    bOwnsCorner = true;
                    break;
                }
            }

            if (bOwnsCorner)
            {
                edge.visible = true;
            }
            else
            {
                bool bOwnsEdge = false;
                for (const uint32_t nextId : edge.next)
                {
                    if (playerId == H3_getEdge(*this, __func__, nextId).owner)
                    {
                        bOwnsEdge = true;
                        break;
                    }
                }
                if (bOwnsEdge)
                {
                    edge.visible = true;
                }
            }
        }
    }
}

void H3_Game::toggleHelpWindow()
{
    DE_WARN("Not implemented")
}

void H3_Game::UI_updateActions()
{
    m_uiGame->updateVisibleActions();
}

void H3_Game::updateWindowTitle()
{
    if (!m_window) return;

    const int desktopW = dbDesktopWidth();
    const int desktopH = dbDesktopHeight();
    const de::Recti r1 = m_window->getWindowRect();
    const de::Recti r2 = m_window->getClientRect();

    int fps = (m_driver) ? m_driver->getFPS() : 0;

    std::ostringstream o; o <<
        "Siedler 3D (c) 2025 by BenjaminHampe@gmx.de | "
        "FPS(" << fps << "), "
        "Zoom(" << int(getZoom()*100.f) << "%), "
        "Desktop("<<desktopW<<","<<desktopH<<"), "
        "Window("<<r1.str()<<"), "
        "Client("<<r2.str()<<")";

    // DE_DEBUG(o.str())

    m_window->setWindowTitle( de_wstr(o.str()) );
}

void H3_Game::drawSkybox()
{
    auto driver = getDriver();
    driver->beginPerf("Skybox");
    driver->getSkyboxRenderer()->render();
    driver->endPerf();
}

void H3_Game::drawSun()
{
    auto driver = getDriver();
    driver->beginPerf("Sun");

    static auto tStart = driver->getTime();
    static auto tLast = 0; // = driver->getTime() - tStart;
    auto tNow = driver->getTime() - tStart;
    auto dt = tNow - tLast;
    tLast = tNow;

    // Move the sun:
    auto & sun = driver->getLights()[0];

    static float sun_angleY = 0.0f;
    static float sun_sign = 1.0f;
    const float sun_radius = 600.0f;
    const float sun_speed = 1.0f; // 3 sec

    sun_angleY += sun_speed * dt;
    sun_angleY = de::Math::wrapAngleToRange0_2pi( sun_angleY );

    // if (sun_angleY > 50.0f) { sun_sign = -sun_sign; }
    // if (sun_angleY <-50.0f) { sun_sign = -sun_sign; }

    glm::vec3 sun_pos(sun_radius * sin( de::Math::DEG2RAD * sun_angleY ),
                      1200.0f,
                      sun_radius * cos( de::Math::DEG2RAD * sun_angleY ));

    sun.pos = sun_pos;

    glm::mat4 sun_mat = glm::translate( glm::mat4(1.0f), sun_pos );
    driver->setModelMatrix(sun_mat);
    driver->getSMaterialRenderer()->draw3D( m_smeshSun );
    driver->endPerf();
}

void H3_Game::drawTiles()
{
    if (m_bUpdateDrawCall_Tiles)
    {
        m_tileRenderer.collectAndUploadInstances(*this);
        m_waterTileRenderer.collectAndUploadInstances( *this );

        m_bUpdateDrawCall_Tiles = false;
        m_bUpdateDrawCall_Chips = true;
    }

    m_driver->beginPerf("Land-Tiles");
    m_tileRenderer.render();
    m_driver->endPerf();

    m_driver->beginPerf("Water-Tiles");
    m_waterTileRenderer.render();
    m_driver->endPerf();
}

void H3_Game::drawPortBridges()
{
    auto driver = getDriver();
    driver->beginPerf("PortBridges");

    auto rend = driver->getSMaterialRenderer();

    de::smesh::SMeshBuffer bridge;
    de::smesh::SMeshBox::add( bridge, m_roadSize, {0,0,0}, 0xFFFFFFFF );
    bridge.primitiveType = de::gpu::PrimitiveType::Triangles;
    bridge.material.Lighting = 1;
    bridge.material.setTexture( 0, getTex(H3_Tex::Wood2, "H3_Game::drawPortBridges()") );
    bridge.upload();

    for (H3_Tile & tile : m_tiles)
    {
        if (!tile.tileType.isPort()) continue;

        int ang60 = tile.angle60;

        // Draw bridge A:
        auto cornerA_pos2 = de::smesh::SMeshHexagon::getCorner(ang60, m_tileSize.x, m_tileSize.z);
        auto cornerA_pos3 = glm::vec3(cornerA_pos2.x, 0, cornerA_pos2.y);
        auto cornerA_ang = atan2( cornerA_pos2.y, cornerA_pos2.x ) * de::Math::RAD2DEG;
        cornerA_ang = de::Math::wrapAngle0_360(180.0f - cornerA_ang);
        de::TRSf trs;
        trs.pos = tile.pos + cornerA_pos3 * 0.5f;
        trs.rotate = glm::vec3(0,cornerA_ang,0);
        trs.update();
        driver->setModelMatrix( trs.modelMat );
        rend->draw3D(bridge);

        // Draw bridge B:
        auto cornerB_pos2 = de::smesh::SMeshHexagon::getCorner((ang60 + 1) % 6, m_tileSize.x, m_tileSize.z);
        auto cornerB_pos3 = glm::vec3(cornerB_pos2.x, 0, cornerB_pos2.y);
        auto cornerB_ang = atan2( cornerB_pos2.y, cornerB_pos2.x ) * de::Math::RAD2DEG;
        cornerB_ang = de::Math::wrapAngle0_360(180.0f - cornerB_ang);
        trs.reset();
        trs.pos = tile.pos + cornerB_pos3 * 0.5f;
        trs.rotate = glm::vec3(0,cornerB_ang,0);
        trs.update();
        driver->setModelMatrix( trs.modelMat );
        rend->draw3D(bridge);
    }

    bridge.destroy();

    driver->endPerf();
}

void H3_Game::drawChips()
{
    auto driver = getDriver();
    driver->beginPerf("Chips");

    m_bUpdateDrawCall_Chips = true;
    if (m_bUpdateDrawCall_Chips)
    {
        float phi = computeCameraAngleXZ(driver->getCamera());
        m_dcChips.collectTiles( *this, phi );
        m_dcChips.uploadInstances();
        m_bUpdateDrawCall_Chips = false;
    }

    m_chipRenderer.render( m_dcChips );
    driver->endPerf();
}

void H3_Game::drawCorners()
{
    auto driver = getDriver();
    driver->beginPerf("Corners");

    m_bUpdateDrawCall_Corners = true;
    if (m_bUpdateDrawCall_Corners)
    {
        float phi = computeCameraAngleXZ(driver->getCamera());
        m_dcCorners.collectCorners( *this, phi );
        m_dcCorners.uploadInstances();
        m_bUpdateDrawCall_Corners = false;
    }

    m_chipRenderer.render( m_dcCorners );
    driver->endPerf();
}

void H3_Game::drawEdges()
{
    auto driver = getDriver();
    driver->beginPerf("Edges");

    m_bUpdateDrawCall_Edges = true;
    if (m_bUpdateDrawCall_Edges)
    {
        float phi = computeCameraAngleXZ(driver->getCamera());
        //DE_DEBUG("phi = ",phi * de::Math::RAD2DEG);
        m_dcEdges.collectEdges( *this, phi );
        m_dcEdges.uploadInstances();
        m_bUpdateDrawCall_Edges = false;
    }

    m_chipRenderer.render( m_dcEdges );
    driver->endPerf();
}

void H3_Game::drawRoads()
{
    auto driver = getDriver();
    driver->beginPerf("Roads");

    if (m_bUpdateDrawCall_Roads)
    {
        m_tmeshRoad.m_instanceMat.clear();
        m_tmeshRoad.m_instanceColor.clear();

        for (const auto& road : m_roads)
        {
            de::TRSf trs;
            trs.pos = road.trs.pos;
            trs.rotate = road.trs.rotate;
            trs.scale = glm::vec3(1,1,1);
            trs.update();
            m_tmeshRoad.m_instanceMat.push_back( trs.modelMat );

            u32 color = getPlayer(road.owner).color;
            m_tmeshRoad.m_instanceColor.push_back( color );
        }
        m_tmeshRoad.uploadInstances();
        m_bUpdateDrawCall_Roads = false;
    }

    m_objRenderer.render( m_tmeshRoad );
    driver->endPerf();
}

void H3_Game::drawFarms()
{
    auto driver = getDriver();
    driver->beginPerf("Farms");

    if (m_bUpdateDrawCall_Farms)
    {
        m_tmeshFarm.m_instanceMat.clear();
        m_tmeshFarm.m_instanceColor.clear();

        for (const auto& farm : m_farms)
        {
            de::TRSf trs;
            trs.pos = farm.trs.pos;
            trs.rotate = farm.trs.rotate;
            trs.scale = glm::vec3(1,1,1);
            trs.update();
            m_tmeshFarm.m_instanceMat.push_back( trs.modelMat );

            u32 color = getPlayer(farm.owner).color;
            m_tmeshFarm.m_instanceColor.push_back( color );
        }
        m_tmeshFarm.uploadInstances();
        m_bUpdateDrawCall_Farms = false;
    }

    m_objRenderer.render( m_tmeshFarm );
    driver->endPerf();
}

void H3_Game::drawCitys()
{
    auto driver = getDriver();
    driver->beginPerf("Citys");

    if (m_bUpdateDrawCall_Citys)
    {
        m_tmeshCity.m_instanceMat.clear();
        m_tmeshCity.m_instanceColor.clear();

        for (const auto& city : m_citys)
        {
            de::TRSf trs;
            trs.pos = city.trs.pos;
            trs.rotate = city.trs.rotate;
            trs.scale = glm::vec3(1,1,1);
            trs.update();
            m_tmeshCity.m_instanceMat.push_back( trs.modelMat );

            u32 color = getPlayer(city.owner).color;
            m_tmeshCity.m_instanceColor.push_back( color );
        }
        m_tmeshCity.uploadInstances();
        m_bUpdateDrawCall_Farms = false;
    }

    m_objRenderer.render( m_tmeshCity );
    driver->endPerf();
}

void H3_Game::drawThief()
{
    auto driver = getDriver();
    driver->beginPerf("Thief");
    driver->setModelMatrix( m_thief.trs.modelMat );
    driver->getSMaterialRenderer()->draw3D( m_smeshThief );
    driver->endPerf();
}

void H3_Game::drawDice()
{
    m_driver->beginPerf("Dice");

    m_diceRenderer.render();

    m_driver->endPerf();
}

void H3_Game::drawHoveredEdge()
{
    if (!m_hoverEdgeId) return;

    H3_Edge & edge = H3_getEdge( *this, __func__, m_hoverEdgeId );

    auto driver = getDriver();
    auto rend = driver->getLine3DRenderer();

    de::Box3f bbox;
    const auto a = m_edgeSize.x * 0.5f;
    const auto b = m_edgeSize.y;
    const auto c = m_edgeSize.z * 0.5f;
    bbox.m_Min = edge.trs.transform( glm::vec3(-a,0,-c) );
    bbox.m_Max = edge.trs.transform( glm::vec3( a,b, c) );

    driver->resetModelMatrix();
    rend->draw3DLineBox( bbox, 0xFF00FFFF );

    if (m_playerId)
    {
        H3_Player & player = getPlayer( m_playerId );

        if (m_state == H3_State::PlaceRoad)
        {
            de::smesh::SMeshBufferTool::colorVertices( m_smeshRoad, player.color );

            m_smeshRoad.upload( true );

            de::TRSf trs;
            trs.pos = edge.trs.pos;
            trs.rotate = edge.trs.rotate;
            trs.update();

            driver->setModelMatrix( trs.modelMat );
            driver->getSMaterialRenderer()->draw3D( m_smeshRoad );
        }
    }
}

void H3_Game::drawHoveredCorner()
{
    if (!m_hoverCornerId) return;

    H3_Corner & corner = H3_getCorner( *this, __func__, m_hoverCornerId );

    auto driver = getDriver();
    auto rend = driver->getLine3DRenderer();

    de::Box3f bbox;
    const auto a = m_cornerSize.x * 0.5f;
    const auto b = m_cornerSize.y;
    const auto c = m_cornerSize.z * 0.5f;
    bbox.m_Min = corner.trs.transform( glm::vec3(-a,0,-c) );
    bbox.m_Max = corner.trs.transform( glm::vec3( a,b, c) );
    driver->resetModelMatrix();
    rend->draw3DLineBox( bbox, 0xFF0000FF );

    if (m_playerId)
    {
        H3_Player & player = getPlayer( m_playerId );

        if (m_state == H3_State::PlaceFarm)
        {
            de::smesh::SMeshBufferTool::colorVertices( m_smeshFarm, player.color );

            m_smeshFarm.upload( true );

            de::TRSf trs;
            trs.pos = corner.trs.pos;
            trs.rotate = corner.trs.rotate;
            trs.update();

            driver->setModelMatrix( trs.modelMat );
            driver->getSMaterialRenderer()->draw3D( m_smeshFarm );
        }
        else if (m_state == H3_State::PlaceCity)
        {
            de::smesh::SMeshBufferTool::colorVertices( m_smeshCity, player.color );

            m_smeshCity.upload( true );

            de::TRSf trs;
            trs.pos = corner.trs.pos;
            trs.rotate = corner.trs.rotate;
            trs.update();

            driver->setModelMatrix( trs.modelMat );
            driver->getSMaterialRenderer()->draw3D( m_smeshCity );
        }
    }
}

void H3_Game::drawHoveredTile()
{
    if (!m_hoverTileId) return;

    H3_Tile & tile = H3_getTile( *this, __func__, m_hoverTileId );

    auto driver = getDriver();
    /*
    auto rend = driver->getLine3DRenderer();

    de::Box3f bbox;
    const auto a = m_tileSize.x * 0.5f;
    const auto b = m_tileSize.y;
    const auto c = m_tileSize.z * 0.5f;
    bbox.m_Min = glm::vec3(-a,0,-c) + tile.pos;
    bbox.m_Max = glm::vec3( a,b, c) + tile.pos;

    driver->resetModelMatrix();
    rend->draw3DLineBox( bbox, 0xFFC0C0C0 );
    */

    de::TRSf trsHull;
    trsHull.pos = tile.pos;
    trsHull.update();

    // Hull-Points:
    auto A = de::smesh::SMeshHexagon::cornerA3(m_tileSize.x,m_tileSize.z);
    auto B = de::smesh::SMeshHexagon::cornerB3(m_tileSize.x,m_tileSize.z);
    auto C = de::smesh::SMeshHexagon::cornerC3(m_tileSize.x,m_tileSize.z);
    auto D = de::smesh::SMeshHexagon::cornerD3(m_tileSize.x,m_tileSize.z);
    auto E = de::smesh::SMeshHexagon::cornerE3(m_tileSize.x,m_tileSize.z);
    auto F = de::smesh::SMeshHexagon::cornerF3(m_tileSize.x,m_tileSize.z);
    // std::vector<glm::vec3> corners{ A,B,C,D,E,F };

    float h = 32.0;

    // Hull-Wall:
    de::smesh::SMeshBuffer hull;
    hull.primitiveType = de::gpu::PrimitiveType::Triangles;
    de::smesh::SMeshPolyHull::addLeftStripXZ(   hull,
                                                { A,B,C,D,E,F },
                                                h,
                                                0xFFFFFFFF,
                                                glm::vec3(0,h * 0.5f,0)   );
    hull.upload();
    driver->setModelMatrix(trsHull.modelMat);
    m_animatedTexRenderer.draw3D(hull);
    driver->resetModelMatrix();
    hull.destroy();

    // Hexagon-Line-Box:
    de::smesh::SMeshBuffer lines;
    lines.primitiveType = de::gpu::PrimitiveType::Lines;
    lines.material.state.culling = de::gpu::Culling::disabled();
    lines.vertices.reserve(36);
    lines.vertices.emplace_back(A.x,0,A.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(B.x,0,B.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(B.x,0,B.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(C.x,0,C.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(C.x,0,C.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(D.x,0,D.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(D.x,0,D.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(E.x,0,E.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(E.x,0,E.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(F.x,0,F.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(F.x,0,F.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(A.x,0,A.z,0,0,0,0xFFFFFFFF,0,0);

    lines.vertices.emplace_back(A.x,h,A.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(B.x,h,B.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(B.x,h,B.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(C.x,h,C.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(C.x,h,C.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(D.x,h,D.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(D.x,h,D.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(E.x,h,E.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(E.x,h,E.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(F.x,h,F.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(F.x,h,F.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(A.x,h,A.z,0,0,0,0xFFFFFFFF,0,0);

    lines.vertices.emplace_back(A.x,0,A.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(A.x,h,A.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(B.x,0,B.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(B.x,h,B.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(C.x,0,C.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(C.x,h,C.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(D.x,0,D.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(D.x,h,D.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(E.x,0,E.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(E.x,h,E.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(F.x,0,F.z,0,0,0,0xFFFFFFFF,0,0);
    lines.vertices.emplace_back(F.x,h,F.z,0,0,0,0xFFFFFFFF,0,0);

    lines.upload();
    driver->setModelMatrix(trsHull.modelMat);
    driver->getSMaterialRenderer()->draw3D(lines);
    driver->resetModelMatrix();
    lines.destroy();
}

void H3_Game::clearMarquee()
{
    m_uiGame->m_marquee->clearMarquee();
}

void H3_Game::marqueeH1( std::wstring msg, int durationMs, uint32_t color )
{
    m_uiGame->m_marquee->marqueeH1(msg,durationMs,color);
}

void H3_Game::marqueeH3( std::wstring msg, int durationMs, uint32_t color )
{
    m_uiGame->m_marquee->marqueeH3(msg,durationMs,color);
}

bool H3_MessageBox(std::string title, std::string msg)
{
    return dbPromtBool(msg, title);
}

/*
void
repairSchriftrolle( de::Image & img )
{
    int w = img.w();
    int h = img.h();
    uint8_t d = 0xCE;
    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            u32 color = img.getPixel(x,y);
            uint8_t r = dbRGB_R(color);
            uint8_t g = dbRGB_G(color);
            uint8_t b = dbRGB_B(color);
            if (r>d && g>d && b>=d)
            {
                img.setPixel( x,y, 0x00000000 ); // Make transparent
            }
        }
    }
}
*/


const de::Image&
H3_Game::getImg( H3_Img::eID id ) const
{
    return *H3_getImg(*this,id);
}

de::gpu::TexRef
H3_Game::getTex( H3_Tex::eID id, const std::string & caller ) const
{
    const auto it = m_tex.m_refs.find(id);
    if (it == m_tex.m_refs.end())
    {
        std::ostringstream o;
        o << "No texRef with id " << static_cast<uint32_t>(id) << ", caller("<<caller<<")";
        throw std::runtime_error(o.str());
    }

    if (it->second.empty())
    {
        std::ostringstream o;
        o << "Got empty texRef with id " << static_cast<uint32_t>(id);
        throw std::runtime_error(o.str());
    }

    return it->second;
}

float H3_Game::getZoom() const
{
    return m_guienv.getZoom();
}

void H3_Game::setZoom( float zoom )
{
    DE_DEBUG("SetZoom(",zoom,")")
    m_guienv.setZoom( zoom );

    m_uiMenu->forceResize();
    m_uiGame->forceResize();
}
