set(PLG_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(PLG_SOURCES
    ${PLG_SOURCE_DIR}/JucePluginConfig.h
    ${PLG_SOURCE_DIR}/JuceHeader.h
    ${PLG_SOURCE_DIR}/PluginEditor.cpp
    ${PLG_SOURCE_DIR}/PluginEditor.h
    ${PLG_SOURCE_DIR}/PluginProcessor.cpp
    ${PLG_SOURCE_DIR}/PluginProcessor.h

    ${PLG_SOURCE_DIR}/Engine/AdsrEnvelope.h
    ${PLG_SOURCE_DIR}/Engine/APInterpolator.h
    ${PLG_SOURCE_DIR}/Engine/AudioUtils.h
    ${PLG_SOURCE_DIR}/Engine/BlepData.h
    ${PLG_SOURCE_DIR}/Engine/Decimator.h
    ${PLG_SOURCE_DIR}/Engine/DelayLine.h
    ${PLG_SOURCE_DIR}/Engine/Filter.h
    ${PLG_SOURCE_DIR}/Engine/Lfo.h
    ${PLG_SOURCE_DIR}/Engine/midiMap.h
    ${PLG_SOURCE_DIR}/Engine/Motherboard.h
    ${PLG_SOURCE_DIR}/Engine/ObxdBank.h
    ${PLG_SOURCE_DIR}/Engine/ObxdOscillatorB.h
    ${PLG_SOURCE_DIR}/Engine/ObxdVoice.h
    ${PLG_SOURCE_DIR}/Engine/Params.h
    ${PLG_SOURCE_DIR}/Engine/ParamsEnum.h
    ${PLG_SOURCE_DIR}/Engine/ParamSmoother.h
    ${PLG_SOURCE_DIR}/Engine/PulseOsc.h
    ${PLG_SOURCE_DIR}/Engine/SawOsc.h
    ${PLG_SOURCE_DIR}/Engine/SynthEngine.h
    ${PLG_SOURCE_DIR}/Engine/TriangleOsc.h
    ${PLG_SOURCE_DIR}/Engine/VoiceQueue.h

    ${PLG_SOURCE_DIR}/Gui/ButtonList.h
    ${PLG_SOURCE_DIR}/Gui/Knob.h
    ${PLG_SOURCE_DIR}/Gui/res.cpp
    ${PLG_SOURCE_DIR}/Gui/res.h
    ${PLG_SOURCE_DIR}/Gui/TooglableButton.h
)