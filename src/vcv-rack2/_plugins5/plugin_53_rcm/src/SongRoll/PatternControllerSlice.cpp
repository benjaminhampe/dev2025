#include "../SongRoll/PatternControllerSlice.hpp"
#include "../SongRoll/PatternHeaderWidget.hpp"
#include "../SongRoll/PatternControlWidget.hpp"
#include "../SongRoll/RepeatControlWidget.hpp"
#include "../SongRoll/ClockDivControlWidget.hpp"
#include "../SongRoll/SongRollData.hpp"
#include "../Consts.hpp"

namespace SongRoll {

  PatternControllerSlice::PatternControllerSlice(int channel, SongRollData& data, int section) : channel(channel), data(data), section(section) {
    layout = createWidget<SequentialLayout>(Vec(0, 0));
    layout->orientation = SequentialLayout::VERTICAL_ORIENTATION;
    layout->alignment = SequentialLayout::LEFT_ALIGNMENT;
    layout->spacing = 10;

    header = new PatternHeaderWidget(8, 2, 1);
    layout->addChild(header);

    pattern = createWidget<PatternControlWidget>(math::Vec());
    layout->addChild(pattern);

    repeats = createWidget<RepeatControlWidget>(math::Vec());
    layout->addChild(repeats);

    clock_div = createWidget<ClockDivControlWidget>(math::Vec());
    layout->addChild(clock_div);

    addChild(layout);
  }



  void PatternControllerSlice::step() {
    if (sectionChanged) {
      sectionChanged = false;
      pattern->pattern = data.sections[section].channels[channel].pattern;
    } else {
      data.sections[section].channels[channel].pattern = pattern->pattern;
      data.sections[section].channels[channel].repeats = repeats->repeats;
      data.sections[section].channels[channel].repeat_mode = (ChannelConfig::eRepeatMode)repeats->repeat_mode;
      data.sections[section].channels[channel].clock_div = clock_div->clock_div;
    }

    header->pattern = data.sections[section].channels[channel].pattern;
    header->repeats = data.sections[section].channels[channel].repeats;
    header->active = data.sections[section].channels[channel].clock_div > 0;

    for(auto *widget : layout->children) {
      widget->box.size.x = box.size.x;
    }

    layout->box.size.y = box.size.y;

    Widget::step();
  }

  void PatternControllerSlice::draw(const DrawArgs& args) {

    Widget::draw(args);
    return;

  }

  void PatternControllerSlice::onButton(const ButtonEvent& e) 
  {
        Widget::onButton(e);
  }

}
