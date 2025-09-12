set(LIB_NAME de_bespoke_ableton_link)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		#   ____
		#  / ___|___  _ __ ___
		# | |   / _ \| '__/ _ \
		# | |__| (_) | | |  __/
		#  \____\___/|_|  \___|
		#	
		${LIB_HEADER_DIR}/ableton/link/Beats.hpp
		${LIB_HEADER_DIR}/ableton/link/CircularFifo.hpp
		${LIB_HEADER_DIR}/ableton/link/ClientSessionTimelines.hpp
		${LIB_HEADER_DIR}/ableton/link/Controller.hpp
		${LIB_HEADER_DIR}/ableton/link/Gateway.hpp
		${LIB_HEADER_DIR}/ableton/link/GhostXForm.hpp
		${LIB_HEADER_DIR}/ableton/link/HostTimeFilter.hpp
		${LIB_HEADER_DIR}/ableton/link/LinearRegression.hpp
		${LIB_HEADER_DIR}/ableton/link/Measurement.hpp
		${LIB_HEADER_DIR}/ableton/link/MeasurementEndpointV4.hpp
		${LIB_HEADER_DIR}/ableton/link/MeasurementService.hpp
		${LIB_HEADER_DIR}/ableton/link/Median.hpp
		${LIB_HEADER_DIR}/ableton/link/NodeId.hpp
		${LIB_HEADER_DIR}/ableton/link/NodeState.hpp
		${LIB_HEADER_DIR}/ableton/link/PayloadEntries.hpp
		${LIB_HEADER_DIR}/ableton/link/Optional.hpp
		${LIB_HEADER_DIR}/ableton/link/Peers.hpp
		${LIB_HEADER_DIR}/ableton/link/PeerState.hpp
		${LIB_HEADER_DIR}/ableton/link/Phase.hpp
		${LIB_HEADER_DIR}/ableton/link/PingResponder.hpp
		${LIB_HEADER_DIR}/ableton/link/SessionId.hpp
		${LIB_HEADER_DIR}/ableton/link/SessionState.hpp
		${LIB_HEADER_DIR}/ableton/link/Sessions.hpp
		${LIB_HEADER_DIR}/ableton/link/StartStopState.hpp
		${LIB_HEADER_DIR}/ableton/link/Tempo.hpp
		${LIB_HEADER_DIR}/ableton/link/Timeline.hpp
		${LIB_HEADER_DIR}/ableton/link/v1/Messages.hpp		
		#  ____  _
		# |  _ \(_)___  ___ _____   _____ _ __ _   _
		# | | | | / __|/ __/ _ \ \ / / _ \ '__| | | |
		# | |_| | \__ \ (_| (_) \ V /  __/ |  | |_| |
		# |____/|_|___/\___\___/ \_/ \___|_|   \__, |
		#     
		${LIB_HEADER_DIR}/ableton/discovery/InterfaceScanner.hpp
		${LIB_HEADER_DIR}/ableton/discovery/IpV4Interface.hpp
		${LIB_HEADER_DIR}/ableton/discovery/MessageTypes.hpp
		${LIB_HEADER_DIR}/ableton/discovery/NetworkByteStreamSerializable.hpp
		${LIB_HEADER_DIR}/ableton/discovery/Payload.hpp
		${LIB_HEADER_DIR}/ableton/discovery/PeerGateway.hpp
		${LIB_HEADER_DIR}/ableton/discovery/PeerGateways.hpp
		${LIB_HEADER_DIR}/ableton/discovery/Service.hpp
		${LIB_HEADER_DIR}/ableton/discovery/UdpMessenger.hpp
		${LIB_HEADER_DIR}/ableton/discovery/v1/Messages.hpp	
		#  ____  _       _    __
		# |  _ \| | __ _| |_ / _| ___  _ __ _ __ ___
		# | |_) | |/ _` | __| |_ / _ \| '__| '_ ` _ \
		# |  __/| | (_| | |_|  _| (_) | |  | | | | | |
		# |_|   |_|\__,_|\__|_|  \___/|_|  |_| |_| |_|
		#
		${LIB_HEADER_DIR}/ableton/platforms/Config.hpp
		${LIB_HEADER_DIR}/ableton/platforms/asio/AsioTimer.hpp
		${LIB_HEADER_DIR}/ableton/platforms/asio/AsioWrapper.hpp
		${LIB_HEADER_DIR}/ableton/platforms/asio/Context.hpp
		${LIB_HEADER_DIR}/ableton/platforms/asio/LockFreeCallbackDispatcher.hpp
		${LIB_HEADER_DIR}/ableton/platforms/asio/Socket.hpp
		${LIB_HEADER_DIR}/ableton/platforms/asio/Util.hpp
		#  _   _ _   _ _
		# | | | | |_(_) |
		# | | | | __| | |
		# | |_| | |_| | |
		#  \___/ \__|_|_|
		#
		${LIB_HEADER_DIR}/ableton/util/Injected.hpp
		${LIB_HEADER_DIR}/ableton/util/Log.hpp
		${LIB_HEADER_DIR}/ableton/util/SafeAsyncHandler.hpp
		${LIB_HEADER_DIR}/ableton/util/SampleTiming.hpp
		#  _____                       _           _
		# | ____|_  ___ __   ___  _ __| |_ ___  __| |
		# |  _| \ \/ / '_ \ / _ \| '__| __/ _ \/ _` |
		# | |___ >  <| |_) | (_) | |  | ||  __/ (_| |
		# |_____/_/\_\ .__/ \___/|_|   \__\___|\__,_|
		#            |_|		
		${LIB_HEADER_DIR}/ableton/Link.hpp
		${LIB_HEADER_DIR}/ableton/Link.ipp		
	)

	if (WIN32)
		list(APPEND LIB_HEADERS
			${LIB_HEADER_DIR}/ableton/platforms/stl/Random.hpp
			${LIB_HEADER_DIR}/ableton/platforms/windows/Clock.hpp
			${LIB_HEADER_DIR}/ableton/platforms/windows/ScanIpIfAddrs.hpp
			${LIB_HEADER_DIR}/ableton/platforms/windows/ThreadFactory.hpp
			${LIB_HEADER_DIR}/ableton/platforms/windows/Windows.hpp
		)
	# else()		
		# if (ESP_PLATFORM)
			# list(APPEND LIB_HEADERS
				# ${link_platform_DIR}/esp32/Clock.hpp
				# ${link_platform_DIR}/esp32/Context.hpp
				# ${link_platform_DIR}/esp32/Esp32.hpp
				# ${link_platform_DIR}/esp32/Random.hpp
				# ${link_platform_DIR}/esp32/ScanIpIfAddrs.hpp
			# )
		# elseif(UNIX)
			# list(APPEND LIB_HEADERS ${link_platform_DIR}/posix/ScanIpIfAddrs.hpp)
			# if(APPLE)
				# list(APPEND LIB_HEADERS
				# ${link_platform_DIR}/darwin/Clock.hpp
				# ${link_platform_DIR}/darwin/Darwin.hpp
				# ${link_platform_DIR}/darwin/ThreadFactory.hpp
				# ${link_platform_DIR}/stl/Random.hpp)
			# elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "kFreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "GNU")
				# list(APPEND LIB_HEADERS
				# ${link_platform_DIR}/linux/Clock.hpp
				# ${link_platform_DIR}/linux/Linux.hpp
				# ${link_platform_DIR}/stl/Clock.hpp
				# ${link_platform_DIR}/stl/Random.hpp)
			# if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
				# list(APPEND LIB_HEADERS
				# ${link_platform_DIR}/linux/ThreadFactory.hpp)
			# endif()
		# endif()
	endif()

	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/Clock.cpp
	)

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")	
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ABLETONLINK=1)
	target_link_libraries(${LIB_NAME} PUBLIC de_bespoke_boost_asio_standalone)
	
    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _bespoke)
    else()
		#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
		#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()
    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
