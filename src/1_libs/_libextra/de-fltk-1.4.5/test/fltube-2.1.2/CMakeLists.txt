set(APP_NAME fltube-2.1.2)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(APP_HEADERS
        ${APP_HEADER_DIR}/cache.h
        ${APP_HEADER_DIR}/configuration_manager.h
        ${APP_HEADER_DIR}/custom_widgets.h
        ${APP_HEADER_DIR}/FLTube.h
        ${APP_HEADER_DIR}/fltube_utils.h
        ${APP_HEADER_DIR}/FLTube_View.h
        ${APP_HEADER_DIR}/gnugettext_utils.h
        ${APP_HEADER_DIR}/userdata_manager.h
        ${APP_HEADER_DIR}/ytdlp_helper.h
    )

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/cache.cxx
        ${APP_SOURCE_DIR}/configuration_manager.cxx
        ${APP_SOURCE_DIR}/custom_widgets.cxx
        ${APP_SOURCE_DIR}/FLTube.cxx
        ${APP_SOURCE_DIR}/fltube_utils.cxx
        ${APP_SOURCE_DIR}/FLTube_View.cxx
        ${APP_SOURCE_DIR}/gnugettext_utils.cxx
        ${APP_SOURCE_DIR}/userdata_manager.cxx
        ${APP_SOURCE_DIR}/ytdlp_helper.cxx
    )

# ./doc/.gitignore
# ./doc/fltube.conf
# ./doc/fluid
# ./doc/fluid/FLTube_View.fl
# ./doc/install_deps.sh
# ./doc/LICENSE
# ./doc/locales
# ./doc/locales/es
# ./doc/locales/es/LC_MESSAGES
# ./doc/locales/es/LC_MESSAGES/FLTube.po
# ./doc/locales/es/LC_MESSAGES/install_yt-dlp.po
# ./doc/locales/FLTube.pot
# ./doc/locales/install_yt-dlp.pot
# ./doc/Makefile
# ./doc/packaging
# ./doc/packaging/common
# ./doc/packaging/common/readme.txt
# ./doc/packaging/common/yt-dlp_config
# ./doc/packaging/debian
# ./doc/packaging/debian/control.template
# ./doc/packaging/debian/postinst
# ./doc/packaging/debian/postrm
# ./doc/packaging/flinux
# ./doc/packaging/flinux/fltube.tcz.dep
# ./doc/packaging/flinux/fltube.tcz.tree
# ./doc/packaging/flinux/README.txt
# ./doc/packaging/tinycore
# ./doc/packaging/tinycore/fltube.tcz.dep
# ./doc/packaging/tinycore/fltube.tcz.info_TEMPLATE
# ./doc/packaging/tinycore/install.sh
# ./doc/packaging/tinycore/tce.installed
# ./doc/packaging/tinycore/tce.installed/fltube
# ./doc/README.md
# ./doc/README_es.MD
# ./doc/resources
# ./doc/resources/desktop
# ./doc/resources/desktop/fltube.desktop
# ./doc/resources/icons
# ./doc/resources/icons/128x128.png
# ./doc/resources/icons/256x256.png
# ./doc/resources/icons/64x64.png
# ./doc/resources/img
# ./doc/resources/img/arrow_down.png
# ./doc/resources/img/arrow_up.png
# ./doc/resources/img/cache_icon_18p.png
# ./doc/resources/img/clock_18p.png
# ./doc/resources/img/heart_18p.png
# ./doc/resources/img/heart_18p_red.png
# ./doc/resources/img/livebutton_18p.png
# ./doc/resources/img/loading.png
# ./doc/resources/img/no_thumbnail_available.png
# ./doc/resources/img/playicon.png
# ./doc/resources/img/remove_video_icon_14p.png
# ./doc/resources/img/watch_later.png
# ./doc/resources/img/watch_later_filled.png
# ./doc/scripts
# ./doc/scripts/install_yt-dlp.sh
# ./doc/VERSION

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_HEADERS} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_HEADER_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    target_link_libraries(${APP_NAME} PUBLIC de_fltk_145)
    target_link_libraries(${APP_NAME} PUBLIC de_iconv)
    target_link_libraries(${APP_NAME} PUBLIC de_iconv_icrt)
    target_link_libraries(${APP_NAME} PUBLIC de_curl_shared)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
endif()
