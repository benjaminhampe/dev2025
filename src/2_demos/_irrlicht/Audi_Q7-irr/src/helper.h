#pragma once
#include <irrlicht.h>
#include "driverChoice.h"
#include <sstream>
#include <irrExt/irrExt.h>

inline irr::core::recti
mk_recti( irr::s32 x, irr::s32 y, irr::s32 w, irr::s32 h )
{
    irr::core::position2d<irr::s32> p(x,y);
    irr::core::dimension2d<irr::s32> d(w,h);
    return irr::core::recti(p,p+d);
}

inline irr::video::ITexture*
loadTexture( irr::video::IVideoDriver* driver, std::string const & uri )
{
    return driver->getTexture( uri.c_str() );
}

inline irr::gui::IGUIFont*
loadFont( irr::gui::IGUIEnvironment* env,
          std::string const & uri,
          irr::u32 fontSize = 16, bool bAntiAlias = true, bool bTransparent = true)
{
    if (!env)
    {
        throw std::runtime_error("Got nullptr for env");
    }

    irr::gui::IGUIFont* font = env->getFont(uri.c_str());
    if (!font)
    {
        font = irr::gui::CGUITTFont::create(env,uri.c_str(),fontSize, bAntiAlias, bTransparent);
    }

    if (!font)
    {
        throw std::runtime_error(std::string("Can't load font ") + uri);
    }

    return font;
}

inline void
setSkinFont( irr::gui::IGUIEnvironment* env, irr::gui::IGUIFont* font)
{
    if (!env)
    {
        throw std::runtime_error("Got nullptr for env");
    }

    if (!font)
    {
        throw std::runtime_error("Got nullptr for font");
    }

    irr::gui::IGUISkin* skin = env->getSkin();
    if (!skin)
    {
        throw std::runtime_error("Got nullptr for skin");
    }

    skin->setFont(font);
}

inline void
setSkinTransparency( irr::gui::IGUIEnvironment* env, uint8_t alpha = 255 )
{
    for (irr::s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
        auto a_index = (irr::gui::EGUI_DEFAULT_COLOR)i;
        irr::video::SColor a_color = env->getSkin()->getColor(a_index);
        a_color.setAlpha(alpha);
        env->getSkin()->setColor(a_index, a_color);
    }
}

inline void
setActiveCamera(irr::scene::ISceneManager* smgr,
                irr::scene::ICameraSceneNode* newActive)
{
    if (!smgr) return;
    auto oldActive = smgr->getActiveCamera();
    if (oldActive) oldActive->setInputReceiverEnabled(false);
    if (newActive)
    {
        newActive->setInputReceiverEnabled(true);
        smgr->setActiveCamera(newActive);
    }
}

inline void
setTextureFilter( irr::scene::ISceneNode* model, irr::s32 filter )
{
    if (!model) return;
    switch (filter)
    {
        case 0: {   model->setMaterialFlag(irr::video::EMF_BILINEAR_FILTER, false);
                    model->setMaterialFlag(irr::video::EMF_TRILINEAR_FILTER, false);
                    model->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, false);  } break;
        case 1: {   model->setMaterialFlag(irr::video::EMF_BILINEAR_FILTER, true);
                    model->setMaterialFlag(irr::video::EMF_TRILINEAR_FILTER, false);    } break;
        case 2: {   model->setMaterialFlag(irr::video::EMF_BILINEAR_FILTER, false);
                    model->setMaterialFlag(irr::video::EMF_TRILINEAR_FILTER, true);     } break;
        case 3: {   model->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, true);   } break;
        case 4: {   model->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, false);  } break;
        default: break;
    }
}

/*
Function hasModalDialog() checks if we currently have a modal dialog open.
*/
inline bool
hasModalDialog( irr::IrrlichtDevice* device )
{
    if (!device) return false;
    irr::gui::IGUIElement* focusedElement = device->getGUIEnvironment()->getFocus();
    while (focusedElement)
    {
        if (focusedElement->isVisible() &&
            focusedElement->hasType(irr::gui::EGUIET_MODAL_SCREEN) )
        {
            return true;
        }
        focusedElement = focusedElement->getParent();
    }
    return false;
}

#if 0
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
    updateScaleInfo(g_model);
}
#endif
