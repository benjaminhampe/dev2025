set(APP_NAME demo_Irrlicht_03_CustomSceneNode)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    if (WIN32)
    enable_language(RC)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} main.cpp icon.rc)

    target_link_libraries(${APP_NAME} PRIVATE de_irrlicht)
endif()
