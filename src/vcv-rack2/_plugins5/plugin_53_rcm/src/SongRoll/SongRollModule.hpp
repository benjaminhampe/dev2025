#include "rack.hpp"

#include "SongRollData.hpp"
#include "Transport.hpp"

#include "../ValueChangeTrigger.hpp"

namespace SongRoll {

  struct SongRollModule : rack::Module {
    enum ParamIds {
      NUM_PARAMS
    };
    enum InputIds {
      NUM_INPUTS
    };
    enum OutputIds {
      NUM_OUTPUTS
    };
    enum LightIds {
      NUM_LIGHTS
    };

    rack::dsp::SchmittTrigger clockInputTrigger;
    rack::dsp::SchmittTrigger resetInputTrigger;
    rack::dsp::SchmittTrigger runInputTrigger;

    ValueChangeTrigger<bool> runInputActive;
    rack::dsp::RingBuffer<float, 16> clockBuffer;
    int clockDelay = 0;

    SongRollData songRollData;
    Transport transport;

    SongRollModule();

    void step() override;
    void onReset() override;

    json_t *dataToJson() override;
    void dataFromJson(json_t *rootJ) override;
  };

}
