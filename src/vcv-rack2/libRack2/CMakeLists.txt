set(LIB_NAME de_rack2)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/arch.hpp
        ${LIB_HEADER_DIR}/asset.hpp
        ${LIB_HEADER_DIR}/audio.hpp
        ${LIB_HEADER_DIR}/color.hpp
        ${LIB_HEADER_DIR}/common.hpp
        ${LIB_HEADER_DIR}/componentlibrary.hpp
        ${LIB_HEADER_DIR}/context.hpp
        ${LIB_HEADER_DIR}/gamepad.hpp
        ${LIB_HEADER_DIR}/helpers.hpp
        ${LIB_HEADER_DIR}/history.hpp
        ${LIB_HEADER_DIR}/keyboard.hpp
        ${LIB_HEADER_DIR}/library.hpp
        ${LIB_HEADER_DIR}/logger.hpp
        ${LIB_HEADER_DIR}/math.hpp
        ${LIB_HEADER_DIR}/midi.hpp
        ${LIB_HEADER_DIR}/midiloopback.hpp
        ${LIB_HEADER_DIR}/mutex.hpp
        ${LIB_HEADER_DIR}/network.hpp
        ${LIB_HEADER_DIR}/patch.hpp
        ${LIB_HEADER_DIR}/plugin.hpp
        ${LIB_HEADER_DIR}/Quantity.hpp
        ${LIB_HEADER_DIR}/rack.hpp
        ${LIB_HEADER_DIR}/rack0.hpp
        ${LIB_HEADER_DIR}/random.hpp
        ${LIB_HEADER_DIR}/rtaudio.hpp
        ${LIB_HEADER_DIR}/rtmidi.hpp
        ${LIB_HEADER_DIR}/settings.hpp
        ${LIB_HEADER_DIR}/string.hpp
        ${LIB_HEADER_DIR}/system.hpp
        ${LIB_HEADER_DIR}/tag.hpp
        ${LIB_HEADER_DIR}/weakptr.hpp
        
        ${LIB_HEADER_DIR}/app/AudioDisplay.hpp
        ${LIB_HEADER_DIR}/app/Browser.hpp
        ${LIB_HEADER_DIR}/app/CableWidget.hpp
        ${LIB_HEADER_DIR}/app/CircularShadow.hpp
        ${LIB_HEADER_DIR}/app/common.hpp
        ${LIB_HEADER_DIR}/app/Knob.hpp
        ${LIB_HEADER_DIR}/app/LedDisplay.hpp
        ${LIB_HEADER_DIR}/app/LightWidget.hpp
        ${LIB_HEADER_DIR}/app/MenuBar.hpp
        ${LIB_HEADER_DIR}/app/MidiDisplay.hpp
        ${LIB_HEADER_DIR}/app/ModuleLightWidget.hpp
        ${LIB_HEADER_DIR}/app/ModuleWidget.hpp
        ${LIB_HEADER_DIR}/app/MultiLightWidget.hpp
        ${LIB_HEADER_DIR}/app/ParamWidget.hpp
        ${LIB_HEADER_DIR}/app/PortWidget.hpp
        ${LIB_HEADER_DIR}/app/RackScrollWidget.hpp
        ${LIB_HEADER_DIR}/app/RackWidget.hpp
        ${LIB_HEADER_DIR}/app/RailWidget.hpp
        ${LIB_HEADER_DIR}/app/Scene.hpp
        ${LIB_HEADER_DIR}/app/SliderKnob.hpp
        ${LIB_HEADER_DIR}/app/SvgButton.hpp
        ${LIB_HEADER_DIR}/app/SvgKnob.hpp
        ${LIB_HEADER_DIR}/app/SvgPanel.hpp
        ${LIB_HEADER_DIR}/app/SvgPort.hpp
        ${LIB_HEADER_DIR}/app/SvgScrew.hpp
        ${LIB_HEADER_DIR}/app/SvgSlider.hpp
        ${LIB_HEADER_DIR}/app/SvgSwitch.hpp
        ${LIB_HEADER_DIR}/app/Switch.hpp
        ${LIB_HEADER_DIR}/app/TipWindow.hpp

        ${LIB_HEADER_DIR}/dsp/approx.hpp
        ${LIB_HEADER_DIR}/dsp/common.hpp
        ${LIB_HEADER_DIR}/dsp/convert.hpp
        ${LIB_HEADER_DIR}/dsp/digital.hpp
        ${LIB_HEADER_DIR}/dsp/fft.hpp
        ${LIB_HEADER_DIR}/dsp/filter.hpp
        ${LIB_HEADER_DIR}/dsp/fir.hpp
        ${LIB_HEADER_DIR}/dsp/midi.hpp
        ${LIB_HEADER_DIR}/dsp/minblep.hpp
        ${LIB_HEADER_DIR}/dsp/ode.hpp
        ${LIB_HEADER_DIR}/dsp/resampler.hpp
        ${LIB_HEADER_DIR}/dsp/ringbuffer.hpp
        ${LIB_HEADER_DIR}/dsp/vumeter.hpp
        ${LIB_HEADER_DIR}/dsp/window.hpp

        ${LIB_HEADER_DIR}/engine/Cable.hpp
        ${LIB_HEADER_DIR}/engine/Engine.hpp
        ${LIB_HEADER_DIR}/engine/Light.hpp
        ${LIB_HEADER_DIR}/engine/LightInfo.hpp
        ${LIB_HEADER_DIR}/engine/Module.hpp
        ${LIB_HEADER_DIR}/engine/Param.hpp
        ${LIB_HEADER_DIR}/engine/ParamHandle.hpp
        ${LIB_HEADER_DIR}/engine/ParamQuantity.hpp
        ${LIB_HEADER_DIR}/engine/Port.hpp
        ${LIB_HEADER_DIR}/engine/PortInfo.hpp

        ${LIB_HEADER_DIR}/plugin/callbacks.hpp
        ${LIB_HEADER_DIR}/plugin/Model.hpp
        ${LIB_HEADER_DIR}/plugin/Plugin.hpp

        ${LIB_HEADER_DIR}/simd/common.hpp
        ${LIB_HEADER_DIR}/simd/functions.hpp
        ${LIB_HEADER_DIR}/simd/sse_mathfun.h
        ${LIB_HEADER_DIR}/simd/sse_mathfun_extension.h
        ${LIB_HEADER_DIR}/simd/Vector.hpp

        ${LIB_HEADER_DIR}/ui/Button.hpp
        ${LIB_HEADER_DIR}/ui/ChoiceButton.hpp
        ${LIB_HEADER_DIR}/ui/common.hpp
        ${LIB_HEADER_DIR}/ui/Label.hpp
        ${LIB_HEADER_DIR}/ui/List.hpp
        ${LIB_HEADER_DIR}/ui/Menu.hpp
        ${LIB_HEADER_DIR}/ui/MenuEntry.hpp
        ${LIB_HEADER_DIR}/ui/MenuItem.hpp
        ${LIB_HEADER_DIR}/ui/MenuLabel.hpp
        ${LIB_HEADER_DIR}/ui/MenuOverlay.hpp
        ${LIB_HEADER_DIR}/ui/MenuSeparator.hpp
        ${LIB_HEADER_DIR}/ui/OptionButton.hpp
        ${LIB_HEADER_DIR}/ui/ProgressBar.hpp
        ${LIB_HEADER_DIR}/ui/RadioButton.hpp
        ${LIB_HEADER_DIR}/ui/Scrollbar.hpp
        ${LIB_HEADER_DIR}/ui/ScrollWidget.hpp
        ${LIB_HEADER_DIR}/ui/SequentialLayout.hpp
        ${LIB_HEADER_DIR}/ui/Slider.hpp
        ${LIB_HEADER_DIR}/ui/TextField.hpp
        ${LIB_HEADER_DIR}/ui/Tooltip.hpp
        ${LIB_HEADER_DIR}/ui/TooltipOverlay.hpp

        ${LIB_HEADER_DIR}/widget/event.hpp
        ${LIB_HEADER_DIR}/widget/FramebufferWidget.hpp
        ${LIB_HEADER_DIR}/widget/OpaqueWidget.hpp
        ${LIB_HEADER_DIR}/widget/OpenGlWidget.hpp
        ${LIB_HEADER_DIR}/widget/SvgWidget.hpp
        ${LIB_HEADER_DIR}/widget/TransformWidget.hpp
        ${LIB_HEADER_DIR}/widget/TransparentWidget.hpp
        ${LIB_HEADER_DIR}/widget/Widget.hpp
        ${LIB_HEADER_DIR}/widget/ZoomWidget.hpp

        ${LIB_HEADER_DIR}/window/Svg.hpp
        ${LIB_HEADER_DIR}/window/Window.hpp
    )
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/asset.cpp
        # ${LIB_SOURCE_DIR}/asset.mm
        ${LIB_SOURCE_DIR}/audio.cpp
        ${LIB_SOURCE_DIR}/color.cpp
        ${LIB_SOURCE_DIR}/common.cpp
        ${LIB_SOURCE_DIR}/context.cpp
        ${LIB_SOURCE_DIR}/dep.cpp
        ${LIB_SOURCE_DIR}/gamepad.cpp
        ${LIB_SOURCE_DIR}/history.cpp
        ${LIB_SOURCE_DIR}/keyboard.cpp
        ${LIB_SOURCE_DIR}/library.cpp
        ${LIB_SOURCE_DIR}/logger.cpp
        ${LIB_SOURCE_DIR}/midi.cpp
        ${LIB_SOURCE_DIR}/midiloopback.cpp
        ${LIB_SOURCE_DIR}/network.cpp
        ${LIB_SOURCE_DIR}/patch.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/Quantity.cpp
        ${LIB_SOURCE_DIR}/random.cpp
        ${LIB_SOURCE_DIR}/rtaudio.cpp
        # ${LIB_SOURCE_DIR}/rtaudio.mm
        ${LIB_SOURCE_DIR}/rtmidi.cpp
        ${LIB_SOURCE_DIR}/settings.cpp
        ${LIB_SOURCE_DIR}/string.cpp
        ${LIB_SOURCE_DIR}/system.cpp
        ${LIB_SOURCE_DIR}/tag.cpp

        ${LIB_SOURCE_DIR}/app/AudioDisplay.cpp
        ${LIB_SOURCE_DIR}/app/Browser.cpp
        ${LIB_SOURCE_DIR}/app/CableWidget.cpp
        ${LIB_SOURCE_DIR}/app/CircularShadow.cpp
        ${LIB_SOURCE_DIR}/app/Knob.cpp
        ${LIB_SOURCE_DIR}/app/LedDisplay.cpp
        ${LIB_SOURCE_DIR}/app/LightWidget.cpp
        ${LIB_SOURCE_DIR}/app/MenuBar.cpp
        ${LIB_SOURCE_DIR}/app/MidiDisplay.cpp
        ${LIB_SOURCE_DIR}/app/ModuleLightWidget.cpp
        ${LIB_SOURCE_DIR}/app/ModuleWidget.cpp
        ${LIB_SOURCE_DIR}/app/MultiLightWidget.cpp
        ${LIB_SOURCE_DIR}/app/ParamWidget.cpp
        ${LIB_SOURCE_DIR}/app/PortWidget.cpp
        ${LIB_SOURCE_DIR}/app/RackScrollWidget.cpp
        ${LIB_SOURCE_DIR}/app/RackWidget.cpp
        ${LIB_SOURCE_DIR}/app/RailWidget.cpp
        ${LIB_SOURCE_DIR}/app/Scene.cpp
        ${LIB_SOURCE_DIR}/app/SliderKnob.cpp
        ${LIB_SOURCE_DIR}/app/SvgButton.cpp
        ${LIB_SOURCE_DIR}/app/SvgKnob.cpp
        ${LIB_SOURCE_DIR}/app/SvgPanel.cpp
        ${LIB_SOURCE_DIR}/app/SvgPort.cpp
        ${LIB_SOURCE_DIR}/app/SvgScrew.cpp
        ${LIB_SOURCE_DIR}/app/SvgSlider.cpp
        ${LIB_SOURCE_DIR}/app/SvgSwitch.cpp
        ${LIB_SOURCE_DIR}/app/Switch.cpp
        ${LIB_SOURCE_DIR}/app/TipWindow.cpp

        ${LIB_SOURCE_DIR}/core/Audio.cpp
        ${LIB_SOURCE_DIR}/core/Blank.cpp
        ${LIB_SOURCE_DIR}/core/CV_MIDI.cpp
        ${LIB_SOURCE_DIR}/core/CV_MIDICC.cpp
        ${LIB_SOURCE_DIR}/core/Gate_MIDI.cpp
        ${LIB_SOURCE_DIR}/core/MIDICC_CV.cpp
        ${LIB_SOURCE_DIR}/core/MIDIMap.cpp
        ${LIB_SOURCE_DIR}/core/MIDI_CV.cpp
        ${LIB_SOURCE_DIR}/core/MIDI_Gate.cpp
        ${LIB_SOURCE_DIR}/core/Notes.cpp
        ${LIB_SOURCE_DIR}/core/plugin.cpp
        ${LIB_SOURCE_DIR}/core/plugin.hpp

        ${LIB_SOURCE_DIR}/dsp/minblep.cpp

        ${LIB_SOURCE_DIR}/engine/Cable.cpp
        ${LIB_SOURCE_DIR}/engine/Engine.cpp
        ${LIB_SOURCE_DIR}/engine/LightInfo.cpp
        ${LIB_SOURCE_DIR}/engine/Module.cpp
        ${LIB_SOURCE_DIR}/engine/ParamQuantity.cpp
        ${LIB_SOURCE_DIR}/engine/PortInfo.cpp

        ${LIB_SOURCE_DIR}/plugin/Model.cpp
        ${LIB_SOURCE_DIR}/plugin/Plugin.cpp

        ${LIB_SOURCE_DIR}/ui/Button.cpp
        ${LIB_SOURCE_DIR}/ui/ChoiceButton.cpp
        ${LIB_SOURCE_DIR}/ui/common.cpp
        ${LIB_SOURCE_DIR}/ui/Label.cpp
        ${LIB_SOURCE_DIR}/ui/List.cpp
        ${LIB_SOURCE_DIR}/ui/Menu.cpp
        ${LIB_SOURCE_DIR}/ui/MenuEntry.cpp
        ${LIB_SOURCE_DIR}/ui/MenuItem.cpp
        ${LIB_SOURCE_DIR}/ui/MenuLabel.cpp
        ${LIB_SOURCE_DIR}/ui/MenuOverlay.cpp
        ${LIB_SOURCE_DIR}/ui/MenuSeparator.cpp
        ${LIB_SOURCE_DIR}/ui/OptionButton.cpp
        ${LIB_SOURCE_DIR}/ui/ProgressBar.cpp
        ${LIB_SOURCE_DIR}/ui/RadioButton.cpp
        ${LIB_SOURCE_DIR}/ui/Scrollbar.cpp
        ${LIB_SOURCE_DIR}/ui/ScrollWidget.cpp
        ${LIB_SOURCE_DIR}/ui/SequentialLayout.cpp
        ${LIB_SOURCE_DIR}/ui/Slider.cpp
        ${LIB_SOURCE_DIR}/ui/TextField.cpp
        ${LIB_SOURCE_DIR}/ui/Tooltip.cpp

        ${LIB_SOURCE_DIR}/widget/event.cpp
        ${LIB_SOURCE_DIR}/widget/FramebufferWidget.cpp
        ${LIB_SOURCE_DIR}/widget/OpenGlWidget.cpp
        ${LIB_SOURCE_DIR}/widget/SvgWidget.cpp
        ${LIB_SOURCE_DIR}/widget/Widget.cpp
        ${LIB_SOURCE_DIR}/widget/ZoomWidget.cpp

        ${LIB_SOURCE_DIR}/window/Svg.cpp
        ${LIB_SOURCE_DIR}/window/Window.cpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")	
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
	target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LIBRACK2=1)
	
    target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_zlib)
    target_link_libraries(${LIB_NAME} PRIVATE de_archive)
    target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    target_link_libraries(${LIB_NAME} PUBLIC de_glfw)
    target_link_libraries(${LIB_NAME} PUBLIC de_jansson_json)
    target_link_libraries(${LIB_NAME} PRIVATE de_ghc_filesystem)
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2_fuzzysearchdatabase)
    target_link_libraries(${LIB_NAME} PUBLIC de_nanosvg)
    target_link_libraries(${LIB_NAME} PUBLIC de_nanovg_tint)
    target_link_libraries(${LIB_NAME} PUBLIC de_osdialog)
    target_link_libraries(${LIB_NAME} PUBLIC de_oui_blendish)
    target_link_libraries(${LIB_NAME} PUBLIC de_pffft)
#    target_link_libraries(${LIB_NAME} PRIVATE de_rack2_rtaudio)
    target_link_libraries(${LIB_NAME} PRIVATE de_rtaudio_601)
    target_link_libraries(${LIB_NAME} PRIVATE de_rtmidi)
    target_link_libraries(${LIB_NAME} PUBLIC de_rack2_speexdsp)
    target_link_libraries(${LIB_NAME} PRIVATE de_tinyexpr)
    target_link_libraries(${LIB_NAME} PRIVATE de_stb_image)
    target_link_libraries(${LIB_NAME} PRIVATE de_stb_image_write)
    target_link_libraries(${LIB_NAME} PRIVATE de_openssl_static)
    target_link_libraries(${LIB_NAME} PRIVATE de_curl_shared)
    
    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        target_link_options(${LIB_NAME} PRIVATE -mwindows)

        if (WIN32)
            target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        endif()
    endif()
    
    
    # target_link_libraries(${LIB_NAME} PUBLIC de_rack2_nanovg)
    

    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
