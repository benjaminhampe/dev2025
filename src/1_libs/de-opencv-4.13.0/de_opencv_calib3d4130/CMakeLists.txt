set(LIB_NAME de_opencv_calib3d4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/calib3d/calib3d.hpp
        ${LIB_HEADER_DIR}/opencv2/calib3d/calib3d_c.h
        ${LIB_HEADER_DIR}/opencv2/calib3d.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_calib3d.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_calib3d.hpp
        # ${LIB_SOURCE_DIR}/_benni/undistort.avx2.cpp
        ${LIB_SOURCE_DIR}/_benni/undistort.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/undistort.sse2.cpp
	
        ${LIB_SOURCE_DIR}/ap3p.cpp
        ${LIB_SOURCE_DIR}/ap3p.h
        ${LIB_SOURCE_DIR}/calibinit.cpp
        ${LIB_SOURCE_DIR}/calibration.cpp
        ${LIB_SOURCE_DIR}/calibration_base.cpp
        ${LIB_SOURCE_DIR}/calibration_handeye.cpp
        ${LIB_SOURCE_DIR}/checkchessboard.cpp
        ${LIB_SOURCE_DIR}/chessboard.cpp
        ${LIB_SOURCE_DIR}/chessboard.hpp
        ${LIB_SOURCE_DIR}/circlesgrid.cpp
        ${LIB_SOURCE_DIR}/circlesgrid.hpp
        ${LIB_SOURCE_DIR}/compat_ptsetreg.cpp
        ${LIB_SOURCE_DIR}/distortion_model.hpp
        ${LIB_SOURCE_DIR}/dls.cpp
        ${LIB_SOURCE_DIR}/dls.h
        ${LIB_SOURCE_DIR}/epnp.cpp
        ${LIB_SOURCE_DIR}/epnp.h
        ${LIB_SOURCE_DIR}/fisheye.cpp
        ${LIB_SOURCE_DIR}/fisheye.hpp
        ${LIB_SOURCE_DIR}/five-point.cpp
        ${LIB_SOURCE_DIR}/fundam.cpp
        ${LIB_SOURCE_DIR}/hal_replacement.hpp
        ${LIB_SOURCE_DIR}/homography_decomp.cpp
        ${LIB_SOURCE_DIR}/ippe.cpp
        ${LIB_SOURCE_DIR}/ippe.hpp
        ${LIB_SOURCE_DIR}/levmarq.cpp
        ${LIB_SOURCE_DIR}/main.cpp

        ${LIB_SOURCE_DIR}/opencl/stereobm.cl
        ${LIB_SOURCE_DIR}/p3p.cpp
        ${LIB_SOURCE_DIR}/p3p.h
        ${LIB_SOURCE_DIR}/polynom_solver.cpp
        ${LIB_SOURCE_DIR}/polynom_solver.h
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/ptsetreg.cpp
        ${LIB_SOURCE_DIR}/quadsubpix.cpp
        ${LIB_SOURCE_DIR}/rho.cpp
        ${LIB_SOURCE_DIR}/rho.h
        ${LIB_SOURCE_DIR}/solvepnp.cpp
        ${LIB_SOURCE_DIR}/sqpnp.cpp
        ${LIB_SOURCE_DIR}/sqpnp.hpp
        ${LIB_SOURCE_DIR}/stereobm.cpp
        ${LIB_SOURCE_DIR}/stereosgbm.cpp
        ${LIB_SOURCE_DIR}/stereo_geom.cpp
        ${LIB_SOURCE_DIR}/triangulate.cpp
        ${LIB_SOURCE_DIR}/undistort.dispatch.cpp
        ${LIB_SOURCE_DIR}/undistort.simd.hpp
        ${LIB_SOURCE_DIR}/upnp.cpp
        ${LIB_SOURCE_DIR}/upnp.h

        ${LIB_SOURCE_DIR}/usac/bundle.cpp
        ${LIB_SOURCE_DIR}/usac/degeneracy.cpp
        ${LIB_SOURCE_DIR}/usac/dls_solver.cpp
        ${LIB_SOURCE_DIR}/usac/essential_solver.cpp
        ${LIB_SOURCE_DIR}/usac/estimator.cpp
        ${LIB_SOURCE_DIR}/usac/fundamental_solver.cpp
        ${LIB_SOURCE_DIR}/usac/gamma_values.cpp
        ${LIB_SOURCE_DIR}/usac/homography_solver.cpp
        ${LIB_SOURCE_DIR}/usac/local_optimization.cpp
        ${LIB_SOURCE_DIR}/usac/pnp_solver.cpp
        ${LIB_SOURCE_DIR}/usac/quality.cpp
        ${LIB_SOURCE_DIR}/usac/ransac_solvers.cpp
        ${LIB_SOURCE_DIR}/usac/sampler.cpp
        ${LIB_SOURCE_DIR}/usac/termination.cpp
        ${LIB_SOURCE_DIR}/usac/utils.cpp
        ${LIB_SOURCE_DIR}/usac.hpp
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/_benni)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __OPENCV_BUILD=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_core4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_features2d4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgproc4130)

endif()

# add_subdirectory(test)
