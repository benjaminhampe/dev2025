#include "MyGlobals.h"

IrrlichtDevice *Device = 0;
core::stringc StartUpModelFile;
core::stringw MessageText;
core::stringw Caption;
scene::ISceneNode* Model = 0;
scene::ISceneNode* SkyBox = 0;
bool Octree=false;
bool UseLight=false;
scene::ICameraSceneNode* Camera[2] = {0, 0};

void setActiveCamera(scene::ICameraSceneNode* newActive)
{
    if (0 == Device)
        return;

    scene::ICameraSceneNode * active = Device->getSceneManager()->getActiveCamera();
    active->setInputReceiverEnabled(false);

    newActive->setInputReceiverEnabled(true);
    Device->getSceneManager()->setActiveCamera(newActive);
}

/*
    Set the skin transparency by changing the alpha values of all skin-colors
*/
void setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin)
{
    for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
        video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
        col.setAlpha(alpha);
        skin->setColor((EGUI_DEFAULT_COLOR)i, col);
    }
}

/*
  Update the display of the model scaling
*/
void updateScaleInfo(scene::ISceneNode* model)
{
    IGUIElement* toolboxWnd = Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
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
void showAboutText()
{
    // create modal message box with the text
    // loaded from the xml file.
    Device->getGUIEnvironment()->addMessageBox(
        Caption.c_str(), MessageText.c_str());
}


/*
Function loadModel() loads a model and displays it using an
addAnimatedMeshSceneNode and the scene manager. Nothing difficult. It also
displays a short message box, if the model could not be loaded.
*/
void loadModel(const c8* fn)
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
            Device->getVideoDriver()->getTexture( filename );
        if ( texture && Model )
        {
            // always reload texture
            Device->getVideoDriver()->removeTexture(texture);
            texture = Device->getVideoDriver()->getTexture( filename );

            Model->setMaterialTexture(0, texture);
        }
        return;
    }
    // if a archive is loaded add it to the FileArchive..
    else if (extension == ".pk3" || extension == ".zip" || extension == ".pak" || extension == ".npk")
    {
        Device->getFileSystem()->addFileArchive(filename.c_str());
        return;
    }

    // load a model into the engine

    if (Model)
        Model->remove();

    Model = 0;

    if (extension==".irr")
    {
        core::array<scene::ISceneNode*> outNodes;
        Device->getSceneManager()->loadScene(filename);
        Device->getSceneManager()->getSceneNodesFromType(scene::ESNT_ANIMATED_MESH, outNodes);
        if (outNodes.size())
            Model = outNodes[0];
        return;
    }

    scene::IAnimatedMesh* m = Device->getSceneManager()->getMesh( filename.c_str() );

    if (!m)
    {
        // model could not be loaded

        if (StartUpModelFile != filename)
            Device->getGUIEnvironment()->addMessageBox(
            Caption.c_str(), L"The model could not be loaded. " \
            L"Maybe it is not a supported file format.");
        return;
    }

    // set default material properties

    if (Octree)
        Model = Device->getSceneManager()->addOctreeSceneNode(m->getMesh(0));
    else
    {
        scene::IAnimatedMeshSceneNode* animModel = Device->getSceneManager()->addAnimatedMeshSceneNode(m);
        animModel->setAnimationSpeed(30);
        Model = animModel;
    }
    Model->setMaterialFlag(video::EMF_LIGHTING, UseLight);
    Model->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, UseLight);
//	Model->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
    Model->setDebugDataVisible(scene::EDS_OFF);

    // we need to uncheck the menu entries. would be cool to fake a menu event, but
    // that's not so simple. so we do it brute force
    gui::IGUIContextMenu* menu = (gui::IGUIContextMenu*)Device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_TOGGLE_DEBUG_INFO, true);
    if (menu)
        for(int item = 1; item < 6; ++item)
            menu->setItemChecked(item, false);
    updateScaleInfo(Model);
}


/*
Function createToolBox() creates a toolbox window. In this simple mesh
viewer, this toolbox only contains a tab control with three edit boxes for
changing the scale of the displayed model.
*/
void createToolBox()
{
    // remove tool box if already there
    IGUIEnvironment* env = Device->getGUIEnvironment();
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

    updateScaleInfo(Model);

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

/*
Function updateToolBox() is called each frame to update dynamic information in
the toolbox.
*/
void updateToolBox()
{
    IGUIEnvironment* env = Device->getGUIEnvironment();
    IGUIElement* root = env->getRootGUIElement();
    IGUIElement* dlg = root->getElementFromId(GUI_ID_DIALOG_ROOT_WINDOW, true);
    if (!dlg )
        return;

    // update the info we have about the animation of the model
    IGUIStaticText *  aniInfo = (IGUIStaticText *)(dlg->getElementFromId(GUI_ID_ANIMATION_INFO, true));
    if (aniInfo)
    {
        if ( Model && scene::ESNT_ANIMATED_MESH == Model->getType() )
        {
            scene::IAnimatedMeshSceneNode* animatedModel = (scene::IAnimatedMeshSceneNode*)Model;

            core::stringw str( (s32)core::round_(animatedModel->getAnimationSpeed()) );
            str += L" Frame: ";
            str += core::stringw((s32)animatedModel->getFrameNr());
            aniInfo->setText(str.c_str());
        }
        else
            aniInfo->setText(L"");
    }
}

void onKillFocus()
{
    // Avoid that the FPS-camera continues moving when the user presses alt-tab while
    // moving the camera.
    const core::list<scene::ISceneNodeAnimator*>& animators = Camera[1]->getAnimators();
    core::list<irr::scene::ISceneNodeAnimator*>::ConstIterator iter = animators.begin();
    while ( iter != animators.end() )
    {
        if ( (*iter)->getType() == scene::ESNAT_CAMERA_FPS )
        {
            // we send a key-down event for all keys used by this animator
            scene::ISceneNodeAnimatorCameraFPS * fpsAnimator = static_cast<scene::ISceneNodeAnimatorCameraFPS*>(*iter);
            const core::array<SKeyMap>& keyMap = fpsAnimator->getKeyMap();
            for ( irr::u32 i=0; i< keyMap.size(); ++i )
            {
                irr::SEvent event;
                event.EventType = EET_KEY_INPUT_EVENT;
                event.KeyInput.Key = keyMap[i].KeyCode;
                event.KeyInput.PressedDown = false;
                fpsAnimator->OnEvent(event);
            }
        }
        ++iter;
    }
}

/*
Function hasModalDialog() checks if we currently have a modal dialog open.
*/
bool hasModalDialog()
{
    if ( !Device )
        return false;
    IGUIEnvironment* env = Device->getGUIEnvironment();
    IGUIElement * focused = env->getFocus();
    while ( focused )
    {
        if ( focused->isVisible() && focused->hasType(EGUIET_MODAL_SCREEN) )
            return true;
        focused = focused->getParent();
    }
    return false;
}
