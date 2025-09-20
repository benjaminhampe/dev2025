add_subdirectory(minimal_vst2x_host)
add_subdirectory(AVX_Mandelbrot_glfw_glad)
add_subdirectory(AzimutProjection_glew_wgl)
add_subdirectory(Raytracing_glfw_glew)
add_subdirectory(CNC_Parser)
add_subdirectory(_tutorials_AntonsOpenGL_glew)
add_subdirectory(_tutorials_LearnOpenGL_glad)
add_subdirectory(_tutorials_NorbertsTutorials_glew) # TODO: repair
add_subdirectory(AVX_Mandelbrot_de_glew) # TODO: repair
add_subdirectory(_irrlicht)
add_subdirectory(ShaderArt_wgl_glew)

if (BENNI_USE_QT)
	add_subdirectory(_qt)
else()
	message("${Yellow}[benni] Qt Apps are deactivated, enable with BENNI_USE_QT in root recipe!${ColourReset}")
endif()

# add_subdirectory(IrrlichtWidget2021-wx)
# add_subdirectory(HexEditor-v0.24-wx)

#add_subdirectory(demo_fltk)

# add_subdirectory(CommandLine-qt)
# add_subdirectory(file2header)
# add_subdirectory(IrrlichtWidget2019-qt)



# add_subdirectory(wgl_glew_example)


# add_subdirectory(Midi)
# #add_subdirectory(AudioSynth-qt)
# #add_subdirectory(AudioSynth-qt)
# add_subdirectory(VSTi_Osci3D)
# add_subdirectory(VST24_Oscilloskop_wgl)


# add_subdirectory(VSTi_fltk)


# add_subdirectory(win32_paint)
