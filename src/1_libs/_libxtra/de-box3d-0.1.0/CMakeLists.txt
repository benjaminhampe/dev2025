set(LIB_NAME de_box3d)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/box3d/base.h
        ${LIB_HEADER_DIR}/box3d/box3d.h
        ${LIB_HEADER_DIR}/box3d/collision.h
        ${LIB_HEADER_DIR}/box3d/config.h
        ${LIB_HEADER_DIR}/box3d/constants.h
        ${LIB_HEADER_DIR}/box3d/id.h
        ${LIB_HEADER_DIR}/box3d/math_functions.h
        ${LIB_HEADER_DIR}/box3d/types.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/aabb.c
        ${LIB_SOURCE_DIR}/aabb.h
        ${LIB_SOURCE_DIR}/algorithm.h
        ${LIB_SOURCE_DIR}/arena_allocator.c
        ${LIB_SOURCE_DIR}/arena_allocator.h
        ${LIB_SOURCE_DIR}/bitset.c
        ${LIB_SOURCE_DIR}/bitset.h
        ${LIB_SOURCE_DIR}/block_allocator.c
        ${LIB_SOURCE_DIR}/block_allocator.h
        ${LIB_SOURCE_DIR}/body.c
        ${LIB_SOURCE_DIR}/body.h
        ${LIB_SOURCE_DIR}/box3d.natvis
        ${LIB_SOURCE_DIR}/broad_phase.c
        ${LIB_SOURCE_DIR}/broad_phase.h
        ${LIB_SOURCE_DIR}/capsule.c
        # ${LIB_SOURCE_DIR}/CMakeLists.txt
        ${LIB_SOURCE_DIR}/compound.c
        ${LIB_SOURCE_DIR}/compound.h
        ${LIB_SOURCE_DIR}/constraint_graph.c
        ${LIB_SOURCE_DIR}/constraint_graph.h
        ${LIB_SOURCE_DIR}/contact.c
        ${LIB_SOURCE_DIR}/contact.h
        ${LIB_SOURCE_DIR}/contact_solver.c
        ${LIB_SOURCE_DIR}/contact_solver.h
        ${LIB_SOURCE_DIR}/container.h
        ${LIB_SOURCE_DIR}/convex_manifold.c
        ${LIB_SOURCE_DIR}/core.c
        ${LIB_SOURCE_DIR}/core.h
        ${LIB_SOURCE_DIR}/ctz.h
        ${LIB_SOURCE_DIR}/distance.c
        ${LIB_SOURCE_DIR}/distance_joint.c
        ${LIB_SOURCE_DIR}/dynamic_tree.c
        ${LIB_SOURCE_DIR}/height_field.c
        ${LIB_SOURCE_DIR}/hull.c
        ${LIB_SOURCE_DIR}/hull_map.h
        ${LIB_SOURCE_DIR}/id_pool.c
        ${LIB_SOURCE_DIR}/id_pool.h
        ${LIB_SOURCE_DIR}/island.c
        ${LIB_SOURCE_DIR}/island.h
        ${LIB_SOURCE_DIR}/joint.c
        ${LIB_SOURCE_DIR}/joint.h
        ${LIB_SOURCE_DIR}/manifold.c
        ${LIB_SOURCE_DIR}/manifold.h
        ${LIB_SOURCE_DIR}/math_functions.c
        ${LIB_SOURCE_DIR}/math_internal.h
        ${LIB_SOURCE_DIR}/mesh.c
        ${LIB_SOURCE_DIR}/mesh_contact.c
        ${LIB_SOURCE_DIR}/motor_joint.c
        ${LIB_SOURCE_DIR}/mover.c
        ${LIB_SOURCE_DIR}/parallel_for.c
        ${LIB_SOURCE_DIR}/parallel_for.h
        ${LIB_SOURCE_DIR}/parallel_joint.c
        ${LIB_SOURCE_DIR}/physics_world.c
        ${LIB_SOURCE_DIR}/physics_world.h
        ${LIB_SOURCE_DIR}/platform.h
        ${LIB_SOURCE_DIR}/prismatic_joint.c
        ${LIB_SOURCE_DIR}/qsort.h
        ${LIB_SOURCE_DIR}/recording.c
        ${LIB_SOURCE_DIR}/recording.h
        ${LIB_SOURCE_DIR}/recording_ops.inl
        ${LIB_SOURCE_DIR}/recording_replay.c
        ${LIB_SOURCE_DIR}/recording_replay.h
        ${LIB_SOURCE_DIR}/revolute_joint.c
        ${LIB_SOURCE_DIR}/scheduler.c
        ${LIB_SOURCE_DIR}/scheduler.h
        ${LIB_SOURCE_DIR}/sensor.c
        ${LIB_SOURCE_DIR}/sensor.h
        ${LIB_SOURCE_DIR}/shape.c
        ${LIB_SOURCE_DIR}/shape.h
        ${LIB_SOURCE_DIR}/simd.c
        ${LIB_SOURCE_DIR}/simd.h
        ${LIB_SOURCE_DIR}/solver.c
        ${LIB_SOURCE_DIR}/solver.h
        ${LIB_SOURCE_DIR}/solver_set.c
        ${LIB_SOURCE_DIR}/solver_set.h
        ${LIB_SOURCE_DIR}/sphere.c
        ${LIB_SOURCE_DIR}/spherical_joint.c
        ${LIB_SOURCE_DIR}/table.c
        ${LIB_SOURCE_DIR}/table.h
        ${LIB_SOURCE_DIR}/timer.c
        ${LIB_SOURCE_DIR}/triangle_manifold.c
        ${LIB_SOURCE_DIR}/types.c
        ${LIB_SOURCE_DIR}/verstable.h
        ${LIB_SOURCE_DIR}/weld_joint.c
        ${LIB_SOURCE_DIR}/wheel_joint.c
        ${LIB_SOURCE_DIR}/world_snapshot.c
        ${LIB_SOURCE_DIR}/world_snapshot.h
    )

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    # target_compile_definitions(${LIB_NAME} PUBLIC USINGZ=0)
    target_compile_definitions(${LIB_NAME} PUBLIC BOX3D_VALIDATE=0)
    target_compile_definitions(${LIB_NAME} PUBLIC BOX3D_DISABLE_SIMD=8)
    target_compile_definitions(${LIB_NAME} PUBLIC BOX3D_DOUBLE_PRECISION=1)

# set(CMAKE_C_VISIBILITY_PRESET hidden)
# set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# if (BOX3D_PROFILE)
    # target_compile_definitions(box3d PRIVATE BOX3D_PROFILE)
    # set(TRACY_DELAYED_INIT ON CACHE BOOL "Enable delayed init for Tracy" FORCE)
    # set(TRACY_MANUAL_LIFETIME ON CACHE BOOL "Enable manual lifetime for Tracy" FORCE)

    # FetchContent_Declare(
        # tracy
        # GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        # GIT_TAG v0.13.1
        # GIT_SHALLOW TRUE
        # GIT_PROGRESS TRUE
    # )
    # FetchContent_MakeAvailable(tracy)

    # target_link_libraries(box3d PUBLIC TracyClient)
# endif()

    if (MSVC)
        # target_compile_options(${LIB_NAME} PRIVATE /W4 /WX)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
        target_link_libraries(${LIB_NAME} PUBLIC m)
        target_link_libraries(${LIB_NAME} PUBLIC winmm)
    endif()

    # file(COPY ${LIB_HEADERS} DESTINATION ${CLIPPER_INCLUDE_DIR}) # For the SDK
endif()

add_subdirectory(test)
