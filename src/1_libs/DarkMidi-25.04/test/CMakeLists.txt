add_subdirectory(convertMidi2_DebugHtml)
add_subdirectory(convertMidi2_ImageHtml)

if (BENNI_USE_QT)
	add_subdirectory(MidiPlayer-GS-qt)
else()
	message("${Yellow}[benni] Midi Qt Apps are deactivated, enable with BENNI_USE_QT in root recipe!${ColourReset}")
endif()

