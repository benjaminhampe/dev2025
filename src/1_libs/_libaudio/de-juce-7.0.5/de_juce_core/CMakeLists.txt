set(LIB_NAME de_juce_core)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_core)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_core)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/containers/juce_AbstractFifo.h
        ${LIB_HEADER_DIR}/containers/juce_Array.h
        ${LIB_HEADER_DIR}/containers/juce_ArrayAllocationBase.h
        ${LIB_HEADER_DIR}/containers/juce_ArrayBase.h
        ${LIB_HEADER_DIR}/containers/juce_DynamicObject.h
        ${LIB_HEADER_DIR}/containers/juce_ElementComparator.h
        ${LIB_HEADER_DIR}/containers/juce_HashMap.h
        ${LIB_HEADER_DIR}/containers/juce_LinkedListPointer.h
        ${LIB_HEADER_DIR}/containers/juce_ListenerList.h
        ${LIB_HEADER_DIR}/containers/juce_NamedValueSet.h
        ${LIB_HEADER_DIR}/containers/juce_Optional.h
        ${LIB_HEADER_DIR}/containers/juce_OwnedArray.h
        ${LIB_HEADER_DIR}/containers/juce_PropertySet.h
        ${LIB_HEADER_DIR}/containers/juce_ReferenceCountedArray.h
        ${LIB_HEADER_DIR}/containers/juce_ScopedValueSetter.h
        ${LIB_HEADER_DIR}/containers/juce_SingleThreadedAbstractFifo.h
        ${LIB_HEADER_DIR}/containers/juce_SortedSet.h
        ${LIB_HEADER_DIR}/containers/juce_SparseSet.h
        ${LIB_HEADER_DIR}/containers/juce_Variant.h
        ${LIB_HEADER_DIR}/files/juce_AndroidDocument.h
        ${LIB_HEADER_DIR}/files/juce_common_MimeTypes.h
        ${LIB_HEADER_DIR}/files/juce_DirectoryIterator.h
        ${LIB_HEADER_DIR}/files/juce_File.h
        ${LIB_HEADER_DIR}/files/juce_FileFilter.h
        ${LIB_HEADER_DIR}/files/juce_FileInputStream.h
        ${LIB_HEADER_DIR}/files/juce_FileOutputStream.h
        ${LIB_HEADER_DIR}/files/juce_FileSearchPath.h
        ${LIB_HEADER_DIR}/files/juce_MemoryMappedFile.h
        ${LIB_HEADER_DIR}/files/juce_RangedDirectoryIterator.h
        ${LIB_HEADER_DIR}/files/juce_TemporaryFile.h
        ${LIB_HEADER_DIR}/files/juce_WildcardFileFilter.h
        ${LIB_HEADER_DIR}/javascript/juce_Javascript.h
        ${LIB_HEADER_DIR}/javascript/juce_JSON.h
        ${LIB_HEADER_DIR}/juce_core.h
        ${LIB_HEADER_DIR}/juce_core_common.h
        ${LIB_HEADER_DIR}/logging/juce_FileLogger.h
        ${LIB_HEADER_DIR}/logging/juce_Logger.h
        ${LIB_HEADER_DIR}/maths/juce_BigInteger.h
        ${LIB_HEADER_DIR}/maths/juce_Expression.h
        ${LIB_HEADER_DIR}/maths/juce_MathsFunctions.h
        ${LIB_HEADER_DIR}/maths/juce_NormalisableRange.h
        ${LIB_HEADER_DIR}/maths/juce_Random.h
        ${LIB_HEADER_DIR}/maths/juce_Range.h
        ${LIB_HEADER_DIR}/maths/juce_StatisticsAccumulator.h
        ${LIB_HEADER_DIR}/memory/juce_AllocationHooks.h
        ${LIB_HEADER_DIR}/memory/juce_Atomic.h
        ${LIB_HEADER_DIR}/memory/juce_ByteOrder.h
        ${LIB_HEADER_DIR}/memory/juce_ContainerDeletePolicy.h
        ${LIB_HEADER_DIR}/memory/juce_HeapBlock.h
        ${LIB_HEADER_DIR}/memory/juce_HeavyweightLeakedObjectDetector.h
        ${LIB_HEADER_DIR}/memory/juce_LeakedObjectDetector.h
        ${LIB_HEADER_DIR}/memory/juce_Memory.h
        ${LIB_HEADER_DIR}/memory/juce_MemoryBlock.h
        ${LIB_HEADER_DIR}/memory/juce_OptionalScopedPointer.h
        ${LIB_HEADER_DIR}/memory/juce_ReferenceCountedObject.h
        ${LIB_HEADER_DIR}/memory/juce_Reservoir.h
        ${LIB_HEADER_DIR}/memory/juce_ScopedPointer.h
        ${LIB_HEADER_DIR}/memory/juce_SharedResourcePointer.h
        ${LIB_HEADER_DIR}/memory/juce_Singleton.h
        ${LIB_HEADER_DIR}/memory/juce_WeakReference.h
        ${LIB_HEADER_DIR}/misc/juce_ConsoleApplication.h
        ${LIB_HEADER_DIR}/misc/juce_Functional.h
        ${LIB_HEADER_DIR}/misc/juce_Result.h
        ${LIB_HEADER_DIR}/misc/juce_RuntimePermissions.h
        ${LIB_HEADER_DIR}/misc/juce_Uuid.h
        ${LIB_HEADER_DIR}/misc/juce_WindowsRegistry.h
        ${LIB_HEADER_DIR}/native/juce_BasicNativeHeaders.h
        ${LIB_HEADER_DIR}/native/juce_intel_SharedCode.h
        ${LIB_HEADER_DIR}/native/juce_native_ThreadPriorities.h
        ${LIB_HEADER_DIR}/native/juce_win32_ComSmartPtr.h
        ${LIB_HEADER_DIR}/network/juce_IPAddress.h
        ${LIB_HEADER_DIR}/network/juce_MACAddress.h
        ${LIB_HEADER_DIR}/network/juce_NamedPipe.h
        ${LIB_HEADER_DIR}/network/juce_Socket.h
        ${LIB_HEADER_DIR}/network/juce_URL.h
        ${LIB_HEADER_DIR}/network/juce_WebInputStream.h
        ${LIB_HEADER_DIR}/streams/juce_AndroidDocumentInputSource.h
        ${LIB_HEADER_DIR}/streams/juce_BufferedInputStream.h
        ${LIB_HEADER_DIR}/streams/juce_FileInputSource.h
        ${LIB_HEADER_DIR}/streams/juce_InputSource.h
        ${LIB_HEADER_DIR}/streams/juce_InputStream.h
        ${LIB_HEADER_DIR}/streams/juce_MemoryInputStream.h
        ${LIB_HEADER_DIR}/streams/juce_MemoryOutputStream.h
        ${LIB_HEADER_DIR}/streams/juce_OutputStream.h
        ${LIB_HEADER_DIR}/streams/juce_SubregionStream.h
        ${LIB_HEADER_DIR}/streams/juce_URLInputSource.h
        ${LIB_HEADER_DIR}/system/juce_CompilerSupport.h
        ${LIB_HEADER_DIR}/system/juce_CompilerWarnings.h
        ${LIB_HEADER_DIR}/system/juce_PlatformDefs.h
        # ${LIB_HEADER_DIR}/system/juce_StandardHeader.h
        ${LIB_HEADER_DIR}/system/juce_SystemStats.h
        ${LIB_HEADER_DIR}/system/juce_TargetPlatform.h
        ${LIB_HEADER_DIR}/text/juce_Base64.h
        ${LIB_HEADER_DIR}/text/juce_CharacterFunctions.h
        ${LIB_HEADER_DIR}/text/juce_CharPointer_ASCII.h
        ${LIB_HEADER_DIR}/text/juce_CharPointer_UTF16.h
        ${LIB_HEADER_DIR}/text/juce_CharPointer_UTF32.h
        ${LIB_HEADER_DIR}/text/juce_CharPointer_UTF8.h
        ${LIB_HEADER_DIR}/text/juce_Identifier.h
        ${LIB_HEADER_DIR}/text/juce_LocalisedStrings.h
        ${LIB_HEADER_DIR}/text/juce_NewLine.h
        ${LIB_HEADER_DIR}/text/juce_String.h
        ${LIB_HEADER_DIR}/text/juce_StringArray.h
        ${LIB_HEADER_DIR}/text/juce_StringPairArray.h
        ${LIB_HEADER_DIR}/text/juce_StringPool.h
        ${LIB_HEADER_DIR}/text/juce_StringRef.h
        ${LIB_HEADER_DIR}/text/juce_TextDiff.h
        ${LIB_HEADER_DIR}/text/juce_NumbersToStringConverter.h
        ${LIB_HEADER_DIR}/threads/juce_ChildProcess.h
        ${LIB_HEADER_DIR}/threads/juce_CriticalSection.h
        ${LIB_HEADER_DIR}/threads/juce_DynamicLibrary.h
        ${LIB_HEADER_DIR}/threads/juce_HighResolutionTimer.h
        ${LIB_HEADER_DIR}/threads/juce_InterProcessLock.h
        ${LIB_HEADER_DIR}/threads/juce_Process.h
        ${LIB_HEADER_DIR}/threads/juce_ReadWriteLock.h
        ${LIB_HEADER_DIR}/threads/juce_ScopedLock.h
        ${LIB_HEADER_DIR}/threads/juce_ScopedReadLock.h
        ${LIB_HEADER_DIR}/threads/juce_ScopedWriteLock.h
        ${LIB_HEADER_DIR}/threads/juce_SpinLock.h
        ${LIB_HEADER_DIR}/threads/juce_Thread.h
        ${LIB_HEADER_DIR}/threads/juce_ThreadLocalValue.h
        ${LIB_HEADER_DIR}/threads/juce_ThreadPool.h
        ${LIB_HEADER_DIR}/threads/juce_TimeSliceThread.h
        ${LIB_HEADER_DIR}/threads/juce_WaitableEvent.h
        ${LIB_HEADER_DIR}/time/juce_PerformanceCounter.h
        ${LIB_HEADER_DIR}/time/juce_RelativeTime.h
        ${LIB_HEADER_DIR}/time/juce_Time.h
        ${LIB_HEADER_DIR}/unit_tests/juce_UnitTest.h
        ${LIB_HEADER_DIR}/unit_tests/juce_UnitTestCategories.h
        ${LIB_HEADER_DIR}/xml/juce_XmlDocument.h
        ${LIB_HEADER_DIR}/xml/juce_XmlElement.h
        ${LIB_HEADER_DIR}/zip/juce_GZIPCompressorOutputStream.h
        ${LIB_HEADER_DIR}/zip/juce_GZIPDecompressorInputStream.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/containers/juce_AbstractFifo.cpp
        ${LIB_SOURCE_DIR}/containers/juce_ArrayBase.cpp
        ${LIB_SOURCE_DIR}/containers/juce_DynamicObject.cpp
        ${LIB_SOURCE_DIR}/containers/juce_HashMap_test.cpp
        ${LIB_SOURCE_DIR}/containers/juce_ListenerList.cpp
        ${LIB_SOURCE_DIR}/containers/juce_NamedValueSet.cpp
        ${LIB_SOURCE_DIR}/containers/juce_Optional_test.cpp
        ${LIB_SOURCE_DIR}/containers/juce_OwnedArray.cpp
        ${LIB_SOURCE_DIR}/containers/juce_PropertySet.cpp
        ${LIB_SOURCE_DIR}/containers/juce_ReferenceCountedArray.cpp
        ${LIB_SOURCE_DIR}/containers/juce_SparseSet.cpp
        ${LIB_SOURCE_DIR}/containers/juce_Variant.cpp
        ${LIB_SOURCE_DIR}/files/juce_android_AndroidDocument.cpp
        ${LIB_SOURCE_DIR}/files/juce_common_MimeTypes.cpp
        ${LIB_SOURCE_DIR}/files/juce_DirectoryIterator.cpp
        ${LIB_SOURCE_DIR}/files/juce_File.cpp
        ${LIB_SOURCE_DIR}/files/juce_FileFilter.cpp
        ${LIB_SOURCE_DIR}/files/juce_FileInputStream.cpp
        ${LIB_SOURCE_DIR}/files/juce_FileOutputStream.cpp
        ${LIB_SOURCE_DIR}/files/juce_FileSearchPath.cpp
        ${LIB_SOURCE_DIR}/files/juce_RangedDirectoryIterator.cpp
        ${LIB_SOURCE_DIR}/files/juce_TemporaryFile.cpp
        ${LIB_SOURCE_DIR}/files/juce_WildcardFileFilter.cpp
        ${LIB_SOURCE_DIR}/javascript/juce_Javascript.cpp
        ${LIB_SOURCE_DIR}/javascript/juce_JSON.cpp
        ${LIB_SOURCE_DIR}/juce_core.cpp
        ${LIB_SOURCE_DIR}/logging/juce_FileLogger.cpp
        ${LIB_SOURCE_DIR}/logging/juce_Logger.cpp
        ${LIB_SOURCE_DIR}/maths/juce_BigInteger.cpp
        ${LIB_SOURCE_DIR}/maths/juce_Expression.cpp
        ${LIB_SOURCE_DIR}/maths/juce_Random.cpp
        ${LIB_SOURCE_DIR}/memory/juce_AllocationHooks.cpp
        ${LIB_SOURCE_DIR}/memory/juce_MemoryBlock.cpp
        ${LIB_SOURCE_DIR}/misc/juce_ConsoleApplication.cpp
        ${LIB_SOURCE_DIR}/misc/juce_Result.cpp
        ${LIB_SOURCE_DIR}/misc/juce_RuntimePermissions.cpp
        ${LIB_SOURCE_DIR}/misc/juce_Uuid.cpp
        ${LIB_SOURCE_DIR}/native/juce_win32_Files.cpp
        ${LIB_SOURCE_DIR}/native/juce_win32_Network.cpp
        ${LIB_SOURCE_DIR}/native/juce_win32_Registry.cpp
        ${LIB_SOURCE_DIR}/native/juce_win32_SystemStats.cpp
        ${LIB_SOURCE_DIR}/native/juce_win32_Threads.cpp
        # ${LIB_SOURCE_DIR}/network/juce_IPAddress.cpp # included by juce_win32_Network.cpp
        # ${LIB_SOURCE_DIR}/network/juce_MACAddress.cpp # included by juce_win32_Network.cpp
        # ${LIB_SOURCE_DIR}/network/juce_NamedPipe.cpp # included by juce_win32_Files.cpp
        ${LIB_SOURCE_DIR}/network/juce_Socket.cpp
        ${LIB_SOURCE_DIR}/network/juce_URL.cpp
        # ${LIB_SOURCE_DIR}/network/juce_WebInputStream.cpp # included by juce_win32_Network.cpp
        ${LIB_SOURCE_DIR}/streams/juce_BufferedInputStream.cpp
        ${LIB_SOURCE_DIR}/streams/juce_FileInputSource.cpp
        ${LIB_SOURCE_DIR}/streams/juce_InputStream.cpp
        ${LIB_SOURCE_DIR}/streams/juce_MemoryInputStream.cpp
        ${LIB_SOURCE_DIR}/streams/juce_MemoryOutputStream.cpp
        ${LIB_SOURCE_DIR}/streams/juce_OutputStream.cpp
        ${LIB_SOURCE_DIR}/streams/juce_SubregionStream.cpp
        ${LIB_SOURCE_DIR}/streams/juce_URLInputSource.cpp
        ${LIB_SOURCE_DIR}/system/juce_SystemStats.cpp
        ${LIB_SOURCE_DIR}/text/juce_Base64.cpp
        ${LIB_SOURCE_DIR}/text/juce_CharacterFunctions.cpp
        ${LIB_SOURCE_DIR}/text/juce_Identifier.cpp
        ${LIB_SOURCE_DIR}/text/juce_LocalisedStrings.cpp
        ${LIB_SOURCE_DIR}/text/juce_NewLine.cpp
        ${LIB_SOURCE_DIR}/text/juce_String.cpp
        ${LIB_SOURCE_DIR}/text/juce_StringArray.cpp
        ${LIB_SOURCE_DIR}/text/juce_StringPairArray.cpp
        ${LIB_SOURCE_DIR}/text/juce_StringPool.cpp
        ${LIB_SOURCE_DIR}/text/juce_TextDiff.cpp
        # ${LIB_SOURCE_DIR}/threads/juce_ChildProcess.cpp # included by juce_win32_Threads.cpp
        ${LIB_SOURCE_DIR}/threads/juce_DynamicLibrary.cpp
        ${LIB_SOURCE_DIR}/threads/juce_HighResolutionTimer.cpp
        ${LIB_SOURCE_DIR}/threads/juce_ReadWriteLock.cpp
        ${LIB_SOURCE_DIR}/threads/juce_Thread.cpp
        ${LIB_SOURCE_DIR}/threads/juce_ThreadPool.cpp
        ${LIB_SOURCE_DIR}/threads/juce_TimeSliceThread.cpp
        ${LIB_SOURCE_DIR}/threads/juce_WaitableEvent.cpp
        ${LIB_SOURCE_DIR}/time/juce_PerformanceCounter.cpp
        ${LIB_SOURCE_DIR}/time/juce_RelativeTime.cpp
        ${LIB_SOURCE_DIR}/time/juce_Time.cpp
        ${LIB_SOURCE_DIR}/unit_tests/juce_UnitTest.cpp
        ${LIB_SOURCE_DIR}/xml/juce_XmlDocument.cpp
        ${LIB_SOURCE_DIR}/xml/juce_XmlElement.cpp
        ${LIB_SOURCE_DIR}/zip/juce_GZIPCompressorOutputStream.cpp
        ${LIB_SOURCE_DIR}/zip/juce_GZIPDecompressorInputStream.cpp
        ${LIB_SOURCE_DIR}/zip/juce_ZipFile.cpp
    )

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC JUCE_DLL=1)
        target_compile_definitions(${LIB_NAME} PUBLIC JUCE_DLL_BUILD=1)
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC JUCE_DLL=0)
        target_compile_definitions(${LIB_NAME} PUBLIC JUCE_DLL_BUILD=0)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

    # target_compile_definitions(${LIB_NAME} PUBLIC _UNICODE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC UNICODE=1)

    target_compile_definitions(${LIB_NAME} PUBLIC JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
    #target_compile_definitions(${LIB_NAME} PUBLIC JUCE_API=)
    #target_compile_definitions(${LIB_NAME} PUBLIC JUCE_CALLTYPE=)
    # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_GCC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_64BIT=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_OPENGL=1) # GWL_WNDPROC

    target_compile_definitions(${LIB_NAME} PUBLIC JUCE_CHECK_MEMORY_LEAKS=0)
    target_compile_definitions(${LIB_NAME} PUBLIC JUCE_CATCH_UNHANDLED_EXCEPTIONS=0)
    target_compile_definitions(${LIB_NAME} PUBLIC JUCE_ENABLE_REPAINT_DEBUGGING=0)
    target_compile_definitions(${LIB_NAME} PUBLIC JUCE_ONLY_BUILD_CORE_LIBRARY=0)

    if (WIN32)
        target_compile_definitions(${LIB_NAME} PUBLIC _WIN32_WINNT=0x0602)
        target_link_libraries(${LIB_NAME} PUBLIC comctl32 user32 gdi32 kernel32)
        target_link_libraries(${LIB_NAME} PUBLIC mfplat mf mfuuid wmcodecdspuuid)
        target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        target_link_libraries(${LIB_NAME} PUBLIC dxgi dsound winmm ksguid ksuser ks uuid)
        target_link_libraries(${LIB_NAME} PUBLIC advapi32 ole32 oleaut32 comctl32 kernel32 user32 m)
        target_link_libraries(${LIB_NAME} PUBLIC wsock32 ws2_32) # freeaddrinfo
        target_link_libraries(${LIB_NAME} PUBLIC msvfw32 vfw32) # DrawDibDraw
        target_link_libraries(${LIB_NAME} PUBLIC version)
        target_link_libraries(${LIB_NAME} PUBLIC wininet)
        target_link_libraries(${LIB_NAME} PUBLIC shlwapi) # PathStripToRootW
    else()
        target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_ALSA__=1)
        target_compile_definitions(${LIB_NAME} PUBLIC __UNIX_JACK__=1)
        target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
        target_link_libraries(${LIB_NAME} PUBLIC asound)
        target_link_libraries(${LIB_NAME} PUBLIC jack)
    endif()

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-function)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
        #target_compile_options(${LIB_NAME} PUBLIC "-Wa,-mbig-obj")
        # Filesystem c++17 # Threads # Math
        if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
            target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
        endif()

        target_link_libraries(${LIB_NAME} PRIVATE pthread)
        target_link_libraries(${LIB_NAME} PRIVATE m)
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)
    # target_link_libraries(${LIB_NAME} PUBLIC de_asiosdk)
    # target_link_libraries(${LIB_NAME} PRIVATE de_curl)
    
endif()

# add_subdirectory(test)

