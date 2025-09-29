set(LIB_NAME de_juce_dsp)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_dsp)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_dsp)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_dsp.h
      ${LIB_HEADER_DIR}/juce_dsp_config.h
      ${LIB_HEADER_DIR}/containers/juce_AudioBlock.h
      ${LIB_HEADER_DIR}/containers/juce_FixedSizeFunction.h
      ${LIB_HEADER_DIR}/containers/juce_SIMDRegister.h
      ${LIB_HEADER_DIR}/containers/juce_SIMDRegister_Impl.h
      ${LIB_HEADER_DIR}/filter_design/juce_FilterDesign.h
      ${LIB_HEADER_DIR}/frequency/juce_Convolution.h
      ${LIB_HEADER_DIR}/frequency/juce_FFT.h
      ${LIB_HEADER_DIR}/frequency/juce_Windowing.h
      ${LIB_HEADER_DIR}/maths/juce_FastMathApproximations.h
      ${LIB_HEADER_DIR}/maths/juce_LogRampedValue.h
      ${LIB_HEADER_DIR}/maths/juce_LookupTable.h
      ${LIB_HEADER_DIR}/maths/juce_Matrix.h
      ${LIB_HEADER_DIR}/maths/juce_Phase.h
      ${LIB_HEADER_DIR}/maths/juce_Polynomial.h
      ${LIB_HEADER_DIR}/maths/juce_SpecialFunctions.h
      # ${LIB_HEADER_DIR}/native/juce_avx_SIMDNativeOps.h
      ${LIB_HEADER_DIR}/native/juce_fallback_SIMDNativeOps.h
      # ${LIB_HEADER_DIR}/native/juce_neon_SIMDNativeOps.h
      ${LIB_HEADER_DIR}/native/juce_sse_SIMDNativeOps.h
      ${LIB_HEADER_DIR}/processors/juce_BallisticsFilter.h
      ${LIB_HEADER_DIR}/processors/juce_DelayLine.h
      ${LIB_HEADER_DIR}/processors/juce_DryWetMixer.h
      ${LIB_HEADER_DIR}/processors/juce_FIRFilter.h
      ${LIB_HEADER_DIR}/processors/juce_FirstOrderTPTFilter.h
      ${LIB_HEADER_DIR}/processors/juce_IIRFilter.h
      ${LIB_HEADER_DIR}/processors/juce_IIRFilter_Impl.h
      ${LIB_HEADER_DIR}/processors/juce_LinkwitzRileyFilter.h
      ${LIB_HEADER_DIR}/processors/juce_Oversampling.h
      ${LIB_HEADER_DIR}/processors/juce_Panner.h
      ${LIB_HEADER_DIR}/processors/juce_ProcessContext.h
      ${LIB_HEADER_DIR}/processors/juce_ProcessorChain.h
      ${LIB_HEADER_DIR}/processors/juce_ProcessorDuplicator.h
      ${LIB_HEADER_DIR}/processors/juce_ProcessorWrapper.h
      ${LIB_HEADER_DIR}/processors/juce_StateVariableFilter.h
      ${LIB_HEADER_DIR}/processors/juce_StateVariableTPTFilter.h
      ${LIB_HEADER_DIR}/widgets/juce_Bias.h
      ${LIB_HEADER_DIR}/widgets/juce_Chorus.h
      ${LIB_HEADER_DIR}/widgets/juce_Compressor.h
      ${LIB_HEADER_DIR}/widgets/juce_Gain.h
      ${LIB_HEADER_DIR}/widgets/juce_LadderFilter.h
      ${LIB_HEADER_DIR}/widgets/juce_Limiter.h
      ${LIB_HEADER_DIR}/widgets/juce_NoiseGate.h
      ${LIB_HEADER_DIR}/widgets/juce_Oscillator.h
      ${LIB_HEADER_DIR}/widgets/juce_Phaser.h
      ${LIB_HEADER_DIR}/widgets/juce_Reverb.h
      ${LIB_HEADER_DIR}/widgets/juce_WaveShaper.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_dsp.cpp
      ${LIB_SOURCE_DIR}/containers/juce_AudioBlock_test.cpp
      ${LIB_SOURCE_DIR}/containers/juce_FixedSizeFunction_test.cpp
      ${LIB_SOURCE_DIR}/containers/juce_SIMDRegister_test.cpp
      ${LIB_SOURCE_DIR}/filter_design/juce_FilterDesign.cpp
      ${LIB_SOURCE_DIR}/frequency/juce_Convolution.cpp
      ${LIB_SOURCE_DIR}/frequency/juce_Convolution_test.cpp
      ${LIB_SOURCE_DIR}/frequency/juce_FFT.cpp
      ${LIB_SOURCE_DIR}/frequency/juce_FFT_test.cpp
      ${LIB_SOURCE_DIR}/frequency/juce_Windowing.cpp
      ${LIB_SOURCE_DIR}/maths/juce_LogRampedValue_test.cpp
      ${LIB_SOURCE_DIR}/maths/juce_LookupTable.cpp
      ${LIB_SOURCE_DIR}/maths/juce_Matrix.cpp
      ${LIB_SOURCE_DIR}/maths/juce_Matrix_test.cpp
      ${LIB_SOURCE_DIR}/maths/juce_SpecialFunctions.cpp
      # ${LIB_SOURCE_DIR}/native/juce_avx_SIMDNativeOps.cpp
      # ${LIB_SOURCE_DIR}/native/juce_neon_SIMDNativeOps.cpp
      ${LIB_SOURCE_DIR}/native/juce_sse_SIMDNativeOps.cpp
      ${LIB_SOURCE_DIR}/processors/juce_BallisticsFilter.cpp
      ${LIB_SOURCE_DIR}/processors/juce_DelayLine.cpp
      ${LIB_SOURCE_DIR}/processors/juce_DryWetMixer.cpp
      ${LIB_SOURCE_DIR}/processors/juce_FIRFilter.cpp
      ${LIB_SOURCE_DIR}/processors/juce_FIRFilter_test.cpp
      ${LIB_SOURCE_DIR}/processors/juce_FirstOrderTPTFilter.cpp
      ${LIB_SOURCE_DIR}/processors/juce_IIRFilter.cpp
      ${LIB_SOURCE_DIR}/processors/juce_LinkwitzRileyFilter.cpp
      ${LIB_SOURCE_DIR}/processors/juce_Oversampling.cpp
      ${LIB_SOURCE_DIR}/processors/juce_Panner.cpp
      ${LIB_SOURCE_DIR}/processors/juce_ProcessorChain_test.cpp
      ${LIB_SOURCE_DIR}/processors/juce_StateVariableTPTFilter.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_Chorus.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_Compressor.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_LadderFilter.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_Limiter.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_NoiseGate.cpp
      ${LIB_SOURCE_DIR}/widgets/juce_Phaser.cpp
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_basics)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_formats)
   # target_link_libraries(${LIB_NAME} PRIVATE de_fftw3) # double version
   target_link_libraries(${LIB_NAME} PRIVATE de_fftw3f) # float version

endif()

# add_subdirectory(test)

