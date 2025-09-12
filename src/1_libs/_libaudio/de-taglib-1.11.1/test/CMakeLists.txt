
add_executable(framelist framelist.cpp)
target_link_libraries(framelist de_taglib DarkImage)

add_executable(tagreader tagreader.cpp)
target_link_libraries(tagreader de_taglib)

add_executable(tagwriter tagwriter.cpp)
target_link_libraries(tagwriter de_taglib)

add_executable(strip-id3v1 strip-id3v1.cpp)
target_link_libraries(strip-id3v1 de_taglib)

#add_executable(tagreader_c tagreader_c.c)
#target_link_libraries(tagreader_c tag_c)

