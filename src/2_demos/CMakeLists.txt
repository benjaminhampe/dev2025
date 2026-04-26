add_subdirectory(_binary2cpp)

if (BENNI_USE_IRR)
	add_subdirectory(_irrlicht)
else()
	message("${Yellow}[benni] Irrlicht Apps are deactivated, enable with BENNI_USE_IRR in root recipe!${ColourReset}")
endif()

if (BENNI_USE_QT)
	add_subdirectory(_qt)
else()
	message("${Yellow}[benni] Qt Apps are deactivated, enable with BENNI_USE_QT in root recipe!${ColourReset}")
endif()

# add_subdirectory(_raylib)

if (BENNI_USE_OPENGL_TUTORIALS)
	# add_subdirectory(_tutorials_AntonsOpenGL_glew)
	add_subdirectory(_tutorials_LearnOpenGL_glad)
	add_subdirectory(_tutorials_NorbertsTutorials_glew) # TODO: repair
else()
	message("${Yellow}[benni] OpenGL tutorials are deactivated, enable with BENNI_USE_OPENGL_TUTORIALS in root recipe!${ColourReset}")
endif()

if (BENNI_USE_WX)
	add_subdirectory(_wx)
else()
	message("${Yellow}[benni] wxWidgets Apps are deactivated, enable with BENNI_USE_WX in root recipe!${ColourReset}")
endif()

add_subdirectory(ActiveWallpaper)
add_subdirectory(AVX_Mandelbrot_glfw_glad)
add_subdirectory(AVX_Mandelbrot_de_glew) # TODO: repair
add_subdirectory(AzimutProjection_glew_wgl)
add_subdirectory(CNC_Parser)
add_subdirectory(F3_FreeLancer)
add_subdirectory(OpenGLAppDemo)
# add_subdirectory(PlanetarySystem3D_OpenGL)

add_subdirectory(Raytracing_glfw_glew)
add_subdirectory(ShaderArt_wgl_glew)
add_subdirectory(SonnenSystem_glfw_glew)

# add_subdirectory(win32_paint)
