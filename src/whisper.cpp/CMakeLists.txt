# file(COPY media/whisper.cpp DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)

add_subdirectory(de_ggml)
add_subdirectory(de_whisper_cpp)
add_subdirectory(test_speech2text)

