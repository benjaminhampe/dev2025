#include <tuple>
#include <limits>

#include "rack.hpp"
#include "../BaseWidget.hpp"

using namespace rack;


namespace SongRoll {

  struct ModuleDragType;
  struct SongRollModule;

  struct SongRollWidget : BaseWidget {
    SongRollModule* module;
    Rect colourHotZone;
    float backgroundHue;
    float backgroundSaturation;
    float backgroundLuminosity;

    SongRollWidget(SongRollModule *module);

    Rect getRollArea();

    void drawBackgroundColour(NVGcontext* ctx);
    void drawPatternEditors(NVGcontext* ctx);

    // Event Handlers

    void appendContextMenu(Menu* menu) override;
    // void draw(NVGcontext* ctx) override;
    void draw(const DrawArgs& args) override;

    //void onHover(const HoverEvent& e) override;
    //void onHoverKey(const HoverKeyEvent& e) override;
    void onButton(const ButtonEvent& e) override;
    void onDragStart(const DragStartEvent& e) override;
    // void onDragEnd(const DragEndEvent& e) override;
    // void onDragMove(const DragMoveEvent& e) override;
    // void onDragHover(const DragHoverEvent& e) override;

    // void onMouseDown(EventMouseDown& e) override;
    // void onDragStart(EventDragStart& e) override;
    // void baseDragMove(EventDragMove& e);
    // void onDragMove(EventDragMove& e) override;
    // void onDragEnd(EventDragEnd& e) override;
    json_t* toSongRollJson();
    void fromSongRollJson(json_t* rootJ);

    // json_t *dataToJson() override;
    // void dataFromJson(json_t *rootJ) override;

  };
}
