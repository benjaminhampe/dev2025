### ========== DarkAssimp ==========
add_subdirectory(de-utf8cpp)
# add_subdirectory(de-minizip)
add_subdirectory(de-rapidjson)
# add_subdirectory(de-unzip)
add_subdirectory(de-poly2tri)    # geometry
add_subdirectory(de-clipper)     # geometry 2d
#add_subdirectory(de-clipper-6.4.2)     # geometry 2d
add_subdirectory(de-irrXML)
add_subdirectory(de-open3dgc)
add_subdirectory(de-openddlparser)
add_subdirectory(de-assimp-4.1.0)

### ========== DarkWindow ==========
# add_subdirectory(de-qtsdk) # not needed, use a benni qt app template recipe.
# add_subdirectory(de-fltk) # not tested
# add_subdirectory(de-wxWidgets) # not ready, so much source code
### ========== DarkVideo ==========
# add_subdirectory(de-ffmpeg-4.3.2-win64-gcc-shared)
# add_subdirectory(de-fftw3)                # Fourier Transform n-radix
# add_subdirectory(de-fltk-1.3.5)           # gui
# add_subdirectory(de-glfw-3.3.2)           # Like GLUT
# add_subdirectory(de-irrlicht-1.8.5)       # 3d engine, gui, input, joystick
# add_subdirectory(de-irrExt-1.8.5)         # benni addons utf8 + freetype rendering + glm
# add_subdirectory(de-wx-3.1.5)

### ========== DarkGeometry ==========
# add_subdirectory(de-boost_1_75_0-static)
# add_subdirectory(de-gmp-6.1.2-static)     # bigint
# add_subdirectory(de-mpfr-4.0.2-static)    # bigint
# add_subdirectory(de-CGAL-5.0.2)           # CSG constructive solid geometry
#add_subdirectory(de-concaveman)           # concaveman.h:212:43: error: call of overloaded 'make_unique<rtree<double, 2, 16, std::array<double, 2> >::TreeType>(rtree<double, 2, 16, std::array<double, 2> >::DataType&, const bounds_type&)' is ambiguous
# add_subdirectory(de-earcut)
# add_subdirectory(de-expat-2.2.9)          # XML lib
# add_subdirectory(de-cork)                 # CSG booleans, #include <mpir.h>? uses GMP?
# add_subdirectory(de-ginac)               # Ti89 CAS,  fatal error: cln/complex.h: No such file or directory
# add_subdirectory(de-sqlite3)            # Can i exclude the big file from Qt parsing? ( to make loading faster )

