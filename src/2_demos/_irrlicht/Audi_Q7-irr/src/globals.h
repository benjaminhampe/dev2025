#pragma once
#include "helper.h"
#include "irrext_ui_ColorControl.h"
#include "irrext_ui_TextureList.h"
#include "irrext_ui_ImagePanel.h"

using namespace irr;
using namespace gui;
IrrlichtDevice* g_Device = 0;

std::string g_mediaDir = "media/Audi_Q7/";
std::string g_modelFile = g_mediaDir + "Audi_Q7.obj";
//std::string g_fontFile = g_mediaDir + "font_haettenschweiler.png";
std::string g_fontFile = g_mediaDir + "font_courier.png";

scene::ICameraSceneNode* g_cameras[2] = {0, 0};

scene::ISceneNode* g_skybox = 0;
scene::ISceneNode* g_model = 0;

core::stringc StartUpModelFile;
core::stringw MessageText;
core::stringw Caption;

bool g_bOctree=false;
bool g_bUseLight=false;

CGUITextureList* g_ui_textureList = nullptr;
CGUIColorControl* g_ui_colorControl = nullptr;
CGUIImagePanel* g_ui_imagePanel = nullptr;

enum
{
	GUI_ID_DIALOG_ROOT_WINDOW  = 0x10000,

	GUI_ID_X_SCALE,
	GUI_ID_Y_SCALE,
	GUI_ID_Z_SCALE,

	GUI_ID_OPEN_MODEL,
	GUI_ID_SET_MODEL_ARCHIVE,
	GUI_ID_LOAD_AS_OCTREE,

	GUI_ID_SKY_BOX_VISIBLE,
	GUI_ID_TOGGLE_DEBUG_INFO,

	GUI_ID_DEBUG_OFF,
	GUI_ID_DEBUG_BOUNDING_BOX,
	GUI_ID_DEBUG_NORMALS,
	GUI_ID_DEBUG_SKELETON,
	GUI_ID_DEBUG_WIRE_OVERLAY,
	GUI_ID_DEBUG_HALF_TRANSPARENT,
	GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES,
	GUI_ID_DEBUG_ALL,

	GUI_ID_MODEL_MATERIAL_SOLID,
	GUI_ID_MODEL_MATERIAL_TRANSPARENT,
	GUI_ID_MODEL_MATERIAL_REFLECTION,

	GUI_ID_CAMERA_MAYA,
	GUI_ID_CAMERA_FIRST_PERSON,

	GUI_ID_POSITION_TEXT,

	GUI_ID_ABOUT,
	GUI_ID_QUIT,

	GUI_ID_TEXTUREFILTER,
	GUI_ID_SKIN_TRANSPARENCY,
	GUI_ID_SKIN_ANIMATION_FPS,

	GUI_ID_BUTTON_SET_SCALE,
	GUI_ID_BUTTON_SCALE_MUL10,
	GUI_ID_BUTTON_SCALE_DIV10,
	GUI_ID_BUTTON_OPEN_MODEL,
	GUI_ID_BUTTON_SHOW_ABOUT,
	GUI_ID_BUTTON_SHOW_TOOLBOX,
	GUI_ID_BUTTON_SELECT_ARCHIVE,

	GUI_ID_ANIMATION_INFO,

	// And some magic numbers
	MAX_FRAMERATE = 80,
	DEFAULT_FRAMERATE = 30
};


/*
  Update the display of the model scaling
*/
inline void my_updateScaleInfo(scene::ISceneNode* model)
{
    IGUIElement* toolboxWnd = g_Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
    if (!toolboxWnd)
        return;
    if (!model)
    {
        toolboxWnd->getElementFromId(GUI_ID_X_SCALE, true)->setText( L"-" );
        toolboxWnd->getElementFromId(GUI_ID_Y_SCALE, true)->setText( L"-" );
        toolboxWnd->getElementFromId(GUI_ID_Z_SCALE, true)->setText( L"-" );
    }
    else
    {
        core::vector3df scale = model->getScale();
        toolboxWnd->getElementFromId(GUI_ID_X_SCALE, true)->setText( core::stringw(scale.X).c_str() );
        toolboxWnd->getElementFromId(GUI_ID_Y_SCALE, true)->setText( core::stringw(scale.Y).c_str() );
        toolboxWnd->getElementFromId(GUI_ID_Z_SCALE, true)->setText( core::stringw(scale.Z).c_str() );
    }
}

/*
Function showAboutText() displays a messagebox with a caption and
a message text. The texts will be stored in the MessageText and Caption
variables at startup.
*/
inline void my_showAboutText()
{
    // create modal message box with the text
    // loaded from the xml file.
    g_Device->getGUIEnvironment()->addMessageBox(
        Caption.c_str(), MessageText.c_str());
}

inline void
my_onKillFocus()
{
    // Avoid that the FPS-camera continues moving when the user presses alt-tab while
    // moving the camera.
    auto const& animators = g_cameras[1]->getAnimators();

    auto it = animators.begin();
    while ( it != animators.end() )
    {
        if ( (*it)->getType() == irr::scene::ESNAT_CAMERA_FPS )
        {
            // we send a key-down event for all keys used by this animator
            auto fpsAnimator = static_cast<irr::scene::ISceneNodeAnimatorCameraFPS*>(*it);
            if (fpsAnimator)
            {
                irr::core::array<SKeyMap> const & keyMap = fpsAnimator->getKeyMap();
                for ( irr::u32 i=0; i< keyMap.size(); ++i )
                {
                    irr::SEvent event;
                    event.EventType = EET_KEY_INPUT_EVENT;
                    event.KeyInput.Key = keyMap[i].KeyCode;
                    event.KeyInput.PressedDown = false;
                    fpsAnimator->OnEvent(event);
                }
            }
        }
        ++it;
    }
}

/*
Function loadModel() loads a model and displays it using an
addAnimatedMeshSceneNode and the scene manager. Nothing difficult. It also
displays a short message box, if the model could not be loaded.
*/
inline void my_loadModel(const c8* fn)
{
	// modify the name if it a .pk3 file

	io::path filename(fn);

	io::path extension;
	core::getFileNameExtension(extension, filename);
	extension.make_lower();

	// if a texture is loaded apply it to the current model..
	if (extension == ".jpg" || extension == ".pcx" ||
		extension == ".png" || extension == ".ppm" ||
		extension == ".pgm" || extension == ".pbm" ||
		extension == ".psd" || extension == ".tga" ||
		extension == ".bmp" || extension == ".wal" ||
		extension == ".rgb" || extension == ".rgba")
	{
		video::ITexture * texture =
			g_Device->getVideoDriver()->getTexture( filename );
        if ( texture && g_model )
		{
			// always reload texture
			g_Device->getVideoDriver()->removeTexture(texture);
			texture = g_Device->getVideoDriver()->getTexture( filename );

            g_model->setMaterialTexture(0, texture);
		}
		return;
	}
	// if a archive is loaded add it to the FileArchive..
	else if (extension == ".pk3" || extension == ".zip" || extension == ".pak" || extension == ".npk")
	{
		g_Device->getFileSystem()->addFileArchive(filename.c_str());
		return;
	}

	// load a model into the engine

    if (g_model)
    {
        g_model->remove();
        g_model = nullptr;
    }

	if (extension==".irr")
	{
		core::array<scene::ISceneNode*> outNodes;
		g_Device->getSceneManager()->loadScene(filename);
		g_Device->getSceneManager()->getSceneNodesFromType(scene::ESNT_ANIMATED_MESH, outNodes);
		if (outNodes.size())
        {
            g_model = outNodes[0];
        }
		return;
	}

    auto m = g_Device->getSceneManager()->getMesh( filename.c_str() );
	if (!m)
	{
		// model could not be loaded

		if (StartUpModelFile != filename)
			g_Device->getGUIEnvironment()->addMessageBox(
			Caption.c_str(), L"The model could not be loaded. " \
			L"Maybe it is not a supported file format.");
		return;
	}

	// set default material properties

    if (g_bOctree)
        g_model = g_Device->getSceneManager()->addOctreeSceneNode(m->getMesh(0));
	else
	{
        auto animModel = g_Device->getSceneManager()->addAnimatedMeshSceneNode(m);
		animModel->setAnimationSpeed(30);
        g_model = animModel;
	}
    g_model->setMaterialFlag(video::EMF_LIGHTING, g_bUseLight);
    g_model->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, g_bUseLight);
//	Model->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
    g_model->setDebugDataVisible(scene::EDS_OFF);

	// we need to uncheck the menu entries. would be cool to fake a menu event, but
	// that's not so simple. so we do it brute force
	gui::IGUIContextMenu* menu = (gui::IGUIContextMenu*)g_Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_TOGGLE_DEBUG_INFO, true);
	if (menu)
		for(int item = 1; item < 6; ++item)
			menu->setItemChecked(item, false);
    my_updateScaleInfo(g_model);
}



/*
Function createToolBox() creates a toolbox window. In this simple mesh
viewer, this toolbox only contains a tab control with three edit boxes for
changing the scale of the displayed model.
*/
inline void my_createToolBox()
{
	// remove tool box if already there
	IGUIEnvironment* env = g_Device->getGUIEnvironment();
	IGUIElement* root = env->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
	if (e)
		e->remove();

	// create the toolbox window
	IGUIWindow* wnd = env->addWindow(core::rect<s32>(600,45,800,480),
		false, L"Toolset", 0, GUI_ID_DIALOG_ROOT_WINDOW);

	// create tab control and tabs
	IGUITabControl* tab = env->addTabControl(
		core::rect<s32>(2,20,800-602,480-7), wnd, true, true);

	IGUITab* t1 = tab->addTab(L"Config");

	// add some edit boxes and a button to tab one
	env->addStaticText(L"Scale:",
			core::rect<s32>(10,20,60,45), false, false, t1);
	env->addStaticText(L"X:", core::rect<s32>(22,48,40,66), false, false, t1);
	env->addEditBox(L"1.0", core::rect<s32>(40,46,130,66), true, t1, GUI_ID_X_SCALE);
	env->addStaticText(L"Y:", core::rect<s32>(22,82,40,96), false, false, t1);
	env->addEditBox(L"1.0", core::rect<s32>(40,76,130,96), true, t1, GUI_ID_Y_SCALE);
	env->addStaticText(L"Z:", core::rect<s32>(22,108,40,126), false, false, t1);
	env->addEditBox(L"1.0", core::rect<s32>(40,106,130,126), true, t1, GUI_ID_Z_SCALE);

	env->addButton(core::rect<s32>(10,134,85,165), t1, GUI_ID_BUTTON_SET_SCALE, L"Set");

	// quick scale buttons
	env->addButton(core::rect<s32>(65,20,95,40), t1, GUI_ID_BUTTON_SCALE_MUL10, L"* 10");
	env->addButton(core::rect<s32>(100,20,130,40), t1, GUI_ID_BUTTON_SCALE_DIV10, L"* 0.1");

    my_updateScaleInfo(g_model);

	// add transparency control
	env->addStaticText(L"GUI Transparency Control:",
			core::rect<s32>(10,200,150,225), true, false, t1);
	IGUIScrollBar* scrollbar = env->addScrollBar(true,
			core::rect<s32>(10,225,150,240), t1, GUI_ID_SKIN_TRANSPARENCY);
	scrollbar->setMax(255);
	scrollbar->setPos(255);

	// add framerate control
	env->addStaticText(L":", core::rect<s32>(10,240,150,265), true, false, t1);
	env->addStaticText(L"Framerate:",
			core::rect<s32>(12,240,75,265), false, false, t1);
	// current frame info
	env->addStaticText(L"", core::rect<s32>(75,240,200,265), false, false, t1,
			GUI_ID_ANIMATION_INFO);
	scrollbar = env->addScrollBar(true,
			core::rect<s32>(10,265,150,280), t1, GUI_ID_SKIN_ANIMATION_FPS);
	scrollbar->setMax(MAX_FRAMERATE);
	scrollbar->setMin(-MAX_FRAMERATE);
	scrollbar->setPos(DEFAULT_FRAMERATE);
	scrollbar->setSmallStep(1);
}
