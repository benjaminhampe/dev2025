set(APP_NAME box3d_samples)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/tiny_obj_loader.h
        ${APP_SOURCE_DIR}/earcut.h
        ${APP_SOURCE_DIR}/jsmn.h
        ${APP_SOURCE_DIR}/main.cpp
        ${APP_SOURCE_DIR}/mesh_loader.cpp
        ${APP_SOURCE_DIR}/mesh_loader.h
        ${APP_SOURCE_DIR}/sample.cpp
        ${APP_SOURCE_DIR}/sample.h
        ${APP_SOURCE_DIR}/sample_benchmark.cpp
        ${APP_SOURCE_DIR}/sample_bodies.cpp
        ${APP_SOURCE_DIR}/sample_character.cpp
        ${APP_SOURCE_DIR}/sample_collision.cpp
        ${APP_SOURCE_DIR}/sample_compound.cpp
        ${APP_SOURCE_DIR}/sample_continuous.cpp
        ${APP_SOURCE_DIR}/sample_determinism.cpp
        ${APP_SOURCE_DIR}/sample_events.cpp
        ${APP_SOURCE_DIR}/sample_geometry.cpp
        ${APP_SOURCE_DIR}/sample_issues.cpp
        ${APP_SOURCE_DIR}/sample_joint.cpp
        ${APP_SOURCE_DIR}/sample_manifold.cpp
        ${APP_SOURCE_DIR}/sample_mesh.cpp
        ${APP_SOURCE_DIR}/sample_ragdoll.cpp
        ${APP_SOURCE_DIR}/sample_replay.cpp
        ${APP_SOURCE_DIR}/sample_robustness.cpp
        ${APP_SOURCE_DIR}/sample_shapes.cpp
        ${APP_SOURCE_DIR}/sample_stacking.cpp
        ${APP_SOURCE_DIR}/sample_tree.cpp
        ${APP_SOURCE_DIR}/sample_world.cpp

        ${APP_SOURCE_DIR}/gfx/arena.c
        ${APP_SOURCE_DIR}/gfx/arena.h
        ${APP_SOURCE_DIR}/gfx/CMakeLists.txt
        ${APP_SOURCE_DIR}/gfx/debug_adapter.c
        ${APP_SOURCE_DIR}/gfx/debug_adapter.h
        ${APP_SOURCE_DIR}/gfx/debug_shapes.c
        ${APP_SOURCE_DIR}/gfx/debug_shapes.h
        ${APP_SOURCE_DIR}/gfx/draw.c
        ${APP_SOURCE_DIR}/gfx/draw.h
        ${APP_SOURCE_DIR}/gfx/draw_overlay.h
        ${APP_SOURCE_DIR}/gfx/edges.c
        ${APP_SOURCE_DIR}/gfx/edges.h
        ${APP_SOURCE_DIR}/gfx/geometry_registry.c
        ${APP_SOURCE_DIR}/gfx/geometry_registry.h
        ${APP_SOURCE_DIR}/gfx/gtao.c
        ${APP_SOURCE_DIR}/gfx/gtao.h
        ${APP_SOURCE_DIR}/gfx/highlight_mask.c
        ${APP_SOURCE_DIR}/gfx/highlight_mask.h
        ${APP_SOURCE_DIR}/gfx/highlight_outline.c
        ${APP_SOURCE_DIR}/gfx/highlight_outline.h
        ${APP_SOURCE_DIR}/gfx/highlight_target.c
        ${APP_SOURCE_DIR}/gfx/highlight_target.h
        ${APP_SOURCE_DIR}/gfx/ibl.c
        ${APP_SOURCE_DIR}/gfx/ibl.h
        ${APP_SOURCE_DIR}/gfx/keycodes.h
        ${APP_SOURCE_DIR}/gfx/overlay.c
        ${APP_SOURCE_DIR}/gfx/overlay.h
        ${APP_SOURCE_DIR}/gfx/projection.h
        ${APP_SOURCE_DIR}/gfx/qsort.h
        ${APP_SOURCE_DIR}/gfx/renderer.c
        ${APP_SOURCE_DIR}/gfx/renderer.h
        ${APP_SOURCE_DIR}/gfx/scene_target.c
        ${APP_SOURCE_DIR}/gfx/scene_target.h
        ${APP_SOURCE_DIR}/gfx/shadow.c
        ${APP_SOURCE_DIR}/gfx/shadow.h
        ${APP_SOURCE_DIR}/gfx/sky.c
        ${APP_SOURCE_DIR}/gfx/sky.h
        ${APP_SOURCE_DIR}/gfx/sokol_impl.c
        ${APP_SOURCE_DIR}/gfx/text.c
        ${APP_SOURCE_DIR}/gfx/text.h
        ${APP_SOURCE_DIR}/gfx/tone_map.c
        ${APP_SOURCE_DIR}/gfx/tone_map.h
        ${APP_SOURCE_DIR}/gfx/utility.h

        ${APP_SOURCE_DIR}/host/camera.cpp
        ${APP_SOURCE_DIR}/host/camera.h
        ${APP_SOURCE_DIR}/host/gui.cpp
        ${APP_SOURCE_DIR}/host/gui.h
        ${APP_SOURCE_DIR}/host/imgui_impl.cpp
        ${APP_SOURCE_DIR}/host/sokol_app_impl.c

        ${APP_SOURCE_DIR}/shaders/generated/brdf_lut.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/capsule.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/cube.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/depth_only_capsule.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/depth_only_cube.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/depth_only_geom.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/depth_only_sphere.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/edge.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/geom.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/gtao_denoise.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/gtao_main_pass.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/gtao_prefilter_depth.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/highlight_outline.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/mask_capsule.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/mask_hull.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/mask_sphere.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/overlay_line.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/overlay_point.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/prefilter.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/shadow_caster_capsule.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/shadow_caster_cube.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/shadow_caster_geom.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/shadow_caster_sphere.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/sky.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/sky_to_cube.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/sphere.glsl.h
        ${APP_SOURCE_DIR}/shaders/generated/tonemap.glsl.h

        # ${APP_SOURCE_DIR}/shaders/common/pbr.glsl
        # ${APP_SOURCE_DIR}/shaders/common/preetham.glsl
        # ${APP_SOURCE_DIR}/shaders/common/xe_gtao_common.glsl

        # ${APP_SOURCE_DIR}/shaders/ibl/brdf_lut.glsl
        # ${APP_SOURCE_DIR}/shaders/ibl/prefilter.glsl
        # ${APP_SOURCE_DIR}/shaders/ibl/sky.glsl
        # ${APP_SOURCE_DIR}/shaders/ibl/sky_to_cube.glsl

        # ${APP_SOURCE_DIR}/shaders/overlays/line.glsl
        # ${APP_SOURCE_DIR}/shaders/overlays/point.glsl

        # ${APP_SOURCE_DIR}/shaders/post/depth_only_capsule.glsl
        # ${APP_SOURCE_DIR}/shaders/post/depth_only_cube.glsl
        # ${APP_SOURCE_DIR}/shaders/post/depth_only_geom.glsl
        # ${APP_SOURCE_DIR}/shaders/post/depth_only_sphere.glsl
        # ${APP_SOURCE_DIR}/shaders/post/gtao_denoise.glsl
        # ${APP_SOURCE_DIR}/shaders/post/gtao_main_pass.glsl
        # ${APP_SOURCE_DIR}/shaders/post/gtao_prefilter_depth.glsl
        # ${APP_SOURCE_DIR}/shaders/post/highlight_outline.glsl
        # ${APP_SOURCE_DIR}/shaders/post/mask_capsule.glsl
        # ${APP_SOURCE_DIR}/shaders/post/mask_hull.glsl
        # ${APP_SOURCE_DIR}/shaders/post/mask_sphere.glsl
        # ${APP_SOURCE_DIR}/shaders/post/tonemap.glsl

        # ${APP_SOURCE_DIR}/shaders/shapes/capsule.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/cube.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/edge.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/geom.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/shadow_caster_capsule.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/shadow_caster_cube.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/shadow_caster_geom.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/shadow_caster_sphere.glsl
        # ${APP_SOURCE_DIR}/shaders/shapes/sphere.glsl

        ${APP_SOURCE_DIR}/shared/benchmarks.c
        ${APP_SOURCE_DIR}/shared/benchmarks.h
        # ${APP_SOURCE_DIR}/shared/CMakeLists.txt
        ${APP_SOURCE_DIR}/shared/determinism.c
        ${APP_SOURCE_DIR}/shared/determinism.h
        ${APP_SOURCE_DIR}/shared/human.c
        ${APP_SOURCE_DIR}/shared/human.h
        ${APP_SOURCE_DIR}/shared/overflow_color.c
        ${APP_SOURCE_DIR}/shared/overflow_color.h
        ${APP_SOURCE_DIR}/shared/stability.c
        ${APP_SOURCE_DIR}/shared/stability.h
        ${APP_SOURCE_DIR}/shared/utils.c
        ${APP_SOURCE_DIR}/shared/utils.h
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
      # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/shared)

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/shaders/generated)

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${APP_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${APP_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

    target_link_libraries(${APP_NAME} PRIVATE de_box3d)
    target_link_libraries(${APP_NAME} PRIVATE de_sokol)
    target_link_libraries(${APP_NAME} PRIVATE de_implot_0_17)
    target_link_libraries(${APP_NAME} PRIVATE de_glfw)
    target_link_libraries(${APP_NAME} PRIVATE de_glew)
    target_link_libraries(${APP_NAME} PRIVATE de_nfd_130)
    # target_link_libraries(${APP_NAME} PRIVATE de_stb_image_write)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    # if (CMAKE_BUILD_TYPE STREQUAL "Release")
        # message(STATUS "Building in Release mode, strip symbols")
        # add_custom_command(TARGET ${APP_NAME} POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )
    # endif()

    file(COPY media/box3d_samples DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
