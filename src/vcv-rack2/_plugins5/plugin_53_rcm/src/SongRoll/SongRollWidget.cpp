#include "../rcm_plugin.hpp"
// #include "window.hpp"
#include "../SongRoll/SongRollWidget.hpp"
#include "../SongRoll/SongRollModule.hpp"
#include "../SongRoll/DragModes.hpp"
#include "../SongRoll/RollArea.hpp"
#include "../Consts.hpp"

using namespace rack;
using namespace SongRoll;

//extern Plugin* plugin;

static const int NUM_CHANNELS = 8;


SongRollWidget::SongRollWidget(SongRollModule *module)
    : BaseWidget() //module)
{
  this->module = (SongRollModule*)module;

  colourHotZone = Rect(Vec(506, 10), Vec(85, 13));
	backgroundHue = 0.33f;
	backgroundSaturation = 1.f;
	backgroundLuminosity = 0.25f;

  setPanel(SVG::load(asset::plugin(pluginInstance, "res/SongRoll.svg")));

  // addInput(createPort<PJ301MPort>(Vec(50.114, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::CLOCK_INPUT));
  // addInput(createPort<PJ301MPort>(Vec(85.642, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::RESET_INPUT));
  // addInput(createPort<PJ301MPort>(Vec(121.170, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::PATTERN_INPUT));
  // addInput(createPort<PJ301MPort>(Vec(156.697, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::RUN_INPUT));
  // addInput(createPort<PJ301MPort>(Vec(192.224, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::RECORD_INPUT));

  // addInput(createPort<PJ301MPort>(Vec(421.394, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::VOCT_INPUT));
  // addInput(createPort<PJ301MPort>(Vec(456.921, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::GATE_INPUT));
  // addInput(createPort<PJ301MPort>(Vec(492.448, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::RETRIGGER_INPUT));
  // addInput(createPort<PJ301MPort>(Vec(527.976, 380.f-91-23.6), PortWidget::INPUT, module, SongRollModule::VELOCITY_INPUT));

  // addOutput(createPort<PJ301MPort>(Vec(50.114, 380.f-25.9-23.6), PortWidget::OUTPUT, module, SongRollModule::CLOCK_OUTPUT));
  // addOutput(createPort<PJ301MPort>(Vec(85.642, 380.f-25.9-23.6), PortWidget::OUTPUT, module, SongRollModule::RESET_OUTPUT));
  // addOutput(createPort<PJ301MPort>(Vec(121.170, 380.f-25.9-23.6), PortWidget::OUTPUT, module, SongRollModule::PATTERN_OUTPUT));
  // addOutput(createPort<PJ301MPort>(Vec(156.697, 380.f-25.9-23.6), PortWidget::OUTPUT, module, SongRollModule::RUN_OUTPUT));
  // addOutput(createPort<PJ301MPort>(Vec(192.224, 380.f-25.9-23.6), PortWidget::OUTPUT, module, SongRollModule::RECORD_OUTPUT));

  // addOutput(createPort<PJ301MPort>(Vec(421.394, 380.f-25.9-23.6), PortWidget::OUTPUT, module, SongRollModule::VOCT_OUTPUT));
  // addOutput(createPort<PJ301MPo  json_t *lowestDisplayNoteJ = json_object_get(rootJ, "lowestDisplayNote");

  // patternWidget->widget = this;
  // addChild(patternWidget);

  auto *rollArea = new RollArea(getRollArea(), module->songRollData);
  addChild(rollArea);
}

void SongRollWidget::appendContextMenu(Menu* menu) {

}

Rect SongRollWidget::getRollArea() {
  return Rect(Vec(16, 380-218-145), Vec(477, 217));
}

void SongRollWidget::drawBackgroundColour(NVGcontext* ctx) {
    nvgBeginPath(ctx);
    nvgFillColor(ctx, nvgHSL(backgroundHue, backgroundSaturation, backgroundLuminosity));
    nvgRect(ctx, 0, 0, box.size.x, box.size.y);
    nvgFill(ctx);
}

static int stepcount = 0;
void SongRollWidget::drawPatternEditors(NVGcontext* ctx) {
  //stepcount += 1;

  Rect rollArea = getRollArea();
  static const float PATTERN_AREA_HEIGHT = 1;
  static const float leftMargin = 0;
  // Rect patternArea(Vec(rollArea.pos.x, rollArea.pos.y + rollArea.size.y - (rollArea.size.y * PATTERN_AREA_HEIGHT)), Vec(rollArea.size.x, rollArea.size.y * PATTERN_AREA_HEIGHT));
  Rect patternArea(Vec(rollArea.pos.x + leftMargin, rollArea.pos.y + rollArea.size.y - (rollArea.size.y * PATTERN_AREA_HEIGHT)), Vec(rollArea.size.x - leftMargin, rollArea.size.y * PATTERN_AREA_HEIGHT));

  nvgBeginPath(ctx);
  nvgFillColor(ctx, nvgRGBA(1, 1, 1, 1));
  nvgRect(ctx, rollArea.pos.x, rollArea.pos.y, rollArea.size.x, rollArea.size.y);
  nvgFill(ctx);


  float channelWidth = patternArea.size.x / NUM_CHANNELS;

  nvgSave(ctx);
  nvgScissor(ctx, patternArea.pos.x, patternArea.pos.y, patternArea.size.x, patternArea.size.y);

  for(int i = 1; i < NUM_CHANNELS; i++) {
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, patternArea.pos.x + (channelWidth * i), patternArea.pos.y);
    nvgLineTo(ctx, patternArea.pos.x + (channelWidth * i), patternArea.pos.y + patternArea.size.y);
    nvgStrokeWidth(ctx, 1.f);
    nvgStrokeColor(ctx, NV_YELLOW_H);
    nvgStroke(ctx);
  }

  nvgRestore(ctx);
}

//void SongRollWidget::draw(NVGcontext* ctx)
void SongRollWidget::draw(const DrawArgs& args)
{
    drawBackgroundColour(args.vg);

    BaseWidget::draw(args);

    drawPatternEditors(args.vg);
}

struct ClickZone {
  Rect r;

};

//void std::vector<

//void SongRollWidget::onMouseDown(EventMouseDown& e)
void SongRollWidget::onButton(const ButtonEvent& e)
{
    if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // Vec pos = gRackWidget->lastMousePos.minus(box.pos);
        // Vec pos = APP->scene->rack->getMousePos().minus(box.pos);

        Vec nullVec;
        Rect repeatsMinus(nullVec, nullVec);
        Rect repeatsPlus(nullVec, nullVec);
        Rect modeFree(nullVec, nullVec);
        Rect modeRepeats(nullVec, nullVec);
        Rect modeLimit(nullVec, nullVec);
    }

    BaseWidget::onButton(e);
}

void SongRollWidget::onDragStart(const DragStartEvent& e)
{
    //Vec pos = gRackWidget->lastMousePos.minus(box.pos);
    //Vec pos = APP->scene->rack->getMousePos().minus(box.pos);
    BaseWidget::onDragStart(e);
}

// void SongRollWidget::onDragMove(EventDragMove& e) {
//   BaseWidget::onDragMove(e);
// }

// void SongRollWidget::onDragMove(EventDragMove& e) {
//   BaseWidget::onDragMove(e);
// }

// void SongRollWidget::onDragEnd(EventDragEnd& e) {
//   BaseWidget::onDragEnd(e);
// }

#if 1
json_t* SongRollWidget::toSongRollJson()
{
    json_t* rootJ = json_object();
    // Add your custom fields
    // json_object_set_new(rootJ, "loopStart", json_integer(loopStart));
    return rootJ;
}

void SongRollWidget::fromSongRollJson(json_t* rootJ)
{
    // Read your custom fields
    // json_t* loopStartJ = json_object_get(rootJ, "loopStart");
    // if (loopStartJ)
    //     loopStart = json_integer_value(loopStartJ);
}
/*
json_t* MyModuleWidget::toJson() {
    json_t* rootJ = ModuleWidget::toJson();

    // Serialize SongRollWidget
    if (songRollWidget) {
        json_object_set_new(rootJ, "songRoll", songRollWidget->toSongRollJson());
    }

    return rootJ;
}

void MyModuleWidget::fromJson(json_t* rootJ) {
    ModuleWidget::fromJson(rootJ);

    // Deserialize SongRollWidget
    json_t* songRollJ = json_object_get(rootJ, "songRoll");
    if (songRollJ && songRollWidget) {
        songRollWidget->fromSongRollJson(songRollJ);
    }
}
*/
#else

json_t *SongRollWidget::dataToJson() {
  json_t *rootJ = BaseWidget::dataToJson();
  if (rootJ == NULL) {
      rootJ = json_object();
  }

  return rootJ;
}

void SongRollWidget::dataFromJson(json_t *rootJ) {
  BaseWidget::dataFromJson(rootJ);

}
#endif

