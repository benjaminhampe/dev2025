#include "rack.hpp"

using namespace rack;

namespace SongRoll {

  struct SongRollData;

  class ClockDivControlWidget : public Widget {
  public:
    int clock_div=1;

    ClockDivControlWidget();

    void draw(const DrawArgs& args) override;
    // void onMouseDown(EventMouseDown& e) override;

    void onButton(const rack::event::Button& e) override;

  };

}
