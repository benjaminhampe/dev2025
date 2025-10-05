#include "rack.hpp"
#include "dsp/window.hpp"

#include "../rcm_plugin.hpp"
#include "PatternWidget.hpp"
#include "PianoRollWidget.hpp"
#include "PianoRollModule.hpp"
#include "MenuItems/CancelPasteItem.hpp"
#include "MenuItems/ClearNotesItem.hpp"
#include "MenuItems/ClockBufferItem.hpp"
#include "MenuItems/CopyMeasureItem.hpp"
#include "MenuItems/CopyPatternItem.hpp"
#include "MenuItems/NotesToShowItem.hpp"
#include "MenuItems/PasteMeasureItem.hpp"
#include "MenuItems/PastePatternItem.hpp"

using namespace rack;

extern Plugin* plugin;

// standalone module for the module browser
PianoRollModule browserModule;

PianoRollWidget::PianoRollWidget(PianoRollModule *module) {
  setModule(module);
  PianoRollModule *originalmodule = module;
  module = this->module = module == NULL ? &browserModule : module;
  setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PianoRoll.svg")));

  addInput(createInput<PJ301MPort>(Vec(50.114, 380.f-91-23.6), originalmodule, PianoRollModule::CLOCK_INPUT));
  addInput(createInput<PJ301MPort>(Vec(85.642, 380.f-91-23.6), originalmodule, PianoRollModule::RESET_INPUT));
  addInput(createInput<PJ301MPort>(Vec(121.170, 380.f-91-23.6), originalmodule, PianoRollModule::PATTERN_INPUT));
  addInput(createInput<PJ301MPort>(Vec(156.697, 380.f-91-23.6), originalmodule, PianoRollModule::RUN_INPUT));
  addInput(createInput<PJ301MPort>(Vec(192.224, 380.f-91-23.6), originalmodule, PianoRollModule::RECORD_INPUT));

  addInput(createInput<PJ301MPort>(Vec(421.394, 380.f-91-23.6), originalmodule, PianoRollModule::VOCT_INPUT));
  addInput(createInput<PJ301MPort>(Vec(456.921, 380.f-91-23.6), originalmodule, PianoRollModule::GATE_INPUT));
  addInput(createInput<PJ301MPort>(Vec(492.448, 380.f-91-23.6), originalmodule, PianoRollModule::RETRIGGER_INPUT));
  addInput(createInput<PJ301MPort>(Vec(527.976, 380.f-91-23.6), originalmodule, PianoRollModule::VELOCITY_INPUT));

  addOutput(createOutput<PJ301MPort>(Vec(50.114, 380.f-25.9-23.6), originalmodule, PianoRollModule::CLOCK_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(85.642, 380.f-25.9-23.6), originalmodule, PianoRollModule::RESET_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(121.170, 380.f-25.9-23.6), originalmodule, PianoRollModule::PATTERN_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(156.697, 380.f-25.9-23.6), originalmodule, PianoRollModule::RUN_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(192.224, 380.f-25.9-23.6), originalmodule, PianoRollModule::RECORD_OUTPUT));

  addOutput(createOutput<PJ301MPort>(Vec(421.394, 380.f-25.9-23.6), originalmodule, PianoRollModule::VOCT_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(456.921, 380.f-25.9-23.6), originalmodule, PianoRollModule::GATE_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(492.448, 380.f-25.9-23.6), originalmodule, PianoRollModule::RETRIGGER_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(527.976, 380.f-25.9-23.6), originalmodule, PianoRollModule::VELOCITY_OUTPUT));
  addOutput(createOutput<PJ301MPort>(Vec(563.503, 380.f-25.9-23.6), originalmodule, PianoRollModule::END_OF_PATTERN_OUTPUT));

  rollAreaWidget = new RollAreaWidget(&module->patternData, &module->transport, &module->auditioner);
  rollAreaWidget->box = getRollArea();
  addChild(rollAreaWidget);

  PatternWidget* patternWidget = createWidget<PatternWidget>(Vec(505.257, 380.f-224.259-125.586));
  patternWidget->module = module;
  patternWidget->widget = this;
  addChild(patternWidget);

  initColourChange(Rect(Vec(506, 10), Vec(85, 13)), module, 0.5f, 1.f, 0.25f);
}

void PianoRollWidget::appendContextMenu(Menu* menu) {

  menu->addChild(createMenuLabel(""));
  menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Copy / Paste"));

  CopyPatternItem *copyPatternItem = new CopyPatternItem();
  copyPatternItem->widget = this;
  copyPatternItem->module = module;
  copyPatternItem->text = "Copy Pattern";
  menu->addChild(copyPatternItem);

  CopyMeasureItem *copyMeasureItem = new CopyMeasureItem();
  copyMeasureItem->widget = this;
  copyMeasureItem->module = module;
  copyMeasureItem->text = "Copy Measure";
  menu->addChild(copyMeasureItem);

  switch(state) {
    case COPYREADY:
      break;
    case PATTERNLOADED:
      {
        PastePatternItem *pastePatternItem = new PastePatternItem();
        pastePatternItem->widget = this;
        pastePatternItem->module = module;
        pastePatternItem->text = "Paste Pattern";
        menu->addChild(pastePatternItem);
      }
      break;
    case MEASURELOADED:
      {
        PasteMeasureItem *pasteMeasureItem = new PasteMeasureItem();
        pasteMeasureItem->widget = this;
        pasteMeasureItem->module = module;
        pasteMeasureItem->text = "Paste Measure";
        menu->addChild(pasteMeasureItem);
      }
      break;
    default:
      state = COPYREADY;
      break;
  }

  menu->addChild(createMenuLabel(""));
    menu->addChild(new ClearNotesItem(this->module));

  menu->addChild(createMenuLabel(""));
  menu->addChild(createMenuLabel("Notes to Show"));
    menu->addChild(new NotesToShowItem(this, 12));
    menu->addChild(new NotesToShowItem(this, 18));
    menu->addChild(new NotesToShowItem(this, 24));
    menu->addChild(new NotesToShowItem(this, 36));
    menu->addChild(new NotesToShowItem(this, 48));
    menu->addChild(new NotesToShowItem(this, 60));
  menu->addChild(createMenuLabel(""));
  menu->addChild(createMenuLabel("Clock Delay (samples)"));
    menu->addChild(new ClockBufferItem(module, 0));
    menu->addChild(new ClockBufferItem(module, 1));
    menu->addChild(new ClockBufferItem(module, 2));
    menu->addChild(new ClockBufferItem(module, 3));
    menu->addChild(new ClockBufferItem(module, 4));
    menu->addChild(new ClockBufferItem(module, 5));
    menu->addChild(new ClockBufferItem(module, 10));
  menu->addChild(createMenuLabel(""));
		if (module) {
			menu->addChild(createBoolPtrMenuItem("Driver Mode", "", &((PianoRollModule*)module)->driverMode));
		}
}

Rect PianoRollWidget::getRollArea() {
  Rect roll;
  roll.pos.x = 15.f;
  roll.pos.y = 380-365.f;
  roll.size.x = 480.f;
  roll.size.y = 220.f;
  return roll;
}

void PianoRollWidget::step() {
  if (module && module->state.dirty) {
    this->rollAreaWidget->state.lowestDisplayNote = module->state.lowestDisplayNote;
    this->rollAreaWidget->state.notesToShow = module->state.notesToShow;
    this->rollAreaWidget->state.currentMeasure = module->state.currentMeasure;
    this->rollAreaWidget->state.dirty = true;
    module->state.dirty = false;
  } else if (module && this->rollAreaWidget->stateNeedsSaving) {
    module->state.lowestDisplayNote = this->rollAreaWidget->state.lowestDisplayNote;
    module->state.notesToShow = this->rollAreaWidget->state.notesToShow;
    module->state.currentMeasure = this->rollAreaWidget->state.currentMeasure;
    this->rollAreaWidget->stateNeedsSaving = false;
  }

  BaseWidget::step();
}

