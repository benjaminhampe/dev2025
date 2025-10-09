  # PUBLIC
  # fmt
  # luajit-5.1
# add_subdirectory(de-luajit-5.1)
  # samplerate
  # surge::airwindows
add_subdirectory(de-airwindows)
  # surge::eurorack
#add_subdirectory(de-eurorack)

add_subdirectory(de-taocpp-pegtl)
add_subdirectory(de-sst-strnatcmp)
add_subdirectory(de-sst-cpputils)
add_subdirectory(de-sst-plugininfra)
add_subdirectory(de-sst-filters)
# add_subdirectory(de-sst-filters-extras) # Merged to de-sst-filters
add_subdirectory(de-sst-waveshapers)
add_subdirectory(de-sst-basic-blocks)
add_subdirectory(de-sst-effects)
add_subdirectory(de-sst-rackhelpers)

  # surge::oddsound-mts
  # surge::sqlite
  # surge-common-binary
  # tuning-library
  # PRIVATE
  # surge-lua-src
  # surge-platform
  # surge-juce
  # taocpp::pegtl
  # PRIVATE
  # juce::juce_dsp

add_subdirectory(surge-common)