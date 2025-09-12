set(LIB_NAME vstgui4_uidescription)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/vstgui/uidescription/base64codec.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/compresseduidescription.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/cstream.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/delegationcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/doc.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/icontentprovider.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/icontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/iuidescription.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/iviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/iviewfactory.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiattributes.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uicontentprovider.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uidescription.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uidescriptionfwd.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uidescriptionlistener.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiviewfactory.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/uiviewswitchcontainer.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/xmlparser.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/xmluidescdoc.h
		
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/locale.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/parsecolor.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/scalefactorutils.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uidesclist.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uijsonpersistence.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uinode.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uiviewcreatorattributes.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/detail/uixmlpersistence.h

        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/createuidescdata.sh
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/doc.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/iaction.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/igridprocessor.h

        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-bottom.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-left.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-right.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/attach-top.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-bottom.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-left.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-right.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top-non#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top-non.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-attach-top.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-fonts#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-fonts.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-tags#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-tags.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-views#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/darkmode-segment-views.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-bitmaps#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-bitmaps.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-colors#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-colors.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-fonts#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-fonts.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-gradients#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-gradients.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-tags#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-tags.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-views#2.0x.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/segment-views.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-center-white.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-center.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-left-white.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-left.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-right-white.png
        # ${LIB_HEADER_DIR}/vstgui/uidescription/editing/resources/text-align-right.png

        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiactions.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiattributescontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uibasedatasource.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uibitmapscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolor.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolorchoosercontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolorscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicolorslider.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uicrosslines.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidescriptioneditor.uidesc
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidescriptioneditor_res_dark.uidesc
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidescriptioneditor_res_light.uidesc
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uidialogcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uieditcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uieditmenucontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uieditview.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uifocussettingscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uifontscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uigradientscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uigrid.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uigridcontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uioverlayview.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiselection.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uitagscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uitemplatecontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uitemplatesettingscontroller.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiundomanager.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/editing/uiviewcreatecontroller.h

        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/animationsplashscreencreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/animknobcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/autoanimationcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/checkboxcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/controlcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/gradientviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/kickbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/knobcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/layeredviewcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/moviebitmapcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/moviebuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/multibitmapcontrolcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/multilinetextlabelcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/onoffbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/optionmenucreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/paramdisplaycreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/rockerswitchcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/rowcolumnviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/scrollviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/searchtexteditcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/segmentbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/shadowviewcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/sliderviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/splitviewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/stringlistcontrolcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/switchcreators.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/textbuttoncreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/texteditcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/textlabelcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/uiviewswitchcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/viewcontainercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/viewcreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/vumetercreator.h
        ${LIB_HEADER_DIR}/vstgui/uidescription/viewcreator/xypadcreator.h
    )	
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/vstgui/uidescription/compresseduidescription.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/cstream.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiattributes.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uicontentprovider.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uidescription.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiviewfactory.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/uiviewswitchcontainer.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/xmlparser.cpp
		
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uidesclist.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uijsonpersistence.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uinode.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/detail/uixmlpersistence.cpp

        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiactions.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiattributescontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uibitmapscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolor.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolorchoosercontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolorscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicolorslider.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uicrosslines.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uidialogcontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uieditcontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uieditmenucontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uieditview.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uifocussettingscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uifontscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uigradientscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uigridcontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uioverlayview.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiselection.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uitagscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uitemplatecontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uitemplatesettingscontroller.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiundomanager.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/editing/uiviewcreatecontroller.cpp

        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/animationsplashscreencreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/animknobcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/autoanimationcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/checkboxcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/controlcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/gradientviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/kickbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/knobcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/layeredviewcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/moviebitmapcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/moviebuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/multibitmapcontrolcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/multilinetextlabelcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/onoffbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/optionmenucreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/paramdisplaycreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/rockerswitchcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/rowcolumnviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/scrollviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/searchtexteditcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/segmentbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/shadowviewcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/sliderviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/splitviewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/stringlistcontrolcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/switchcreators.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/textbuttoncreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/texteditcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/textlabelcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/uiviewswitchcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/viewcontainercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/viewcreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/vumetercreator.cpp
        ${LIB_SOURCE_DIR}/vstgui/uidescription/viewcreator/xypadcreator.cpp
    )
	
	# if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/vstgui/uidescription)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # "$<$<CONFIG:Debug>:-DDEVELOPMENT=1>"
    # "$<$<CONFIG:Release>:-DRELEASE=1>"
    # "$<$<CONFIG:RelWithDebInfo>:-DRELEASE=1>"
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_definitions(${LIB_NAME} PRIVATE DEVELOPMENT=1)
    else()
            target_compile_definitions(${LIB_NAME} PRIVATE RELEASE=1)
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC de_vstgui4_lib)

endif()


