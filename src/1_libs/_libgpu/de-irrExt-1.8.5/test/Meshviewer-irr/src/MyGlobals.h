#include <irrlicht.h>
#include <irrExt/irrExt.h>
#include "driverChoice.h"

using namespace irr;
using namespace gui;

extern IrrlichtDevice *Device;
extern core::stringc StartUpModelFile;
extern core::stringw MessageText;
extern core::stringw Caption;
extern scene::ISceneNode* Model;
extern scene::ISceneNode* SkyBox;
extern bool Octree;
extern bool UseLight;
extern scene::ICameraSceneNode* Camera[2];

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

void setActiveCamera(scene::ICameraSceneNode* newActive);

/*
    Set the skin transparency by changing the alpha values of all skin-colors
*/
void setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin);

/*
  Update the display of the model scaling
*/
void updateScaleInfo(scene::ISceneNode* model);

/*
Function showAboutText() displays a messagebox with a caption and
a message text. The texts will be stored in the MessageText and Caption
variables at startup.
*/
void showAboutText();

/*
Function loadModel() loads a model and displays it using an
addAnimatedMeshSceneNode and the scene manager. Nothing difficult. It also
displays a short message box, if the model could not be loaded.
*/
void loadModel(const c8* fn);

/*
Function createToolBox() creates a toolbox window. In this simple mesh
viewer, this toolbox only contains a tab control with three edit boxes for
changing the scale of the displayed model.
*/
void createToolBox();

/*
Function updateToolBox() is called each frame to update dynamic information in
the toolbox.
*/
void updateToolBox();

// Avoid that the FPS-camera continues moving when the user presses alt-tab while
// moving the camera.
void onKillFocus();

/*
Function hasModalDialog() checks if we currently have a modal dialog open.
*/
bool hasModalDialog();
