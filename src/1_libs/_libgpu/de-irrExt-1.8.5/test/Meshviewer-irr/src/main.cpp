#include "MyEventReceiver.h"

/*
Most of the hard work is done. We only need to create the Irrlicht Engine
device and all the buttons, menus and toolbars. We start up the engine as
usual, using createDevice(). To make our application catch events, we set our
eventreceiver as parameter. As you can see, there is also a call to
IrrlichtDevice::setResizeable(). This makes the render window resizeable, which
is quite useful for a mesh viewer.
*/
int main(int argc, char* argv[])
{
    // ask user for driver
    video::E_DRIVER_TYPE driverType=driverChoiceConsole();
    if (driverType==video::EDT_COUNT)
        return 1;

    // create device and exit if creation failed
    MyEventReceiver receiver;
    Device = createDevice(driverType, core::dimension2d<u32>(800, 600),
        16, false, false, false, &receiver);

    if (Device == 0)
        return 1; // could not create selected driver.

    Device->setResizable(true);

    Device->setWindowCaption(L"Irrlicht Engine - Loading...");

    video::IVideoDriver* driver = Device->getVideoDriver();
    IGUIEnvironment* env = Device->getGUIEnvironment();
    scene::ISceneManager* smgr = Device->getSceneManager();
    smgr->getParameters()->
        setAttribute(scene::COLLADA_CREATE_SCENE_INSTANCES, true);

    driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

    smgr->addLightSceneNode(0, core::vector3df(200,200,200),
        video::SColorf(1.0f,1.0f,1.0f),2000);
    smgr->setAmbientLight(video::SColorf(0.3f,0.3f,0.3f));
    // add our media directory as "search path"

    auto mediaDir = Device->getFileSystem()->
                        getAbsolutePath("./media/ABC_Meshviewer");

    Device->getLogger()->log( mediaDir.c_str(), L"mediaDir", irr::ELL_ERROR);

    Device->getFileSystem()->
        addFileArchive(mediaDir);

    /*
    The next step is to read the configuration file. It is stored in the xml
    format and looks a little bit like this:

    @verbatim
    <?xml version="1.0"?>
    <config>
        <startUpModel file="some filename" />
        <messageText caption="Irrlicht Engine Mesh Viewer">
            Hello!
        </messageText>
    </config>
    @endverbatim

    We need the data stored in there to be written into the global variables
    StartUpModelFile, MessageText and Caption. This is now done using the
    Irrlicht Engine integrated XML parser:
    */

    // read configuration from xml file

    io::IXMLReader* xml = Device->getFileSystem()->createXMLReader( L"config.xml");

    while(xml && xml->read())
    {
        switch(xml->getNodeType())
        {
        case io::EXN_TEXT:
            // in this xml file, the only text which occurs is the
            // messageText
            MessageText = xml->getNodeData();
            break;
        case io::EXN_ELEMENT:
            {
                if (core::stringw("startUpModel") == xml->getNodeName())
                    StartUpModelFile = xml->getAttributeValue(L"file");
                else
                if (core::stringw("messageText") == xml->getNodeName())
                    Caption = xml->getAttributeValue(L"caption");
            }
            break;
        default:
            break;
        }
    }

    if (xml)
        xml->drop(); // don't forget to delete the xml reader

    if (argc > 1)
        StartUpModelFile = argv[1];

    /*
    That wasn't difficult. Now we'll set a nicer font and create the Menu.
    It is possible to create submenus for every menu item. The call
    menu->addItem(L"File", -1, true, true); for example adds a new menu
    Item with the name "File" and the id -1. The following parameter says
    that the menu item should be enabled, and the last one says, that there
    should be a submenu. The submenu can now be accessed with
    menu->getSubMenu(0), because the "File" entry is the menu item with
    index 0.
    */

    // set a nicer font

    IGUISkin* skin = env->getSkin();
    IGUIFont* font = env->getFont("fonthaettenschweiler.bmp");
    if (font)
        skin->setFont(font);

    // create menu
    gui::IGUIContextMenu* menu = env->addMenu();
    menu->addItem(L"File", -1, true, true);
    menu->addItem(L"View", -1, true, true);
    menu->addItem(L"Camera", -1, true, true);
    menu->addItem(L"Help", -1, true, true);

    gui::IGUIContextMenu* submenu;
    submenu = menu->getSubMenu(0);
    submenu->addItem(L"Open Model File & Texture...", GUI_ID_OPEN_MODEL);
    submenu->addItem(L"Set Model Archive...", GUI_ID_SET_MODEL_ARCHIVE);
    submenu->addItem(L"Load as Octree", GUI_ID_LOAD_AS_OCTREE);
    submenu->addSeparator();
    submenu->addItem(L"Quit", GUI_ID_QUIT);

    submenu = menu->getSubMenu(1);
    submenu->addItem(L"sky box visible", GUI_ID_SKY_BOX_VISIBLE, true, false, true);
    submenu->addItem(L"toggle model debug information", GUI_ID_TOGGLE_DEBUG_INFO, true, true);
    submenu->addItem(L"model material", -1, true, true );

    submenu = submenu->getSubMenu(1);
    submenu->addItem(L"Off", GUI_ID_DEBUG_OFF);
    submenu->addItem(L"Bounding Box", GUI_ID_DEBUG_BOUNDING_BOX);
    submenu->addItem(L"Normals", GUI_ID_DEBUG_NORMALS);
    submenu->addItem(L"Skeleton", GUI_ID_DEBUG_SKELETON);
    submenu->addItem(L"Wire overlay", GUI_ID_DEBUG_WIRE_OVERLAY);
    submenu->addItem(L"Half-Transparent", GUI_ID_DEBUG_HALF_TRANSPARENT);
    submenu->addItem(L"Buffers bounding boxes", GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES);
    submenu->addItem(L"All", GUI_ID_DEBUG_ALL);

    submenu = menu->getSubMenu(1)->getSubMenu(2);
    submenu->addItem(L"Solid", GUI_ID_MODEL_MATERIAL_SOLID);
    submenu->addItem(L"Transparent", GUI_ID_MODEL_MATERIAL_TRANSPARENT);
    submenu->addItem(L"Reflection", GUI_ID_MODEL_MATERIAL_REFLECTION);

    submenu = menu->getSubMenu(2);
    submenu->addItem(L"Maya Style", GUI_ID_CAMERA_MAYA);
    submenu->addItem(L"First Person", GUI_ID_CAMERA_FIRST_PERSON);

    submenu = menu->getSubMenu(3);
    submenu->addItem(L"About", GUI_ID_ABOUT);

    /*
    Below the menu we want a toolbar, onto which we can place colored
    buttons and important looking stuff like a senseless combobox.
    */

    // create toolbar

    gui::IGUIToolBar* bar = env->addToolBar();

    video::ITexture* image = driver->getTexture("open.png");
    bar->addButton(GUI_ID_BUTTON_OPEN_MODEL, 0, L"Open a model",image, 0, false, true);

    image = driver->getTexture("tools.png");
    bar->addButton(GUI_ID_BUTTON_SHOW_TOOLBOX, 0, L"Open Toolset",image, 0, false, true);

    image = driver->getTexture("zip.png");
    bar->addButton(GUI_ID_BUTTON_SELECT_ARCHIVE, 0, L"Set Model Archive",image, 0, false, true);

    image = driver->getTexture("help.png");
    bar->addButton(GUI_ID_BUTTON_SHOW_ABOUT, 0, L"Open Help", image, 0, false, true);

    // create a combobox for texture filters

    gui::IGUIComboBox* box = env->addComboBox(core::rect<s32>(250,4,350,23), bar, GUI_ID_TEXTUREFILTER);
    box->addItem(L"No filtering");
    box->addItem(L"Bilinear");
    box->addItem(L"Trilinear");
    box->addItem(L"Anisotropic");
    box->addItem(L"Isotropic");

    /*
    To make the editor look a little bit better, we disable transparent gui
    elements, and add an Irrlicht Engine logo. In addition, a text showing
    the current frames per second value is created and the window caption is
    changed.
    */

    // disable alpha

    for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
    {
        video::SColor col = env->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
        col.setAlpha(255);
        env->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
    }

    // add a tabcontrol

    createToolBox();

    // create fps text

    IGUIStaticText* fpstext = env->addStaticText(L"",
            core::rect<s32>(400,4,570,23), true, false, bar);

    IGUIStaticText* postext = env->addStaticText(L"",
            core::rect<s32>(10,50,470,80),false, false, 0, GUI_ID_POSITION_TEXT);
    postext->setVisible(false);

    // set window caption

    Caption += " - [";
    Caption += driver->getName();
    Caption += "]";
    Device->setWindowCaption(Caption.c_str());

    /*
    That's nearly the whole application. We simply show the about message
    box at start up, and load the first model. To make everything look
    better, a skybox is created and a user controlled camera, to make the
    application a little bit more interactive. Finally, everything is drawn
    in a standard drawing loop.
    */

    // show about message box and load default model
    if (argc==1)
        showAboutText();
    loadModel(StartUpModelFile.c_str());

    // add skybox

    SkyBox = smgr->addSkyBoxSceneNode(
      // driver->getTexture("MarriottMadisonWest/py.jpg"),
      // driver->getTexture("MarriottMadisonWest/ny.jpg"),
      // driver->getTexture("MarriottMadisonWest/pz.jpg"),
      // driver->getTexture("MarriottMadisonWest/nz.jpg"),
      // driver->getTexture("MarriottMadisonWest/px.jpg"),
      // driver->getTexture("MarriottMadisonWest/nx.jpg"));
        driver->getTexture("irrlicht2_up.jpg"),
        driver->getTexture("irrlicht2_dn.jpg"),
        driver->getTexture("irrlicht2_lf.jpg"),
        driver->getTexture("irrlicht2_rt.jpg"),
        driver->getTexture("irrlicht2_ft.jpg"),
        driver->getTexture("irrlicht2_bk.jpg"));

    // add a camera scene node
    Camera[0] = smgr->addCameraSceneNodeMaya();
    Camera[0]->setFarValue(20000.f);
    // Maya cameras reposition themselves relative to their target, so target the location
    // where the mesh scene node is placed.
    Camera[0]->setTarget(core::vector3df(0,30,0));

    Camera[1] = smgr->addCameraSceneNodeFPS();
    Camera[1]->setFarValue(20000.f);
    Camera[1]->setPosition(core::vector3df(0,0,-70));
    Camera[1]->setTarget(core::vector3df(0,30,0));

    setActiveCamera(Camera[0]);

    // load the irrlicht engine logo
    IGUIImage *img =
        env->addImage(driver->getTexture("irrlichtlogo2.png"),
            core::position2d<s32>(10, driver->getScreenSize().Height - 128));

    // lock the logo's edges to the bottom left corner of the screen
    img->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,
            EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

    // remember state so we notice when the window does lose the focus
    bool hasFocus = Device->isWindowFocused();

    // draw everything

    while(Device->run() && driver)
    {
        // Catch focus changes (workaround until Irrlicht has events for this)
        bool focused = Device->isWindowFocused();
        if ( hasFocus && !focused )
            onKillFocus();
        hasFocus = focused;

        if (Device->isWindowActive())
        {
            driver->beginScene(true, true, video::SColor(150,50,50,50));

            smgr->drawAll();
            env->drawAll();

            driver->endScene();

            // update information about current frame-rate
            core::stringw str(L"FPS: ");
            str.append(core::stringw(driver->getFPS()));
            str += L" Tris: ";
            str.append(core::stringw(driver->getPrimitiveCountDrawn()));
            fpstext->setText(str.c_str());

            // update information about the active camera
            scene::ICameraSceneNode* cam = Device->getSceneManager()->getActiveCamera();
            str = L"Pos: ";
            str.append(core::stringw(cam->getPosition().X));
            str += L" ";
            str.append(core::stringw(cam->getPosition().Y));
            str += L" ";
            str.append(core::stringw(cam->getPosition().Z));
            str += L" Tgt: ";
            str.append(core::stringw(cam->getTarget().X));
            str += L" ";
            str.append(core::stringw(cam->getTarget().Y));
            str += L" ";
            str.append(core::stringw(cam->getTarget().Z));
            postext->setText(str.c_str());

            // update the tool dialog
            updateToolBox();
        }
        else
            Device->yield();
    }

    Device->drop();
    return 0;
}

/*
**/
