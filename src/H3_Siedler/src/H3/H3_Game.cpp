#include "H3_Game.h"
#include <de/os/CommonDialogs.h>
#include <H3/details/H3_Topology.h>
#include <H3/details/H3_AI.h>
#include <H3/details/H3_UI.h>
#include <H3/details/UI_MainMenu.h>
#include <H3/details/UI_WorldCreator.h>
#include <H3/details/UI_Actions.h>
#include <H3/details/UI_ThiefIntro.h>
#include <H3/details/UI_WinnerScreen.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>
#include "../res/resource.h" // setWindowIcon(aaaa)

// ===========================================================================
H3_Game::H3_Game()
// ===========================================================================
    : m_device(nullptr)
    //, m_window(nullptr)
    , m_uiHelpWindow(nullptr)
    , m_uiMainmenu(nullptr)
    , m_uiActions(nullptr)
    , m_uiThiefIntroPanel(nullptr)
    , m_uiWinnerScreen(nullptr)
    , m_uiWorldCreator(nullptr)
// , m_fpsTimerId(0)
// , m_capTimerId(0)
{
    //m_audioEngine.play();
}

H3_Game::~H3_Game()
{
    //m_audioEngine.stop();
}

de::gpu::TexRef H3_Game::getTex( H3_Tex::eID id, const std::string & caller ) const
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

void H3_Game::exitProgram()
{
    if (isRunning())
    {
        bool wannaExit = H3_MessageBox("Exit Game",
                                       "Do you want to exit the running Game?");

        if (!wannaExit)
        {
            return;
        }
    }

    if (m_device)
    {
        m_device->requestClose();
    }
}

void H3_Game::exitGame()
{

}

bool H3_Game::run()
{
    return (m_device != nullptr) && m_device->run();
}

void createSMeshTile( de::gpu::SMeshBuffer & o, de::gpu::VideoDriver* driver,
    glm::vec3 tileSize, de::Image bumpMap, const de::gpu::TexRef& diffuseMap )
{
    DE_BENNI("Loaded bumpMap = ", dbStrBytes(bumpMap.computeMemoryConsumption()) )

    de::gpu::BumpMapUtil::saturateHeightmap( bumpMap );
    DE_BENNI("Saturated bumpMap = ", bumpMap.str() )

    bumpMap = de::ImageScaler::resizeBilinear( bumpMap, 118,118 );
    DE_BENNI("Resized bumpMap = ", bumpMap.str() )

    //Image filterMap = createHexagonFilter( bumpMap.w() );
    //filterGrey( bumpMap, filterMap );

    //std::string uriSave1 = FileSystem::fileBase( uriBumpMap ) + ".png";
    //dbSaveImage(bumpMap, uriSave1 );

    // =============== Mesh Creation & Clipping: =============================

    de::gpu::BumpMapUtil::Triangles in_triangles;
    de::gpu::BumpMapUtil::createTriangles( in_triangles, bumpMap, tileSize );
    de::gpu::BumpMapUtil::centerTrianglesXZ( in_triangles );

    de::gpu::BumpMapUtil::BBox bbox = de::gpu::BumpMapUtil::computeBBox( in_triangles );
    DE_BENNI("BBox Min(",bbox.minEdge,"), Max(",bbox.maxEdge,")")

    de::gpu::BumpMapUtil::Triangles out_triangles;
    de::gpu::BumpMapUtil::clipTrianglesHexagon( in_triangles, out_triangles, tileSize );

    de::gpu::BumpMapUtil::repairWinding( out_triangles );
    DE_OK("Before SMeshBuffer has ", out_triangles.size() * 3, " vertices")
    DE_OK("Before SMeshBuffer has ", out_triangles.size() * 3, " indices")

    o.setTexture( 0, diffuseMap );

    //auto tex = driver->createTexture2D("img4x4", imgDiffuseMap);
    //o.setTexture( 0, tex );
    o.name = de::FileSystem::fileBase(bumpMap.uri());
    o.material.Lighting = 0;
    o.material.FogEnable = false;
    o.material.state.blend = de::gpu::Blend::alphaBlend();
    o.material.state.culling = de::gpu::Culling::disabled();
    de::gpu::BumpMapUtil::addTriangles( o, out_triangles );

    DE_OK("After SMeshBuffer has ", o.vertices.size(), " vertices")
    DE_OK("After SMeshBuffer has ", o.indices.size(), " indices")

    de::gpu::BumpMapUtil::windowVertices( o );

    de::gpu::SMeshBufferTool::computeNormals( o );

    //o.upload();

    DE_OK( o.str() )
}

void H3_Game::init(de::IrrlichtDevice* device)
{
    DE_OK("")
    dbRandomize();

    m_device = device;
    m_device->setEventReceiver( this );
    m_device->getWindow()->setWindowIcon( aaaa );
    m_device->getWindow()->setWindowTitle( "Die Siedler von Satan 3D | <benjaminhampe@gmx.de>" );
    m_device->run();
    m_device->getWindow()->bringToFront();
    m_device->run();

    // =======================
    // === Draw LoadScreen ===
    // =======================
    //m_img.initLoadScreen();
    m_img.loadLoadScreen();
    m_tex.initLoadScreen( *this );
    m_device->run();
    auto driver = m_device->getVideoDriver();
    driver->beginRender();
    int w = driver->getScreenWidth();
    int h = driver->getScreenHeight();
    driver->getScreenRenderer()->draw2DRect( de::Recti(0,0,w,h), 0xFFFFFFFF, getTex( H3_Tex::Satan, "LoadScreen" ) );
    driver->endRender();
    m_device->getWindow()->swapBuffers();
    m_device->run();

    // ======================
    // === Load images ===
    // ======================
    //m_img.init();
    //m_img.save("media/H3/atlas");
    m_img.load();

    // ======================
    // === Load fonts ===
    // ======================
    //m_font.init();
    //m_font.save("media/H3/font_atlas");
    m_font.load();

    m_guienv.init( driver );

    // ===================
    // === init camera ===
    // ===================
    auto camera = driver->getCamera();
    camera->lookAt( glm::vec3(10,100,-100),
                   glm::vec3(0,0,0) );
    camera->setMoveSpeed(1.0);
    camera->setStrafeSpeed(1.0);
    camera->setUpSpeed(1.0);
    m_isCameraMouseInputEnabled = false;

    // ===================
    // === init skybox ===
    // ===================
    auto nx = H3_getImg( *this, H3_Img::Sky1NegX);
    auto px = H3_getImg( *this, H3_Img::Sky1PosX);
    auto ny = H3_getImg( *this, H3_Img::Sky1NegY);
    auto py = H3_getImg( *this, H3_Img::Sky1PosY);
    auto nz = H3_getImg( *this, H3_Img::Sky1NegZ);
    auto pz = H3_getImg( *this, H3_Img::Sky1PosZ);
    driver->getSkyboxRenderer()->load( nx, px, ny, py, nz, pz);

    // ======================
    // === init textures ===
    // ======================
    m_tex.init( *this );

    // =========================
    // === init lights (sun) ===
    // =========================
    driver->addLight(
        glm::vec3(0,300,0),
        glm::vec3(1,1,0.9));

    de::gpu::SMeshSphere::add(
        m_smeshSun,
        glm::vec3(50,50,50),
        dbRGBA(255,255,200),
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
    de::gpu::SMeshHexagon::add(
        m_smeshTile,
        m_tileSize.x,
        m_tileSize.z,
        0xFFFFFFFF );
    m_smeshTile.recalculateBoundingBox();

    // === Create geometry ===
    m_smeshChip.clear();
    de::gpu::SMeshCylinder::addXZ(
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
    de::gpu::SMeshCylinder::addXZ(
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
    de::gpu::SMeshCylinder::addXZ(
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
    de::gpu::SMeshCone::addXZ(
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

    // === Create hexagon tiles geometry ===
    de::gpu::SMeshBuffer m_meshTileDesert; // Nevada
    de::gpu::SMeshBuffer m_meshTileA; // Mt Wilder
    de::gpu::SMeshBuffer m_meshTileB; // ?
    de::gpu::SMeshBuffer m_meshTileC; // Thuringen
    de::gpu::SMeshBuffer m_meshTileD; // Italia
    de::gpu::SMeshBuffer m_meshTileE; // Alpen

    createSMeshTile( m_meshTileDesert, driver, m_tileSize, getImg(H3_Img::TileDesert_H), getTex(H3_Tex::TileDesert_D, "m_meshTileDesert"));
    createSMeshTile( m_meshTileA, driver, m_tileSize, getImg(H3_Img::TileA_H), getTex(H3_Tex::TileA_D, "m_meshTileA"));
    createSMeshTile( m_meshTileB, driver, m_tileSize, getImg(H3_Img::TileB_H), getTex(H3_Tex::TileB_D, "m_meshTileB"));
    createSMeshTile( m_meshTileC, driver, m_tileSize, getImg(H3_Img::TileC_H), getTex(H3_Tex::TileC_D, "m_meshTileC"));
    createSMeshTile( m_meshTileD, driver, m_tileSize, getImg(H3_Img::TileD_H), getTex(H3_Tex::TileD_D, "m_meshTileD"));
    createSMeshTile( m_meshTileE, driver, m_tileSize, getImg(H3_Img::TileE_H), getTex(H3_Tex::TileE_D, "m_meshTileE"));

    // de::gpu::BumpMapUtil::testNevada( m_meshTileDesert, m_driver );
    // de::gpu::BumpMapUtil::testMountWilder( m_meshTileA, m_driver );
    // de::gpu::BumpMapUtil::testLehm( m_meshTileB, m_driver );
    // de::gpu::BumpMapUtil::testThuringen( m_meshTileC, m_driver );
    // de::gpu::BumpMapUtil::testWeizen( m_meshTileD, m_driver );
    // de::gpu::BumpMapUtil::testAlpen( m_meshTileE, m_driver );

    m_hmeshTileDesert = de::gpu::HMesh::fromSMesh( m_meshTileDesert );
    m_hmeshTileA = de::gpu::HMesh::fromSMesh( m_meshTileA );
    m_hmeshTileB = de::gpu::HMesh::fromSMesh( m_meshTileB );
    m_hmeshTileC = de::gpu::HMesh::fromSMesh( m_meshTileC );
    m_hmeshTileD = de::gpu::HMesh::fromSMesh( m_meshTileD );
    m_hmeshTileE = de::gpu::HMesh::fromSMesh( m_meshTileE );

    m_hmeshRenderer.init( driver );
    m_waterTileRenderer.init( *this );
    //m_tileRenderer.init( *this );
    m_chipRenderer.init( *this );
    m_objRenderer.init( *this );
    m_diceRenderer.init( *this );
    //m_uiRenderer.init( *this );

    auto g = this;
    auto e = &m_guienv;
    auto n = nullptr;
    auto r = de::Recti(100,100,800,600 );
    m_uiMainmenu = new UI_MainMenu( g, e, n, 0, r );
    m_uiMainmenu->setVisible( false );

    m_uiWorldCreator = new UI_WorldCreator( g, e, n, 0, r );
    m_uiWorldCreator->setVisible( false );

    m_uiActions = new UI_Actions( g, e, n, 0, r );
    m_uiActions->setVisible( false );

    m_uiThiefIntroPanel = new UI_ThiefIntro( g, e, n, 0, r );
    m_uiThiefIntroPanel->setVisible( false );

    m_uiWinnerScreen = new UI_WinnerScreen( g, e, n, 0, r );
    m_uiWinnerScreen->setVisible(false);

    m_uiWorldCreator->setPreset( H3_Cfg::createStandard() );

    m_fpsTimerId = m_device->startTimer(33);
    m_capTimerId = m_device->startTimer(200);
    m_acceptPaintEvents = true;

    enterStartMenu();
}

void H3_Game::setPreset(const H3_Cfg& preset)
{
    m_cfg = preset;

    // === Reset keyboard ===
    for ( bool & bValue : m_keyStates ) { bValue = false; }

    // === Players ===
    m_players.clear();
    for (const auto& cfg : m_cfg.playerList)
    {
        H3_Player player;
        player.reset();
        player.id = H3_createId();
        player.typ = cfg.typ;
        player.connect = cfg.connect;
        player.color = cfg.color;
        player.name = cfg.name;
        player.avatar = H3_Tex::PlayU + m_players.size();
        // player.state.bank.A = 4;
        // player.state.bank.B = 3;
        // player.state.bank.C = 2;
        // player.state.bank.D = 1;
        // player.state.bank.E = 0;
        m_players.emplace_back( std::move( player ) );
    }
    DE_WARN("Created players = ", m_players.size())

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
        tile.corners.clear();
        tile.edges.clear();
        tile.hasRobber = false;
        tile.next.clear();

        const auto& tx = m_tileSize.x;
        const auto& tz = m_tileSize.z;
        const auto x = de::gpu::SMeshHexagon::computeBoardPosX(tx,tz,tile.i,tile.j);
        const auto z = de::gpu::SMeshHexagon::computeBoardPosY(tx,tz,tile.i,tile.j);
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

    // Camera:
    getCamera()->lookAt( glm::vec3(-1,260,-123), glm::vec3(0,0,-55) );

    // Place thief:
    H3_Tile& fdt = H3_findFirstDesertTile( *this );
    m_thief.owner = fdt.id;
    m_thief.trs.reset();
    m_thief.trs.pos = fdt.pos + glm::vec3(10,0,-30);
    m_thief.trs.update();

    GL_VALIDATE;
}

void H3_Game::enterStartMenu()
{
    DE_WARN("")
    m_state = H3_State::StartMenu;
    m_round = 0;

    if (m_uiMainmenu)
        m_uiMainmenu->setVisible( true );
    if (m_uiHelpWindow)
        m_uiHelpWindow->setVisible( false );
    if (m_uiWorldCreator)
        m_uiWorldCreator->setVisible( false );
    if (m_uiActions)
        m_uiActions->setVisible( false );
    if (m_uiThiefIntroPanel)
        m_uiThiefIntroPanel->setVisible( false );
    if (m_uiWinnerScreen)
        m_uiWinnerScreen->setVisible( false );
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

    if (m_uiMainmenu)
        m_uiMainmenu->setVisible( false );

    m_round = 1;
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

    if (m_uiWorldCreator) {
        m_uiWorldCreator->setVisible(true);
        m_uiWorldCreator->resizeLayout();
    }
    if (m_uiMainmenu) {
        m_uiMainmenu->setVisible(false);
    }
    if (m_uiThiefIntroPanel) {
        m_uiThiefIntroPanel->setVisible(false);
    }    
}

void H3_Game::enterWinnerScreen()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
    }

    m_state = H3_State::Winner;

    if (m_uiWinnerScreen) {
        m_uiWinnerScreen->setVisible( true );
        m_uiWinnerScreen->resizeLayout();
    }
}

void H3_Game::enterRound1()
{
    DE_DEBUG("")

    if (m_uiMainmenu) {
        m_uiMainmenu->setVisible(false);
    }
    if (m_uiWorldCreator) {
        m_uiWorldCreator->setVisible(false);
        setPreset( m_uiWorldCreator->getPreset() );
    }
    else
    {
        setPreset( H3_Cfg::createStandard() );
    }
    if (m_uiActions) {
        m_uiActions->setVisible(true);
    }
    if (m_uiThiefIntroPanel) {
        m_uiThiefIntroPanel->setVisible(false);
    }
    if (m_uiWinnerScreen) {
        m_uiWinnerScreen->setVisible(false);
    }

    // if (m_round != 0)
    // {
    //     DE_ERROR("Invalid round ", m_round)
    //     return;
    // }

    m_state = H3_State::Idle;
    m_round = 1;
    m_turn = 1;

    //startRanking();
    m_ranking.clear();
    m_ranking.reserve( m_players.size() );

    for ( const auto& player : m_players )
    {
        H3_RankedPlayer item;
        item.playerId = player.id;
        item.diceSum = 0;
        m_ranking.push_back( item );
    }

    // m_player = m_players.back().id;
    // m_uiThiefIntroPanel->setPlayer( m_player );

    m_player = m_ranking.front().playerId;

    computeVisibleCorners( 0 );
    computeVisibleEdges( 0 );
    m_bUpdateDrawCall_Tiles = true;
    m_bUpdateDrawCall_Chips = true;
    m_bUpdateDrawCall_Corners = true;
    m_bUpdateDrawCall_Edges = true;
    m_bUpdateDrawCall_Roads = true;
    m_bUpdateDrawCall_Farms = true;
    m_bUpdateDrawCall_Citys = true;

    marqueeH1( L"Round 1 - Ranking by Dice", 2000, 0xFF00CC10 );
    marqueeH1( L"Runde 1 - Rang über Würfeln", 3000, 0xFF00CC10 );

    auto s1 = dbStrW(L"New Game with (", m_players.size(), L") Players" );
    marqueeH3( s1, 4000, 0xFF00CF00 );
    auto s2 = dbStrW(L"Neues Spiel mit (", m_players.size(), L") Spielern" );
    marqueeH3( s2, 4000, 0xFF00CF00 );

    marqueeH3( L"Please dice to determine your start rank.", 4000, 0xFF00DC10 );
    marqueeH3( L"Bitte würfeln um deinen Rang zu bestimmen.", 4000, 0xFF00DC10 );

    UI_updateActions();
}

void H3_Game::enterRound2()
{
    DE_DEBUG("")

    if (m_round != 1)
    {
        DE_ERROR("Invalid round ", m_round)
        return;
    }

    m_state = H3_State::Idle;
    m_round = 2;
    m_turn = 1;
    m_player = m_ranking.front().playerId;

    marqueeH1( L"Round 2", 2000, 0xFF00CC10 );
    marqueeH1( L"Placing first Road and Farm", 3000, 0xFF00CC10 );

    marqueeH3( L"Each player has to place one farm\n"
              L"and one road next to the farm.", 6000, dbRGBA(255,128,0) );

    UI_updateActions();
}

void H3_Game::enterRound3()
{
    DE_DEBUG("")

    if (m_round != 2)
    {
        DE_ERROR("Invalid round ", m_round)
        return;
    }

    m_state = H3_State::Idle;
    m_round = 3;
    m_turn = 1;
    m_player = m_ranking.back().playerId;

    marqueeH1( L"Round 3", 2000, 0xFF00CC10 );
    marqueeH1( L"Placing second Road and Farm", 3000, 0xFF00CC10 );

    marqueeH3( L"Each player has to place one farm\n"
              L"and one road next to the farm.", 6000, dbRGBA(255,128,0) );

    UI_updateActions();
}

void H3_Game::enterRoundN()
{
    DE_DEBUG("")

    m_state = H3_State::Idle;
    m_round++;
    m_turn = 1;
    m_player = m_ranking.front().playerId;

    marqueeH1( dbStrW(L"Round ",m_round), 3000, 0xFF00CC10 );
    marqueeH1( L"Normal round and Farm", 2000, 0xFF00CC10 );

    marqueeH3( L"Each player has to place one farm\n"
              L"and one road next to the farm.", 6000, dbRGBA(255,128,0) );

    UI_updateActions();
}


void H3_Game::doEndTurn()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        return;
    }

    DE_DEBUG("")

    // Check for winner
    if (getCurrentPlayer().state.victoryPoints >= m_cfg.victoryPoints)
    {
        enterWinnerScreen();
    }

    // Reset Turn variables of all players:
    for ( H3_Player & player : m_players )
    {
        player.endTurn();
    }

    auto& player = getCurrentPlayer();

    UI_logEndTurn( *this );
    // DE_DEBUG("EndTurn[",m_turn,"].Round[",m_round,"] playerId = ", player.id )

    m_turn++;

    // End of Round:
    if ( m_turn > m_players.size() )
    {
        DE_DEBUG("[EndRound] playerId(",player.id,"), diceSum(",player.state.dice.sum(),")" )

        //DE_DEBUG("EndRound[",m_round,"], EndTurn[",m_turn,"] playerId = ", m_playerId )
        if ( m_round == 1 )
        {
            computeRanking();
        }

        // if ( m_round == 3 )
        // {
        //     m_player = m_ranking.back().playerId; // Next player reversed
        // }
        // else
        // {
        //     m_player = m_ranking.front().playerId; // Next player normal
        // }

        //m_turn = 1;   // Next turn
        //m_round++;    // Next round

        if (m_round == 1)
        {
            enterRound2();
        }
        else if (m_round == 2)
        {
            enterRound3();
        }
        else
        {
            enterRoundN();
        }
    }

    // End of Turn:
    DE_DEBUG("[EndTurn] playerId(",player.id,"), diceSum(",player.state.dice.sum(),")" )
    //DE_DEBUG("EndTurn[",m_turn,"].Round[",m_round,"] playerId = ", m_playerId )

    // Compute next player by id ( start case )
    if ( m_round == 1 )
    {
        m_player = m_players[ m_turn - 1 ].id;
        //DE_DEBUG( "Next player 1st round is ", m_playerId )
    }
    // Compute next player by ranking ( normal case )
    else
    {
        // index is forward ranking ( normal case )
        int index = m_turn - 1;

        // index is revered ranking ( special case )
        if ( m_round == 3 )
        {
            index = int(m_ranking.size()) - m_turn;
        }

        if ( index < 0 || index >= m_ranking.size() )
        {
            DE_ERROR("Invalid ranking index ", index, ", ranks = ", m_ranking.size() )
        }
        else
        {
            m_player = m_ranking[ index ].playerId;
        }

        //DE_DEBUG( "Next player Round[", m_round, "].Turn[",m_turn,"] is ", m_playerId )
    }

    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state.str())
        m_state = H3_State::Idle;
    }

    UI_updateActions();
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

        // if ( player.diceHistory.empty() )
        // {
        //     DE_RUNTIME_ERROR("player.diceHistory.empty()", player.name)
        // }

        //item.diceSum = player.diceHistory.back().sum();
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
        return;
    }

    m_state = H3_State::Dice;

    m_dice.dice();
    auto& player = getCurrentPlayer();
    player.state.dice = m_dice;
    player.didDice = true;

    auto s1 = dbStr(
        "Player[", player.id, "] ", player.name, " has "
        "Dice Values (", int(player.state.dice.a), " + ", int(player.state.dice.b), ")"
        );
    marqueeH3( de::StringUtil::to_wstr(s1), 2000, player.color );

    UI_logDice( *this );

    // Beginner Round:

    if ( m_round < 4)
    {
        m_state = H3_State::Idle;
        UI_updateActions();
        return;
    }



    // Normal Round:

    const int sum = m_dice.sum();

    // --> --> --> --> --> --> --> --> --> --> --> --> --> -->
    // Donate cards, Move robber, Select Player to steal card
    // --> --> --> --> --> --> --> --> --> --> --> --> --> -->
    if (sum == 7)
    {
        for (auto & cpu_player : m_players)
        {
            if (cpu_player.typ == H3_PlayerType::CPU)
            {
                AI_donateCardsToRobber(*this, cpu_player.id);
            }
        }
        enterThiefIntro();
        return;
    }

    // --> --> --> --> --> --> --> --> --> --> --> -->
    // Collect ressources from tiles
    // --> --> --> --> --> --> --> --> --> --> --> -->
    for ( size_t i = 0; i < m_tiles.size(); ++i )
    {
        auto& tile = m_tiles[ i ];
        if ( sum == tile.chipValue )
        {
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
                    player.state.bank.addAmount( tile.tileType, corner.building_type );
                }
            }
        }
    }

    m_state = H3_State::Idle;
    UI_updateActions();
}

void H3_Game::enterThiefIntro()
{
    if (m_state != H3_State::Dice)
    {
        DE_ERROR("Invalid state ", m_state.str())
        m_state = H3_State::Idle;
        return;
    }

    m_state = H3_State::DonateCards;
    UI_updateActions();
    if (m_uiThiefIntroPanel) {
        m_uiThiefIntroPanel->setPlayer( m_player );
        m_uiThiefIntroPanel->setVisible(true);
    }
}
void H3_Game::enterThiefPlacement()
{
    if (m_state != H3_State::DonateCards &&
        m_state != H3_State::UseKnightCard )
    {
        DE_ERROR("Invalid state ", m_state.str())
        m_state = H3_State::Idle;
        return;
    }

    DE_OK("")
    m_state = H3_State::PlaceThief;

    if (m_uiThiefIntroPanel) {
        m_uiThiefIntroPanel->setVisible(false);
    }

    marqueeH1(L"Move Robber to new Tile", 1500, 0xFFC0C0C0);
    // enterThiefOutro();
}

void H3_Game::leaveThiefPlacement( H3_Tile& tile )
{
    if (m_state != H3_State::PlaceThief )
    {
        DE_ERROR("Invalid state ", m_state.str())
        m_state = H3_State::Idle;
        return;
    }

    if (m_thief.owner == tile.id)
    {
        DE_WARN("Cannot place robber on same tile ", tile.id)
        return;
    }

    DE_OK("")

    // Place Thief:
    H3_Tile& old_tile = H3_getTile( *this, __func__, m_thief.owner);
    old_tile.hasRobber = false;
    tile.hasRobber = true;
    m_thief.owner = tile.id;
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
        if (corner.owner == m_player) continue;
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
        m_state = H3_State::Idle;
        UI_updateActions();
        return;
    }
    else if (owners.size() == 1)
    {
        DE_DEBUG("Auto steal from player ", getPlayer(owners[0]).name);
        stealRandomCard( owners[0], m_player );
        m_state = H3_State::Idle;
        UI_updateActions();
        return;
    }

    // We have atleast 2 players on the Robber tile
    // Ergo the current player needs to select one of them using the mouse and 3d collision detection
    m_state = H3_State::StealRandomCard;
    marqueeH1(L"Select Player to steal a card...", 1500, 0xFFC0C0C0);
}

void H3_Game::leaveThiefOutro( H3_Corner& corner )
{
    // [1.] Corner must be on Robber's tile...
    H3_Tile & tileOfRobber = H3_getTile(*this, __func__, m_thief.owner);

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
    if (corner.owner > 0 && corner.owner != m_player)
    {
        stealRandomCard( corner.owner, m_player );
    }

    // [3.] Reset state and update
    m_state = H3_State::Idle;
    UI_updateActions();

}

void H3_Game::leaveThiefOutro( H3_Edge& edge )
{
    // [1.] Corner must be on Robber's tile...
    H3_Tile & tileOfRobber = H3_getTile(*this, __func__, m_thief.owner);

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
    if (edge.owner > 0 && edge.owner != m_player)
    {
        stealRandomCard( edge.owner, m_player );
    }

    // [3.] Reset state and update
    m_state = H3_State::Idle;
    UI_updateActions();
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
    UI_updateActions();
    computeVisibleCorners( m_player );
    computeVisibleEdges( m_player );
    m_bUpdateDrawCall_Corners = true;
    m_bUpdateDrawCall_Edges = true;    
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
    computeVisibleEdges( m_player );
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
    player.didBuyRoad++;

    edge.owner = player.id;
    if ( m_round > 3 )
    {
        player.state.bank.buyRoad();
    }

    UI_logBuyRoad( *this, edge.id );

    m_state = H3_State::Idle;
    UI_updateActions();
    computeVisibleEdges( m_player );
    m_bUpdateDrawCall_Edges = true;
    m_bUpdateDrawCall_Roads = true;
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
    computeVisibleCorners( m_player );
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
    player.didBuyFarm++;

    corner.building_type = 1; // Farm
    corner.owner = player.id;

    // TODO: Repair to only use 2 edges of port tile:
    for ( int tileId : corner.tiles )
    {
        if ( !tileId ) continue;
        const auto& tile = H3_getTile( *this, __func__, tileId );

        switch (tile.tileType)
        {
        case H3_TileType::Port3v1:
            player.state.flags |= H3_Player::eFlagPort3v1;
            break;
        case H3_TileType::PortA:
            player.state.flags |= H3_Player::eFlagPortA;
            break;
        case H3_TileType::PortB:
            player.state.flags |= H3_Player::eFlagPortB;
            break;
        case H3_TileType::PortC:
            player.state.flags |= H3_Player::eFlagPortC;
            break;
        case H3_TileType::PortD:
            player.state.flags |= H3_Player::eFlagPortD;
            break;
        case H3_TileType::PortE:
            player.state.flags |= H3_Player::eFlagPortE;
            break;
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
    computeVisibleCorners( m_player );
    m_bUpdateDrawCall_Corners = true;
    m_bUpdateDrawCall_Farms = true;
}

void H3_Game::doBuyCity()
{
    if ( m_state != H3_State::Idle )
    {
        DE_ERROR("Incorrect state ", m_state)
        return;
    }

    m_state = H3_State::PlaceCity;
    UI_updateActions();
}

void H3_Game::finalizeBuyCity( H3_Farm & farm )
{    
    DE_DEBUG("H3_finalizeBuyCity()")

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

    if ( farm.owner != m_player )
    {
        DE_ERROR("Farm owner ",farm.owner," is not player ", m_player )
        return;
    }

    auto& player = getCurrentPlayer();
    if ( !player.state.bank.canBuyCity() )
    {
        DE_ERROR("Player ", player.id, " cannot afford a city" )
        return;
    }

    H3_Corner & corner = H3_getCorner(*this, __func__, farm.cornerId);
    if ( corner.building_type != 1 )
    {
        DE_ERROR("Corner not built with farm, corner.id = ", corner.id )
        return;
    }

    if ( !player.state.bank.canBuyCity() )
    {
        DE_ERROR("Player ", player.id, " cannot afford a city" )
        return;
    }

    if ( !player.state.bank.buyCity() )
    {
        DE_ERROR("Player ", player.id, " cannot buy a city" )
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
    player.didBuyCity++;

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
    UI_updateActions();
    m_bUpdateDrawCall_Farms = true;
    m_bUpdateDrawCall_Citys = true;
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
        player.state.oldPointCards.push_back( card.id );
        player.state.victoryPoints++;
    }
    else
    {
        card.typ = H3_Tex::EventCardKnight;
        player.state.newKnightCards.push_back( card.id );
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

H3_Player& H3_Game::getPlayer( const u32 playerId )
{
    auto it = std::find_if( m_players.begin(), m_players.end(),
                           [=] (const H3_Player& player) { return player.id == playerId; });

    if (it == m_players.end())
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

    return m_players[ std::distance(m_players.begin(),it) ];
}

H3_Player& H3_Game::getCurrentPlayer()
{
    return getPlayer( m_player );
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
    if (m_uiHelpWindow)
    {
        //m_helpWindow->setVisible( !m_helpWindow->isVisible() );
    }
}

void H3_Game::UI_resizeLayouts()
{
    if (m_uiMainmenu)
        m_uiMainmenu->resizeLayout();

    if (m_uiActions)
        m_uiActions->resizeLayout();

    if (m_uiThiefIntroPanel)
        m_uiThiefIntroPanel->resizeLayout();

    if (m_uiWorldCreator)
        m_uiWorldCreator->resizeLayout();

    if (m_uiWinnerScreen)
        m_uiWinnerScreen->resizeLayout();
}

void H3_Game::UI_updateActions()
{
    if (m_uiActions) m_uiActions->updateVisibility();
    UI_Stats_update( *this );
    UIBanking_update( *this );
}

void H3_Game::updateWindowTitle()
{
    if (!m_device) return;

    const int desktopW = dbDesktopWidth();
    const int desktopH = dbDesktopHeight();
    const de::Recti r1 = m_device->getWindow()->getWindowRect();
    const de::Recti r2 = m_device->getWindow()->getClientRect();

    int zoom = m_guienv.getScalePc();

    int fps = 0;
    auto driver = getDriver();
    if (driver)
    {
        fps = driver->getFPS();
    }

    std::ostringstream o; o <<
        "Siedler 3D (c) 2025 by BenjaminHampe@gmx.de | "
        "FPS(" << fps << "), "
        "Zoom(" << zoom << "%), "
        "Desktop("<<desktopW<<","<<desktopH<<"), "
        "Window("<<r1.str()<<"), "
        "Client("<<r2.str()<<")";

    m_device->getWindow()->setWindowTitle( o.str().c_str() );
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

    static float sun_angleZ = 0.0f;
    static float sun_sign = 1.0f;
    const float sun_radius = 600.0f;
    const float sun_speed = 1.0f; // 3 sec

    sun_angleZ += sun_sign * sun_speed * dt; // phi = omega * t
    if (sun_angleZ > 50.0f)
    {
        sun_sign = -sun_sign;
    }
    if (sun_angleZ < -50.0f)
    {
        sun_sign = -sun_sign;
    }

    const float D2R = 3.141f / 180.0f;

    glm::vec3 sun_pos( sun_radius * sin( D2R * sun_angleZ ),
                      sun_radius * cos( D2R * sun_angleZ ),
                      200.0f - sun_radius * cos( D2R * sun_angleZ ) );

    sun.pos = sun_pos;

    glm::mat4 sun_mat = glm::translate( glm::mat4(1.0f), sun_pos );
    driver->setModelMatrix(sun_mat);
    driver->getSMaterialRenderer()->draw3D( m_smeshSun );
    driver->endPerf();
}


void H3_Game::drawTiles()
{
    auto driver = getDriver();
    driver->beginPerf("Tiles");

    if (m_bUpdateDrawCall_Tiles)
    {
        // ========================
        // === Create DrawCalls ===
        // ========================
        // HMesh uses f16 half floats, aka 16-bit floats
        // to render terrain (from heightmap) like hexagonal tiles.
        // The heightmap is converted to vector of 3D Triangles,
        // clipped to hexagon using math, then as cleanup vector is
        // rebuild to use only unique vertices (reduce vertices)
        // and then finally converted to HMesh for fast rendering.
        // Uses f16 vertices to increase render speed. (reduce GPU memory)
        // Indices are still uint32_t.
        auto mkHMeshDrawCall = [&]( de::gpu::HMesh & hmesh, H3_TileType tt )
        {
            hmesh.m_instanceMat.clear();
            //hmesh.m_instanceAngle60.clear();
            for (const auto& t : m_tiles)
            {
                if (t.tileType.value != tt) { continue; }

                de::TRSf trs;
                trs.pos = t.pos;
                trs.rotate = glm::vec3(0,0,0);
                float sx = m_tileSize.x * 0.5f;
                float sy = m_tileSize.y;
                float sz = m_tileSize.z * 0.5f;
                trs.scale = glm::vec3(sx,sy,sz);
                trs.update();

                hmesh.m_instanceMat.push_back( trs.modelMat );
                //m_instanceAngle60.emplace_back( tile.angle60 );
            }

            //DE_OK(tt.toString()," : instance = ", hmesh.m_instanceMat.size())
            hmesh.upload( true );
        };

        mkHMeshDrawCall( m_hmeshTileA, H3_TileType::TileA );
        mkHMeshDrawCall( m_hmeshTileB, H3_TileType::TileB );
        mkHMeshDrawCall( m_hmeshTileC, H3_TileType::TileC );
        mkHMeshDrawCall( m_hmeshTileD, H3_TileType::TileD );
        mkHMeshDrawCall( m_hmeshTileE, H3_TileType::TileE );
        mkHMeshDrawCall( m_hmeshTileDesert, H3_TileType::Desert );

        m_waterTileRenderer.collectAndUploadInstances( *this );

        m_bUpdateDrawCall_Tiles = false;
        m_bUpdateDrawCall_Chips = true;
    }

    //m_driver->beginPerf("TileDesert");
    m_hmeshRenderer.draw( m_hmeshTileDesert );
    //m_driver->endPerf();

    //m_driver->beginPerf("TileA");
    m_hmeshRenderer.draw( m_hmeshTileA );
    //m_driver->endPerf();

    //m_driver->beginPerf("TileB");
    m_hmeshRenderer.draw( m_hmeshTileB );
    //m_driver->endPerf();

    //m_driver->beginPerf("TileC");
    m_hmeshRenderer.draw( m_hmeshTileC );
    //m_driver->endPerf();

    //m_driver->beginPerf("TileD");
    m_hmeshRenderer.draw( m_hmeshTileD );
    //m_driver->endPerf();

    //m_driver->beginPerf("TileE");
    m_hmeshRenderer.draw( m_hmeshTileE );
    //m_driver->endPerf();

    //m_driver->beginPerf("TileWater");
    m_waterTileRenderer.render();
    //m_driver->endPerf();

    /*
    // <Desert>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(-150,0,0));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileDesert );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileDesert.boundingBox );
    }
    // <A>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileA );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileA.boundingBox );
    }
    // <B>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(150,0,0));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileB );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileB.boundingBox );
    }

    // <C>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(-150,0,150));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileC );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileC.boundingBox );
    }
    // <D>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,150));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileD );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileD.boundingBox );
    }
    // <E>
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(150,0,150));
        m_driver->setModelMatrix( m );
        m_driver->getSMaterialRenderer()->draw3D( m_meshTileE );
        m_driver->getLine3DRenderer()->draw3DLineBox( m_meshTileE.boundingBox );
    }
    */

    driver->endPerf();
}

void H3_Game::drawPortBridges()
{
    auto driver = getDriver();
    driver->beginPerf("PortBridges");

    auto rend = driver->getSMaterialRenderer();

    de::gpu::SMeshBuffer bridge;
    de::gpu::SMeshBox::add( bridge, m_roadSize, {0,0,0}, 0xFFFFFFFF );
    bridge.primitiveType = de::gpu::PrimitiveType::Triangles;
    bridge.material.Lighting = 1;
    bridge.material.setTexture( 0, getTex(H3_Tex::Wood2, "H3_Game::drawPortBridges()") );
    bridge.upload();



    for (H3_Tile & tile : m_tiles)
    {
        if (!tile.tileType.isPort()) continue;

        int ang60 = tile.angle60;

        // Draw bridge A:
        auto cornerA_pos2 = de::gpu::SMeshHexagon::getCorner(ang60, m_tileSize.x, m_tileSize.z);
        auto cornerA_pos3 = glm::vec3(cornerA_pos2.x, 0, cornerA_pos2.y);
        auto cornerA_ang = atan2( cornerA_pos2.y, cornerA_pos2.x ) * de::Math::RAD2DEG;
        cornerA_ang = de::Math::wrapAngle(180.0f - cornerA_ang);
        de::TRSf trs;
        trs.pos = tile.pos + cornerA_pos3 * 0.5f;
        trs.rotate = glm::vec3(0,cornerA_ang,0);
        trs.update();
        driver->setModelMatrix( trs.modelMat );
        rend->draw3D(bridge);

        // Draw bridge B:
        auto cornerB_pos2 = de::gpu::SMeshHexagon::getCorner((ang60 + 1) % 6, m_tileSize.x, m_tileSize.z);
        auto cornerB_pos3 = glm::vec3(cornerB_pos2.x, 0, cornerB_pos2.y);
        auto cornerB_ang = atan2( cornerB_pos2.y, cornerB_pos2.x ) * de::Math::RAD2DEG;
        cornerB_ang = de::Math::wrapAngle(180.0f - cornerB_ang);
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

    if (m_bUpdateDrawCall_Chips)
    {
        m_dcChips.collectTiles( *this );
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

    if (m_bUpdateDrawCall_Corners)
    {
        m_dcCorners.collectCorners( *this );
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

    if (m_bUpdateDrawCall_Edges)
    {
        m_dcEdges.collectEdges( *this );
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

            m_tmeshRoad.m_instanceColor.push_back( getPlayerColor( road.owner ) );
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

            m_tmeshFarm.m_instanceColor.push_back( getPlayerColor( farm.owner ) );
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
            m_tmeshCity.m_instanceColor.push_back( getPlayerColor( city.owner ) );
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
    auto driver = getDriver();
    driver->beginPerf("Dice");
    m_diceRenderer.render();
    driver->endPerf();
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

    if (m_player)
    {
        H3_Player & player = getPlayer( m_player );

        if (m_state == H3_State::PlaceRoad)
        {
            de::gpu::SMeshBufferTool::colorVertices( m_smeshRoad, player.color );

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

    if (m_player)
    {
        H3_Player & player = getPlayer( m_player );

        if (m_state == H3_State::PlaceFarm)
        {
            de::gpu::SMeshBufferTool::colorVertices( m_smeshFarm, player.color );

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
            de::gpu::SMeshBufferTool::colorVertices( m_smeshCity, player.color );

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
    auto rend = driver->getLine3DRenderer();

    de::Box3f bbox;
    const auto a = m_tileSize.x * 0.5f;
    const auto b = m_tileSize.y;
    const auto c = m_tileSize.z * 0.5f;
    bbox.m_Min = glm::vec3(-a,0,-c) + tile.pos;
    bbox.m_Max = glm::vec3( a,b, c) + tile.pos;

    driver->resetModelMatrix();
    rend->draw3DLineBox( bbox, 0xFFC0C0C0 );
}

void H3_Game::draw()
{
    if (!m_acceptPaintEvents)
    {
        DE_WARN("No m_acceptPaintEvents")
        return;
    }

    //DE_OK("")
    auto driver = getDriver();
    if (!driver) { DE_ERROR("No driver") return; }

    // Time:
    static double tStart = dbTimeInSeconds();
    double t = dbTimeInSeconds() - tStart;

    driver->beginRender();

    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();

    if ( isRunning() )
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
        drawHoveredTile();
        drawHoveredEdge();
        drawHoveredCorner();

        driver->beginPerf("UI_Stats");
        UI_Stats_render( *this );
        driver->endPerf();

        driver->beginPerf("UI_PlayerStats");
        for ( int i = 0; i < m_players.size(); ++i )
        {
            UI_drawPlayerStats( *this, 20, 20+100*i, m_players[ i ].id );
        }
        driver->endPerf();

        driver->beginPerf("UI_RoundStats");
        UI_drawRoundStats( *this );
        driver->endPerf();

        auto fScale = 0.01f * getScalePc();
        auto p = de::Recti( 20, h/2-100, fScale * 200, fScale * 300 );

        driver->beginPerf("UI_Info");
        UI_drawInfoPanelTile( *this, p, m_hoverTileId );
        UI_drawInfoPanelCorner( *this, p, m_hoverCornerId );
        UI_drawInfoPanelEdge( *this, p, m_hoverEdgeId );
        driver->endPerf();

        driver->beginPerf("UI_Log");
        UI_drawLog( *this );
        driver->endPerf();

        //auto fontGarton = m_fontGarton32;
        //fontGarton.pixelSize = 2.0f * fScale * fontGarton.pixelSize;
        //H3_drawText( game, w/2, 0, "Hello Marquee text!!!", 0xFFFFFFFF,
        //             de::Align::TopCenter, fontGarton);

        /*
        if ( m_ui.m_hoverElement > -1 )
        {
            auto & elem = m_ui.m_elements[ m_ui.m_hoverElement ];
            H3_draw2DRect( game, elem.pos, 0x6F0000FF );
        }
        */

        drawMarquee();
        driver->endPerf();


        driver->drawPerf( de::Recti( w- 300, 500, 300, 300));
    }
    else
    {
        auto wallpaper = getTex(H3_Tex::Satan, "draw().wallpaper");
        driver->getScreenRenderer()->draw2DRect(de::Recti(0,0,w,h),0xFFFFFFFF, wallpaper);
    }

    driver->beginPerf("GUIEnv");
    m_guienv.draw();
    driver->endPerf();

    driver->endRender();

    m_device->getWindow()->swapBuffers();
}

void H3_Game::onEvent( const de::Event& event )
{
    m_guienv.onEvent( event );

    if ( event.type == de::EventType::TIMER )
    {
        auto const& evt = event.timerEvent;
        if ( evt.id == m_fpsTimerId )
        {
            if (m_device) m_device->getWindow()->update();
        }
        else if ( evt.id == m_capTimerId )
        {
            updateWindowTitle();
        }
    }
    else if ( event.type == de::EventType::RESIZE )
    {
        UI_resizeLayouts();
    }
    else if ( event.type == de::EventType::PAINT )
    {
        draw();
    }
    else if ( event.type == de::EventType::KEY_PRESS )
    {
        const auto key = event.keyPressEvent.key;
        // Toggle visibility of MainMenu:
        if (key == de::KEY_ESCAPE)
        {
            if (m_uiMainmenu)
            {
                bool bVisible = m_uiMainmenu->isVisible();
                m_uiMainmenu->setVisible( !bVisible );
            }
        }

        // F11 - Toggle window resizable
        if (key == de::KEY_F11)
        {
            auto window = m_device->getWindow();
            if (window)
            {
                window->setResizable( !window->isResizable() );
            }
        }
        // F12|F - Toggle window fullscreen
        if (key == de::KEY_F12 || key == de::KEY_F )
        {
            auto window = m_device->getWindow();
            if (window)
            {
                window->setFullScreen( !window->isFullScreen() );
            }
        }


        // SPACE - Toggle help overlay
        if (key == de::KEY_SPACE) // SPACE - Toggle overlay
        {
            m_isCameraMouseInputEnabled = !m_isCameraMouseInputEnabled;
            if (m_isCameraMouseInputEnabled)
            {
                m_firstMouse = true;
            }
            //m_camera.setInputEnabled( !m_camera.isInputEnabled() );
            // m_showHelpOverlay = !m_showHelpOverlay;
        }
        // PAGE_UP - Increase font size
        if (key == de::KEY_PAGE_UP)
        {
            setScalePc( std::min( 400, getScalePc() + 10 ) );
            UI_resizeLayouts();
        }
        // PAGE_DOWN - decrease font size
        if (key == de::KEY_PAGE_DOWN)
        {
            setScalePc( std::max( 50, getScalePc() - 10 ) );
            UI_resizeLayouts();
        }
        // ARROW_UP - Increase frame wait time in ms - lower FPS
        if (key == de::KEY_UP)
        {
        }

        // ARROW_DOWN - Decrease frame wait time in ms - increase FPS
        if (key == de::KEY_DOWN)
        {
        }

        // // Move camera
        // auto camera = getCamera();
        // if (camera)
        // {
        //     if (key == de::KEY_UP)   { camera->move( 2.0f ); }
        //     if (key == de::KEY_DOWN) { camera->move( -1.0f ); }
        //     if (key == de::KEY_LEFT) { camera->strafe( -1.0f ); }
        //     if (key == de::KEY_RIGHT) { camera->strafe( 1.0f ); }
        //     if (key == de::KEY_W) { camera->move( 2.0f ); }
        //     if (key == de::KEY_S) { camera->move( -1.0f ); }
        //     if (key == de::KEY_A) { camera->strafe( -1.0f ); }
        //     if (key == de::KEY_D) { camera->strafe( 1.0f ); }
        // }
    }
    else if ( event.type == de::EventType::KEY_RELEASE )
    {
    }
    else if ( event.type == de::EventType::MOUSE_DBLCLICK )
    {
        // if (event.mouseDblClickEvent.isLeft())
        // {
        //     H3_MessageBox("Left DoubleClick","New MouseEvents");
        // }
    }
    else if ( event.type == de::EventType::MOUSE_PRESS )
    {
        //DE_BENNI("MousePressEvent = ", event.mousePressEvent.str())
        if (event.mousePressEvent.isLeft())
        {
            m_isMouseLeftPressed = true;
            m_leftDragStartX = event.mousePressEvent.x;
            m_leftDragStartY = event.mousePressEvent.y;
            m_leftDragLastX = event.mousePressEvent.x;
            m_leftDragLastY = event.mousePressEvent.y;
        }
        else if (event.mousePressEvent.isRight())
        {
            m_isMouseRightPressed = true;
        }
        else if (event.mousePressEvent.isMiddle())
        {
            m_isMouseMiddlePressed = true;
        }

    }
    else if ( event.type == de::EventType::MOUSE_RELEASE )
    {
        //DE_BENNI("MouseReleaseEvent = ", event.mouseReleaseEvent.str())
        if (event.mouseReleaseEvent.isLeft())
        {
            if (m_isMouseLeftPressed)
            {
                if (m_state == H3_State::PlaceRoad && m_hoverEdgeId)
                {
                    H3_Edge & edge = H3_getEdge(*this, __func__, m_hoverEdgeId);
                    finalizeBuyRoad( edge );
                }
                else if (m_state == H3_State::PlaceFarm && m_hoverCornerId)
                {
                    H3_Corner & corner = H3_getCorner(*this, __func__, m_hoverCornerId);
                    finalizeBuyFarm( corner );
                }
                else if (m_state == H3_State::PlaceCity && m_hoverFarmId)
                {
                    H3_Farm & farm = H3_getFarm(*this, __func__, m_hoverFarmId);
                    finalizeBuyCity( farm );
                }
                else if (m_state == H3_State::PlaceThief && m_hoverTileId)
                {
                    H3_Tile & tile = H3_getTile(*this, __func__, m_hoverTileId);
                    leaveThiefPlacement( tile );
                }
                else if (m_state == H3_State::StealRandomCard)
                {
                    if (m_hoverRoadId)
                    {
                        H3_Road & road = H3_getRoad(*this, __func__, m_hoverRoadId);
                        H3_Edge & edge = H3_getEdge(*this, __func__, road.edgeId);
                        leaveThiefOutro( edge );
                    }
                    else if (m_hoverFarmId)
                    {
                        H3_Farm & farm = H3_getFarm(*this, __func__, m_hoverFarmId);
                        H3_Corner & corner = H3_getCorner(*this, __func__, farm.cornerId);
                        leaveThiefOutro( corner );
                    }
                    else if (m_hoverCityId)
                    {
                        H3_City & city = H3_getCity(*this, __func__, m_hoverCityId);
                        H3_Corner & corner = H3_getCorner(*this, __func__, city.cornerId);
                        leaveThiefOutro( corner );
                    }
                }
                else
                {

                }
            }
            m_isMouseLeftPressed = false;
        }
        else if (event.mouseReleaseEvent.isRight())
        {
            m_isMouseRightPressed = false;
        }
        else if (event.mouseReleaseEvent.isMiddle())
        {
            if (m_isMouseMiddlePressed &&
                m_hoverTileId &&
                (m_state == H3_State::Idle))
            {
                auto driver = getDriver();
                auto camera = driver->getCamera();

                H3_Tile & tile = H3_getTile(*this, __func__, m_hoverTileId);

                glm::dvec3 A = camera->getPos();
                glm::dvec3 B = camera->getTarget();
                glm::dvec3 AB = B - A;

                glm::dvec3 D = tile.pos;
                glm::dvec3 C = D - AB;

                camera->lookAt( C, D );
            }

            m_isMouseMiddlePressed = false;
        }
    }
    else if ( event.type == de::EventType::MOUSE_WHEEL )
    {
        auto camera = getCamera();
        if (camera)
        {
            if ( m_isCameraMouseInputEnabled )
            {
                if ( event.mouseWheelEvent.y < 0.0f )
                {
                    camera->move( -2.5f );
                }
                else if ( event.mouseWheelEvent.y > 0.0f )
                {
                    camera->move( 2.5f );
                }
            }
            else
            {
                if ( event.mouseWheelEvent.y < 0.0f )
                {
                    cylinderCamRadius += 5.f;
                }
                else if ( event.mouseWheelEvent.y > 0.0f )
                {
                    cylinderCamRadius -= 5.f;
                }

                if (cylinderCamRadius < 1.0)
                    cylinderCamHeight = 1.0;

                if (cylinderCamRadius > 2000.0)
                    cylinderCamRadius = 2000.0;

                DE_DEBUG("CylinderCam: "
                        "phi(",cylinderCamAngleY,"), "
                        "radius(",cylinderCamRadius,"), "
                        "height(",cylinderCamHeight,")")

                auto camPos = glm::dvec3( cylinderCamRadius * sin(cylinderCamAngleY),
                                     cylinderCamHeight,
                                     cylinderCamRadius * cos(cylinderCamAngleY));

                auto camTar = glm::dvec3(0,0,0);
                camera->lookAt( camPos, camTar );

            }
        }
    }
    else if ( event.type == de::EventType::MOUSE_MOVE )
    {
        const int mx = event.mouseMoveEvent.x;
        const int my = event.mouseMoveEvent.y;

        if ( m_isCameraMouseInputEnabled )
        {
            if (m_firstMouse)
            {
                m_firstMouse = false;
            }
            else
            {
                m_mouseMoveX = mx - m_mouseX;
                m_mouseMoveY = my - m_mouseY;

                auto camera = getCamera();
                if (camera)
                {
                    camera->yaw( 0.003f * m_mouseMoveX );
                    camera->pitch( 0.003f * m_mouseMoveY );
                }
            }
        }
        else
        {
            if (m_isMouseLeftPressed)
            {
                int mouseDragDeltaX = mx - m_leftDragLastX;
                int mouseDragDeltaY = my - m_leftDragLastY;
                m_leftDragLastX = mx;
                m_leftDragLastY = my;
                auto camera = getCamera();
                if (camera)
                {
                    auto camPos = camera->getPos();
                    auto camTar = glm::dvec3(0,0,0);

                    auto camDir = camTar - camPos;
                    //cylinderCamRadius = glm::length( glm::dvec2(camDir.x, camDir.z) );
                    //cylinderCamHeight = camDir.y;
                    //cylinderCamAngleY = atan2(camDir.z, camDir.x) * de::Math::RAD2DEG;
                    DE_DEBUG("CylinderCam: "
                            "phi(",cylinderCamAngleY,"), "
                            "radius(",cylinderCamRadius,"), "
                            "height(",cylinderCamHeight,")")

                    cylinderCamAngleY += 0.01f * mouseDragDeltaX;
                    cylinderCamHeight += mouseDragDeltaY;

                    if (cylinderCamHeight < 0.0)
                        cylinderCamHeight = 0.0;

                    if (cylinderCamHeight > 1000.0)
                        cylinderCamHeight = 1000.0;

                    camPos = glm::dvec3( cylinderCamRadius * sin(cylinderCamAngleY),
                                         cylinderCamHeight,
                                         cylinderCamRadius * cos(cylinderCamAngleY));

                    camera->lookAt( camPos, camTar );
                }

            }
        }
        m_mouseMoveX = 0; // Reset
        m_mouseMoveY = 0; // Reset
        m_mouseX = mx; // Store current value
        m_mouseY = my; // Store current value

        pick();
    }
}



void H3_Game::clearMarquee()
{
    m_uiMarquee.h1.clear();
    m_uiMarquee.h3.clear();
}

void H3_Game::marqueeH1( std::wstring msg, int durationMs, uint32_t color )
{
    UI_MarqueeItem item;
    item.msg = msg;
    item.timeDurationInMs = durationMs;
    item.timeStartInSec = std::nullopt;
    item.color = color;
    item.font = m_uiMarquee.fontH1;
    m_uiMarquee.h1.push_back( std::move( item ) );
}

void H3_Game::marqueeH3( std::wstring msg, int durationMs, uint32_t color )
{
    UI_MarqueeItem item;
    item.msg = msg;
    item.timeDurationInMs = durationMs;
    item.timeStartInSec = std::nullopt;
    item.color = color;
    item.font = m_uiMarquee.fontH3;
    m_uiMarquee.h3.push_back( std::move( item ) );
}

void H3_Game::drawMarquee()
{
    auto driver = getDriver();
    if (!driver)
    {
        DE_ERROR("No driver")
        return;
    }
    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();
    const double t = dbTimeInSeconds();

    const auto fScale = 0.01f * getScalePc();
    const auto fontSizeH1 = fScale * m_uiMarquee.fontH1.pixelSize;
    const auto fontSizeH3 = fScale * m_uiMarquee.fontH3.pixelSize;

    const auto align = de::Align::TopCenter;

    // === H1 ===
    if (!m_uiMarquee.h1.empty())
    {
        auto & item = m_uiMarquee.h1.front();

        if (!item.timeStartInSec)
        {
            item.timeStartInSec = t;
        }

        // Draw marquee when duration = 0, or timeout not reached.
        if (item.timeDurationInMs < 1 || ((t - item.timeStartInSec.value()) <= (0.001 * item.timeDurationInMs)))
        {
            auto font = m_uiMarquee.fontH1;
            font.pixelSize = fontSizeH1;
            const int x = w / 2;
            const int y = h / 20;

            driver->draw2DText( x, y, item.msg,item.color, align,font,0xA0000000,10 );
        }
        else
        {
            m_uiMarquee.h1.pop_front();
        }
    }

    // === H3 ===
    if (!m_uiMarquee.h3.empty())
    {
        auto & item = m_uiMarquee.h3.front();

        if (!item.timeStartInSec)
        {
            item.timeStartInSec = t;
        }

        // Draw marquee when duration = 0, or timeout not reached.
        if (item.timeDurationInMs < 1 || ((t - item.timeStartInSec.value()) <= (0.001 * item.timeDurationInMs)))
        {
            de::Font font = m_uiMarquee.fontH3;
            font.pixelSize = fontSizeH3;
            const int x = w / 2;
            const int y = h / 20 + fontSizeH1 + 20;
            driver->draw2DText( x, y, item.msg, item.color, align, font, 0xA0000000, 10 );
        }
        else
        {
            m_uiMarquee.h3.pop_front();
        }
    }
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
            uint8_t r = dbRGBA_R(color);
            uint8_t g = dbRGBA_G(color);
            uint8_t b = dbRGBA_B(color);
            if (r>d && g>d && b>=d)
            {
                img.setPixel( x,y, 0x00000000 ); // Make transparent
            }
        }
    }
}
*/
