set(LIB_NAME de_SheenBidi_300)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SheenBidi/SBAlgorithm.h
        ${LIB_HEADER_DIR}/SheenBidi/SBAllocator.h
        ${LIB_HEADER_DIR}/SheenBidi/SBAttributeInfo.h
        ${LIB_HEADER_DIR}/SheenBidi/SBAttributeList.h
        ${LIB_HEADER_DIR}/SheenBidi/SBAttributeRegistry.h
        ${LIB_HEADER_DIR}/SheenBidi/SBBase.h
        ${LIB_HEADER_DIR}/SheenBidi/SBBidiType.h
        ${LIB_HEADER_DIR}/SheenBidi/SBCodepoint.h
        ${LIB_HEADER_DIR}/SheenBidi/SBCodepointSequence.h
        ${LIB_HEADER_DIR}/SheenBidi/SBConfig.h
        ${LIB_HEADER_DIR}/SheenBidi/SBGeneralCategory.h
        ${LIB_HEADER_DIR}/SheenBidi/SBLine.h
        ${LIB_HEADER_DIR}/SheenBidi/SBMirrorLocator.h
        ${LIB_HEADER_DIR}/SheenBidi/SBParagraph.h
        ${LIB_HEADER_DIR}/SheenBidi/SBRun.h
        ${LIB_HEADER_DIR}/SheenBidi/SBScript.h
        ${LIB_HEADER_DIR}/SheenBidi/SBScriptLocator.h
        ${LIB_HEADER_DIR}/SheenBidi/SBText.h
        ${LIB_HEADER_DIR}/SheenBidi/SBTextConfig.h
        ${LIB_HEADER_DIR}/SheenBidi/SBTextIterators.h
        ${LIB_HEADER_DIR}/SheenBidi/SBTextType.h
        ${LIB_HEADER_DIR}/SheenBidi/SBVersion.h
        ${LIB_HEADER_DIR}/SheenBidi/SheenBidi.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/SheenBidi.c
    
        ${LIB_SOURCE_DIR}/API/SBAlgorithm.c
        ${LIB_SOURCE_DIR}/API/SBAlgorithm.h
        ${LIB_SOURCE_DIR}/API/SBAllocator.c
        ${LIB_SOURCE_DIR}/API/SBAllocator.h
        ${LIB_SOURCE_DIR}/API/SBAssert.h
        ${LIB_SOURCE_DIR}/API/SBAttributeInfo.h
        ${LIB_SOURCE_DIR}/API/SBAttributeList.c
        ${LIB_SOURCE_DIR}/API/SBAttributeList.h
        ${LIB_SOURCE_DIR}/API/SBAttributeRegistry.c
        ${LIB_SOURCE_DIR}/API/SBAttributeRegistry.h
        ${LIB_SOURCE_DIR}/API/SBBase.c
        ${LIB_SOURCE_DIR}/API/SBBase.h
        ${LIB_SOURCE_DIR}/API/SBCodepoint.c
        ${LIB_SOURCE_DIR}/API/SBCodepoint.h
        ${LIB_SOURCE_DIR}/API/SBCodepointSequence.c
        ${LIB_SOURCE_DIR}/API/SBCodepointSequence.h
        ${LIB_SOURCE_DIR}/API/SBLine.c
        ${LIB_SOURCE_DIR}/API/SBLine.h
        ${LIB_SOURCE_DIR}/API/SBLog.c
        ${LIB_SOURCE_DIR}/API/SBLog.h
        ${LIB_SOURCE_DIR}/API/SBMirrorLocator.c
        ${LIB_SOURCE_DIR}/API/SBMirrorLocator.h
        ${LIB_SOURCE_DIR}/API/SBParagraph.c
        ${LIB_SOURCE_DIR}/API/SBParagraph.h
        ${LIB_SOURCE_DIR}/API/SBScriptLocator.c
        ${LIB_SOURCE_DIR}/API/SBScriptLocator.h
        ${LIB_SOURCE_DIR}/API/SBText.c
        ${LIB_SOURCE_DIR}/API/SBText.h
        ${LIB_SOURCE_DIR}/API/SBTextConfig.c
        ${LIB_SOURCE_DIR}/API/SBTextConfig.h
        ${LIB_SOURCE_DIR}/API/SBTextIterators.c
        ${LIB_SOURCE_DIR}/API/SBTextIterators.h

        ${LIB_SOURCE_DIR}/Core/AtomicFlag.h
        ${LIB_SOURCE_DIR}/Core/AtomicPointer.h
        ${LIB_SOURCE_DIR}/Core/AtomicUInt.h
        ${LIB_SOURCE_DIR}/Core/List.c
        ${LIB_SOURCE_DIR}/Core/List.h
        ${LIB_SOURCE_DIR}/Core/Memory.c
        ${LIB_SOURCE_DIR}/Core/Memory.h
        ${LIB_SOURCE_DIR}/Core/Object.c
        ${LIB_SOURCE_DIR}/Core/Object.h
        ${LIB_SOURCE_DIR}/Core/Once.c
        ${LIB_SOURCE_DIR}/Core/Once.h
        ${LIB_SOURCE_DIR}/Core/ThreadFence.h
        ${LIB_SOURCE_DIR}/Core/ThreadLocalStorage.h

        ${LIB_SOURCE_DIR}/Data/BidiTypeLookup.c
        ${LIB_SOURCE_DIR}/Data/BidiTypeLookup.h
        ${LIB_SOURCE_DIR}/Data/GeneralCategoryLookup.c
        ${LIB_SOURCE_DIR}/Data/GeneralCategoryLookup.h
        ${LIB_SOURCE_DIR}/Data/PairingLookup.c
        ${LIB_SOURCE_DIR}/Data/PairingLookup.h
        ${LIB_SOURCE_DIR}/Data/ScriptLookup.c
        ${LIB_SOURCE_DIR}/Data/ScriptLookup.h

        ${LIB_SOURCE_DIR}/Script/ScriptStack.c
        ${LIB_SOURCE_DIR}/Script/ScriptStack.h

        ${LIB_SOURCE_DIR}/Text/AttributeDictionary.c
        ${LIB_SOURCE_DIR}/Text/AttributeDictionary.h
        ${LIB_SOURCE_DIR}/Text/AttributeManager.c
        ${LIB_SOURCE_DIR}/Text/AttributeManager.h

        ${LIB_SOURCE_DIR}/UBA/BidiChain.c
        ${LIB_SOURCE_DIR}/UBA/BidiChain.h
        ${LIB_SOURCE_DIR}/UBA/BracketQueue.c
        ${LIB_SOURCE_DIR}/UBA/BracketQueue.h
        ${LIB_SOURCE_DIR}/UBA/BracketType.h
        ${LIB_SOURCE_DIR}/UBA/IsolatingRun.c
        ${LIB_SOURCE_DIR}/UBA/IsolatingRun.h
        ${LIB_SOURCE_DIR}/UBA/LevelRun.c
        ${LIB_SOURCE_DIR}/UBA/LevelRun.h
        ${LIB_SOURCE_DIR}/UBA/RunExtrema.h
        ${LIB_SOURCE_DIR}/UBA/RunKind.h
        ${LIB_SOURCE_DIR}/UBA/RunQueue.c
        ${LIB_SOURCE_DIR}/UBA/RunQueue.h
        ${LIB_SOURCE_DIR}/UBA/StatusStack.c
        ${LIB_SOURCE_DIR}/UBA/StatusStack.h
    )

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC SB_CONFIG_DLL_IMPORT=1)
        target_compile_definitions(${LIB_NAME} PRIVATE SB_CONFIG_DLL_EXPORT=1)
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC SB_CONFIG_EXPERIMENTAL_TEXT_API=1)
    
    # set(LIB_EXTLIB_DIR ${LIB_SOURCE_DIR}/extlibs/headers)
    # target_include_directories(${LIB_NAME} SYSTEM PRIVATE "${LIB_EXTLIB_DIR}/cpp-unicodelib")

    # target_compile_definitions(${LIB_NAME} PUBLIC SFML_OS_WINDOWS=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC SFML_OS_ANDROID=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC SFML_OS_LINUX=1)

    # if(SFML_OS_LINUX)
        # target_link_libraries(${LIB_NAME} PRIVATE rt)
    # elseif(SFML_OS_WINDOWS)
        # target_link_libraries(${LIB_NAME} PRIVATE winmm)
    # elseif(SFML_OS_ANDROID)
        # target_link_libraries(${LIB_NAME} PRIVATE android log)
    # endif()

    # target_link_libraries(${LIB_NAME} PUBLIC de_sfml_extlibs)


    # target_compile_definitions(${LIB_NAME} PRIVATE LUNASVG_DISABLE_LOAD_SYSTEM_FONTS)

    #target_compile_definitions(${LIB_NAME} PUBLIC -D_IRR_STATIC_LIB_=1)
    # if (MSVC)
    # else()
    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    # endif()
    # target_link_libraries(${LIB_NAME} PUBLIC de_irrlicht)
    # target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    # #target_link_libraries(${LIB_NAME} PUBLIC de_glad)

    # target_link_libraries(${LIB_NAME} PUBLIC de_plutovg_131)

endif()

# add_subdirectory(test)
