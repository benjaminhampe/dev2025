#include "rack.hpp"

using namespace rack;

namespace SongRoll {

  struct SongRollData;

  class PatternControlWidget : public Widget {
  public:
    int pattern=0;

    PatternControlWidget();

    void draw(const DrawArgs& args) override;
    void onButton(const ButtonEvent& e) override;
  };

}
