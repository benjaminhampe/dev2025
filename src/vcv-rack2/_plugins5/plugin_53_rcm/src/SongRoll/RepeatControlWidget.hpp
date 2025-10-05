#include "rack.hpp"

using namespace rack;

namespace SongRoll {

  struct SongRollData;

  class RepeatControlWidget : public Widget {
  public:
    int repeats=1;
    int repeats_complete=0;
    int repeat_mode=1;

    RepeatControlWidget();

    void draw(const DrawArgs& args) override;
    void onButton(const ButtonEvent& e) override;
  };

}
