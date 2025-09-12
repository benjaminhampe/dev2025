#include "EventReceiver.h"

inline void
my_createSkyboxMarriotHotel( irr::scene::ISceneManager* smgr )
{
    auto driver = smgr->getVideoDriver();

    std::string mediaDir = g_mediaDir + "skybox/";

    g_skybox = smgr->addSkyBoxSceneNode(
        loadTexture(driver, mediaDir + "MarriottMadisonWest/py.jpg"),
        loadTexture(driver, mediaDir + "MarriottMadisonWest/ny.jpg"),
        loadTexture(driver, mediaDir + "MarriottMadisonWest/pz.jpg"),
        loadTexture(driver, mediaDir + "MarriottMadisonWest/nz.jpg"),
        loadTexture(driver, mediaDir + "MarriottMadisonWest/px.jpg"),
        loadTexture(driver, mediaDir + "MarriottMadisonWest/nx.jpg")
    );
}

inline void
my_createLights( irr::scene::ISceneManager* smgr )
{
    smgr->addLightSceneNode(0, core::vector3df(200,200,200),
                            video::SColorf(1.0f,1.0f,1.0f),2000);
    smgr->setAmbientLight(video::SColorf(0.3f,0.3f,0.3f));
}

// create menu
inline void
my_createMenuBar( irr::gui::IGUIEnvironment* env )
{
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
}

// create toolbar
inline void
my_createToolBar( irr::gui::IGUIEnvironment* env )
{
    gui::IGUIToolBar* bar = env->addToolBar();
    video::IVideoDriver* driver = env->getVideoDriver();
    video::ITexture* image = driver->getTexture("open.png");
    bar->addButton(GUI_ID_BUTTON_OPEN_MODEL, 0, L"Open a model",image, 0, false, true);
    image = driver->getTexture("tools.png");
    bar->addButton(GUI_ID_BUTTON_SHOW_TOOLBOX, 0, L"Open Toolset",image, 0, false, true);
    image = driver->getTexture("zip.png");
    bar->addButton(GUI_ID_BUTTON_SELECT_ARCHIVE, 0, L"Set Model Archive",image, 0, false, true);
    image = driver->getTexture("help.png");
    bar->addButton(GUI_ID_BUTTON_SHOW_ABOUT, 0, L"Open Help", image, 0, false, true);

    gui::IGUIComboBox* box = env->addComboBox(
        core::rect<s32>(250,4,350,23), bar, GUI_ID_TEXTUREFILTER);

    box->addItem(L"No filtering");
    box->addItem(L"Bilinear");
    box->addItem(L"Trilinear");
    box->addItem(L"Anisotropic");
    box->addItem(L"Isotropic");

    // create fps text
    IGUIStaticText* fpstext = env->addStaticText(L"",
        core::rect<s32>(400,4,570,23), true, false, bar);

    IGUIStaticText* postext = env->addStaticText(L"",
        core::rect<s32>(10,50,470,80),false, false, 0, GUI_ID_POSITION_TEXT);

    postext->setVisible(false);
}

/*
Function updateToolBox() is called each frame to update dynamic information in
the toolbox.
*/
inline void my_updateToolBox()
{
    IGUIEnvironment* env = g_Device->getGUIEnvironment();
    IGUIElement* root = env->getRootGUIElement();
    IGUIElement* dlg = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
    if (!dlg )
        return;

    // update the info we have about the animation of the model
    auto aniInfo = (IGUIStaticText*)(dlg->getElementFromId(GUI_ID_ANIMATION_INFO, true));
    if (aniInfo)
    {
        if (g_model && (g_model->getType() == scene::ESNT_ANIMATED_MESH))
        {
            scene::IAnimatedMeshSceneNode* animatedModel = (scene::IAnimatedMeshSceneNode*)g_model;

            core::stringw str( (s32)core::round_(animatedModel->getAnimationSpeed()) );
            str += L" Frame: ";
            str += core::stringw((s32)animatedModel->getFrameNr());
            aniInfo->setText(str.c_str());
        }
        else
            aniInfo->setText(L"");
    }
}


// create cameras
inline void
my_createCameras( irr::scene::ISceneManager* smgr )
{
    g_cameras[0] = smgr->addCameraSceneNodeMaya();
    g_cameras[0]->setFarValue(20000.f);
    // Maya cameras reposition themselves relative to their target, so target the location
    // where the mesh scene node is placed.
    g_cameras[0]->setTarget(core::vector3df(0,30,0));

    g_cameras[1] = smgr->addCameraSceneNodeFPS();
    g_cameras[1]->setFarValue(20000.f);
    g_cameras[1]->setPosition(core::vector3df(0,0,-70));
    g_cameras[1]->setTarget(core::vector3df(0,30,0));
    setActiveCamera(smgr, g_cameras[0]);
}

int main(int argc, char** argv)
{
	// ask user for driver
    video::E_DRIVER_TYPE driverType = video::EDT_OPENGL; // driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	// create device and exit if creation failed
	MyEventReceiver receiver;
	
    g_Device = createDevice(driverType, core::dimension2d<u32>(1024, 768), 32, false, false, false, &receiver);
    if (g_Device == 0) return 1; // could not create selected driver.
	g_Device->setResizable(true);
    g_Device->setWindowCaption(L"LibreCAD (c) 2024 <hampe.benjamin@gmail.com>");

    io::IFileSystem* fsys = g_Device->getFileSystem();
    fsys->addFileArchive(g_mediaDir.c_str()); // add media dir as "search path"

	video::IVideoDriver* driver = g_Device->getVideoDriver();
    driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

    // scene
    scene::ISceneManager* smgr = g_Device->getSceneManager();
	smgr->getParameters()->setAttribute(scene::COLLADA_CREATE_SCENE_INSTANCES, true);
    my_createSkyboxMarriotHotel(smgr);
    my_createLights( smgr );
    my_createCameras( smgr );

    // gui
    gui::IGUIEnvironment* env = g_Device->getGUIEnvironment();
    gui::IGUIElement* envRoot = env->getRootGUIElement();
    //gui::IGUIFont* fontMatrix = loadFont( env, "matrix code nfi.ttf");
    //gui::IGUIFont* fontAwesome = loadFont( env, "fontawesome463.ttf");
    gui::IGUIFont* fontFiraReg = loadFont( env, "fonts/Fira_Code_v6.2/FiraCode-Regular.ttf");
    setSkinFont(env, fontFiraReg );

#if 0
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

	io::IXMLReader* xml = g_Device->getFileSystem()->createXMLReader( L"config.xml");

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

#endif

    my_createMenuBar( env );
    my_createToolBar( env );
    setSkinTransparency( env, 255 ); // disable alpha
    my_createToolBox(); // add a tabcontrol

	// set window caption
	Caption += " - [";
	Caption += driver->getName();
	Caption += "]";
	g_Device->setWindowCaption(Caption.c_str());

	// show about message box and load default model
	if (argc==1)
        my_showAboutText();

    my_loadModel(g_modelFile.c_str());

	// load the irrlicht engine logo
	IGUIImage *img =
		env->addImage(driver->getTexture("irrlichtlogo2.png"),
			core::position2d<s32>(10, driver->getScreenSize().Height - 128));

	// lock the logo's edges to the bottom left corner of the screen
	img->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,
			EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);


    g_ui_textureList = new CGUITextureList(env, mk_recti(100,100,800,28), envRoot, -1);

    g_ui_colorControl = new CGUIColorControl(env, mk_recti(100,200,300,100), L"ColorRGBA", envRoot, -1);

    g_ui_imagePanel = new CGUIImagePanel(env, mk_recti(250,500,300,200), envRoot, -1);

	// remember state so we notice when the window does lose the focus
	bool hasFocus = g_Device->isWindowFocused();

	// draw everything

	while(g_Device->run() && driver)
	{
		// Catch focus changes (workaround until Irrlicht has events for this)
        bool const focused = g_Device->isWindowFocused();
		if ( hasFocus && !focused )
        {
            my_onKillFocus();
        }
		hasFocus = focused;

		if (g_Device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(150,50,50,50));

			smgr->drawAll();
			env->drawAll();

			driver->endScene();
/*
			// update information about current frame-rate
			core::stringw str(L"FPS: ");
			str.append(core::stringw(driver->getFPS()));
			str += L" Tris: ";
			str.append(core::stringw(driver->getPrimitiveCountDrawn()));
			fpstext->setText(str.c_str());

			// update information about the active camera
			scene::ICameraSceneNode* cam = g_Device->getSceneManager()->getActiveCamera();
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

*/
			// update the tool dialog
            my_updateToolBox();
		}
		else
			g_Device->yield();
	}

	g_Device->drop();
	return 0;
}

/*
**/
