#include "BaseWidget.hpp"
#include "dsp/window.hpp"

BaseWidget::BaseWidget() : ModuleWidget() {
  box.size.y = RACK_GRID_HEIGHT;
}

static const float COLOURDRAG_SENSITIVITY = 0.0015f;

void ColourChangeWidget::onButton(const event::Button &e) {
  Widget::onButton(e);
  e.stopPropagating();
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && (APP->window->getMods() & GLFW_MOD_SHIFT)) {
    // Consume if not consumed by child
    if (!e.isConsumed())
      e.consume(this);
  }
}

void ColourChangeWidget::onDragStart(const event::DragStart& e) {
  if ((APP->window->getMods() & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
    dragging = true;
    APP->window->cursorLock();
  }

  Widget::onDragStart(e);
}

void ColourChangeWidget::onDragMove(const event::DragMove& e) {
  if (dragging) {
    float speed = 1.f;
    float range = 1.f;

    float delta = COLOURDRAG_SENSITIVITY * e.mouseDelta.y * speed * range;
    if (APP->window->getMods() & GLFW_MOD_CONTROL) {
      delta /= 16.f;
    }

    if (colourData) {
      colourData->backgroundHue = clamp(colourData->backgroundHue + delta, 0.f, 1.f);
      colourData->dirty = true;
    }
  } else {
    Widget::onDragMove(e);
  }
}

void ColourChangeWidget::onDragEnd(const event::DragEnd& e) {
  if (dragging) {
    dragging = false;
    APP->window->cursorUnlock();
  }

  Widget::onDragEnd(e);
}

json_t *BaseModule::dataToJson() {
  json_t *rootJ = Module::dataToJson();
  if (rootJ == NULL) {
      rootJ = json_object();
  }

  json_object_set_new(rootJ, "backgroundHue", json_real(colourData.backgroundHue));
  json_object_set_new(rootJ, "backgroundSaturation", json_real(colourData.backgroundSaturation));
  json_object_set_new(rootJ, "backgroundLuminosity", json_real(colourData.backgroundLuminosity));

  return rootJ;
}

void BaseModule::dataFromJson(json_t *rootJ) {
  Module::dataFromJson(rootJ);

  json_t *backgroundHueJ = json_object_get(rootJ, "backgroundHue");
  if (backgroundHueJ) {
    colourData.backgroundHue = json_real_value(backgroundHueJ);
    colourData.dirty = true;
  }

  json_t *backgroundSaturationJ = json_object_get(rootJ, "backgroundSaturation");
  if (backgroundSaturationJ) {
    colourData.backgroundSaturation = json_real_value(backgroundSaturationJ);
    colourData.dirty = true;
  }

  json_t *backgroundLuminosityJ = json_object_get(rootJ, "backgroundLuminosity");
  if (backgroundLuminosityJ) {
    colourData.backgroundLuminosity = json_real_value(backgroundLuminosityJ);
    colourData.dirty = true;
  }
}

void BaseWidget::initColourChange(Rect hotspot, BaseModule* baseModule, float hue, float saturation, float luminosity) {
  colourData.backgroundHue = hue;
  colourData.backgroundSaturation = saturation;
  colourData.backgroundLuminosity = luminosity;

  if (baseModule) {
    moduleColourData = &baseModule->colourData;

    if (!moduleColourData->dirty) {
      moduleColourData->backgroundHue = hue;
      moduleColourData->backgroundSaturation = saturation;
      moduleColourData->backgroundLuminosity = luminosity;
    }
  }

  auto colourChangeWidget = createWidget<ColourChangeWidget>(hotspot.pos);
  colourChangeWidget->box.size = hotspot.size;
  colourChangeWidget->colourData = moduleColourData;
  addChild(colourChangeWidget);
}

void BaseWidget::step() {
  if (moduleColourData && moduleColourData->dirty) {
    colourData.backgroundHue = moduleColourData->backgroundHue;
    colourData.backgroundLuminosity = moduleColourData->backgroundLuminosity;
    colourData.backgroundSaturation = moduleColourData->backgroundSaturation;
    moduleColourData->dirty = false;
  }

  ModuleWidget::step();
}

void BaseWidget::draw(const DrawArgs& args) {
  nvgBeginPath(args.vg);
  nvgFillColor(args.vg, nvgHSL(colourData.backgroundHue, colourData.backgroundSaturation, colourData.backgroundLuminosity));
  nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
  nvgFill(args.vg);

  ModuleWidget::draw(args);
}

