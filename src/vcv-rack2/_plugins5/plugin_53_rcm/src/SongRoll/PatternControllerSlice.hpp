#include "rack.hpp"
using namespace rack;

namespace SongRoll {

  struct SongRollData;
  struct PatternHeaderWidget;
  struct PatternControlWidget;
  struct RepeatControlWidget;
  struct ClockDivControlWidget;

  class PatternControllerSlice : public Widget {
  public:
    const int channel;
    SongRollData& data;
    SequentialLayout* layout;
    PatternHeaderWidget* header;
    PatternControlWidget* pattern;
    RepeatControlWidget* repeats;
    ClockDivControlWidget* clock_div;

    PatternControllerSlice(int channel, SongRollData& data, int section);
    void draw(const DrawArgs& args) override;
    void step() override;
    void onButton(const ButtonEvent& e) override;
    void setSection(int section);

  private:
      int section = 0;
      bool sectionChanged = true;
  };

}
