add_subdirectory(libDarkCommonDialogs)
add_subdirectory(libDarkCore)
add_subdirectory(libDarkMidi)
add_subdirectory(convertMidi2_DebugHtml)

add_subdirectory(libDarkImage)
add_subdirectory(convertMidi2_ImageHtml)

if (BENNI_USE_QT)
	add_subdirectory(_qt)
else()
	message("${Yellow}[benni] Midi Qt Apps are deactivated, enable with BENNI_USE_QT in root recipe!${ColourReset}")
endif()

