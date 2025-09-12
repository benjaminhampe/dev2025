add_subdirectory(midi-probe)
add_subdirectory(midi-in)
add_subdirectory(midi-out)

if (BENNI_USE_QT GREATER 0)
add_subdirectory(midi-keyboard-GS-qt)
add_subdirectory(midi-keyboard-cc-GS-qt)
endif()